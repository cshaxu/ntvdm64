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
