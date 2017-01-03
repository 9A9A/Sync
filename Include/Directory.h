#ifndef _DIRECTORY_H_
#define _DIRECTORY_H_
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <functional>
#include <iostream>
#include "Overlapped.h"
#include "CompletionPort.h"
#include <Shlwapi.h>
class Directory : public Async
{
    HANDLE m_hDirectory;
    DWORD m_Flags;
    std::wstring m_DirectoryName;
    Directory ( const Directory& ) = delete;
public:
    enum : DWORD
    {
        FILE_NAME = FILE_NOTIFY_CHANGE_FILE_NAME ,
        DIR_NAME = FILE_NOTIFY_CHANGE_DIR_NAME ,
        ATTRIBUTES = FILE_NOTIFY_CHANGE_ATTRIBUTES ,
        SIZE = FILE_NOTIFY_CHANGE_SIZE ,
        LAST_WRITE = FILE_NOTIFY_CHANGE_LAST_WRITE ,
        LAST_ACCESS = FILE_NOTIFY_CHANGE_LAST_ACCESS ,
        CREATION = FILE_NOTIFY_CHANGE_CREATION ,
        SECURITY = FILE_NOTIFY_CHANGE_SECURITY ,
    };
    enum MODE
    {
        OPEN ,
        CREATE
    };
    struct Change
    {
        std::wstring m_Filename;
        DWORD m_Action;
        Directory* m_pDir;
    };
    Directory ( );
    Directory ( Directory&& );
    explicit Directory ( const std::wstring& , DWORD flags , MODE );

    ~Directory ( );
    // if directory is valid - register it on iocp
    virtual void RegisterOnCompletionPort ( CompletionPort* );
    // request changes on opened directory - asynchronously
    void RequestCheckChanges ( CompletionPort* port , OverlappedEx* ev = nullptr );
    // check for changes on opened directory (before check you must register directory on iocp)
    std::pair<int , DWORD> CheckChanges ( OverlappedEx* ev , bool recursive = true );
    // create directory with specified name
    std::pair<int , DWORD> Create ( const std::wstring& dirname );
    bool IsSubdirectory ( Directory* subdir ) const;
    bool IsParentdirectory ( Directory* parentdir )const;
    HANDLE Handle ( ) const;
    // returns true if directory handle != INVALID_HANDLE_VALUE
    bool IsValid ( )const;
    bool IsDirectoryEmpty ( ) const;
    DWORD Flags ( ) const;
    Directory* CreateSubdirectory ( const std::wstring& subdir )const;
    // close directory handle and cancel all io
    bool Close ( );
    // cancel all pending io 
    bool CancelAllIO ( );
    std::pair<int , DWORD> DeleteDirectory ( );
    // get dir name
    std::wstring GetName ( ) const;
    // open existing dir
    std::pair<int , DWORD> Open ( const std::wstring& str );
    using EventChange = std::function<void ( Change& )>;
    EventChange OnChange;
};
#endif // _DIRECTORY_H_
