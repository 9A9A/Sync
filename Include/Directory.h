#ifndef _DIRECTORY_H_
#define _DIRECTORY_H_
#include <Windows.h>
#include <functional>
#include <iostream>
#include "Overlapped.h"
class Directory
{
    HANDLE m_hDirectory;
    DWORD m_Flags;
public:
    struct Change
    {
        WCHAR m_Filename [ MAX_PATH ];
        DWORD m_Action;
    };
    Directory ( LPCWSTR filename , DWORD Flags );
    ~Directory ( );
    void MakeRequest ( OverlappedEx* ev = nullptr );
    using EventChange = std::function<void ( Change& )>;
    EventChange OnChange;
};
#endif // _DIRECTORY_H_
