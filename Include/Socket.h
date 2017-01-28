#ifndef _SOCKET_H_
#define _SOCKET_H_
#include <winsock2.h>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <Windows.h>
#include "Overlapped.h"
#include <memory>
#include "CompletionPort.h"
#include <functional>
#include <atomic>
#include <unordered_map>
#include <mutex>

using STATUS = std::pair<int , DWORD>;

class NetworkSocket
{
public:
    enum SocketType
    {
        TCP = SOCK_STREAM ,
        UDP = SOCK_DGRAM ,
    };
    enum IPFamily
    {
        IPv4 = AF_INET ,
        IPv6 = AF_INET6 ,
    };
    virtual ~NetworkSocket ( )
    {
        std::cout << "NetworkSocket class destroyed\n";
    }
    size_t BytesSent ( )const
    {
        std::lock_guard<std::recursive_mutex> lock ( m_Locker );
        return m_nBtSnt;
    }
    size_t BytesRecv ( )const
    {
        std::lock_guard<std::recursive_mutex> lock ( m_Locker );
        return m_nBtRcv;
    }
    size_t GetSimultaneousRecvCalls ( )const
    {
        return m_nSimultaneousRecvCalls;
    }
    void SetSimultaneousRecvCalls ( size_t max_calls )
    {
        m_nSimultaneousRecvCalls = max_calls;
    }
    virtual std::string GetId ( )const = 0;
    bool IsValid ( )const
    {
        std::lock_guard<std::recursive_mutex> lock ( m_Locker );
        return ( m_hSocket != INVALID_SOCKET ) ? true : false;
    }
    void Bind ( unsigned short int port );
    virtual void Close ( ) = 0;

    virtual void InheritCallbacks ( NetworkSocket* );
    using EventIoctl = std::function<void ( NetworkSocket* )>;
    void RegisterCallback ( const EventIoctl& );
    static SOCKET InitSocket ( SocketType type , IPFamily family = IPv4 );
protected:
    SOCKET m_hSocket;
    mutable std::recursive_mutex m_Locker;
    std::atomic<size_t> m_nBtSnt;
    std::atomic<size_t> m_nBtRcv;
    std::atomic<size_t> m_nSimultaneousRecvCalls;
    virtual void EventHandlerClose ( OverlappedEx* , DWORD , DWORD ) = 0;
    NetworkSocket ( ) :
        m_hSocket ( INVALID_SOCKET ) ,
        m_nBtSnt ( 0 ) ,
        m_nBtRcv ( 0 ) ,
        m_nSimultaneousRecvCalls ( 0 ) // unlimited recv calls
    {
    }
    NetworkSocket ( const NetworkSocket& ) = delete;
    NetworkSocket ( SOCKET sock ) :
        m_hSocket ( sock ) ,
        m_nBtSnt ( 0 ) ,
        m_nBtRcv ( 0 ) ,
        m_nSimultaneousRecvCalls ( 0 )
    {
    }
    NetworkSocket ( SocketType type ) :
        m_hSocket ( InitSocket ( type ) ) ,
        m_nBtSnt ( 0 ) ,
        m_nBtRcv ( 0 ) ,
        m_nSimultaneousRecvCalls ( 0 )
    {
    }

};
//multiple callbacks may be invoked simultaneously
class UDPSocketHT : public Async , public NetworkSocket , public std::enable_shared_from_this<UDPSocketHT>
{
public:
    virtual ~UDPSocketHT ( );
    static std::shared_ptr<UDPSocketHT> Create ( );
    using EventRecv = std::function<void ( UDPSocketHT& , NetworkAddress* , char* , size_t )>;
    using EventSend = std::function<void ( UDPSocketHT& , NetworkAddress* , size_t )>;
    using EventClose = std::function<void ( UDPSocketHT& , DWORD errorCode )>;
    void RequestRecvFrom ( CompletionPort& port , OverlappedEx* ev = nullptr );
    void RequestSendTo ( CompletionPort& port , char* pbuffer , unsigned int buf_len , NetworkAddress& addr , OverlappedEx* ev = nullptr );

    void RegisterCallback ( const EventClose& );
    void RegisterCallback ( const EventRecv& );
    void RegisterCallback ( const EventSend& );

    virtual std::string GetId ( )const;

    virtual void Close ( );
    void UnregisterCallbacks ( );

    virtual void RegisterOnCompletionPort ( CompletionPort& );

    virtual void HandleEvents ( OverlappedEx* ev , DWORD bytesTransferred , DWORD statusCode );
    UDPSocketHT ( );
protected:
    virtual void EventHandlerRecvFrom ( OverlappedEx* ev , DWORD bytesTransferred , DWORD statusCode );
    virtual void EventHandlerSendTo ( OverlappedEx* ev , DWORD bytesTransferred , DWORD statusCode );
    virtual void EventHandlerRequestRecvFrom ( OverlappedEx* ev , DWORD bytesTransferred , DWORD statusCode );
    virtual void EventHandlerRequestSendTo ( OverlappedEx* ev , DWORD bytesTransferred , DWORD statusCode );
    virtual void EventHandlerClose ( OverlappedEx* ev , DWORD bytesTransferred , DWORD statusCode );
    STATUS RecvFrom ( OverlappedEx* ev , LPWSABUF bufs , DWORD count );
    STATUS SendTo ( OverlappedEx* ev , LPWSABUF bufs , DWORD count );
    // sanity of sequeunce udp drgams isn't needed
    //size_t m_nRcvSeqPosted;
    //size_t m_nRcvSeqCompleted;
    UDPSocketHT ( const UDPSocketHT& ) = delete;
    EventClose OnClose;
    EventRecv m_OnRecv;
    EventSend OnSend;
    std::shared_ptr<UDPSocketHT>* m_pCompletionKey;
    //std::unordered_map<size_t , OverlappedEx*> m_RrdQueue;
};
class TCPSocketSQ : public Async , public NetworkSocket , public std::enable_shared_from_this<TCPSocketSQ>
{
public:

    static std::shared_ptr<TCPSocketSQ> Create ( );
    using EventAccept = std::function<void ( TCPSocketSQ& , NetworkAddress* )>;
    using EventConnect = std::function<void ( TCPSocketSQ& , NetworkAddress* , bool )>;
    using EventDisconnect = std::function<void ( TCPSocketSQ& )>;
    using EventRecv = std::function<void ( TCPSocketSQ& , char* , size_t )>;
    using EventSend = std::function<void ( TCPSocketSQ& , size_t )>;

    void Listen ( );
    void RequestAccept ( CompletionPort& , OverlappedEx* ev = nullptr );
    void RequestConnect ( CompletionPort& , NetworkAddress& , OverlappedEx* ev = nullptr );
    void RequestDisconnect ( CompletionPort& , OverlappedEx* ev = nullptr );
    void RequestRecv ( CompletionPort& , OverlappedEx* ev = nullptr );
    void RequestSend ( CompletionPort& , char* buffer , DWORD len , OverlappedEx* ev = nullptr );

    virtual std::string GetId ( )const;
    virtual void Close ( );

    void RegisterCallback ( const EventAccept& );
    void RegisterCallback ( const EventConnect& );
    void RegisterCallback ( const EventDisconnect& );
    void RegisterCallback ( const EventRecv& );
    void RegisterCallback ( const EventSend& );

    virtual void InheritCallbacks ( NetworkSocket* )override;
    virtual void RegisterOnCompletionPort ( CompletionPort& );
    virtual void HandleEvents ( OverlappedEx* , DWORD bytesTransferred , DWORD statusCode );
protected:
    std::shared_ptr<TCPSocketSQ>* m_pCompletionKey;
    std::unordered_map<size_t , OverlappedEx*> m_RcvQueue;
    size_t m_nRcvSeqPosted;
    size_t m_nRcvSeqCompleted;

    EventAccept OnAccept;
    EventConnect OnConnect;
    EventDisconnect OnDisconnect;
    EventRecv OnRecv;
    EventSend OnSend;

    STATUS Accept ( OverlappedEx* ev );
    STATUS Connect ( );
    STATUS Disconnect ( );
    STATUS Recv ( OverlappedEx* ev , LPWSABUF buf , DWORD count );
    STATUS Send ( OverlappedEx* ev , LPWSABUF buf , DWORD count );

    virtual void EventHandlerAccept (  OverlappedEx* ev , DWORD bytesTransferred , DWORD statusCode );
    virtual void EventHandlerConnect (  OverlappedEx* ev , DWORD bytesTransferred , DWORD statusCode );
    virtual void EventHandlerDisconnect (  OverlappedEx* ev , DWORD bytesTransferred , DWORD statusCode );
    virtual void EventHandlerRecv (  OverlappedEx* ev , DWORD bytesTransferred , DWORD statusCode );
    virtual void EventHandlerSend (  OverlappedEx* ev , DWORD bytesTransferred , DWORD statusCode );
    virtual void EventHandlerClose ( OverlappedEx* , DWORD , DWORD );
};
template<typename _Socket> class Socket
{
    std::shared_ptr<_Socket> m_Socket;
public:
    Socket ( )
    {
        m_Socket = _Socket::Create ( );
    }
    explicit Socket ( std::shared_ptr<_Socket>& sock ) : m_Socket ( sock )
    {
    }
    Socket ( const Socket<_Socket>& _OthSock ) : m_Socket ( _OthSock )
    {
    }
    Socket ( Socket<_Socket>&& _OthSock )
    {
        m_Socket = std::move ( _OthSock.m_Socket );
    }
    Socket<_Socket>& operator = ( const Socket<_Socket>& _OthSock )
    {
        if ( this != &_OthSock )
        {
            if ( m_Socket.use_count ( ) == 2 )
            {
                m_Socket->Close ( );
            }
            m_Socket = _OthSock.m_Socket;
        }
        return *this;
    }
    Socket<_Socket>& operator=( Socket<_Socket>&& _OthSock )
    {
        if ( this != &_OthSock )
        {
            m_Socket = std::move ( _OthSock.m_Socket );
        }
        return *this;
    }
    virtual ~Socket ( )
    {
        if ( m_Socket.use_count ( ) == 2 )
        {
            m_Socket->Close ( );
        }
    }
    const std::shared_ptr<_Socket>& operator()( )
    {
        return m_Socket;
    }
};
class NetworkingInitialization
{
    WSADATA m_WsaData;
    GUID m_AcceptEx;
    GUID m_ConnectEx;
    GUID m_DisconnectEx;
    GUID m_GetAcceptExSockaddrs;
    GUID m_TransmitFile;
    GUID m_TransmitPackets;
    NetworkingInitialization ( );
    ~NetworkingInitialization ( );
public:
    static NetworkingInitialization& Instance ( );

    static LPFN_ACCEPTEX _AcceptEx;
    static LPFN_CONNECTEX _ConnectEx;
    static LPFN_DISCONNECTEX _DisconnectEx;
    static LPFN_GETACCEPTEXSOCKADDRS _GetAcceptExSockaddrs;
    static LPFN_TRANSMITFILE _TransmitFile;
    static LPFN_TRANSMITPACKETS _TransmitPackets;
};

#endif