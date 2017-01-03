#define WIN32_LEAN_AND_MEAN
#include "ThreadPool.h"
#include "Overlapped.h"
#include "Directory.h"
#include "ObjectPool.h"
#include "Socket.h"
thread_local TLSObjectPool<OverlappedEx> ThreadPool::m_ObjectPool;
void ThreadPool::Execute ( )
{
   LPOVERLAPPED pOverlapped = nullptr;
   DWORD bytesTransferred = 0;
   ULONG_PTR completionKey;
   while ( true )
   {
       try
       {
           auto status = GetQueuedCompletionStatus ( &bytesTransferred , &completionKey , &pOverlapped , INFINITE );
           if ( !completionKey && !pOverlapped )
           {
               PostQueuedCompletionStatus ( NULL , NULL , NULL ); // unblock all threads
               break;
           }
           auto pOvlpEx = ( OverlappedEx * ) pOverlapped;
           if ( pOvlpEx )
           {
               if ( status.first )
               {
                   switch ( pOvlpEx->Operation )
                   {
                       case OverlappedEx::RecvFrom:
                       {
                           auto sock_t = reinterpret_cast< std::shared_ptr<UDPASocket>* >( completionKey );
                           UDPASocket::EventHandlerRecvFrom ( *sock_t , pOvlpEx , bytesTransferred , status.second );
                       }
                       break;
                       case OverlappedEx::SendTo:
                       {
                           auto sock_t = reinterpret_cast< std::shared_ptr<UDPASocket>* >( completionKey );
                           UDPASocket::EventHandlerSendTo ( *sock_t , pOvlpEx , bytesTransferred , status.second );
                       }
                       break;
                       case OverlappedEx::Req_SendTo:
                       {
                           auto sock_t = reinterpret_cast< std::shared_ptr<UDPASocket>* >( completionKey );
                           UDPASocket::EventHandlerRequestSendTo ( *sock_t , pOvlpEx , bytesTransferred , status.second );
                       }
                       break;
                       case OverlappedEx::Req_RecvFrom:
                       {
                           auto sock_t = reinterpret_cast< std::shared_ptr<UDPASocket>* >( completionKey );
                           UDPASocket::EventHandlerRequestRecvFrom ( *sock_t , pOvlpEx , bytesTransferred , status.second );
                       }
                       break;
                       default:
                           break;
                   }
               }
               else
               {
                   switch ( pOvlpEx->Operation )
                   {
                       case OverlappedEx::Req_SendTo:
                       case OverlappedEx::Req_RecvFrom:
                       case OverlappedEx::SendTo:
                       case OverlappedEx::RecvFrom:
                       {
                           auto sock_t = reinterpret_cast< std::shared_ptr<UDPASocket>* >( completionKey );
                           UDPASocket::EventHandlerClose ( *sock_t , pOvlpEx , bytesTransferred , status.second );
                       }
                       break;
                       default:
                           break;
                   }
               }
           }
           else
           {
               if ( status.first == 0 )
               {
                   
             }
           }
       }
       catch ( std::system_error& err )
       {
           std::cout << "Caught system error : " << err.what ( ) << " System Error Code : " << err.code ( ) << std::endl;
       }
       catch ( std::runtime_error& err )
       {
           std::cout << "Caught runtime error : " << err.what ( ) << std::endl;
       }
       catch ( ... )
       {
           std::cout << "Caught unknown error\n";
       }
   }
}
ThreadPool& ThreadPool::Instance ( )
{
   static ThreadPool g_obj;
   return g_obj;
}
ThreadPool::ThreadPool ( ) : CompletionPort ( )
{
    for ( size_t i = 0; i < std::thread::hardware_concurrency ( ); ++i )
    {
        m_pthreads.push_back ( std::make_unique<std::thread> ( &ThreadPool::Execute , this ) );
    }
}
ThreadPool::~ThreadPool ( )
{
   PostQueuedCompletionStatus ( NULL , NULL , NULL );
   for ( auto&i : m_pthreads )
   {
       if ( i->joinable ( ) )
       {
           i->join ( );
       }
   }
}
OverlappedEx* ThreadPool::Allocate ( )
{
   return m_ObjectPool.Alloc ( );
}
void ThreadPool::Free ( OverlappedEx* obj )
{
   m_ObjectPool.Free ( obj );
}