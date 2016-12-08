#include "Directory.h"
#include "ThreadPool.h"

Directory::Directory ( LPCWSTR filename , DWORD Flags ) : m_Flags ( Flags )
{
    if ( ( m_hDirectory = ::CreateFileW ( filename ,
         FILE_LIST_DIRECTORY ,
         FILE_SHARE_READ |
         FILE_SHARE_DELETE |
         FILE_SHARE_WRITE ,
         NULL ,
         OPEN_EXISTING ,
         FILE_FLAG_BACKUP_SEMANTICS |
         FILE_FLAG_OVERLAPPED ,
         NULL ) ) == INVALID_HANDLE_VALUE )
    {
        throw std::runtime_error ( "Can't open directory : " + std::to_string ( ::GetLastError ( ) ) );
    }
    ThreadPool::Instance ( ).RegisterHandle ( m_hDirectory , ( ULONG_PTR )this );
}

Directory::~Directory ( )
{
    ::CloseHandle ( m_hDirectory );
}

void Directory::MakeRequest ( OverlappedEx* ev )
{
    ev = ( !ev ) ? ThreadPool::Instance ( ).Allocate ( ) : ev;
    ev->reset ( );
    ev->Operation = OverlappedEx::Req_ReadDirChanges;
    ev->Device = m_hDirectory;
    ev->Flags = m_Flags;
    LPOVERLAPPED ovlp = ( LPOVERLAPPED ) ev;
    ThreadPool::Instance ( ).PostQueuedCompletionStatus ( NULL , ( ULONG_PTR )this , ovlp );
}