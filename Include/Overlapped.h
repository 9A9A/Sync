#ifndef _OVERLAPPED_H_
#define _OVERLAPPED_H_
#include <Windows.h>
#include "NetworkAddress.h"
#define DEFAULT_BUFFER_SIZE 32768
#ifdef _WIN64
#define OVERLAPPED_MAX_ENTRIES 32767
#else
#define OVERLAPPED_MAX_ENTRIES 2500
#endif

struct OverlappedEx : public OVERLAPPED
{
    OverlappedEx ( DWORD length = DEFAULT_BUFFER_SIZE ) :
        Length ( length )
    {
        reset ( );
        Buffer = new CHAR [ Length ];
    }
    virtual ~OverlappedEx ( )
    {
        delete [ ] Buffer;
    }
    void reset ( )
    {
        Internal = 0;
        InternalHigh = 0;
        Offset = 0;
        OffsetHigh = 0;
        Pointer = 0;
        hEvent = 0;
        Flags = 0;
        Operation = UndefinedOperation;
        Device = INVALID_HANDLE_VALUE;
        Socket = INVALID_SOCKET;
        SequenceNum = 0;
    }
    enum OperationType : UCHAR
    {
        UndefinedOperation ,
        AcceptEx,
        ConnectEx,
        DisconnectEx,
        Ioctl,
        LockEx,
        Read ,
        Write ,
        Recv ,
        RecvFrom ,
        Send ,
        SendTo ,
        SendFile ,
        ReadDirChanges ,
        Req_Ioctl,
        Req_AcceptEx,
        Req_ConnectEx,
        Req_DisconnectEx,
        Req_LockEx,
        Req_Read ,
        Req_Write ,
        Req_ReadDirChanges ,
        Req_Recv ,
        Req_RecvFrom ,
        Req_Send ,
        Req_SendTo ,
        Req_SendFile ,
    };
    CHAR* Buffer;
    DWORD Flags;
    HANDLE Device;
    DWORD Length;
    SOCKET Socket;
    uint64_t SequenceNum;
    int AddrLen;
    NetworkAddress NetAddr;
    OperationType Operation;
};
#endif // _OVERLAPPED_H_