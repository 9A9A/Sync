#include "Socket.h"
#include "NetworkAddress.h"
#include "ThreadPool.h"

LPFN_ACCEPTEX NetworkingInitialization::_AcceptEx = nullptr;
LPFN_CONNECTEX NetworkingInitialization::_ConnectEx = nullptr;
LPFN_DISCONNECTEX NetworkingInitialization::_DisconnectEx = nullptr;
LPFN_GETACCEPTEXSOCKADDRS NetworkingInitialization::_GetAcceptExSockaddrs = nullptr;
LPFN_TRANSMITFILE NetworkingInitialization::_TransmitFile = nullptr;
LPFN_TRANSMITPACKETS NetworkingInitialization::_TransmitPackets = nullptr;
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
         ::WSAIoctl ( socket_t , SIO_GET_EXTENSION_FUNCTION_POINTER , &m_GetAcceptExSockaddrs , sizeof ( m_GetAcceptExSockaddrs ) , &_GetAcceptExSockaddrs , sizeof ( _GetAcceptExSockaddrs ) , &bytes_t , NULL , NULL ) == SOCKET_ERROR ||
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

UDPSocketHT::~UDPSocketHT ( )
{
    Close ( );
    std::cout << "UDPASocket class destroyed\n";
}

std::shared_ptr<UDPSocketHT> UDPSocketHT::Create ( )
{
    return std::make_shared<UDPSocketHT> ( );
}

STATUS UDPSocketHT::RecvFrom ( OverlappedEx* ev , LPWSABUF bufs , DWORD count )
{
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
        return STATUS ( res , ::GetLastError ( ) );
    }
}

STATUS UDPSocketHT::SendTo ( OverlappedEx* ev , LPWSABUF bufs , DWORD count )
{
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
        return STATUS ( res , ::GetLastError ( ) );
    }
}

void UDPSocketHT::RequestRecvFrom ( CompletionPort& port , OverlappedEx* ev /*= nullptr */ )
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
void UDPSocketHT::RequestSendTo ( CompletionPort& port , char* pbuffer , unsigned int buf_len , NetworkAddress& addr , OverlappedEx* ev /*= nullptr */ )
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

void UDPSocketHT::RegisterCallback ( const EventClose& callback )
{
    std::lock_guard<std::recursive_mutex> lock ( m_Locker );
    OnClose = ( callback ) ? callback : OnClose;
}

void UDPSocketHT::RegisterCallback ( const EventRecv& callback )
{
    std::lock_guard<std::recursive_mutex> lock ( m_Locker );
    m_OnRecv = ( callback ) ? callback : m_OnRecv;
}

void UDPSocketHT::RegisterCallback ( const EventSend& callback )
{
    std::lock_guard<std::recursive_mutex> lock ( m_Locker );
    OnSend = ( callback ) ? callback : OnSend;
}

std::string UDPSocketHT::GetId ( ) const
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

void UDPSocketHT::UnregisterCallbacks ( )
{
    std::lock_guard<std::recursive_mutex> lock ( m_Locker );
    OnClose = nullptr;
    m_OnRecv = nullptr;
    OnSend = nullptr;
}

void UDPSocketHT::RegisterOnCompletionPort ( CompletionPort& port )
{
    std::lock_guard<std::recursive_mutex> lock ( m_Locker );
    if ( IsValid ( ) && !m_pCompletionKey )
    {
        m_pCompletionKey = new std::shared_ptr<UDPSocketHT> ( shared_from_this ( ) );
        auto result = port.RegisterHandle ( ( HANDLE ) m_hSocket , ( ULONG_PTR ) m_pCompletionKey );
        if ( !result )
        {
            delete m_pCompletionKey;
            m_pCompletionKey = nullptr;
            SYSTEM_ERROR ( "Can't register socket handle on completion port" );
        }
    }
}

void UDPSocketHT::HandleEvents ( OverlappedEx* ev , DWORD bytesTransferred , DWORD statusCode )
{
    switch ( ev->Operation )
    {
        case OverlappedEx::RecvFrom:
            EventHandlerRecvFrom ( ev , bytesTransferred , statusCode );
            break;
        case OverlappedEx::SendTo:
            EventHandlerSendTo ( ev , bytesTransferred , statusCode );
            break;
        case OverlappedEx::Req_SendTo:
            EventHandlerRequestSendTo ( ev , bytesTransferred , statusCode );
            break;
        case OverlappedEx::Req_RecvFrom:
            EventHandlerRequestRecvFrom ( ev , bytesTransferred , statusCode );
            break;
        default:
            EventHandlerClose ( ev , bytesTransferred , statusCode );
            break;
    }
}

void UDPSocketHT::EventHandlerRecvFrom ( OverlappedEx* ev , DWORD bytesTransferred , DWORD statusCode )
{
    m_nBtRcv += bytesTransferred;
    auto fastrecv = [ & ] ( OverlappedEx* ev_struct )->void
    {
        ev_struct->Operation = OverlappedEx::RecvFrom;
        //ev_struct->SequenceNum = socket->m_nRcvSeqPosted++;
        WSABUF buf;
        buf.buf = ev_struct->Buffer;
        buf.len = ev_struct->Length;
        auto result = RecvFrom ( ev_struct , &buf , 1 );
    };
    m_nBtRcv += bytesTransferred;
    if ( m_OnRecv )
    {
        m_OnRecv ( *this , &ev->NetAddr , ev->Buffer , bytesTransferred );
    }
    ev->reset ( );
    m_nPendingOperations--;
    if ( !m_nSimultaneousRecvCalls || ( m_nPendingOperations + 1 <= m_nSimultaneousRecvCalls ) )
    {
        fastrecv ( ev );
    }
    else
    {
        ThreadPool::Instance ( ).Free ( ev );
    }
}

void UDPSocketHT::EventHandlerSendTo ( OverlappedEx* ev , DWORD bytesTransferred , DWORD statusCode )
{
    //std::lock_guard<std::recursive_mutex> lock ( socket->m_Locker );
    m_nBtSnt += bytesTransferred;
    if ( OnSend )
    {
        OnSend ( *this , &ev->NetAddr , bytesTransferred );
    }
    ThreadPool::Instance ( ).Free ( ev );
    m_nPendingOperations--;
}

void UDPSocketHT::EventHandlerRequestRecvFrom ( OverlappedEx* ev , DWORD bytesTransferred , DWORD statusCode )
{
    if ( !m_nSimultaneousRecvCalls || ( m_nPendingOperations + 1 <= m_nSimultaneousRecvCalls ) )
    {
        ev->Operation = OverlappedEx::RecvFrom;
        //ev->SequenceNum = socket->m_nRcvSeqPosted++;
        WSABUF buf;
        buf.buf = ev->Buffer;
        buf.len = ev->Length;
        auto result = RecvFrom ( ev , &buf , 1 );
    }
    else
    {
        ThreadPool::Instance ( ).Free ( ev );
    }
}

void UDPSocketHT::EventHandlerRequestSendTo ( OverlappedEx* ev , DWORD bytesTransferred , DWORD statusCode )
{
    ev->Operation = OverlappedEx::SendTo;
    WSABUF buf;
    buf.buf = ev->BufferHolder.buf;
    buf.len = ev->BufferHolder.len;
    auto result = SendTo ( ev , &buf , 1 );
}
void UDPSocketHT::Close ( )
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
void UDPSocketHT::EventHandlerClose ( OverlappedEx* ev , DWORD bytesTransferred , DWORD statusCode )
{
    std::unique_lock<std::recursive_mutex> lock ( m_Locker );
    if ( IsValid ( ) )
    {
        Close ( );
    }
    if ( OnClose )
    {
        OnClose ( *this , statusCode ); //one-shot only
        UnregisterCallbacks ( );
    }
    m_nPendingOperations--;
    //if ( !socket->m_RrdQueue.empty ( ) )
    //{
    //    for ( auto&i : socket->m_RrdQueue )
    //    { 
    //        auto ev_t = i.second;
    //        ev_t->reset ( );
    //        socket->m_RrdQueue.erase ( i.first );
    //        socket->m_nPendingOperations--;
    //        ThreadPool::Instance ( ).Free ( ev_t );
    //    }
    //}
    if ( m_nPendingOperations == 0 )
    {
        auto ptr = this->m_pCompletionKey;
        m_pCompletionKey = nullptr;
        lock.unlock ( );
        delete ptr;
    }
    ThreadPool::Instance ( ).Free ( ev );
}

UDPSocketHT::UDPSocketHT ( ) :
    //m_nRcvSeqCompleted ( 0 ) ,
    //m_nRcvSeqPosted ( 1 ) ,
    m_pCompletionKey ( nullptr ) ,
    NetworkSocket ( UDP )
{
}

std::shared_ptr<TCPSocketSQ> TCPSocketSQ::Create ( )
{
    return std::make_shared<TCPSocketSQ> ( );
}

void TCPSocketSQ::Listen ( )
{

}

void TCPSocketSQ::RequestAccept ( CompletionPort& , OverlappedEx* ev /*= nullptr */ )
{

}

void TCPSocketSQ::RequestConnect ( CompletionPort& , NetworkAddress& , OverlappedEx* ev /*= nullptr */ )
{

}

void TCPSocketSQ::RequestDisconnect ( CompletionPort& , OverlappedEx* ev /*= nullptr */ )
{

}

void TCPSocketSQ::RequestRecv ( CompletionPort& , OverlappedEx* ev /*= nullptr */ )
{

}

void TCPSocketSQ::RequestSend ( CompletionPort& , char* buffer , DWORD len , OverlappedEx* ev /*= nullptr */ )
{

}

std::string TCPSocketSQ::GetId ( ) const
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

void TCPSocketSQ::Close ( )
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

void TCPSocketSQ::RegisterCallback ( const EventAccept& cb )
{
    std::lock_guard<std::recursive_mutex> lock ( m_Locker );
    OnAccept = ( cb ) ? cb : OnAccept;
}

void TCPSocketSQ::RegisterCallback ( const EventConnect& cb )
{
    std::lock_guard<std::recursive_mutex> lock ( m_Locker );
    OnConnect = ( cb ) ? cb : OnConnect;
}

void TCPSocketSQ::RegisterCallback ( const EventDisconnect& cb )
{
    std::lock_guard<std::recursive_mutex> lock ( m_Locker );
    OnDisconnect = ( cb ) ? cb : OnDisconnect;
}

void TCPSocketSQ::RegisterCallback ( const EventRecv& cb )
{
    std::lock_guard<std::recursive_mutex> lock ( m_Locker );
    OnRecv = ( cb ) ? cb : OnRecv;
}

void TCPSocketSQ::RegisterCallback ( const EventSend& cb )
{
    std::lock_guard<std::recursive_mutex> lock ( m_Locker );
    OnSend = ( cb ) ? cb : OnSend;
}

void TCPSocketSQ::InheritCallbacks ( NetworkSocket* )
{

}

void TCPSocketSQ::RegisterOnCompletionPort ( CompletionPort& )
{

}

void TCPSocketSQ::HandleEvents ( OverlappedEx* , DWORD bytesTransferred , DWORD statusCode )
{

}

STATUS TCPSocketSQ::Accept ( OverlappedEx* ev )
{

}

STATUS TCPSocketSQ::Connect ( )
{

}

STATUS TCPSocketSQ::Disconnect ( )
{

}

STATUS TCPSocketSQ::Recv ( OverlappedEx* ev , LPWSABUF buf , DWORD count )
{

}

STATUS TCPSocketSQ::Send ( OverlappedEx* ev , LPWSABUF buf , DWORD count )
{

}

void TCPSocketSQ::EventHandlerAccept ( OverlappedEx* ev , DWORD bytesTransferred , DWORD statusCode )
{
    std::lock_guard<std::recursive_mutex> lock ( m_Locker );

}

void TCPSocketSQ::EventHandlerConnect ( OverlappedEx* ev , DWORD bytesTransferred , DWORD statusCode )
{
    std::lock_guard<std::recursive_mutex> lock ( m_Locker );
}

void TCPSocketSQ::EventHandlerDisconnect ( OverlappedEx* ev , DWORD bytesTransferred , DWORD statusCode )
{
    std::lock_guard<std::recursive_mutex> lock ( m_Locker );
}

void TCPSocketSQ::EventHandlerRecv ( OverlappedEx* ev , DWORD bytesTransferred , DWORD statusCode )
{
    std::lock_guard<std::recursive_mutex> lock ( m_Locker );
    auto fastrecv = [ & ] ( OverlappedEx* ev_struct )->void
    {
        ev_struct->Operation = OverlappedEx::RecvFrom;
        ev_struct->SequenceNum = m_nRcvSeqPosted++;
        WSABUF buf;
        buf.buf = ev_struct->Buffer;
        buf.len = ev_struct->Length;
        Recv ( ev_struct , &buf , 1 );
    };
    ev->Flags = bytesTransferred;
    if ( ev->SequenceNum - m_nRcvSeqCompleted > 1 )
    {
        m_RcvQueue [ ev->SequenceNum ] = ev;
    }
    else if ( ev->SequenceNum - m_nRcvSeqCompleted == 1 )
    {
        if ( OnRecv )
        {
            OnRecv ( *this , ev->Buffer , bytesTransferred );
        }
        m_nBtRcv += bytesTransferred;
        m_nRcvSeqCompleted++;
        ev->reset ( );
        m_nPendingOperations--;

        fastrecv ( ev );
        //socket->RequestRecvFrom ( ThreadPool::Instance ( ) , ev );

        while ( !m_RcvQueue.empty ( ) )
        {
            auto it = m_RcvQueue.find ( m_nRcvSeqCompleted + 1 );
            if ( it != m_RcvQueue.end ( ) )
            {
                auto ev_t = it->second;
                if ( OnRecv )
                {
                    OnRecv ( *this , ev_t->Buffer , ev_t->Flags );
                }
                m_nRcvSeqCompleted++;
                m_RcvQueue.erase ( ev_t->SequenceNum );
                m_nBtRcv += ev_t->Flags;
                ev_t->reset ( );
                m_nPendingOperations--;

                fastrecv ( ev_t );
                //socket->RequestRecvFrom ( ThreadPool::Instance ( ) , ev_t );
            }
            else
            {
                break;
            }
        }
    }
}

void TCPSocketSQ::EventHandlerSend ( OverlappedEx* ev , DWORD bytesTransferred , DWORD statusCode )
{
    std::lock_guard<std::recursive_mutex> lock ( m_Locker );
}

void TCPSocketSQ::EventHandlerClose ( OverlappedEx* , DWORD , DWORD )
{
    std::lock_guard<std::recursive_mutex> lock ( m_Locker );
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

void NetworkSocket::RegisterCallback ( const EventIoctl& )
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
