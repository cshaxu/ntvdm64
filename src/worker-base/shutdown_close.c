#include "connection.h"

void worker_base_shutdown_close(LPTHREAD_START_ROUTINE close,void *context,
    DWORD grace,DWORD exit_code,HANDLE closed)
{
    DWORD error=ERROR_INVALID_PARAMETER;
    if(close) {
        if(grace==INFINITE)error=close(context);
        else {
            HANDLE thread=CreateThread(NULL,0,close,context,0,NULL);
            if(!thread)error=GetLastError();
            else {
                DWORD wait=WaitForSingleObject(thread,grace);
                if(wait==WAIT_OBJECT_0) {
                    if(!GetExitCodeThread(thread,&error))error=GetLastError();
                } else error=wait==WAIT_TIMEOUT ? ERROR_TIMEOUT : GetLastError();
                CloseHandle(thread);
            }
        }
    }
    if(!error && closed && !SetEvent(closed))error=GetLastError();
    /* A close-acknowledging owner reports its real failure. The original VDM
     * forced-close owner keeps its declared exit code after the close grace. */
    TerminateProcess(GetCurrentProcess(),closed && error ? error : exit_code);
}
