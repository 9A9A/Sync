
#include "ThreadPool.h"
#include "Directory.h"
#include <string>

int main ( )
{
    try
    {

        {
            Directory dir ( L"D:\\common" , FILE_NOTIFY_CHANGE_FILE_NAME |
                            FILE_NOTIFY_CHANGE_DIR_NAME |
                            FILE_NOTIFY_CHANGE_ATTRIBUTES |
                            FILE_NOTIFY_CHANGE_SIZE |
                            FILE_NOTIFY_CHANGE_LAST_WRITE |
                            FILE_NOTIFY_CHANGE_LAST_ACCESS |
                            FILE_NOTIFY_CHANGE_CREATION |
                            FILE_NOTIFY_CHANGE_SECURITY );
            auto output = [ ] ( Directory::Change& change ) ->void
            {
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
                std::wcout << change.m_Filename << std::endl;
            };
            dir.OnChange = output;
            for ( size_t i = 0; i < 512; ++i )
            {
                dir.MakeRequest ( );
            }
            std::string yo;
            std::getline ( std::cin , yo );
        }
        volatile int rff = 42;
    }
    catch ( std::runtime_error& err )
    {
        std::cout << err.what ( ) << std::endl;
        system ( "pause" );
    }
}