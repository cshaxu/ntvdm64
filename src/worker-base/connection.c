#include "connection.h"
#include "ntsrv-exe/opennt/include/base_rpc_client.h"

/* Existing NTVDM bootstrap ordering. No launcher admission, frontend ownership
 * or DOS/native execution policy lives here. The service owns authentication. */
DWORD worker_base_connect(void)
{
    DWORD error=OpenNtBaseClientConnectCurrent();
    if(error)return error;
    error=OpenNtBaseClientWatchBroker();
    if(error)OpenNtBaseClientDisconnectCurrent();
    return error;
}

void worker_base_disconnect(void)
{
    OpenNtBaseClientDisconnectCurrent();
}

DWORD worker_base_retain_frontend_root(HANDLE capability,HANDLE *process)
{
    DWORD generation,error;
    if(!process)return ERROR_INVALID_PARAMETER;
    *process=NULL;
    if(!capability || capability==INVALID_HANDLE_VALUE)return ERROR_INVALID_HANDLE;
    error=OpenNtBaseClientRetainFrontendRoot(capability,process,&generation);
    if(error)return error;
    if(!*process || WaitForSingleObject(*process,0)!=WAIT_TIMEOUT) {
        if(*process)CloseHandle(*process);
        *process=NULL;
        return ERROR_PIPE_NOT_CONNECTED;
    }
    return ERROR_SUCCESS;
}
