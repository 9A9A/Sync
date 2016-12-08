#include "Directory.h"
#include "ThreadPool.h"

Directory::Directory ( ) :
   m_hDirectory ( INVALID_HANDLE_VALUE )
{
}

Directory::Directory ( Directory&& rhs )
{
   Close ( );
   m_hDirectory = rhs.m_hDirectory;
   m_Flags = rhs.m_Flags;
   m_DirectoryName = std::move ( rhs.m_DirectoryName );
}

Directory::Directory ( const std::wstring& dirname , DWORD flags , MODE mode ) :
   m_Flags ( flags )
{
   std::pair<int , DWORD> res;
   switch ( mode )
   {
   case OPEN:
      res = Open ( dirname );
      if ( !res.first )
      {
         throw std::runtime_error ( "Can't open directory : " + std::to_string ( res.second ) );
      }
      break;
   case CREATE:
      res = Create ( dirname );
      if ( !res.first )
      {
         throw std::runtime_error ( "Can't create directory : " + std::to_string ( res.second ) );
      }
      break;
   default:
      throw std::runtime_error ( "Invalid mode type" );
   }
}

Directory::~Directory ( )
{
   Close ( );
}

void Directory::RegisterOnCompletionPort ( CompletionPort* port )
{
   if ( port && ( m_hDirectory != INVALID_HANDLE_VALUE ) )
   {
      port->RegisterHandle ( m_hDirectory , ( ULONG_PTR )this );
   }
}

void Directory::RequestCheckChanges ( CompletionPort* port , OverlappedEx* ev /*= nullptr */ )
{
   if ( port )
   {
      ev = ( !ev ) ? ThreadPool::Instance ( ).Allocate ( ) : ev;
      ev->reset ( );
      ev->Operation = OverlappedEx::Req_ReadDirChanges;
      ev->Device = m_hDirectory;
      ev->Flags = m_Flags;
      LPOVERLAPPED ovlp = ( LPOVERLAPPED ) ev;
      port->PostQueuedCompletionStatus ( NULL , ( ULONG_PTR )this , ovlp );
   }
}

std::pair<int, DWORD> Directory::CheckChanges ( OverlappedEx* ev , bool recursive )
{
   return std::pair<int , DWORD> ( ::ReadDirectoryChangesW ( m_hDirectory ,
                                                             ev->Buffer ,
                                                             ev->Length ,
                                                             recursive ,
                                                             ev->Flags ,
                                                             NULL ,
                                                             ev ,
                                                             NULL ) , ::GetLastError ( ) );
}

std::pair<int, DWORD> Directory::Create ( const std::wstring& dirname )
{
   Close ( );
   if ( ::PathIsDirectoryW ( dirname.c_str ( ) ) )
   {
      m_DirectoryName = dirname;
      std::pair<int , DWORD> ret ( ::CreateDirectoryW ( m_DirectoryName.c_str ( ) , NULL ) , ::GetLastError ( ) );
      if ( !ret.first )
      {
         ret = Open ( dirname );
      }
      return ret;
   }
   else
   {
      throw std::runtime_error ( "Can't create directory : path isn't directory" );
   }
}

bool Directory::IsSubdirectory ( Directory* subdir ) const
{
   return ( subdir && ( m_DirectoryName.size ( ) < subdir->m_DirectoryName.size ( ) ) && ( subdir->m_DirectoryName.find ( m_DirectoryName ) != std::wstring::npos ) ) ? true : false;
}

bool Directory::IsParentdirectory ( Directory* parentdir ) const
{
   return ( parentdir && ( m_DirectoryName.size ( ) > parentdir->m_DirectoryName.size ( ) ) && ( m_DirectoryName.find ( parentdir->m_DirectoryName ) != std::wstring::npos ) ) ? true : false;
}

HANDLE Directory::Handle ( ) const
{
   return m_hDirectory;
}

bool Directory::IsValid ( ) const
{
   return ( m_hDirectory != INVALID_HANDLE_VALUE ) ? true : false;
}

bool Directory::IsDirectoryEmpty ( ) const
{
   return ( ::PathIsDirectoryEmptyW ( m_DirectoryName.c_str ( ) ) ) ? true : false;
}

DWORD Directory::Flags ( ) const
{
   return m_Flags;
}

Directory* Directory::CreateSubdirectory ( const std::wstring& subdir ) const
{
   return nullptr;
}

bool Directory::Close ( )
{
   m_DirectoryName.clear ( );
   auto res = ::CloseHandle ( m_hDirectory );
   m_hDirectory = INVALID_HANDLE_VALUE;
   return ( res ) ? true : false;
}

bool Directory::CancelAllIO ( )
{
   return ( ::CancelIoEx ( m_hDirectory , NULL ) ) ? true : false;
}

std::pair<int , DWORD> Directory::DeleteDirectory ( )
{
   return std::pair<int , DWORD> ( ::RemoveDirectoryW ( m_DirectoryName.c_str ( ) ) , ::GetLastError ( ) );
}

std::wstring Directory::GetName ( ) const
{
   return m_DirectoryName;
}

std::pair<int,DWORD> Directory::Open ( const std::wstring& str )
{
   Close ( );
   if ( ::PathIsDirectoryW ( str.c_str ( ) ) )
   {
      m_DirectoryName = str;
      m_hDirectory = ::CreateFileW ( m_DirectoryName.c_str ( ) ,
                                     FILE_LIST_DIRECTORY ,
                                     FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE ,
                                     NULL ,
                                     OPEN_EXISTING ,
                                     FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED ,
                                     NULL );
      return std::pair<int , DWORD> ( ( m_hDirectory != INVALID_HANDLE_VALUE ) ? true : false , ::GetLastError ( ) );
   }
   else
   {
      throw std::runtime_error ( "Can't open : path isn't directory" );
   }
}
