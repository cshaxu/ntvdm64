/* Project-added worker/control RPC adaptation; not original VDM dispatch,
 * task completion, blocking/resume or resource lifetime decisions. */
#include "worker_control.h"
#include "async_call.h"
#include "common/protocol/frontend_lifecycle.h"
#include "common/protocol/frontend_protocol.h"

typedef error_status_t (*worker_event_rpc)(PRPC_ASYNC_STATE,handle_t,VDM_CONNECTION,HANDLE,ULONG,HANDLE *);
static DWORD worker_event(const common_rpc_connection *state,HANDLE *output,
    worker_event_rpc invoke,BOOL required)
{
    RPC_ASYNC_STATE async;
    DWORD error=ERROR_INVALID_STATE;
    HANDLE local=NULL;
    if (!output) return ERROR_INVALID_PARAMETER;
    *output=NULL;
    if(!state || !state->connection || !state->binding || !state->process) return error;
    error=common_rpc_begin_async(&async);if(error)return error;
    RpcTryExcept {
        invoke(&async,state->binding,state->connection,state->process,state->generation,&local);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    error=common_rpc_end_async(&async,error,NULL,INFINITE);
    if(error){if(local)CloseHandle(local);return error;}
    if(required && !local)return ERROR_INVALID_HANDLE;
    *output=local;return ERROR_SUCCESS;
}
DWORD common_rpc_worker_frontend_capability(const common_rpc_connection *state,HANDLE *capability)
{ return worker_event(state,capability,Client_WorkerFrontendCapability,FALSE); }

DWORD common_rpc_acquire_console_context(const common_rpc_connection *state,HANDLE frontend,HANDLE *capability)
{
    RPC_ASYNC_STATE async;
    DWORD error=ERROR_INVALID_STATE;
    if (!capability) return ERROR_INVALID_PARAMETER;
    *capability=NULL;
    if(!state || !state->connection || !state->binding || !state->process) return error;
    error=common_rpc_begin_async(&async);if(error)return error;
    RpcTryExcept {
        Client_AcquireConsoleContext(&async,state->binding,state->connection,state->process,
            state->generation,frontend,capability);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    error=common_rpc_end_async(&async,error,NULL,INFINITE);
    if (error && *capability) { CloseHandle(*capability);*capability=NULL; }
    return error;
}

typedef error_status_t (*worker_handle_rpc)(PRPC_ASYNC_STATE,handle_t,VDM_CONNECTION,HANDLE,ULONG,HANDLE);
static DWORD worker_handle(const common_rpc_connection *state,HANDLE handle,worker_handle_rpc invoke)
{
    RPC_ASYNC_STATE async;
    DWORD error=ERROR_INVALID_STATE;
    if(!state || !state->connection || !state->binding || !state->process) return error;
    error=common_rpc_begin_async(&async);if(error)return error;
    RpcTryExcept {
        invoke(&async,state->binding,state->connection,state->process,state->generation,handle);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    return common_rpc_end_async(&async,error,NULL,INFINITE);
}
DWORD common_rpc_bind_console_context(const common_rpc_connection *state,HANDLE capability)
{ return worker_handle(state,capability,Client_BindConsoleContext); }
DWORD common_rpc_register_native_backend(const common_rpc_connection *state,HANDLE frontend)
{ return worker_handle(state,frontend,Client_RegisterNativeBackend); }

DWORD common_rpc_worker_shutdown_event(const common_rpc_connection *state,HANDLE *shutdown)
{ return worker_event(state,shutdown,Client_WorkerShutdownEvent,TRUE); }

DWORD common_rpc_worker_console_close_event(const common_rpc_connection *state,HANDLE *close)
{ return worker_event(state,close,Client_WorkerConsoleCloseEvent,TRUE); }

DWORD common_rpc_worker_io_checkpoint(const common_rpc_connection *state,DWORD reason,DWORD request,DWORD *decision)
{
    RPC_ASYNC_STATE async;
    DWORD error=ERROR_INVALID_STATE,local=WORKER_IO_KEEP;
    if(!decision)return ERROR_INVALID_PARAMETER;
    *decision=WORKER_IO_KEEP;
    if(!state || !state->connection || !state->binding || !state->process)return error;
    error=common_rpc_begin_async(&async);if(error)return error;
    RpcTryExcept {
        Client_WorkerIoCheckpoint(&async,state->binding,state->connection,state->process,
            state->generation,reason,request,&local);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    error=common_rpc_end_async(&async,error,NULL,INFINITE);
    if(error)return error;
    if(local!=WORKER_IO_KEEP && local!=WORKER_IO_RELEASE)return ERROR_INVALID_DATA;
    *decision=local;return ERROR_SUCCESS;
}
DWORD common_rpc_worker_io_transition(const common_rpc_connection *state,DWORD action)
{
    RPC_ASYNC_STATE async;
    DWORD error=ERROR_INVALID_STATE;
    if(!state || !state->connection || !state->binding || !state->process)return error;
    error=common_rpc_begin_async(&async);if(error)return error;
    RpcTryExcept {
        Client_WorkerIoTransition(&async,state->binding,state->connection,state->process,
            state->generation,action);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    return common_rpc_end_async(&async,error,NULL,INFINITE);
}
DWORD common_rpc_frontend_io_disconnected(const common_rpc_connection *state)
{
    RPC_ASYNC_STATE async;DWORD error=ERROR_INVALID_STATE;
    if(!state || !state->connection || !state->binding || !state->process)return error;
    error=common_rpc_begin_async(&async);if(error)return error;
    RpcTryExcept {
        Client_FrontendIoDisconnected(&async,state->binding,state->connection,state->process,
            state->generation);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    return common_rpc_end_async(&async,error,NULL,INFINITE);
}
DWORD common_rpc_worker_io_release_event(const common_rpc_connection *state,HANDLE *release)
{ return worker_event(state,release,Client_WorkerIoReleaseEvent,TRUE); }

DWORD common_rpc_take_frontend(const common_rpc_connection *state,HANDLE *pipe,HANDLE *frontend,DWORD *frontend_generation,HANDLE *ready,BOOL wait)
{
    RPC_ASYNC_STATE async;
    DWORD error=ERROR_INVALID_STATE;
    if (!pipe || !frontend || !frontend_generation || !ready) return ERROR_INVALID_PARAMETER;
    *pipe=NULL; *frontend=NULL; *frontend_generation=0;*ready=NULL;
    if(!state || !state->connection || !state->binding || !state->process) return error;
    error=common_rpc_begin_async(&async);if(error)return error;
    RpcTryExcept {
        if(wait)Client_WaitFrontend(&async,state->binding,state->connection,state->process,
            state->generation,pipe,frontend,frontend_generation,ready);
        else Client_TakeFrontend(&async,state->binding,state->connection,state->process,
            state->generation,pipe,frontend,frontend_generation,ready);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    error=common_rpc_end_async(&async,error,NULL,INFINITE);
    if (error) {
        if (*pipe) CloseHandle(*pipe);
        if (*frontend) CloseHandle(*frontend);
        if (*ready) CloseHandle(*ready);
        *pipe=NULL; *frontend=NULL; *frontend_generation=0;*ready=NULL;
    }
    return error;
}
