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
    void RegisterOnIoctl ( const EventIoctl& );
    static SOCKET InitSocket ( SocketType type , IPFamily family = IPv4 );
protected:
    SOCKET m_hSocket;
    mutable std::recursive_mutex m_Locker;
    size_t m_nBtSnt;
    size_t m_nBtRcv;
    NetworkSocket ( ) :
        m_hSocket ( INVALID_SOCKET ) ,
        m_nBtSnt ( 0 ) ,
        m_nBtRcv ( 0 )
    {
    }
    NetworkSocket ( const NetworkSocket& ) = delete;
    NetworkSocket ( SOCKET sock ) :
        m_hSocket ( sock ) ,
        m_nBtSnt ( 0 ) ,
        m_nBtRcv ( 0 )
    {
    }
    NetworkSocket ( SocketType type ) :
        m_hSocket ( InitSocket ( type ) ) ,
        m_nBtSnt ( 0 ) ,
        m_nBtRcv ( 0 )
    {
    }

};

class UDPASocket : public Async , public NetworkSocket , public std::enable_shared_from_this<UDPASocket>
{
public:
    virtual ~UDPASocket ( );
    static std::shared_ptr<UDPASocket> Create ( );

    using EventRecv = std::function<void ( UDPASocket& , NetworkAddress* , char* , size_t )>;
    using EventSend = std::function<void ( UDPASocket& , NetworkAddress* , size_t )>;
    using EventClose = std::function<void ( UDPASocket& , DWORD errorCode )>;

    void RequestRecvFrom ( CompletionPort& port , OverlappedEx* ev = nullptr );
    void RequestSendTo ( CompletionPort& port , char* pbuffer , unsigned int buf_len , NetworkAddress& addr , OverlappedEx* ev = nullptr );

    void RegisterOnClose ( const EventClose& );
    void RegisterOnRecv ( const EventRecv& );
    void RegisterOnSend ( const EventSend& );

    virtual std::string GetId ( )const;

    virtual void Close ( );
    void UnregisterAllCallbacks ( );

    virtual void RegisterOnCompletionPort ( CompletionPort& );

    static void EventHandlerRecvFrom ( std::shared_ptr<UDPASocket>& socket , OverlappedEx* ev , DWORD bytesTranseferred , DWORD statusCode );
    static void EventHandlerSendTo ( std::shared_ptr<UDPASocket>& socket , OverlappedEx* ev , DWORD bytesTranseferred , DWORD statusCode );
    static void EventHandlerRequestRecvFrom ( std::shared_ptr<UDPASocket>& socket , OverlappedEx* ev , DWORD bytesTranseferred , DWORD statusCode );
    static void EventHandlerRequestSendTo ( std::shared_ptr<UDPASocket>& socket , OverlappedEx* ev , DWORD bytesTranseferred , DWORD statusCode );
    static void EventHandlerClose ( std::shared_ptr<UDPASocket>& socket , OverlappedEx* ev , DWORD bytesTranseferred , DWORD statusCode );
    UDPASocket ( );
protected:
    std::pair<int , DWORD> RecvFrom ( OverlappedEx* ev , LPWSABUF bufs , DWORD count );
    std::pair<int , DWORD> SendTo ( OverlappedEx* ev , LPWSABUF bufs , DWORD count );
    size_t m_nRcvSeqPosted;
    size_t m_nRcvSeqCompleted;
    UDPASocket ( const UDPASocket& ) = delete;
    EventClose OnClose;
    EventRecv OnRecv;
    EventSend OnSend;
    std::shared_ptr<UDPASocket>* m_pCompletionKey;
    std::unordered_map<size_t , OverlappedEx*> m_RrdQueue;
};
class TCPASocket : public Async , public NetworkSocket , public std::enable_shared_from_this<TCPASocket>
{
protected:
    size_t m_nRcvSeqPosted;
    size_t m_nRcvSeqCompleted;
public:
    static std::shared_ptr<TCPASocket> Create ( );
    using EventAccept = std::function<void ( TCPASocket& , NetworkAddress* )>;
    using EventConnect = std::function<void ( TCPASocket& , NetworkAddress* )>;
    using EventDisconnect = std::function<void ( TCPASocket& )>;
    using EventRecv = std::function<void ( TCPASocket& , char* , size_t )>;
    using EventSend = std::function<void ( TCPASocket& , size_t )>;

    void Listen ( );
    void RequestAccept ( );
    void RequestConnect ( );
    void RequestDisconnect ( );
    void RequestRecv ( );
    void RequestSend ( );

    virtual std::string GetId ( )const;
    virtual void Close ( );

    std::pair<int , DWORD> Accept ( );
    std::pair<int , DWORD> Connect ( );
    std::pair<int , DWORD> Disconnect ( );
    std::pair<int , DWORD> Recv ( );
    std::pair<int , DWORD> Send ( );


    void RegisterOnAccept ( const EventAccept& );
    void RegisterOnConnect ( const EventConnect& );
    void RegisterOnDisconnect ( const EventDisconnect& );
    void RegisterOnRecv ( const EventRecv& );
    void RegisterOnSend ( const EventSend& );

    virtual void InheritCallbacks ( NetworkSocket* )override;
    virtual void RegisterOnCompletionPort ( CompletionPort& );

    static void EventHandlerAccept ( std::shared_ptr<TCPASocket>& socket , OverlappedEx* ev , DWORD bytesTranseferred , DWORD statusCode );
    static void EventHandlerConnect ( std::shared_ptr<TCPASocket>& socket , OverlappedEx* ev , DWORD bytesTranseferred , DWORD statusCode );
    static void EventHandlerDisconnect ( std::shared_ptr<TCPASocket>& socket , OverlappedEx* ev , DWORD bytesTranseferred , DWORD statusCode );
    static void EventHandlerRecv ( std::shared_ptr<TCPASocket>& socket , OverlappedEx* ev , DWORD bytesTranseferred , DWORD statusCode );
    static void EventHandlerSend ( std::shared_ptr<TCPASocket>& socket , OverlappedEx* ev , DWORD bytesTranseferred , DWORD statusCode );
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