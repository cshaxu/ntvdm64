#include "native_request_protocol.h"
DWORD frontend_request_transfer(HANDLE pipe,HANDLE peer,HANDLE stop,HANDLE event,
    BOOL write,void *buffer,DWORD bytes)
{
    BYTE *cursor=buffer;
    while(bytes) {
        OVERLAPPED io={0};
        HANDLE waits[3]={event,peer,stop};
        DWORD count=0,error,wait;
        BOOL ok;
        if(stop && WaitForSingleObject(stop,0)==WAIT_OBJECT_0)return ERROR_OPERATION_ABORTED;
        ResetEvent(event);io.hEvent=event;
        ok=write ? WriteFile(pipe,cursor,bytes,&count,&io) : ReadFile(pipe,cursor,bytes,&count,&io);
        if(!ok) {
            error=GetLastError();if(error!=ERROR_IO_PENDING)return error;
            wait=WaitForMultipleObjects(stop ? 3 : 2,waits,FALSE,INFINITE);
            if(wait!=WAIT_OBJECT_0) {
                error=wait==WAIT_OBJECT_0+1 ? ERROR_PROCESS_ABORTED :
                    wait==WAIT_OBJECT_0+2 ? ERROR_OPERATION_ABORTED : GetLastError();
                CancelIoEx(pipe,&io);GetOverlappedResult(pipe,&io,&count,TRUE);return error;
            }
            if(!GetOverlappedResult(pipe,&io,&count,FALSE))return GetLastError();
        }
        if(!count || count>bytes)return ERROR_BROKEN_PIPE;
        cursor+=count;bytes-=count;
    }
    return ERROR_SUCCESS;
}
