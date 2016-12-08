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
        auto status = GetQueuedCompletionStatus ( &bytesTransferred , &completionKey , &pOverlapped , INFINITE );
        if ( !completionKey && !pOverlapped )
        {
            PostQueuedCompletionStatus ( NULL , NULL , NULL );
            break;
        }
        auto pOvlpEx = ( OverlappedEx * ) pOverlapped;
        if ( status.first )
        {
            switch ( pOvlpEx->Operation )
            {
                case OverlappedEx::AcceptEx:
                {
                    auto sock_t = reinterpret_cast< Socket* >( completionKey );
                    Socket::EventHandlerAccept( sock_t , pOvlpEx );
                } break;
                case OverlappedEx::ConnectEx:
                {
                    auto sock_t = reinterpret_cast< Socket* >( completionKey );
                    Socket::EventHandlerConnect ( sock_t , pOvlpEx );
                } break;
                case OverlappedEx::DisconnectEx:
                {
                    auto sock_t = reinterpret_cast< Socket* >( completionKey );
                    Socket::EventHandlerDisconnect ( sock_t , pOvlpEx );
                } break;
                case OverlappedEx::Ioctl:
                {
                } break;
                case OverlappedEx::LockEx:
                {
                } break;
                case OverlappedEx::Read:
                {
                } break;
                case OverlappedEx::Write:
                {
                } break;
                case OverlappedEx::Recv:
                {
                    auto sock_t = reinterpret_cast< Socket* >( completionKey );
                    Socket::EventHandlerRecv ( sock_t , pOvlpEx );
                } break;
                case OverlappedEx::RecvFrom:
                {
                    auto sock_t = reinterpret_cast< Socket* >( completionKey );
                    Socket::EventHandlerRecvFrom ( sock_t , pOvlpEx );
                } break;
                case OverlappedEx::Send:
                {
                    auto sock_t = reinterpret_cast< Socket* >( completionKey );
                    Socket::EventHandlerSend ( sock_t , pOvlpEx );
                } break;
                case OverlappedEx::SendTo:
                {
                    auto sock_t = reinterpret_cast< Socket* >( completionKey );
                    Socket::EventHandlerSendTo ( sock_t , pOvlpEx );
                } break;
                case OverlappedEx::SendFile:
                {
                    auto sock_t = reinterpret_cast< Socket* >( completionKey );
                    Socket::EventHandlerSendFile ( sock_t , pOvlpEx );
                } break;
                case OverlappedEx::ReadDirChanges:
                {
                    auto dir = reinterpret_cast< Directory* >( completionKey );
                    DWORD offset = 0;
                    while ( offset < bytesTransferred )
                    {
                        auto pNotify = reinterpret_cast< PFILE_NOTIFY_INFORMATION >( pOvlpEx->Buffer + offset );
                        offset += pNotify->NextEntryOffset;
                        Directory::Change change;
                        change.m_Filename.resize ( pNotify->FileNameLength / 2 );
                        wcsncpy ( const_cast< wchar_t* >( change.m_Filename.data ( ) ) , pNotify->FileName , pNotify->FileNameLength / 2 );
                        change.m_Action = pNotify->Action;
                        change.m_pDir = dir;
                        if ( dir->OnChange )
                        {
                            dir->OnChange ( change );
                        }
                        if ( pNotify->NextEntryOffset == 0 )
                        {
                            break;
                        }
                    }
                    pOvlpEx->reset ( );
                    pOvlpEx->Operation = OverlappedEx::ReadDirChanges;
                    pOvlpEx->Device = dir->Handle ( );
                    pOvlpEx->Flags = dir->Flags ( );
                    dir->CheckChanges ( pOvlpEx );
                } break;
                case OverlappedEx::Req_Ioctl:
                {
                } break;
                case OverlappedEx::Req_AcceptEx:
                {
                } break;
                case OverlappedEx::Req_ConnectEx:
                {
                } break;
                case OverlappedEx::Req_DisconnectEx:
                {
                } break;
                case OverlappedEx::Req_LockEx:
                {
                } break;
                case OverlappedEx::Req_Read:
                {
                } break;
                case OverlappedEx::Req_Write:
                {
                } break;
                case OverlappedEx::Req_ReadDirChanges:
                {
                    pOvlpEx->Operation = OverlappedEx::ReadDirChanges;
                    auto dir = reinterpret_cast< Directory* >( completionKey );
                    auto result = dir->CheckChanges ( pOvlpEx );
                    if ( !result.first )
                    {
                        pOvlpEx->reset ( );
                        Free ( pOvlpEx );
                    }
                } break;
                case OverlappedEx::Req_Recv:
                {
                } break;
                case OverlappedEx::Req_RecvFrom:
                {
                } break;
                case OverlappedEx::Req_Send:
                {
                } break;
                case OverlappedEx::Req_SendTo:
                {
                } break;
                case OverlappedEx::Req_SendFile:
                {
                } break;
            }
        }
        else
        {
            Free ( pOvlpEx );
        }
    }
}
ThreadPool& ThreadPool::Instance()
{
    static ThreadPool g_obj;
    return g_obj;
}
ThreadPool::ThreadPool ( )
{
    for ( size_t i = 0; i < std::thread::hardware_concurrency ( ); ++i )
    {
        m_pthreads.emplace_back ( std::make_unique<std::thread> ( &ThreadPool::Execute , this ) );
    }
}
ThreadPool::~ThreadPool ( )
{
    this->PostQueuedCompletionStatus ( NULL , NULL , NULL );
}
OverlappedEx* ThreadPool::Allocate ( )
{
    return m_ObjectPool.Alloc ( );
}
void ThreadPool::Free ( OverlappedEx* obj )
{
    m_ObjectPool.Free ( obj );
}