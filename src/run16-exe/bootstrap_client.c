#include "run16-exe/frontend_bootstrap.h"

/* Local references to broker-issued capabilities only. Creation, bootstrap
 * transfer and Console lease return are NTSRV operations, not launcher IPC. */
void frontend_bootstrap_release(frontend_connection *connection)
{
    if(!connection)return;
    if(connection->capability)CloseHandle(connection->capability);
    if(connection->restored)CloseHandle(connection->restored);
    if(connection->process)CloseHandle(connection->process);
    ZeroMemory(connection,sizeof(*connection));
}
