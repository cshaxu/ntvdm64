/* Project-added worker/control RPC adaptation; not original VDM dispatch,
 * task completion, blocking/resume or resource lifetime decisions. */
#include "worker_control.h"

DWORD common_rpc_worker_frontend_capability(const common_rpc_connection *state,HANDLE *capability)
{
    DWORD error=ERROR_INVALID_STATE;
    if (!capability) return ERROR_INVALID_PARAMETER;
    *capability=NULL;
    if(!state || !state->connection || !state->binding || !state->process) return error;
    RpcTryExcept {
        error=Client_WorkerFrontendCapability(state->binding,state->connection,state->process,
            state->generation,capability);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    if (error && *capability) { CloseHandle(*capability);*capability=NULL; }
    return error;
}

DWORD common_rpc_acquire_console_context(const common_rpc_connection *state,HANDLE frontend,HANDLE *capability)
{
    DWORD error=ERROR_INVALID_STATE;
    if (!capability) return ERROR_INVALID_PARAMETER;
    *capability=NULL;
    if(!state || !state->connection || !state->binding || !state->process) return error;
    RpcTryExcept {
        error=Client_AcquireConsoleContext(state->binding,state->connection,state->process,
            state->generation,frontend,capability);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    if (error && *capability) { CloseHandle(*capability);*capability=NULL; }
    return error;
}

DWORD common_rpc_bind_console_context(const common_rpc_connection *state,HANDLE capability)
{
    DWORD error=ERROR_INVALID_STATE;
    if(!state || !state->connection || !state->binding || !state->process) return error;
    RpcTryExcept {
        error=Client_BindConsoleContext(state->binding,state->connection,state->process,
            state->generation,capability);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    return error;
}

DWORD common_rpc_register_native_backend(const common_rpc_connection *state,HANDLE frontend,HANDLE stop,HANDLE closed)
{
    DWORD error=ERROR_INVALID_STATE;
    if(!state || !state->connection || !state->binding || !state->process)return error;
    RpcTryExcept {
        error=Client_RegisterNativeBackend(state->binding,state->connection,state->process,
            state->generation,frontend,stop,closed);
    }
    RpcExcept(1) {error=RpcExceptionCode();}
    RpcEndExcept
    return error;
}

DWORD common_rpc_worker_shutdown_event(const common_rpc_connection *state,HANDLE *shutdown)
{
    DWORD error=ERROR_INVALID_STATE;
    HANDLE local=NULL;
    if(!shutdown)return ERROR_INVALID_PARAMETER;
    *shutdown=NULL;
    if(!state || !state->connection || !state->binding || !state->process)return error;
    RpcTryExcept {
        error=Client_WorkerShutdownEvent(state->binding,state->connection,state->process,
            state->generation,&local);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    if(error){if(local)CloseHandle(local);return error;}
    if(!local)return ERROR_INVALID_HANDLE;
    *shutdown=local;return ERROR_SUCCESS;
}

DWORD common_rpc_worker_state_changed(const common_rpc_connection *state,HANDLE *state_changed)
{
    DWORD error=ERROR_INVALID_STATE;
    HANDLE local=NULL;
    if(!state_changed)return ERROR_INVALID_PARAMETER;
    *state_changed=NULL;
    if(!state || !state->connection || !state->binding || !state->process)return error;
    RpcTryExcept {
        error=Client_WorkerStateChanged(state->binding,state->connection,state->process,
            state->generation,&local);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    if(error) { if(local)CloseHandle(local);return error; }
    if(!local)return ERROR_INVALID_HANDLE;
    *state_changed=local;
    return ERROR_SUCCESS;
}

DWORD common_rpc_take_frontend(const common_rpc_connection *state,HANDLE *pipe,HANDLE *frontend,DWORD *frontend_generation,HANDLE *ready,BOOL wait)
{
    DWORD error=ERROR_INVALID_STATE;
    if (!pipe || !frontend || !frontend_generation || !ready) return ERROR_INVALID_PARAMETER;
    *pipe=NULL; *frontend=NULL; *frontend_generation=0;*ready=NULL;
    if(!state || !state->connection || !state->binding || !state->process) return error;
    RpcTryExcept {
        error=wait ? Client_WaitFrontend(state->binding,state->connection,state->process,
            state->generation,pipe,frontend,frontend_generation,ready) :
            Client_TakeFrontend(state->binding,state->connection,state->process,
            state->generation,pipe,frontend,frontend_generation,ready);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    if (error) {
        if (*pipe) CloseHandle(*pipe);
        if (*frontend) CloseHandle(*frontend);
        if (*ready) CloseHandle(*ready);
        *pipe=NULL; *frontend=NULL; *frontend_generation=0;*ready=NULL;
    }
    return error;
}
