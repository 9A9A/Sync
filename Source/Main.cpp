
#include "ThreadPool.h"
#include "Directory.h"
#include <string>

int main ( )
{
   std::mutex locker;
   try
   {
      auto flags = Directory::FILE_NAME |
         Directory::DIR_NAME |
         Directory::ATTRIBUTES |
         Directory::SIZE |
         Directory::LAST_WRITE |
         Directory::LAST_ACCESS |
         Directory::CREATION |
         Directory::SECURITY;
      Directory dir ( L"\\\\?\\\C:\\" , flags , Directory::OPEN );
      dir.RegisterOnCompletionPort ( ( CompletionPort* ) &ThreadPool::Instance ( ) );
      auto output = [& ] ( Directory::Change& change ) ->void
      {
         std::lock_guard<std::mutex> lock ( locker );
         switch ( change.m_Action )
         {
         case FILE_ACTION_ADDED:
            std::wcout << L"[A] ";
            break;
         case FILE_ACTION_REMOVED:
            std::wcout << L"[R] ";
            break;
         case FILE_ACTION_MODIFIED:
            std::wcout << L"[M] ";
            break;
         case FILE_ACTION_RENAMED_OLD_NAME:
            std::wcout << L"[O] ";
            break;
         case FILE_ACTION_RENAMED_NEW_NAME:
            std::wcout << L"[N] ";
            break;
         }
         std::wcout << change.m_pDir->GetName ( ) << change.m_Filename << std::endl;
      };
      dir.OnChange = output;
      for ( size_t i = 0; i < 512; ++i )
      {
         dir.RequestCheckChanges ( ( CompletionPort* ) &ThreadPool::Instance ( ) );
      }
      ::Sleep ( INFINITE );
   }
    catch ( std::runtime_error& err )
    {
        std::cout << err.what ( ) << std::endl;
        system ( "pause" );
    }
}