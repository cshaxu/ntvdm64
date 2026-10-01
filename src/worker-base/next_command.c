#include "next_command.h"
#include "ntsrv-exe/opennt/include/base_rpc_client.h"

DWORD worker_base_get_next_command(worker_base_next_command *command)
{
    DWORD error;
    if(!command)return ERROR_INVALID_PARAMETER;
    ZeroMemory(command,sizeof(*command));
    error=OpenNtBaseClientGetNextNativeCommand(&command->channel,&command->sender,
        &command->execution,&command->frontend,&command->request);
    if(error)worker_base_dispose_next_command(command);
    return error;
}

DWORD worker_base_complete_next_command(DWORD request)
{
    return request ? OpenNtBaseClientCompleteWorkerChannel(request) : ERROR_INVALID_PARAMETER;
}

void worker_base_dispose_next_command(worker_base_next_command *command)
{
    if(!command)return;
    if(command->channel)CloseHandle(command->channel);
    if(command->sender)CloseHandle(command->sender);
    if(command->execution)CloseHandle(command->execution);
    if(command->frontend)CloseHandle(command->frontend);
    ZeroMemory(command,sizeof(*command));
}
