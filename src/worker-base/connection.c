#include "connection.h"
#include "ntsrv-exe/opennt/include/base_rpc_client.h"
#include "common/protocol/frontend_protocol.h"

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

DWORD worker_base_shutdown_event(HANDLE *shutdown)
{
    return OpenNtBaseClientWorkerShutdownEvent(shutdown);
}

DWORD worker_base_io_release_event(HANDLE *release)
{
    return OpenNtBaseClientWorkerIoReleaseEvent(release);
}
DWORD worker_base_io_checkpoint(DWORD reason,DWORD request,DWORD *decision)
{
    return OpenNtBaseClientWorkerIoCheckpoint(reason,request,decision);
}

DWORD worker_base_io_open(HANDLE *pipe,HANDLE *peer,HANDLE *ready,DWORD *generation)
{
    DWORD error;
    if(!pipe || !peer || !ready || !generation || *pipe || *peer || *ready)
        return ERROR_INVALID_PARAMETER;
    error=OpenNtBaseClientWorkerIoTransition(WORKER_IO_ACQUIRE);
    if(error)return error;
    error=OpenNtBaseClientWaitFrontend(pipe,peer,generation,ready);
    /* A failed granted connection is an infrastructure failure. Never claim
     * release acknowledgement without both endpoints actually closing. */
    return error;
}
DWORD worker_base_io_close(HANDLE *pipe,HANDLE *peer,HANDLE *ready)
{
    DWORD error;
    if(!pipe || !peer || !ready || !*pipe)return ERROR_INVALID_PARAMETER;
    error=OpenNtBaseClientWorkerIoTransition(WORKER_IO_RELEASE_BEGIN);
    if(error)return error;
    if(*pipe)CloseHandle(*pipe);
    if(*peer)CloseHandle(*peer);
    if(*ready)CloseHandle(*ready);
    *pipe=*peer=*ready=NULL;
    error=OpenNtBaseClientWorkerIoTransition(WORKER_IO_RELEASED);
    return error;
}
