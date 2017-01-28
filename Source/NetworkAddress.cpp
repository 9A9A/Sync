#include "NetworkAddress.h"

void NetworkAddress::Build ( const sockaddr& addr )
{
    if ( addr.sa_family == AF_INET6 )
    {
        m_address.addr6 = *( sockaddr_in6* ) &addr;
    }
    else if ( addr.sa_family == AF_INET )
    {
        m_address.addr4 = *( sockaddr_in* ) &addr;
    }
    else
    {
        m_address.addr = addr;
    }
}

NetworkAddress::NetworkAddress ( )
{
    m_address.addr4.sin_family = AF_INET;
}

NetworkAddress::NetworkAddress ( const sockaddr& addr )
{
    Build ( addr );
}

NetworkAddress::NetworkAddress ( const sockaddr_in& addr )
{
    if ( addr.sin_family != AF_INET )
    {
        throw std::runtime_error ( "Incorrect sin_family" );
    }
    m_address.addr4 = addr;
}

NetworkAddress::NetworkAddress ( const sockaddr_in6& addr )
{
    if ( addr.sin6_family != AF_INET6 )
    {
        throw std::runtime_error ( "Incorrect sin6_family" );
    }
    m_address.addr6 = addr;
}

NetworkAddress::NetworkAddress ( const sockaddr_storage& addr )
{
    Build ( *( sockaddr* ) &addr );
}

NetworkAddress::NetworkAddress ( uint32_t ip , uint16_t port )
{
    sockaddr_in addr { };
    addr.sin_family = AF_INET;
    addr.sin_port = ::htons ( port );
    addr.sin_addr.s_addr = ::htonl ( ip );
    m_address.addr4 = addr;
}

uint16_t NetworkAddress::GetPort ( ) const
{
    return ntohs ( GetPort_NetByteOrder ( ) );
}

uint32_t NetworkAddress::GetPort_NetByteOrder ( ) const
{
    return ( m_address.addr.sa_family == AF_INET ) ? m_address.addr4.sin_port : m_address.addr6.sin6_port;
}

void NetworkAddress::Set_Port ( uint16_t port )
{
    if ( m_address.addr.sa_family == AF_INET )
    {
        m_address.addr4.sin_port = htons ( port );
    }
    else
    {
        m_address.addr6.sin6_port = htons ( port );
    }
}

const sockaddr* NetworkAddress::GetSockAddr ( ) const
{
    return &m_address.addr;
}

int NetworkAddress::GetSockAddrLen ( ) const
{
    return ( m_address.addr.sa_family == AF_INET ) ? sizeof ( m_address.addr4 ) : sizeof ( m_address.addr6 );
}

uint16_t NetworkAddress::GetIpLength ( ) const
{
    return ( m_address.addr.sa_family == AF_INET ) ? sizeof ( m_address.addr4.sin_addr.s_addr ) : sizeof ( m_address.addr6.sin6_addr.s6_addr );
}

bool NetworkAddress::IsEmpty ( ) const
{
    const static uint8_t zeroes [ 16 ] = { };
    bool result = false;
    if ( m_address.addr.sa_family == AF_INET )
    {
        result = !::memcmp ( &m_address.addr4.sin_addr , zeroes , sizeof ( m_address.addr4.sin_addr ) );
    }
    else if ( m_address.addr.sa_family == AF_INET6 )
    {
        result = !::memcmp ( &m_address.addr6.sin6_addr , zeroes , sizeof ( m_address.addr6.sin6_addr ) );
    }
    else
    {
        result = !::memcmp ( &m_address.addr.sa_data , zeroes , sizeof ( m_address.addr.sa_data ) );
    }
    return result;
}

bool NetworkAddress::IsSameIp ( NetworkAddress& rhs ) const
{
    bool result = false;
    if ( m_address.addr.sa_family == rhs.m_address.addr.sa_family )
    {
        if ( m_address.addr.sa_family == AF_INET )
        {
            result = !::memcmp ( &m_address.addr4.sin_addr , &rhs.m_address.addr4.sin_addr , sizeof ( m_address.addr4.sin_addr ) );
        }
        else if ( m_address.addr.sa_family == AF_INET6 )
        {
            result = !::memcmp ( &m_address.addr6.sin6_addr , &rhs.m_address.addr6.sin6_addr , sizeof ( m_address.addr6.sin6_addr ) );
        }
        else
        {
            result = !::memcmp ( &m_address.addr.sa_data , &rhs.m_address.addr.sa_data , sizeof ( m_address.addr.sa_data ) );
        }
    }
    return result;
}
bool NetworkAddress::IsSameIpAndPort ( NetworkAddress& ) const
{
    return true;
}

std::string NetworkAddress::ToString ( ) const
{
    if ( m_address.addr.sa_family == AF_INET )
    {
        return std::to_string ( ( int ) m_address.addr4.sin_addr.S_un.S_un_b.s_b1 ) + "." +
            std::to_string ( ( int ) m_address.addr4.sin_addr.S_un.S_un_b.s_b2 ) + "." +
            std::to_string ( ( int ) m_address.addr4.sin_addr.S_un.S_un_b.s_b3 ) + "." +
            std::to_string ( ( int ) m_address.addr4.sin_addr.S_un.S_un_b.s_b4 );
    }
    else
    {
        //to do 
        return std::string ( );
    }
}

bool NetworkAddress::GetLocalHost ( uint16_t InetFamily , NetworkAddress* pAddr )
{
    if ( InetFamily != AF_INET6 || InetFamily != AF_INET || !pAddr )
    {
        return false;
    }
    if ( InetFamily == AF_INET )
    {
        uint32_t ip = 0x7f000001;
        *pAddr = NetworkAddress ( ip , 0 );
    }
    else
    {
        sockaddr_in6 addr6 = { };
        uint8_t ip6 [ 16 ] = { };
        ip6 [ 15 ] = 1;
        addr6.sin6_family = AF_INET6;
        ::memcpy ( &addr6.sin6_addr , ip6 , 16 );
        *pAddr = NetworkAddress ( addr6 );
    }
    return true;
}
