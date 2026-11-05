#include "pipe_pair.h"
DWORD win32_pipe_pair_create(PCWSTR name,HANDLE event,HANDLE *server,HANDLE *client)
{
    HANDLE a=INVALID_HANDLE_VALUE,b=INVALID_HANDLE_VALUE;
    OVERLAPPED connect={0};DWORD error=ERROR_SUCCESS;
    if(!server || !client || server==client)return ERROR_INVALID_PARAMETER;
    *server=*client=INVALID_HANDLE_VALUE;
    if(!name || !*name || !event || event==INVALID_HANDLE_VALUE)return ERROR_INVALID_PARAMETER;
    a=CreateNamedPipeW(name,PIPE_ACCESS_DUPLEX|FILE_FLAG_OVERLAPPED|FILE_FLAG_FIRST_PIPE_INSTANCE,
        PIPE_TYPE_BYTE|PIPE_WAIT|PIPE_REJECT_REMOTE_CLIENTS,1,32768,32768,0,NULL);
    if(a==INVALID_HANDLE_VALUE)return GetLastError();
    b=CreateFileW(name,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,
        FILE_FLAG_OVERLAPPED|SECURITY_SQOS_PRESENT|SECURITY_IDENTIFICATION,NULL);
    if(b==INVALID_HANDLE_VALUE){error=GetLastError();goto fail;}
    connect.hEvent=event;
    if(!ConnectNamedPipe(a,&connect)) {
        error=GetLastError();
        if(error!=ERROR_PIPE_CONNECTED) {
            if(error==ERROR_IO_PENDING) {
                DWORD ignored;
                CancelIoEx(a,&connect);
                (void)GetOverlappedResult(a,&connect,&ignored,TRUE);
            }
            goto fail;
        }
    }
    *server=a;*client=b;return ERROR_SUCCESS;
fail:
    if(b!=INVALID_HANDLE_VALUE)CloseHandle(b);
    CloseHandle(a);return error;
}
