#include "next_command.h"
#include "ntsrv-exe/opennt/include/base_rpc_client.h"

DWORD ntw32_get_next_command(ntw32_next_command *command)
{
    DWORD error;
    if(!command)return ERROR_INVALID_PARAMETER;
    ZeroMemory(command,sizeof(*command));
    error=OpenNtBaseClientGetNextNativeCommand(&command->channel,&command->sender,
        &command->execution,&command->frontend,&command->request);
    if(error)ntw32_dispose_next_command(command);
    return error;
}

DWORD ntw32_complete_next_command(DWORD request,DWORD exit_code)
{
    /* NTSRV uses request zero for a frontend I/O resume rather than a
     * Direct task. It still requires the authenticated completion RPC. */
    return OpenNtBaseClientCompleteWorkerChannel(request,exit_code);
}

void ntw32_dispose_next_command(ntw32_next_command *command)
{
    if(!command)return;
    if(command->channel)CloseHandle(command->channel);
    if(command->sender)CloseHandle(command->sender);
    if(command->execution)CloseHandle(command->execution);
    if(command->frontend)CloseHandle(command->frontend);
    ZeroMemory(command,sizeof(*command));
}
