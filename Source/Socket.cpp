#include "Socket.h"

LPFN_ACCEPTEX Socket::_AcceptEx = nullptr;
LPFN_CONNECTEX Socket::_ConnectEx = nullptr;
LPFN_DISCONNECTEX Socket::_DisconnectEx = nullptr;
LPFN_GETACCEPTEXSOCKADDRS Socket::_GetAcceptExSockaddrs = nullptr;
LPFN_TRANSMITFILE Socket::_TransmitFile = nullptr;
LPFN_TRANSMITPACKETS Socket::_TransmitPackets = nullptr;

Socket::Socket ( SockType type , IPType ip ) :
   m_nBytesSent ( 0 ) ,
   m_nBytesRecv ( 0 ) ,
   m_nPostedSeqNum ( 0 ) ,
   m_nCompletedSeqNum ( 0 ) ,
   m_SockType ( type ) ,
   m_IPType ( ip )
{
   NetworkingInitialization::Instance ( );
   m_hSocket = InitializeSocket ( m_SockType , m_IPType );
   if ( m_hSocket == INVALID_SOCKET )
   {
      throw std::runtime_error ( "Can't create socket : " + std::to_string ( ::GetLastError ( ) ) );
   }
}

Socket::Socket ( SockType type , SOCKET sock , IPType ip ) :
   m_nBytesSent ( 0 ) ,
   m_nBytesRecv ( 0 ) ,
   m_nPostedSeqNum ( 0 ) ,
   m_nCompletedSeqNum ( 0 ) ,
   m_hSocket ( sock ) ,
   m_SockType ( type ) ,
   m_IPType ( ip )
{
   if ( m_hSocket == INVALID_SOCKET )
   {
      throw std::runtime_error ( "Can't create socket" );
   }
}

Socket::~Socket ( )
{
   Close ( );
}

SOCKET Socket::Handle ( ) const
{
   return m_hSocket;
}

Socket::SockType Socket::SocketType ( ) const
{
   return m_SockType;
}

Socket::IPType Socket::IPFamily ( ) const
{
   return m_IPType;
}

size_t Socket::BytesSent ( ) const
{
   return m_nBytesSent;
}

size_t Socket::BytesReceived ( ) const
{
   return m_nBytesRecv;
}

void Socket::RegisterOnCompletionPort ( CompletionPort* port )
{
   if ( port )
   {
      port->RegisterHandle ( ( HANDLE ) m_hSocket , ( ULONG_PTR )this );
   }
}

void Socket::Close ( )
{
   ::closesocket ( m_hSocket );
}

void Socket::Bind ( )
{


}

void Socket::Bind ( USHORT port )
{
   NetworkAddress addr;
   addr.SetPort ( port );
   //to here ipv6 and ipv4 correct binding
   //ADDRINFO hints;
   //::ZeroMemory ( &hints , sizeof ( ADDRINFO ) );
   //hints.ai_family = m_IPType;
   //hints.ai_socktype = m_SockType;

   //

}

void Socket::Listen ( const int clients )
{

}

void Socket::Accept ( OverlappedEx* ov )
{

}

void Socket::Connect ( OverlappedEx* ov , sockaddr* addr )
{

}

void Socket::Disconnect ( OverlappedEx* ov )
{

}

void Socket::Ioctl ( OverlappedEx* ov )
{

}

void Socket::RecvFrom ( OverlappedEx* ov , LPWSABUF bufs , DWORD count )
{

}

void Socket::Recv ( OverlappedEx* ov , LPWSABUF bufs , DWORD count )
{

}

void Socket::Send ( OverlappedEx* ov , LPWSABUF bufs , DWORD count )
{

}

void Socket::SendTo ( OverlappedEx* ov , LPWSABUF bufs , DWORD count )
{

}

void Socket::SendFile ( OverlappedEx* ov )
{

}

void Socket::RequestAccept ( OverlappedEx* ov /*= nullptr */ )
{

}

void Socket::RequestConnect ( sockaddr* addr , OverlappedEx* ov /*= nullptr */ )
{

}

void Socket::RequestDisconnect ( OverlappedEx* ov /*= nullptr */ )
{

}

void Socket::RequestIoctl ( OverlappedEx* ov /*= nullptr */ )
{

}

void Socket::RequestRecv ( OverlappedEx* ov /*= nullptr */ )
{

}

void Socket::RequestRecvFrom ( OverlappedEx* ov /*= nullptr */ )
{

}

void Socket::RequestSend ( OverlappedEx* ov /*= nullptr */ )
{

}

void Socket::RequestSendFile ( OverlappedEx* ov /*= nullptr */ )
{

}

void Socket::RequestSendTo ( OverlappedEx* ov /*= nullptr */ )
{

}

void Socket::RegisterOnAccept ( const EventAccept& cb )
{
   std::lock_guard<std::mutex> lock ( m_Locker );
   if ( cb )
   {
      OnAccept = cb;
   }
}

void Socket::RegisterOnConnect ( const EventConnect& cb )
{
   std::lock_guard<std::mutex> lock ( m_Locker );
   if ( cb )
   {
      OnConnect = cb;
   }
}

void Socket::RegisterOnDisconnect ( const EventDisconnect& cb )
{
   std::lock_guard<std::mutex> lock ( m_Locker );
   if ( cb )
   {
      OnDisconnect = cb;
   }
}

void Socket::RegisterOnIoctl ( const EventIoctl& cb )
{
   std::lock_guard<std::mutex> lock ( m_Locker );
   if ( cb )
   {
      OnIoctl = cb;
   }
}

void Socket::RegisterOnRecv ( const EventRecv& cb )
{
   std::lock_guard<std::mutex> lock ( m_Locker );
   if ( cb )
   {
      OnRecv = cb;
   }
}

void Socket::RegisterOnRecvFrom ( const EventRecvFrom& cb )
{
   std::lock_guard<std::mutex> lock ( m_Locker );
   if ( cb )
   {
      OnRecvFrom = cb;
   }
}

void Socket::RegisterOnSend ( const EventSend& cb )
{
   std::lock_guard<std::mutex> lock ( m_Locker );
   if ( cb )
   {
      OnSend = cb;
   }
}

void Socket::RegisterOnSendTo ( const EventSendTo& cb )
{
   std::lock_guard<std::mutex> lock ( m_Locker );
   if ( cb )
   {
      OnSendTo = cb;
   }
}

void Socket::InheritCallbacks ( Socket* sock )
{
   std::lock_guard<std::mutex> lock ( m_Locker );
   if ( sock )
   {
      OnAccept = sock->OnAccept;
      OnConnect = sock->OnConnect;
      OnDisconnect = sock->OnDisconnect;
      OnError = sock->OnError;
      OnIoctl = sock->OnIoctl;
      OnRecv = sock->OnRecv;
      OnRecvFrom = sock->OnRecvFrom;
      OnSend = sock->OnSend;
      OnSendTo = sock->OnSendTo;
   }
}

void Socket::UnregisterAll ( )
{
   std::lock_guard<std::mutex> lock ( m_Locker );
   OnAccept = nullptr;
   OnConnect = nullptr;
   OnDisconnect = nullptr;
   OnError = nullptr;
   OnIoctl = nullptr;
   OnRecv = nullptr;
   OnRecvFrom = nullptr;
   OnSend = nullptr;
   OnSendTo = nullptr;
}

SOCKET Socket::InitializeSocket ( SockType type , IPType ip )
{
   SOCKET sock_t = INVALID_SOCKET;
   switch ( type )
   {
   case TCP:
      sock_t = ::WSASocketW ( ip , type , IPPROTO_TCP , NULL , NULL , WSA_FLAG_OVERLAPPED );
      break;
   case UDP:
      sock_t = ::WSASocketW ( ip , type , IPPROTO_UDP , NULL , NULL , WSA_FLAG_OVERLAPPED );
      break;
   }
   return sock_t;
}

void Socket::EventHandlerAccept ( Socket* , OverlappedEx* , DWORD bytes )
{

}

void Socket::EventHandlerConnect ( Socket* , OverlappedEx* , DWORD bytes )
{

}

void Socket::EventHandlerDisconnect ( Socket* , OverlappedEx* , DWORD bytes )
{

}

void Socket::EventHandlerIoctl ( Socket* , OverlappedEx* , DWORD bytes )
{

}

void Socket::EventHandlerRecv ( Socket* , OverlappedEx* , DWORD bytes )
{

}

void Socket::EventHandlerRecvFrom ( Socket* , OverlappedEx* , DWORD bytes )
{

}

void Socket::EventHandlerSend ( Socket* , OverlappedEx* , DWORD bytes )
{

}

void Socket::EventHandlerSendFile ( Socket* , OverlappedEx* , DWORD bytes )
{

}

void Socket::EventHandlerSendTo ( Socket* , OverlappedEx* , DWORD bytes )
{

}
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
      throw std::runtime_error ( "Failed initialize networking system : " + std::to_string ( ::GetLastError ( ) ) );
      ::WSACleanup ( );
   }
   auto socket_t = Socket::InitializeSocket ( Socket::TCP );
   DWORD bytes_t;
   if ( ::WSAIoctl ( socket_t , SIO_GET_EXTENSION_FUNCTION_POINTER , &m_AcceptEx , sizeof ( m_AcceptEx ) , &Socket::_AcceptEx , sizeof ( Socket::_AcceptEx ) , &bytes_t , NULL , NULL ) == SOCKET_ERROR ||
        ::WSAIoctl ( socket_t , SIO_GET_EXTENSION_FUNCTION_POINTER , &m_ConnectEx , sizeof ( m_ConnectEx ) , &Socket::_ConnectEx , sizeof ( Socket::_ConnectEx ) , &bytes_t , NULL , NULL ) == SOCKET_ERROR ||
        ::WSAIoctl ( socket_t , SIO_GET_EXTENSION_FUNCTION_POINTER , &m_DisconnectEx , sizeof ( m_DisconnectEx ) , &Socket::_DisconnectEx , sizeof ( Socket::_DisconnectEx ) , &bytes_t , NULL , NULL ) == SOCKET_ERROR ||
        ::WSAIoctl ( socket_t , SIO_GET_EXTENSION_FUNCTION_POINTER , &m_GetAcceptExSockaddrs , sizeof ( m_GetAcceptExSockaddrs ) , &Socket::_GetAcceptExSockaddrs , sizeof ( Socket::_GetAcceptExSockaddrs ) , &bytes_t , NULL , NULL ) == SOCKET_ERROR ||
        ::WSAIoctl ( socket_t , SIO_GET_EXTENSION_FUNCTION_POINTER , &m_TransmitFile , sizeof ( m_TransmitFile ) , &Socket::_TransmitFile , sizeof ( Socket::_TransmitFile ) , &bytes_t , NULL , NULL ) == SOCKET_ERROR ||
        ::WSAIoctl ( socket_t , SIO_GET_EXTENSION_FUNCTION_POINTER , &m_TransmitPackets , sizeof ( m_TransmitPackets ) , &Socket::_TransmitPackets , sizeof ( Socket::_TransmitPackets ) , &bytes_t , NULL , NULL ) == SOCKET_ERROR )
   {
      ::closesocket ( socket_t );
      ::WSACleanup ( );
      throw std::runtime_error ( "Failed obtain winsock function pointers : " + std::to_string ( ::GetLastError ( ) ) );
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
