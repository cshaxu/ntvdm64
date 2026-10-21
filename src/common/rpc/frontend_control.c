/* Project-added frontend control RPC exchanges extracted from the standalone
 * BaseClient adapter. Decisions and per-process resource owners stay local. */
#include "frontend_control.h"
#include "async_call.h"
#include "common/protocol/frontend_lifecycle.h"

DWORD common_rpc_retire_workerless_frontend(const common_rpc_connection *state,DWORD *retired)
{
    RPC_ASYNC_STATE async;DWORD error=ERROR_INVALID_STATE,local=0;
    if(!retired)return ERROR_INVALID_PARAMETER;
    if(!state || !state->connection || !state->binding || !state->process)return error;
    error=common_rpc_begin_async(&async);if(error)return error;
    RpcTryExcept {
        Client_RetireWorkerlessFrontend(&async,state->binding,state->connection,
            state->process,state->generation,&local);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    error=common_rpc_end_async(&async,error,NULL,OPENNT_FRONTEND_LIFECYCLE_TIMEOUT_MS);
    if(!error)*retired=local;
    return error;
}

DWORD common_rpc_frontend_state_changed(const common_rpc_connection *state,HANDLE *state_changed)
{
    RPC_ASYNC_STATE async;
    DWORD error=ERROR_INVALID_STATE;
    HANDLE local=NULL;
    if(!state_changed)return ERROR_INVALID_PARAMETER;
    *state_changed=NULL;
    if(!state || !state->connection || !state->binding || !state->process)return error;
    error=common_rpc_begin_async(&async);if(error)return error;
    RpcTryExcept {
        Client_FrontendStateChanged(&async,state->binding,state->connection,state->process,
            state->generation,&local);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    error=common_rpc_end_async(&async,error,NULL,OPENNT_FRONTEND_LIFECYCLE_TIMEOUT_MS);
    if(error) { if(local)CloseHandle(local);return error; }
    if(!local)return ERROR_INVALID_HANDLE;
    *state_changed=local;
    return ERROR_SUCCESS;
}

DWORD common_rpc_start_frontend(const common_rpc_connection *state,uint64_t window,BOOL borrowed,
    HANDLE *root,HANDLE *capability,HANDLE *restored)
{
    RPC_ASYNC_STATE async;
    DWORD error=ERROR_INVALID_STATE;
    if(!root || !capability || !restored)return ERROR_INVALID_PARAMETER;
    *root=*capability=*restored=NULL;
    if(!state || !state->connection || !state->binding || !state->process)return error;
    error=common_rpc_begin_async(&async);if(error)return error;
    RpcTryExcept {
        Client_StartFrontend(&async,state->binding,state->connection,state->process,
            state->generation,(LONGLONG)window,borrowed!=FALSE,root,capability,restored);
    }
    RpcExcept(1){error=RpcExceptionCode();}
    RpcEndExcept
    error=common_rpc_end_async(&async,error,NULL,OPENNT_FRONTEND_LIFECYCLE_TIMEOUT_MS);
    if(error){
        if(*root)CloseHandle(*root);if(*capability)CloseHandle(*capability);
        if(*restored)CloseHandle(*restored);
        *root=*capability=*restored=NULL;
    }
    return error;
}


DWORD common_rpc_frontend_console_restored(const common_rpc_connection *state)
{
    RPC_ASYNC_STATE async;DWORD error=ERROR_INVALID_STATE;
    if(!state || !state->connection || !state->binding || !state->process)return error;
    error=common_rpc_begin_async(&async);if(error)return error;
    RpcTryExcept {
        Client_FrontendConsoleRestored(&async,state->binding,state->connection,
            state->process,state->generation);
    }
    RpcExcept(1){error=RpcExceptionCode();}
    RpcEndExcept
    return common_rpc_end_async(&async,error,NULL,OPENNT_FRONTEND_LIFECYCLE_TIMEOUT_MS);
}

DWORD common_rpc_frontend_startup_result(const common_rpc_connection *state,HANDLE capability,DWORD status)
{
    RPC_ASYNC_STATE async;
    DWORD error=ERROR_INVALID_STATE;
    if(!state || !state->connection || !state->binding || !state->process)return error;
    error=common_rpc_begin_async(&async);if(error)return error;
    RpcTryExcept {
        Client_FrontendStartupResult(&async,state->binding,state->connection,
            state->process,state->generation,capability,status);
    }
    RpcExcept(1){error=RpcExceptionCode();}
    RpcEndExcept
    return common_rpc_end_async(&async,error,NULL,OPENNT_FRONTEND_LIFECYCLE_TIMEOUT_MS);
}

DWORD common_rpc_wait_frontend_console_restored(const common_rpc_connection *state)
{
    RPC_ASYNC_STATE async={0};DWORD error=ERROR_INVALID_STATE;
    if(!state || !state->connection || !state->binding || !state->process)return error;
    error=common_rpc_begin_async(&async);if(error)return error;
    RpcTryExcept {
        Client_WaitFrontendConsoleRestored(&async,state->binding,state->connection,
            state->process,state->generation);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    return common_rpc_end_async(&async,error,NULL,OPENNT_FRONTEND_LIFECYCLE_TIMEOUT_MS);
}
