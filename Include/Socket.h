#ifndef _SOCKET_H_
#define _SOCKET_H_
#include <Windows.h>
#include <winsock2.h>
#include <MSWSock.h>
#include "Overlapped.h"
#include <functional>
#include <atomic>
#include <unordered_map>
#include <mutex>

class Socket
{
public:
   enum SockType : UCHAR
   {
      TCP = SOCK_STREAM ,
      UDP = SOCK_DGRAM ,
   };
   Socket ( SockType );
   Socket ( SockType , SOCKET );

   virtual ~Socket ( );

   SOCKET Handle ( )const;
   SockType SocketType ( )const;

   size_t BytesSent ( )const;
   size_t BytesReceived ( )const;

   void Close ( );
   void Bind ( );
   void Listen ( const int clients );
   void Accept ( OverlappedEx* ov );
   void Connect ( OverlappedEx* ov , sockaddr* addr );
   void Disconnect ( OverlappedEx* ov );
   void Ioctl ( OverlappedEx* ov );
   void RecvFrom ( OverlappedEx* ov , LPWSABUF bufs , DWORD count );
   void Recv ( OverlappedEx* ov , LPWSABUF bufs , DWORD count );
   void Send ( OverlappedEx* ov , LPWSABUF bufs , DWORD count );
   void SendTo ( OverlappedEx* ov , LPWSABUF bufs , DWORD count );
   void SendFile ( OverlappedEx* ov );


   void RequestAccept ( OverlappedEx* ov = nullptr );
   void RequestConnect ( sockaddr* addr , OverlappedEx* ov = nullptr );
   void RequestDisconnect ( OverlappedEx* ov = nullptr );
   void RequestIoctl ( OverlappedEx* ov = nullptr );
   void RequestRecv ( OverlappedEx* ov = nullptr );
   void RequestRecvFrom ( OverlappedEx* ov = nullptr );
   void RequestSend ( OverlappedEx* ov = nullptr );
   void RequestSendFile ( OverlappedEx* ov = nullptr );
   void RequestSendTo ( OverlappedEx* ov = nullptr );

   using EventAccept = std::function<void ( Socket* , sockaddr* )>;
   using EventConnect = std::function<void ( Socket* , sockaddr* )>;
   using EventDisconnect = std::function<void ( Socket* , sockaddr* )>;
   using EventError = std::function<void ( Socket* , DWORD )>;
   using EventIoctl = std::function<void ( Socket* )>;
   using EventRecv = std::function<void ( Socket* , LPWSABUF , DWORD )>;
   using EventRecvFrom = std::function<void ( Socket* , LPWSABUF , DWORD , sockaddr* )>;
   using EventSend = std::function<void ( Socket* , DWORD )>;
   using EventSendTo = std::function<void ( Socket* , DWORD , sockaddr* )>;

   void RegisterOnAccept ( const EventAccept& );
   void RegisterOnConnect ( const EventConnect& );
   void RegisterOnDisconnect ( const EventDisconnect& );
   void RegisterOnIoctl ( const EventIoctl& );
   void RegisterOnRecv ( const EventRecv& );
   void RegisterOnRecvFrom ( const EventRecvFrom& );
   void RegisterOnSend ( const EventSend& );
   void RegisterOnSendTo ( const EventSendTo& );
   void UnregisterAll ( );
private:
   EventAccept OnAccept;
   EventConnect OnConnect;
   EventDisconnect OnDisconnect;
   EventError OnError;
   EventIoctl OnIoctl;
   EventRecv OnRecv;
   EventRecvFrom OnRecvFrom;
   EventSend OnSend;
   EventSendTo OnSendTo; 


   mutable std::mutex m_Locker;
   std::atomic<size_t> m_nBytesSent;
   std::atomic<size_t> m_nBytesRecv;
   std::atomic<size_t> m_nPostedSeqNum;
   std::atomic<size_t> m_nCompletedSeqNum;
   std::unordered_map<size_t , OverlappedEx*> m_ReorderQueue;
   SOCKET m_hSocket;
   SockType m_SockType;
};

#endif