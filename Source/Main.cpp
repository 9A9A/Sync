#define WIN32_LEAN_AND_MEAN
#include "ThreadPool.h"
#include "Socket.h"
#include <chrono>
#include <fstream>
#include <random>
int main ( int argc , char** argv )
{
    NetworkingInitialization::Instance ( );
    auto time_p = std::chrono::high_resolution_clock::now ( );
    try
    {
        {
            std::vector<char*> arrays;
            std::random_device rand_dev;
            std::uniform_int_distribution<int> distr ( 0 , 255 );
            for ( size_t i = 0; i < 8; i++ )
            {
                auto ptr = new char [ 64000 ];
                for ( size_t j = 0; j < 64000; j++ )
                {
                    ptr [ j ] = distr ( rand_dev );
                }
                arrays.push_back ( ptr );
            }
            std::fstream log_file ( "output.txt" , std::ios::out );
            Socket<UDPSocketHT> sock;    
            std::recursive_mutex output_lock;
            sock ( )->Bind ( 27489 );
            std::cout << sock ( )->GetId ( ) << std::endl;
            sock ( )->RegisterOnCompletionPort ( ThreadPool::Instance ( ) );
            char* data = new char [ 65536 ];
            std::atomic<size_t> calls = 0;
            sock ( )->RegisterCallback ( [ & ] ( UDPSocketHT& sock , NetworkAddress* addr , char* buf , size_t sz )
            {
                auto time_r = std::chrono::high_resolution_clock::now ( );
                //std::lock_guard<std::recursive_mutex> _lock ( output_lock );
                std::cout << "Received from " << addr->ToString ( ) << ":" << addr->GetPort ( ) << " bytes " << sz << " on " << sock.GetId ( ) << "\t" << std::chrono::duration_cast< std::chrono::milliseconds >( time_r - time_p ).count ( ) << " msec\n";
                //log_file << std::chrono::duration_cast< std::chrono::milliseconds >( time_r - time_p ).count ( ) << std::endl;
                time_p = time_r;
                for ( size_t i = 0; i < sz; i++ )
                {
                    std::cout << buf [ i ];
                }
                calls++;
                std::cout << "\tCalls : " << sock.GetSimultaneousRecvCalls ( ) << "\tPending ops : " << sock.PendingOperations ( ) << "\tPool size : " << ThreadPool::Instance ( ).Size ( ) << std::endl;
                if ( calls == 8 )
                {
                    sock.SetSimultaneousRecvCalls ( 2 );
                }
                if ( calls == 25 )
                {
                    sock.SetSimultaneousRecvCalls ( 0 );
                }
                if ( calls == 60 )
                {
                    sock.SetSimultaneousRecvCalls ( 1 );
                }
                sock.RequestRecvFrom ( ThreadPool::Instance ( ) );
                //memcpy ( data , buf , sz );
                //sock.RequestSendTo ( ThreadPool::Instance ( ) , buf , sz , *addr );
                std::cout << std::endl;
            } );
            sock ( )->RegisterCallback ( [ & ] ( UDPSocketHT& sock , NetworkAddress* addr , size_t sz )
            {
                printf ( "Send" );
                //std::cout << "Send to " << addr->ToString ( ) << " bytes " << sz << std::endl;
            } );
            sock ( )->RegisterCallback ( [ & ] ( UDPSocketHT& sock , DWORD err )
            {
                std::cout << "On sock occurred error : " << err << std::endl;
            } );
            for ( size_t i = 0; i < 8; i++ )
            {
                sock ( )->RequestRecvFrom ( ThreadPool::Instance ( ) );
            }
            //int counter = 0;
            //while ( true )
            //{
            //    auto num = std::to_string ( counter++ );
            //    NetworkAddress addr ( 0xC0A80126, 27489 );
            //    sock ( )->RequestSendTo ( ThreadPool::Instance ( ) , (char*)num.data ( ) , num.size ( ) , addr );
            //    Sleep ( 100 );
            //}
            std::string x;
            std::getline ( std::cin , x );
            if ( x == "close" )
            {

            }
        }
        system ( "pause" );
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
    ThreadPool::Instance ( ).Shutdown ( );
}