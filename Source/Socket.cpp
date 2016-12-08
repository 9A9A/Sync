#include "Socket.h"

Socket::Socket ( SockType type ) :
    m_nBytesSent ( 0 ) ,
    m_nBytesRecv ( 0 ) ,
    m_nPostedSeqNum ( 0 ) ,
    m_nCompletedSeqNum ( 0 ) ,
    m_hSocket ( InitializeSocket ( type ) ) ,
    m_SockType ( type )
{
}

Socket::Socket ( SockType type , SOCKET sock ) :
    m_nBytesSent ( 0 ) ,
    m_nBytesRecv ( 0 ) ,
    m_nPostedSeqNum ( 0 ) ,
    m_nCompletedSeqNum ( 0 ) ,
    m_hSocket ( sock ) ,
    m_SockType ( type )
{
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

SOCKET Socket::InitializeSocket ( SockType type)
{
    SOCKET sock_t = INVALID_SOCKET;
    switch ( type )
    {
        case TCP:
            sock_t = ::WSASocket ( AF_INET , type , IPPROTO_TCP , NULL , NULL , WSA_FLAG_OVERLAPPED );
            break;
        case UDP:
            sock_t = ::WSASocket ( AF_INET , type , IPPROTO_UDP , NULL , NULL , WSA_FLAG_OVERLAPPED );
            break;
        default:
            break;
    }
    return sock_t;
}

void Socket::EventHandlerAccept ( Socket* , OverlappedEx* )
{

}

void Socket::EventHandlerConnect ( Socket* , OverlappedEx* )
{

}

void Socket::EventHandlerDisconnect ( Socket* , OverlappedEx* )
{

}

void Socket::EventHandlerIoctl ( Socket* , OverlappedEx* )
{

}

void Socket::EventHandlerRecv ( Socket* , OverlappedEx* )
{

}

void Socket::EventHandlerRecvFrom ( Socket* , OverlappedEx* )
{

}

void Socket::EventHandlerSend ( Socket* , OverlappedEx* )
{

}

void Socket::EventHandlerSendFile ( Socket* , OverlappedEx* )
{

}

void Socket::EventHandlerSendTo ( Socket* , OverlappedEx* )
{

}
