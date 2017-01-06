#define WIN32_LEAN_AND_MEAN
#include "ThreadPool.h"
#include "Socket.h"
int main ( int argc , char** argv )
{
    NetworkingInitialization::Instance ( );
    try
    {
        Socket<UDPASocket> sock;
        sock ( )->Bind ( 27489 );
        std::cout << sock ( )->GetId ( ) << std::endl;
        sock ( )->RegisterOnCompletionPort ( ThreadPool::Instance ( ) );

        sock ( )->RegisterOnRecv ( [ ] ( UDPASocket& sock , NetworkAddress* addr , char* buf , size_t sz )
        {
            std::cout << "Received from " << addr->ToString ( ) << ":" << addr->GetPort ( ) << " bytes " << sz << " on " << sock.GetId ( ) << "\n";
            for ( size_t i = 0; i < sz; i++ )
            {
                std::cout << buf [ i ];
            }
            std::cout << std::endl;
        } );
        sock ( )->RegisterOnSend ( [ ] ( UDPASocket& , NetworkAddress* addr , size_t sz )
        {
            std::cout << "Send to " << addr->ToString ( ) << " bytes " << sz << std::endl;
        } );
        sock ( )->RegisterOnClose ( [ ] ( UDPASocket& sock , DWORD err )
        {
            std::cout << "On sock occurred error : " << err << std::endl;
        } );
        for ( size_t i = 0; i < 5; i++ )
        {
            sock ( )->RequestRecvFrom ( ThreadPool::Instance ( ) );
        }
        ::Sleep ( INFINITE );
    }
    catch ( std::system_error& err )
    {
        std::cout << err.what ( ) << " System Error Code : " << err.code ( ) << std::endl;
    }
    catch ( std::runtime_error& err )
    {
        std::cout << err.what ( ) << std::endl;
    }
    catch ( ... )
    {
        std::cout << "Unknown error caught\n";
    }
    system ( "pause" );
}