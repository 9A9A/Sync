#ifndef _NETWORK_ADDRESS_H_
#define _NETWORK_ADDRESS_H_
#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <Ws2ipdef.h>
#include <Windows.h>
#include <iostream>
union net_sockaddr
{
   sockaddr addr;
   sockaddr_in addr4;
   sockaddr_in6 addr6;
};
class NetworkAddress
{
   net_sockaddr m_address;
   void Build ( const sockaddr& );
public:
   NetworkAddress ( );
   NetworkAddress ( const sockaddr& addr );
   NetworkAddress ( const sockaddr_in& addr );
   NetworkAddress ( const sockaddr_in6& addr );
   NetworkAddress ( const sockaddr_storage& addr );
   NetworkAddress ( uint32_t ip , uint16_t port );

   uint16_t GetPort ( )const;
   uint32_t GetPort_NetByteOrder ( )const;
   void Set_Port ( uint16_t );
   const sockaddr* GetSockAddr ( ) const;
   int GetSockAddrLen ( )const;

   uint16_t GetIpLength ( ) const;

   bool IsEmpty ( ) const;
   bool IsSameIp ( NetworkAddress& )const;
   bool IsSameIpAndPort ( NetworkAddress& )const;
   std::string ToString ( )const;
   static bool GetLocalHost ( uint16_t InetFamily , NetworkAddress* );
};

#endif