#define WIN32_LEAN_AND_MEAN
#include "ThreadPool.h"
#include "Overlapped.h"
#include "Directory.h"
#include "ObjectPool.h"
#include "Socket.h"
void ThreadPool::Execute ( )
{
    while ( true )
    {
        try
        {
            DWORD bytesTransferred = 0;
            LPOVERLAPPED pOverlapped = nullptr;
            ULONG_PTR completionKey = 0;
            auto status = GetQueuedCompletionStatus ( &bytesTransferred , &completionKey , &pOverlapped , INFINITE );
            if ( !completionKey && !pOverlapped )
            {
                PostQueuedCompletionStatus ( NULL , NULL , NULL ); // unblock all threads
                break;
            }
            auto pOvlpEx = ( OverlappedEx * ) pOverlapped;
            if ( pOvlpEx )
            {
                if(!status.first )
                {
                    switch ( pOvlpEx->Operation )
                    {
                        case OverlappedEx::Req_SendTo:
                        case OverlappedEx::Req_RecvFrom:
                        case OverlappedEx::SendTo:
                        case OverlappedEx::RecvFrom:
                        {
                            pOvlpEx->Operation = OverlappedEx::UndefinedOperation;
                        }
                        break;
                        case OverlappedEx::Req_Ioctl:
                        {

                        }
                        break;
                        case OverlappedEx::Req_Send:
                        case OverlappedEx::Req_AcceptEx:
                        case OverlappedEx::Req_ConnectEx:
                        case OverlappedEx::Req_DisconnectEx:
                        case OverlappedEx::Req_Recv:
                        case OverlappedEx::Req_SendFile:
                        case OverlappedEx::AcceptEx:
                        case OverlappedEx::ConnectEx:
                        case OverlappedEx::DisconnectEx:
                        case OverlappedEx::Send:
                        case OverlappedEx::Recv:
                        case OverlappedEx::SendFile:
                        {
                            ( *reinterpret_cast< std::shared_ptr<Async>* >( completionKey ) )->HandleEvents ( pOvlpEx , bytesTransferred , status.second );
                        }
                        default:
                            pOvlpEx->Operation = OverlappedEx::UndefinedOperation;
                            break;
                    }
                }
                ( *reinterpret_cast< std::shared_ptr<Async>* >( completionKey ) )->HandleEvents ( pOvlpEx , bytesTransferred , status.second );
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
    std::cout << "Thread exit\n";
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
    Shutdown ( );
    Wait ( );
}
OverlappedEx* ThreadPool::Allocate ( )
{
   return m_ObjectPool.Alloc ( );
}

void ThreadPool::Wait ( )
{
    for ( auto&i : m_pthreads )
    {
        if ( i->joinable ( ) )
        {
            i->join ( );
        }
    }
}

size_t ThreadPool::Size ( ) const
{
    return m_ObjectPool.Size ( );
}

void ThreadPool::Shutdown ( )
{
    PostQueuedCompletionStatus ( NULL , NULL , NULL );
}

void ThreadPool::Free ( OverlappedEx* obj )
{
   m_ObjectPool.Free ( obj );
}