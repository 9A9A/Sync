#define WIN32_LEAN_AND_MEAN
#include "ThreadPool.h"
#include "Socket.h"

int main (int argc,char** argv )
{
   //std::mutex locker;
   try
   {
       {
           NetworkingInitialization::Instance ( );
           auto sock_t = UDPASocket::Create ( );
           sock_t->Bind ( 27489 );
           sock_t->RegisterOnCompletionPort ( ThreadPool::Instance ( ) );
           sock_t->RegisterOnRecv ( [ ] ( UDPASocket& , NetworkAddress* addr , char* buf , size_t sz )
           {
               std::cout << "Received from " << addr->ToString ( ) << " bytes " << sz << "\n";
               for ( size_t i = 0; i < sz; i++ )
               {
                   std::cout << buf [ i ];
               }
               std::cout << std::endl;
           } );
           sock_t->RegisterOnSend ( [ ] ( UDPASocket& , NetworkAddress* addr , size_t sz )
           {
               std::cout << "Send to " << addr->ToString ( ) << " bytes " << sz << std::endl;
           } );
           sock_t->RegisterOnClose ( [ ] ( UDPASocket& , DWORD err )
           {
               std::cout << "On sock occurred error : " << err << std::endl;
           } );
           for ( size_t i = 0; i < 1; i++ )
           {
               sock_t->RequestRecvFrom ( ThreadPool::Instance ( ) );
           }
           //auto flags = Directory::FILE_NAME |
           //   Directory::DIR_NAME |
           //   Directory::ATTRIBUTES |
           //   Directory::SIZE |
           //   Directory::LAST_WRITE |
           //   Directory::LAST_ACCESS |
           //   Directory::CREATION |
           //   Directory::SECURITY;
           //Directory dir ( L"\\\\?\\\C:\\" , flags , Directory::OPEN );
           //dir.RegisterOnCompletionPort ( ( CompletionPort* ) &ThreadPool::Instance ( ) );
           //auto output = [& ] ( Directory::Change& change ) ->void
           //{
           //   std::lock_guard<std::mutex> lock ( locker );
           //   switch ( change.m_Action )
           //   {
           //   case FILE_ACTION_ADDED:
           //      std::wcout << L"[A] ";
           //      break;
           //   case FILE_ACTION_REMOVED:
           //      std::wcout << L"[R] ";
           //      break;
           //   case FILE_ACTION_MODIFIED:
           //      std::wcout << L"[M] ";
           //      break;
           //   case FILE_ACTION_RENAMED_OLD_NAME:
           //      std::wcout << L"[O] ";
           //      break;
           //   case FILE_ACTION_RENAMED_NEW_NAME:
           //      std::wcout << L"[N] ";
           //      break;
           //   }
           //   std::wcout << change.m_pDir->GetName ( ) << change.m_Filename << std::endl;
           //};
           //dir.OnChange = output;
           //for ( size_t i = 0; i < 128; ++i )
           //{
           //   dir.RequestCheckChanges ( ( CompletionPort* ) &ThreadPool::Instance ( ) );
           //}
           std::string str;
           getline ( std::cin , str );
           if ( str == "close" )
           {
               sock_t->Close ( );
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
   system ( "pause" );
}