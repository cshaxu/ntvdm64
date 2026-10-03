#include "next_command.h"
#include "ntsrv-exe/opennt/include/base_rpc_client.h"
#include "common/codec/native_launch.h"

DWORD ntvwm_get_next_command(ntvwm_next_command *command)
{
    DWORD error;
    if(!command)return ERROR_INVALID_PARAMETER;
    ZeroMemory(command,sizeof(*command));
    command->payload=HeapAlloc(GetProcessHeap(),0,NATIVE_LAUNCH_MAX_BYTES);
    if(!command->payload)return ERROR_NOT_ENOUGH_MEMORY;
    error=OpenNtBaseClientGetNextNativeCommand(NATIVE_LAUNCH_MAX_BYTES,command->payload,
        &command->bytes,&command->sender,
        &command->execution,&command->frontend,&command->request,&command->caller_generation);
    if(error)ntvwm_dispose_next_command(command);
    else if(!command->bytes) {
        HeapFree(GetProcessHeap(),0,command->payload);command->payload=NULL;
    }
    return error;
}

DWORD ntvwm_complete_next_command(DWORD request,DWORD exit_code)
{
    /* NTSRV uses request zero for a frontend I/O resume rather than a
     * Direct task. It still requires the authenticated completion RPC. */
    return OpenNtBaseClientCompleteWorkerChannel(request,exit_code);
}

/* Caller reports final I/O before the broker signals the direct receipt. */
DWORD ntvwm_complete_native_request(DWORD request,DWORD exit_code,DWORD io_error,DWORD io_flags)
{
    return OpenNtBaseClientCompleteNativeRequest(request,exit_code,io_error,io_flags);
}

void ntvwm_dispose_next_command(ntvwm_next_command *command)
{
    if(!command)return;
    if(command->payload)HeapFree(GetProcessHeap(),0,command->payload);
    if(command->sender)CloseHandle(command->sender);
    if(command->execution)CloseHandle(command->execution);
    if(command->frontend)CloseHandle(command->frontend);
    ZeroMemory(command,sizeof(*command));
}
