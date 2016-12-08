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
                    auto result = ::ReadDirectoryChangesW ( pOvlpEx->Device ,
                                                            pOvlpEx->Buffer ,
                                                            pOvlpEx->Length ,
                                                            TRUE ,
                                                            pOvlpEx->Flags ,
                                                            NULL ,
                                                            pOvlpEx ,
                                                            NULL );
                    if ( !result )
                    {
                        pOvlpEx->reset ( );
                        Free ( pOvlpEx );
                        //Free ( pOvlpEx );
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
                        ULONG len = min ( pNotify->FileNameLength / 2 , MAX_PATH - 1 );
                        wcsncpy ( change.m_Filename , pNotify->FileName , len );
                        change.m_Filename [ len ] = L'\0';
                        change.m_Action = pNotify->Action;
                        if ( dir->OnChange )
                        {
                            dir->OnChange ( change );
                        }
                        if ( pNotify->NextEntryOffset == 0 )
                        {
                            break;
                        }
                    }
                    dir->MakeRequest ( pOvlpEx );
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