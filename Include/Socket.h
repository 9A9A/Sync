#ifndef _SOCKET_H_
#define _SOCKET_H_
#include <winsock2.h>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include <Windows.h>
#include "Overlapped.h"
#include "CompletionPort.h"
#include <functional>
#include <atomic>
#include <unordered_map>
#include <mutex>
#include "NetworkAddress.h"

class Socket
{
public:
   enum SockType : UCHAR
   {
      TCP = SOCK_STREAM ,
      UDP = SOCK_DGRAM ,
   };
   enum IPType : UCHAR
   {
      IPv4 = AF_INET ,
      IPv6 = AF_INET6 ,
   };
   Socket ( SockType , IPType = IPv4 );
   Socket ( SockType , SOCKET , IPType = IPv4 );

   virtual ~Socket ( );

   SOCKET Handle ( )const;
   SockType SocketType ( )const;
   IPType IPFamily ( )const;

   size_t BytesSent ( )const;
   size_t BytesReceived ( )const;

   void RegisterOnCompletionPort ( CompletionPort* );
   void Close ( );
   void Bind ( );
   void Bind ( USHORT port );
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

   using EventAccept = std::function<void ( Socket* , NetworkAddress* )>;
   using EventConnect = std::function<void ( Socket* , NetworkAddress* )>;
   using EventDisconnect = std::function<void ( Socket* , NetworkAddress* )>;
   using EventError = std::function<void ( Socket* , DWORD )>;
   using EventIoctl = std::function<void ( Socket* )>;
   using EventRecv = std::function<void ( Socket* , LPWSABUF , DWORD )>;
   using EventRecvFrom = std::function<void ( Socket* , LPWSABUF , DWORD , NetworkAddress* )>;
   using EventSend = std::function<void ( Socket* , DWORD )>;
   using EventSendTo = std::function<void ( Socket* , DWORD , NetworkAddress* )>;

   void RegisterOnAccept ( const EventAccept& );
   void RegisterOnConnect ( const EventConnect& );
   void RegisterOnDisconnect ( const EventDisconnect& );
   void RegisterOnIoctl ( const EventIoctl& );
   void RegisterOnRecv ( const EventRecv& );
   void RegisterOnRecvFrom ( const EventRecvFrom& );
   void RegisterOnSend ( const EventSend& );
   void RegisterOnSendTo ( const EventSendTo& );
   void InheritCallbacks ( Socket* );
   void UnregisterAll ( );

   static SOCKET InitializeSocket ( SockType , IPType = IPv4 );

   static void EventHandlerAccept ( Socket* , OverlappedEx* , DWORD  );
   static void EventHandlerConnect ( Socket* , OverlappedEx* , DWORD  );
   static void EventHandlerDisconnect ( Socket* , OverlappedEx* , DWORD  );
   static void EventHandlerIoctl ( Socket* , OverlappedEx* , DWORD  );
   static void EventHandlerRecv ( Socket* , OverlappedEx* , DWORD  );
   static void EventHandlerRecvFrom ( Socket* , OverlappedEx* , DWORD  );
   static void EventHandlerSend ( Socket* , OverlappedEx* , DWORD  );
   static void EventHandlerSendFile ( Socket* , OverlappedEx* , DWORD  );
   static void EventHandlerSendTo ( Socket* , OverlappedEx* , DWORD  );

   static LPFN_ACCEPTEX _AcceptEx;
   static LPFN_CONNECTEX _ConnectEx;
   static LPFN_DISCONNECTEX _DisconnectEx;
   static LPFN_GETACCEPTEXSOCKADDRS _GetAcceptExSockaddrs;
   static LPFN_TRANSMITFILE _TransmitFile;
   static LPFN_TRANSMITPACKETS _TransmitPackets;


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
   IPType m_IPType;
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
};

#endif