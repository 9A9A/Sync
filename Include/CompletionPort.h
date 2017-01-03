#ifndef _COMPLETION_PORT_H_
#define _COMPLETION_PORT_H_
#include <Windows.h>
#include <string>
#include <atomic>
class CompletionPort
{
    HANDLE m_hPort;
public:
    CompletionPort ( )
    {
        m_hPort = ::CreateIoCompletionPort ( INVALID_HANDLE_VALUE , NULL , NULL , NULL );
        if ( m_hPort == INVALID_HANDLE_VALUE )
        {
            throw std::runtime_error ( "Unable to create io completion port : " + std::to_string ( GetLastError ( ) ) );
        }
    }
    explicit CompletionPort ( HANDLE handle , ULONG_PTR completion_key )
    {
        m_hPort = ::CreateIoCompletionPort ( handle , NULL , completion_key , NULL );
        if ( m_hPort == INVALID_HANDLE_VALUE )
        {
            throw std::runtime_error ( "Unable to create and associate to io completion port : " + std::to_string ( GetLastError ( ) ) );
        }
    }
    virtual ~CompletionPort ( )
    {
        ::CloseHandle ( m_hPort );
    }
    bool RegisterHandle ( HANDLE handle , ULONG_PTR completion_key )
    {
        if ( m_hPort != ::CreateIoCompletionPort ( handle , m_hPort , completion_key , NULL ) )
        {
            return false;
        }
        return true;
    }
    std::pair<BOOL , size_t> GetQueuedCompletionStatus ( LPDWORD number_of_bytes , PULONG_PTR completion_key , LPOVERLAPPED* overlapped , DWORD timemout_ms )
    {
        return std::pair<BOOL , size_t> ( ::GetQueuedCompletionStatus ( m_hPort , number_of_bytes , completion_key , overlapped , timemout_ms ) , ::GetLastError ( ) );
    }
    std::pair<BOOL , size_t> GetQueuedCompletionStatusEx ( LPOVERLAPPED_ENTRY completion_port_entries , ULONG count , PULONG num_entries_removed , DWORD timeout_ms , BOOL alertable )
    {
        return std::pair<BOOL , size_t> ( ::GetQueuedCompletionStatusEx ( m_hPort , completion_port_entries , count , num_entries_removed , timeout_ms , alertable ) , ::GetLastError ( ) );
    }
    std::pair<BOOL , size_t> PostQueuedCompletionStatus ( DWORD bytes_transferred , ULONG_PTR completion_key , LPOVERLAPPED overlapped )
    {
        return std::pair<BOOL , size_t> ( ::PostQueuedCompletionStatus ( m_hPort , bytes_transferred , completion_key , overlapped ) , ::GetLastError ( ) );
    }
};
class Async
{
protected:
    std::atomic<size_t> m_nPendingOperations;
public:
    Async ( ) :
        m_nPendingOperations ( 0 )
    {
    }
    virtual void RegisterOnCompletionPort ( CompletionPort& ) = 0;
};
#endif