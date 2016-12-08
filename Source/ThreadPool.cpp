#include "ThreadPool.h"
#include "Overlapped.h"
#include "Directory.h"
#include "ObjectPool.h"
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
                }
                break;
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
                    dir->RequestCheckChanges ( ( CompletionPort* )this , pOvlpEx );
                }
                break;
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