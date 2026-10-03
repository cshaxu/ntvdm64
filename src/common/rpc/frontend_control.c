/* Project-added frontend control RPC exchanges extracted from the standalone
 * BaseClient adapter. Decisions and per-process resource owners stay local. */
#include "frontend_control.h"

DWORD common_rpc_acquire_frontend_root(const common_rpc_connection *state,uint64_t console_window,DWORD *create_root,
    HANDLE *root,HANDLE *capability,HANDLE *retire,HANDLE *restored)
{
    DWORD error=ERROR_INVALID_STATE;
    if(!create_root || !root || !capability || !retire || !restored)return ERROR_INVALID_PARAMETER;
    *create_root=0;*root=*capability=*retire=*restored=NULL;
    if(!state || !state->connection || !state->binding || !state->process)return error;
    RpcTryExcept {
        error=Client_AcquireFrontendRoot(state->binding,state->connection,state->process,
            state->generation,(LONGLONG)console_window,create_root,root,capability,retire,restored);
    }
    RpcExcept(1){error=RpcExceptionCode();}
    RpcEndExcept
    if(error){
        if(*root)CloseHandle(*root);if(*capability)CloseHandle(*capability);
        if(*retire)CloseHandle(*retire);if(*restored)CloseHandle(*restored);
        *root=*capability=*retire=*restored=NULL;*create_root=0;
    }
    return error;
}

DWORD common_rpc_frontend_usage(const common_rpc_connection *state,DWORD *pending,DWORD *tasks)
{
    DWORD error=ERROR_INVALID_STATE,local_pending=0,local_tasks=0;
    if(!pending || !tasks)return ERROR_INVALID_PARAMETER;
    if(!state || !state->connection || !state->binding || !state->process)return error;
    RpcTryExcept {
        error=Client_FrontendUsage(state->binding,state->connection,state->process,
            state->generation,&local_pending,&local_tasks);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    if(!error){*pending=local_pending;*tasks=local_tasks;}
    return error;
}

DWORD common_rpc_retire_workerless_frontend(const common_rpc_connection *state,DWORD *retired)
{
    DWORD error=ERROR_INVALID_STATE,local=0;
    if(!retired)return ERROR_INVALID_PARAMETER;
    if(!state || !state->connection || !state->binding || !state->process)return error;
    RpcTryExcept {
        error=Client_RetireWorkerlessFrontend(state->binding,state->connection,
            state->process,state->generation,&local);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    if(!error)*retired=local;
    return error;
}

DWORD common_rpc_frontend_state_changed(const common_rpc_connection *state,HANDLE *state_changed)
{
    DWORD error=ERROR_INVALID_STATE;
    HANDLE local=NULL;
    if(!state_changed)return ERROR_INVALID_PARAMETER;
    *state_changed=NULL;
    if(!state || !state->connection || !state->binding || !state->process)return error;
    RpcTryExcept {
        error=Client_FrontendStateChanged(state->binding,state->connection,state->process,
            state->generation,&local);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    if(error) { if(local)CloseHandle(local);return error; }
    if(!local)return ERROR_INVALID_HANDLE;
    *state_changed=local;
    return ERROR_SUCCESS;
}

DWORD common_rpc_start_frontend(const common_rpc_connection *state,uint64_t window,BOOL borrowed,
    HANDLE *root,HANDLE *capability,HANDLE *restored)
{
    DWORD error=ERROR_INVALID_STATE;
    if(!root || !capability || !restored)return ERROR_INVALID_PARAMETER;
    *root=*capability=*restored=NULL;
    if(!state || !state->connection || !state->binding || !state->process)return error;
    RpcTryExcept {
        error=Client_StartFrontend(state->binding,state->connection,state->process,
            state->generation,(LONGLONG)window,borrowed!=FALSE,root,capability,restored);
    }
    RpcExcept(1){error=RpcExceptionCode();}
    RpcEndExcept
    if(error){
        if(*root)CloseHandle(*root);if(*capability)CloseHandle(*capability);
        if(*restored)CloseHandle(*restored);
        *root=*capability=*restored=NULL;
    }
    return error;
}

DWORD common_rpc_return_frontend_console(const common_rpc_connection *state)
{
    DWORD error=ERROR_INVALID_STATE;
    if(!state || !state->connection || !state->binding || !state->process)return error;
    RpcTryExcept {
        error=Client_ReturnFrontendConsole(state->binding,state->connection,
            state->process,state->generation);
    }
    RpcExcept(1){error=RpcExceptionCode();}
    RpcEndExcept
    return error;
}

DWORD common_rpc_frontend_console_restored(const common_rpc_connection *state)
{
    DWORD error=ERROR_INVALID_STATE;
    if(!state || !state->connection || !state->binding || !state->process)return error;
    RpcTryExcept {
        error=Client_FrontendConsoleRestored(state->binding,state->connection,
            state->process,state->generation);
    }
    RpcExcept(1){error=RpcExceptionCode();}
    RpcEndExcept
    return error;
}

DWORD common_rpc_frontend_startup_result(const common_rpc_connection *state,HANDLE capability,DWORD status)
{
    DWORD error=ERROR_INVALID_STATE;
    if(!state || !state->connection || !state->binding || !state->process)return error;
    RpcTryExcept {
        error=Client_FrontendStartupResult(state->binding,state->connection,
            state->process,state->generation,capability,status);
    }
    RpcExcept(1){error=RpcExceptionCode();}
    RpcEndExcept
    return error;
}

DWORD common_rpc_wait_frontend_console_restored(const common_rpc_connection *state)
{
    DWORD error=ERROR_INVALID_STATE;
    if(!state || !state->connection || !state->binding || !state->process)return error;
    RpcTryExcept {
        error=Client_WaitFrontendConsoleRestored(state->binding,state->connection,
            state->process,state->generation);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    return error;
}

DWORD common_rpc_retire_frontend(const common_rpc_connection *state)
{
    DWORD error=ERROR_INVALID_STATE;
    if(!state || !state->connection || !state->binding || !state->process)return error;
    RpcTryExcept {
        error=Client_RetireFrontend(state->binding,state->connection,state->process,state->generation);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    return error;
}
