#include "Socket.h"

Socket::Socket ( SockType )
{

}

Socket::Socket ( SockType , SOCKET )
{

}

Socket::~Socket ( )
{

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

void Socket::Close ( )
{

}

void Socket::Bind ( )
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

void Socket::RegisterOnConnect ( const EventConnect& )
{

}

void Socket::RegisterOnDisconnect ( const EventDisconnect& )
{

}

void Socket::RegisterOnIoctl ( const EventIoctl& )
{

}

void Socket::RegisterOnRecv ( const EventRecv& )
{

}

void Socket::RegisterOnRecvFrom ( const EventRecvFrom& )
{

}

void Socket::RegisterOnSend ( const EventSend& )
{

}

void Socket::RegisterOnSendTo ( const EventSendTo& )
{

}

void Socket::UnregisterAll ( )
{

}
