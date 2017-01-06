#include "Socket.h"
#include "NetworkAddress.h"
#include "ThreadPool.h"

LPFN_ACCEPTEX NetworkingInitialization::_AcceptEx = nullptr;
LPFN_CONNECTEX NetworkingInitialization::_ConnectEx = nullptr;
LPFN_DISCONNECTEX NetworkingInitialization::_DisconnectEx = nullptr;
LPFN_GETACCEPTEXSOCKADDRS NetworkingInitialization::_GetAcceptExSockaddrs = nullptr;
LPFN_TRANSMITFILE NetworkingInitialization::_TransmitFile = nullptr;
LPFN_TRANSMITPACKETS NetworkingInitialization::_TransmitPackets = nullptr;

//Socket::Socket ( SockType type , IPType ip ) :
//    m_nBytesSent ( 0 ) ,
//    m_nBytesRecv ( 0 ) ,
//    m_nPostedSeqNum ( 0 ) ,
//    m_nCompletedSeqNum ( 0 ) ,
//    m_SockType ( type ) ,
//    m_IPType ( ip )
//{
//    NetworkingInitialization::Instance ( );
//    m_hSocket = InitializeSocket ( m_SockType , m_IPType );
//    if ( m_hSocket == INVALID_SOCKET )
//    {
//        SYSTEM_ERROR ( "Can't create socket" );
//    }
//}
//
//Socket::Socket ( SockType type , SOCKET sock , IPType ip ) :
//    m_nBytesSent ( 0 ) ,
//    m_nBytesRecv ( 0 ) ,
//    m_nPostedSeqNum ( 0 ) ,
//    m_nCompletedSeqNum ( 0 ) ,
//    m_hSocket ( sock ) ,
//    m_SockType ( type ) ,
//    m_IPType ( ip )
//{
//    if ( m_hSocket == INVALID_SOCKET )
//    {
//        SYSTEM_ERROR ( "Can't create socket" );
//    }
//}
//
//Socket::~Socket ( )
//{
//    std::lock_guard<std::recursive_mutex> lock ( m_Locker );
//    Close ( );
//}
//
//SOCKET Socket::Handle ( ) const
//{
//    return m_hSocket;
//}
//
//Socket::SockType Socket::SocketType ( ) const
//{
//    return m_SockType;
//}
//
//Socket::IPType Socket::IPFamily ( ) const
//{
//    return m_IPType;
//}
//
//size_t Socket::BytesSent ( ) const
//{
//    return m_nBytesSent;
//}
//
//size_t Socket::BytesReceived ( ) const
//{
//    return m_nBytesRecv;
//}
//
//void Socket::RegisterOnCompletionPort ( CompletionPort* port )
//{
//    if ( port )
//    {
//        port->RegisterHandle ( ( HANDLE ) m_hSocket , ( ULONG_PTR )this );
//    }
//}
//
//void Socket::Close ( )
//{
//    ::closesocket ( m_hSocket );
//}
//
//void Socket::Bind ( )
//{
//
//
//}
//
//void Socket::Bind ( USHORT port )
//{
//    NetworkAddress addr ( 0 , port );
//    auto status = NetworkAddress::GetLocalHost ( m_IPType , &addr );
//    addr.Set_Port ( port );
//    if ( ::bind ( m_hSocket , addr.GetSockAddr ( ) , addr.GetSockAddrLen ( ) ) )
//    {
//        SYSTEM_ERROR ( "Can't bind the socket" );
//    }
//}
//
//void Socket::Listen ( )
//{
//    if ( ::listen ( m_hSocket , SOMAXCONN ) == SOCKET_ERROR )
//    {
//        SYSTEM_ERROR ( "Can't listen the socket" );
//    }
//}
//
//std::pair<int , DWORD> Socket::Accept ( OverlappedEx* ov )
//{
//
//}
//
//std::pair<int , DWORD> Socket::Connect ( OverlappedEx* ov , sockaddr* addr )
//{
//
//}
//
//std::pair<int , DWORD> Socket::Disconnect ( OverlappedEx* ov )
//{
//
//}
//
//std::pair<int , DWORD> Socket::Ioctl ( OverlappedEx* ov )
//{
//
//}
//
//std::pair<int , DWORD> Socket::RecvFrom ( OverlappedEx* ov , LPWSABUF bufs , DWORD count )
//{
//
//}
//
//std::pair<int , DWORD> Socket::Recv ( OverlappedEx* ov , LPWSABUF bufs , DWORD count )
//{
//
//}
//
//std::pair<int , DWORD> Socket::Send ( OverlappedEx* ov , LPWSABUF bufs , DWORD count )
//{
//
//}
//
//std::pair<int , DWORD> Socket::SendTo ( OverlappedEx* ov , LPWSABUF bufs , DWORD count )
//{
//
//}
//
//std::pair<int , DWORD> Socket::SendFile ( OverlappedEx* ov )
//{
//
//}
//
//void Socket::RequestAccept ( CompletionPort* port , OverlappedEx* ov /*= nullptr */ )
//{
//    if ( port )
//    {
//        ov = ( !ov ) ? ThreadPool::Instance ( ).Allocate ( ) : ov;
//        ov->reset ( );
//        ov->Operation = OverlappedEx::Req_AcceptEx;
//        ov->Socket = InitializeSocket ( m_SockType , m_IPType );
//        port->PostQueuedCompletionStatus ( NULL , ( ULONG_PTR )this , ( LPOVERLAPPED ) ov );
//    }
//}
//
//void Socket::RequestConnect ( CompletionPort* port , sockaddr* addr , OverlappedEx* ov /*= nullptr */ )
//{
//    if ( port )
//    {
//        ov = ( !ov ) ? ThreadPool::Instance ( ).Allocate ( ) : ov;
//        ov->reset ( );
//        ov->Operation = OverlappedEx::Req_ConnectEx;
//        port->PostQueuedCompletionStatus ( NULL , ( ULONG_PTR )this , ( LPOVERLAPPED ) ov );
//    }
//}
//
//void Socket::RequestDisconnect ( CompletionPort* port , OverlappedEx* ov /*= nullptr */ )
//{
//    if ( port )
//    {
//        ov = ( !ov ) ? ThreadPool::Instance ( ).Allocate ( ) : ov;
//        ov->reset ( );
//        ov->Operation = OverlappedEx::Req_DisconnectEx;
//        port->PostQueuedCompletionStatus ( NULL , ( ULONG_PTR )this , ( LPOVERLAPPED ) ov );
//    }
//}
//
//void Socket::RequestIoctl ( CompletionPort* port , OverlappedEx* ov /*= nullptr */ )
//{
//    if ( port )
//    {
//
//    }
//}
//
//void Socket::RequestRecv ( CompletionPort* port , OverlappedEx* ov /*= nullptr */ )
//{
//    if ( port )
//    {
//        ov = ( !ov ) ? ThreadPool::Instance ( ).Allocate ( ) : ov;
//        ov->reset;
//        ov->Operation = OverlappedEx::Req_Recv;
//        port->PostQueuedCompletionStatus ( NULL , ( ULONG_PTR )this , ( LPOVERLAPPED ) ov );
//    }
//}
//
//void Socket::RequestRecvFrom ( CompletionPort* port , OverlappedEx* ov /*= nullptr */ )
//{
//    if ( port )
//    {
//        ov = ( !ov ) ? ThreadPool::Instance ( ).Allocate ( ) : ov;
//        ov->reset;
//        ov->Operation = OverlappedEx::Req_RecvFrom;
//        port->PostQueuedCompletionStatus ( NULL , ( ULONG_PTR )this , ( LPOVERLAPPED ) ov );
//    }
//}
//
//void Socket::RequestSend ( CompletionPort* port , char* pbuffer , unsigned int buf_len , OverlappedEx* ov )
//{
//    if ( port && pbuffer && buf_len )
//    {
//        ov = ( !ov ) ? ThreadPool::Instance ( ).Allocate ( ) : ov;
//        ov->reset ( );
//        ov->Operation = OverlappedEx::Req_Send;
//        ov->BufferHolder.buf = pbuffer;
//        ov->BufferHolder.len = buf_len;
//        port->PostQueuedCompletionStatus ( NULL , ( ULONG_PTR )this , ( LPOVERLAPPED ) ov );
//    }
//}
//
//void Socket::RequestSendFile ( CompletionPort* port,  OverlappedEx* ov /*= nullptr */ )
//{
//    if ( port )
//    {
//        ov = ( !ov ) ? ThreadPool::Instance ( ).Allocate ( ) : ov;
//        ov->reset ( );
//        ov->Operation = OverlappedEx::Req_SendFile;
//        //... insert code here
//        port->PostQueuedCompletionStatus ( NULL , ( ULONG_PTR )this , ( LPOVERLAPPED ) ov );
//    }
//}
//
//void Socket::RequestSendTo ( CompletionPort* port , char* pbuffer , unsigned int buf_len , NetworkAddress& addr , OverlappedEx* ov /*= nullptr */ )
//{
//    if ( port )
//    {
//        ov = ( !ov ) ? ThreadPool::Instance ( ).Allocate ( ) : ov;
//        ov->reset ( );
//        ov->Operation = OverlappedEx::Req_SendTo;
//        ov->NetAddr = addr;
//        ov->BufferHolder.buf = pbuffer;
//        ov->BufferHolder.len = buf_len;
//        port->PostQueuedCompletionStatus ( NULL , ( ULONG_PTR )this , ( LPOVERLAPPED ) ov );
//    }
//}
//
//void Socket::RegisterOnAccept ( const EventAccept& cb )
//{
//    std::lock_guard<std::recursive_mutex> lock ( m_Locker );
//    if ( cb )
//    {
//        OnAccept = cb;
//    }
//}
//
//void Socket::RegisterOnConnect ( const EventConnect& cb )
//{
//    std::lock_guard<std::recursive_mutex> lock ( m_Locker );
//    if ( cb )
//    {
//        OnConnect = cb;
//    }
//}
//
//void Socket::RegisterOnDisconnect ( const EventDisconnect& cb )
//{
//    std::lock_guard<std::recursive_mutex> lock ( m_Locker );
//    if ( cb )
//    {
//        OnDisconnect = cb;
//    }
//}
//
//void Socket::RegisterOnIoctl ( const EventIoctl& cb )
//{
//    std::lock_guard<std::recursive_mutex> lock ( m_Locker );
//    if ( cb )
//    {
//        OnIoctl = cb;
//    }
//}
//
//void Socket::RegisterOnRecv ( const EventRecv& cb )
//{
//    std::lock_guard<std::recursive_mutex> lock ( m_Locker );
//    if ( cb )
//    {
//        OnRecv = cb;
//    }
//}
//
//void Socket::RegisterOnRecvFrom ( const EventRecvFrom& cb )
//{
//    std::lock_guard<std::recursive_mutex> lock ( m_Locker );
//    if ( cb )
//    {
//        OnRecvFrom = cb;
//    }
//}
//
//void Socket::RegisterOnSend ( const EventSend& cb )
//{
//    std::lock_guard<std::recursive_mutex> lock ( m_Locker );
//    if ( cb )
//    {
//        OnSend = cb;
//    }
//}
//
//void Socket::RegisterOnSendTo ( const EventSendTo& cb )
//{
//    std::lock_guard<std::recursive_mutex> lock ( m_Locker );
//    if ( cb )
//    {
//        OnSendTo = cb;
//    }
//}
//
//void Socket::InheritCallbacks ( Socket* sock )
//{
//    std::lock_guard<std::recursive_mutex> lock ( m_Locker );
//    if ( sock )
//    {
//        OnAccept = sock->OnAccept;
//        OnConnect = sock->OnConnect;
//        OnDisconnect = sock->OnDisconnect;
//        OnError = sock->OnError;
//        OnIoctl = sock->OnIoctl;
//        OnRecv = sock->OnRecv;
//        OnRecvFrom = sock->OnRecvFrom;
//        OnSend = sock->OnSend;
//        OnSendTo = sock->OnSendTo;
//    }
//}
//
//void Socket::UnregisterAll ( )
//{
//    std::lock_guard<std::recursive_mutex> lock ( m_Locker );
//    OnAccept = nullptr;
//    OnConnect = nullptr;
//    OnDisconnect = nullptr;
//    OnError = nullptr;
//    OnIoctl = nullptr;
//    OnRecv = nullptr;
//    OnRecvFrom = nullptr;
//    OnSend = nullptr;
//    OnSendTo = nullptr;
//}
//
//SOCKET Socket::InitializeSocket ( SockType type , IPType ip )
//{
//    SOCKET sock_t = INVALID_SOCKET;
//    switch ( type )
//    {
//        case TCP:
//            sock_t = ::WSASocketW ( ip , type , IPPROTO_TCP , NULL , NULL , WSA_FLAG_OVERLAPPED );
//            break;
//        case UDP:
//            sock_t = ::WSASocketW ( ip , type , IPPROTO_UDP , NULL , NULL , WSA_FLAG_OVERLAPPED );
//            break;
//    }
//    return sock_t;
//}
//
//void Socket::EventHandlerAccept ( Socket* , OverlappedEx* , DWORD bytes )
//{
//
//}
//
//void Socket::EventHandlerConnect ( Socket* , OverlappedEx* , DWORD bytes )
//{
//
//}
//
//void Socket::EventHandlerDisconnect ( Socket* , OverlappedEx* , DWORD bytes )
//{
//
//}
//
//void Socket::EventHandlerIoctl ( Socket* , OverlappedEx* , DWORD bytes )
//{
//
//}
//
//void Socket::EventHandlerRecv ( Socket* , OverlappedEx* , DWORD bytes )
//{
//
//}
//
//void Socket::EventHandlerRecvFrom ( Socket* , OverlappedEx* , DWORD bytes )
//{
//
//}
//
//void Socket::EventHandlerSend ( Socket* , OverlappedEx* , DWORD bytes )
//{
//
//}
//
//void Socket::EventHandlerSendFile ( Socket* , OverlappedEx* , DWORD bytes )
//{
//
//}
//
//void Socket::EventHandlerSendTo ( Socket* , OverlappedEx* , DWORD bytes )
//{
//
//}
NetworkingInitialization::NetworkingInitialization ( ) :
    m_AcceptEx ( WSAID_ACCEPTEX ) ,
    m_ConnectEx ( WSAID_CONNECTEX ) ,
    m_DisconnectEx ( WSAID_DISCONNECTEX ) ,
    m_GetAcceptExSockaddrs ( WSAID_DISCONNECTEX ) ,
    m_TransmitFile ( WSAID_TRANSMITFILE ) ,
    m_TransmitPackets ( WSAID_TRANSMITPACKETS )
{
    auto status = ::WSAStartup ( MAKEWORD ( 2 , 2 ) , &m_WsaData );
    if ( status != 0 )
    {
        ::WSACleanup ( );
        SYSTEM_ERROR ( "Failed initialize networking system" );
    }
    auto socket_t = NetworkSocket::InitSocket ( NetworkSocket::TCP , NetworkSocket::IPv4 );
    DWORD bytes_t;
    if ( ::WSAIoctl ( socket_t , SIO_GET_EXTENSION_FUNCTION_POINTER , &m_AcceptEx , sizeof ( m_AcceptEx ) , &_AcceptEx , sizeof ( _AcceptEx ) , &bytes_t , NULL , NULL ) == SOCKET_ERROR ||
         ::WSAIoctl ( socket_t , SIO_GET_EXTENSION_FUNCTION_POINTER , &m_ConnectEx , sizeof ( m_ConnectEx ) , &_ConnectEx , sizeof ( _ConnectEx ) , &bytes_t , NULL , NULL ) == SOCKET_ERROR ||
         ::WSAIoctl ( socket_t , SIO_GET_EXTENSION_FUNCTION_POINTER , &m_DisconnectEx , sizeof ( m_DisconnectEx ) , &_DisconnectEx , sizeof ( _DisconnectEx ) , &bytes_t , NULL , NULL ) == SOCKET_ERROR ||
         ::WSAIoctl ( socket_t , SIO_GET_EXTENSION_FUNCTION_POINTER , &m_GetAcceptExSockaddrs , sizeof ( m_GetAcceptExSockaddrs ) , &_GetAcceptExSockaddrs , sizeof (_GetAcceptExSockaddrs ) , &bytes_t , NULL , NULL ) == SOCKET_ERROR ||
         ::WSAIoctl ( socket_t , SIO_GET_EXTENSION_FUNCTION_POINTER , &m_TransmitFile , sizeof ( m_TransmitFile ) , &_TransmitFile , sizeof ( _TransmitFile ) , &bytes_t , NULL , NULL ) == SOCKET_ERROR ||
         ::WSAIoctl ( socket_t , SIO_GET_EXTENSION_FUNCTION_POINTER , &m_TransmitPackets , sizeof ( m_TransmitPackets ) , &_TransmitPackets , sizeof ( _TransmitPackets ) , &bytes_t , NULL , NULL ) == SOCKET_ERROR )
    {
        ::closesocket ( socket_t );
        ::WSACleanup ( );
        SYSTEM_ERROR ( "Failed obtain function pointers" );
    }
    ::closesocket ( socket_t );
}
NetworkingInitialization::~NetworkingInitialization ( )
{
    ::WSACleanup ( );
}
NetworkingInitialization& NetworkingInitialization::Instance ( )
{
    static NetworkingInitialization g_obj;
    return g_obj;
}

UDPASocket::~UDPASocket ( )
{
    Close ( );
    std::cout << "UDPASocket class destroyed\n";
}

std::shared_ptr<UDPASocket> UDPASocket::Create ( )
{
    return std::make_shared<UDPASocket> ( );
}

std::pair<int , DWORD> UDPASocket::RecvFrom ( OverlappedEx* ev , LPWSABUF bufs , DWORD count )
{
    std::lock_guard<std::recursive_mutex> lock ( m_Locker );
    if ( !ev && !IsValid ( ) )
    {
        throw std::runtime_error ( "Pointer to overlapped struct is nullptr or handle isn't valid" );
    }
    else
    {
        DWORD recvflags = 0;
        m_nPendingOperations++;
        sockaddr*  addr = const_cast< sockaddr* >( ev->NetAddr.GetSockAddr ( ) );
        ev->AddrLen = sizeof ( sockaddr_in );
        auto res = ::WSARecvFrom ( m_hSocket , bufs , count , NULL , &recvflags , addr , &ev->AddrLen , ev , NULL );
        if ( res == SOCKET_ERROR )
        {
            auto err = ::GetLastError ( );
            if ( err != ERROR_IO_PENDING )
            {
                ev->reset ( );
                m_nPendingOperations--;
                ThreadPool::Instance ( ).Free ( ev );
                SYSTEM_ERROR ( "Can't execute RecvFrom on this socket" );
            }
        }
        return std::pair<int , DWORD> ( res , ::GetLastError ( ) );
    }
}

std::pair<int , DWORD> UDPASocket::SendTo ( OverlappedEx* ev , LPWSABUF bufs , DWORD count )
{
    std::lock_guard<std::recursive_mutex> lock ( m_Locker );
    if ( !ev && !IsValid ( ) )
    {
        throw std::runtime_error ( "Pointer to overlapped struct is nullptr or handle isn't valid" );
    }
    else
    {
        m_nPendingOperations++;
        auto res = ::WSASendTo ( m_hSocket , bufs , count , NULL , 0 , const_cast< sockaddr* >( ev->NetAddr.GetSockAddr ( ) ) , ev->NetAddr.GetSockAddrLen ( ) , ev , NULL );
        if ( res == SOCKET_ERROR )
        {
            auto err = ::GetLastError ( );
            if ( err != ERROR_IO_PENDING )
            {
                m_nPendingOperations--;
                ev->reset ( );
                ThreadPool::Instance ( ).Free ( ev );
                SYSTEM_ERROR ( "Can't execute SendTo on this socket" );
            }
        }
        return std::pair<int , DWORD> ( res , ::GetLastError ( ) );
    }
}

void UDPASocket::RequestRecvFrom ( CompletionPort& port , OverlappedEx* ev /*= nullptr */ )
{
    if ( m_pCompletionKey )
    {
        ev = ( !ev ) ? ThreadPool::Instance ( ).Allocate ( ) : ev;
        ev->reset ( );
        ev->Operation = OverlappedEx::Req_RecvFrom;
        port.PostQueuedCompletionStatus ( NULL , ( ULONG_PTR ) m_pCompletionKey , ( LPOVERLAPPED ) ev );
    }
    else
    {
        throw std::runtime_error ( "Not registered on completion port" );
    }
}
void UDPASocket::RequestSendTo ( CompletionPort& port , char* pbuffer , unsigned int buf_len , NetworkAddress& addr , OverlappedEx* ev /*= nullptr */ )
{
    if ( m_pCompletionKey )
    {
        ev = ( !ev ) ? ThreadPool::Instance ( ).Allocate ( ) : ev;
        ev->reset ( );
        ev->Operation = OverlappedEx::Req_SendTo;
        ev->NetAddr = addr;
        ev->BufferHolder.buf = pbuffer;
        ev->BufferHolder.len = buf_len;
        port.PostQueuedCompletionStatus ( NULL , ( ULONG_PTR ) m_pCompletionKey , ( LPOVERLAPPED ) ev );
    }
    else
    {
        throw std::runtime_error ( "Not registered on completion port" );
    }
}

void UDPASocket::RegisterOnClose ( const EventClose& callback )
{
    std::lock_guard<std::recursive_mutex> lock ( m_Locker );
    if ( callback )
    {
        OnClose = callback;
    }
}

void UDPASocket::RegisterOnRecv ( const EventRecv& callback )
{
    std::lock_guard<std::recursive_mutex> lock ( m_Locker );
    if ( callback )
    {
        OnRecv = callback;
    }
}

void UDPASocket::RegisterOnSend ( const EventSend& callback )
{
    std::lock_guard<std::recursive_mutex> lock ( m_Locker );
    if ( callback )
    {
        OnSend = callback;
    }
}

std::string UDPASocket::GetId ( ) const
{
    static const std::string udp ( "UDP : " );
    if ( m_hSocket != INVALID_SOCKET )
    {
        return udp + std::to_string ( ( unsigned int ) m_hSocket );
    }
    else
    {
        return udp + "Invalid handle";
    }
}

void UDPASocket::UnregisterAllCallbacks ( )
{
    std::lock_guard<std::recursive_mutex> lock ( m_Locker );
    OnClose = nullptr;
    OnRecv = nullptr;
    OnSend = nullptr;
}

void UDPASocket::RegisterOnCompletionPort ( CompletionPort& port )
{ 
    if ( IsValid ( ) && !m_pCompletionKey )
    {
        m_pCompletionKey = new std::shared_ptr<UDPASocket> ( shared_from_this ( ) );
        auto result = port.RegisterHandle ( ( HANDLE ) m_hSocket , ( ULONG_PTR ) m_pCompletionKey );
        if ( !result )
        {
            delete m_pCompletionKey;
            m_pCompletionKey = nullptr;
            SYSTEM_ERROR ( "Can't register socket handle on completion port" );
        }
    }
}
void UDPASocket::EventHandlerRecvFrom ( std::shared_ptr<UDPASocket>& socket , OverlappedEx* ev , DWORD bytesTransferred , DWORD statusCode )
{
    std::lock_guard<std::recursive_mutex> lock ( socket->m_Locker );
    socket->m_nBtRcv += bytesTransferred;
    ev->Flags = bytesTransferred;
    if ( ev->SequenceNum - socket->m_nRcvSeqCompleted > 1 )
    {
        socket->m_RrdQueue [ ev->SequenceNum ] = ev;
    }
    else if ( ev->SequenceNum - socket->m_nRcvSeqCompleted == 1 )
    {
        if ( socket->OnRecv )
        {
            socket->OnRecv ( *socket , &ev->NetAddr , ev->Buffer , bytesTransferred );
        }
        socket->m_nRcvSeqCompleted++;
        ev->reset ( );
        socket->m_nPendingOperations--;
        socket->RequestRecvFrom ( ThreadPool::Instance ( ) , ev );
        while ( !socket->m_RrdQueue.empty ( ) )
        {
            auto it = socket->m_RrdQueue.find ( socket->m_nRcvSeqCompleted + 1 );
            if ( it != socket->m_RrdQueue.end ( ) )
            {
                auto ev_t= it->second;
                if ( socket->OnRecv )
                {
                    socket->OnRecv ( *socket , &ev_t->NetAddr , ev_t->Buffer , ev_t->Flags );
                }
                socket->m_nRcvSeqCompleted++;
                socket->m_RrdQueue.erase ( ev_t->SequenceNum );
                ev_t->reset ( );
                socket->m_nPendingOperations--;
                socket->RequestRecvFrom ( ThreadPool::Instance ( ) , ev_t );
            }
            else
            {
                break;
            }
        }
    }
}

void UDPASocket::EventHandlerSendTo ( std::shared_ptr<UDPASocket>& socket , OverlappedEx* ev , DWORD bytesTransferred , DWORD statusCode )
{
    std::lock_guard<std::recursive_mutex> lock ( socket->m_Locker );
    socket->m_nBtSnt += bytesTransferred;
    if ( socket->OnSend )
    {
        socket->OnSend ( *socket , &ev->NetAddr , bytesTransferred );
    }
    ThreadPool::Instance ( ).Free ( ev );
    socket->m_nPendingOperations--;
}

void UDPASocket::EventHandlerRequestRecvFrom ( std::shared_ptr<UDPASocket>& socket , OverlappedEx* ev , DWORD bytesTransferred , DWORD statusCode )
{
    std::lock_guard<std::recursive_mutex> lock ( socket->m_Locker );
    ev->Operation = OverlappedEx::RecvFrom;
    ev->SequenceNum = socket->m_nRcvSeqPosted++;
    WSABUF buf;
    buf.buf = ev->Buffer;
    buf.len = ev->Length;
    auto result = socket->RecvFrom ( ev , &buf , 1 );
}

void UDPASocket::EventHandlerRequestSendTo ( std::shared_ptr<UDPASocket>& socket , OverlappedEx* ev , DWORD bytesTransferred , DWORD statusCode )
{
    std::lock_guard<std::recursive_mutex> lock ( socket->m_Locker );
    ev->Operation = OverlappedEx::SendTo;
    WSABUF buf;
    buf.buf = ev->BufferHolder.buf;
    buf.len = ev->BufferHolder.len;
    auto result = socket->SendTo ( ev , &buf , 1 );
}
void UDPASocket::Close ( )
{
    std::lock_guard<std::recursive_mutex> lock ( m_Locker );
    if ( m_hSocket != INVALID_SOCKET )
    {
        if ( closesocket ( m_hSocket ) == SOCKET_ERROR )
        {
            SYSTEM_ERROR ( "Error during closing socket" );
        }
        else
        {
            m_hSocket = INVALID_SOCKET;
            if ( !m_nPendingOperations && m_pCompletionKey )
            {
                delete m_pCompletionKey;
                m_pCompletionKey = nullptr;
            }
        }
    }
}
void UDPASocket::EventHandlerClose ( std::shared_ptr<UDPASocket>& socket , OverlappedEx* ev , DWORD bytesTransferred , DWORD statusCode )
{
    std::unique_lock<std::recursive_mutex> lock ( socket->m_Locker );
    if ( socket->IsValid ( ) )
    {
        socket->Close ( );
    }
    if ( socket->OnClose )
    {
        socket->OnClose ( *socket , statusCode ); //one-shot only
        socket->UnregisterAllCallbacks ( );
    }
    socket->m_nPendingOperations--;
    if ( !socket->m_RrdQueue.empty ( ) )
    {
        for ( auto&i : socket->m_RrdQueue )
        { 
            auto ev_t = i.second;
            ev_t->reset ( );
            socket->m_RrdQueue.erase ( i.first );
            socket->m_nPendingOperations--;
            ThreadPool::Instance ( ).Free ( ev_t );
        }
    }
    if ( socket->m_nPendingOperations == 0 )
    {
        auto ptr = &socket;
        socket->m_pCompletionKey = nullptr;
        lock.unlock ( );
        delete ptr;
    }
    ThreadPool::Instance ( ).Free ( ev );
}

UDPASocket::UDPASocket ( ) :
    m_nRcvSeqCompleted ( 0 ) ,
    m_nRcvSeqPosted ( 1 ) ,
    m_pCompletionKey ( nullptr ) ,
    NetworkSocket ( UDP )
{
}

std::shared_ptr<TCPASocket> TCPASocket::Create ( )
{
    return std::make_shared<TCPASocket> ( );
}

void TCPASocket::Listen ( )
{

}

std::string TCPASocket::GetId ( ) const
{
    static const std::string udp ( "TCP : " );
    if ( m_hSocket != INVALID_SOCKET )
    {
        return udp + std::to_string ( ( unsigned int ) m_hSocket );
    }
    else
    {
        return udp + "Invalid handle";
    }
}

void TCPASocket::Close ( )
{

}

void TCPASocket::RegisterOnAccept ( const EventAccept& )
{

}

void TCPASocket::RegisterOnConnect ( const EventConnect& )
{

}

void TCPASocket::RegisterOnDisconnect ( const EventDisconnect& )
{

}

void TCPASocket::RegisterOnRecv ( const EventRecv& )
{

}

void TCPASocket::RegisterOnSend ( const EventSend& )
{

}

void TCPASocket::InheritCallbacks ( NetworkSocket* )
{

}

void TCPASocket::RegisterOnCompletionPort ( CompletionPort& )
{

}

void TCPASocket::EventHandlerAccept ( std::shared_ptr<TCPASocket>& socket , OverlappedEx* ev , DWORD bytesTranseferred , DWORD statusCode )
{

}

void TCPASocket::EventHandlerConnect ( std::shared_ptr<TCPASocket>& socket , OverlappedEx* ev , DWORD bytesTranseferred , DWORD statusCode )
{

}

void TCPASocket::EventHandlerDisconnect ( std::shared_ptr<TCPASocket>& socket , OverlappedEx* ev , DWORD bytesTranseferred , DWORD statusCode )
{

}

void TCPASocket::EventHandlerRecv ( std::shared_ptr<TCPASocket>& socket , OverlappedEx* ev , DWORD bytesTranseferred , DWORD statusCode )
{

}

void TCPASocket::EventHandlerSend ( std::shared_ptr<TCPASocket>& socket , OverlappedEx* ev , DWORD bytesTranseferred , DWORD statusCode )
{

}

void NetworkSocket::Bind ( unsigned short int port )
{
    NetworkAddress addr ( 0 , port );
    auto status = NetworkAddress::GetLocalHost ( IPv4 , &addr );
    addr.Set_Port ( port );
    if ( ::bind ( m_hSocket , addr.GetSockAddr ( ) , addr.GetSockAddrLen ( ) ) )
    {
        SYSTEM_ERROR ( "Can't bind the socket" );
    }
}



void NetworkSocket::InheritCallbacks ( NetworkSocket* )
{

}

void NetworkSocket::RegisterOnIoctl ( const EventIoctl& )
{

}

SOCKET NetworkSocket::InitSocket ( SocketType type , IPFamily family /*= IPv4 */ )
{
    SOCKET sock_t = INVALID_SOCKET;
    switch ( type )
    {
        case TCP:
            sock_t = ::WSASocketW ( family , type , IPPROTO_TCP , NULL , 0 , WSA_FLAG_OVERLAPPED );
            break;
        case UDP:
            sock_t = ::WSASocketW ( family , type , IPPROTO_UDP , NULL , 0 , WSA_FLAG_OVERLAPPED );
            break;
        default:
            throw std::runtime_error ( "Invalid socket type" );
    }
    return sock_t;
}
