/* Project-added copied native-control RPC client, extracted from BaseClient's
 * standalone adapter. Original DOS/WOW command and receipt state stays there. */
#include "native_command.h"
#include "common/codec/native_launch.h"
#include "async_call.h"
#include "common/protocol/frontend_lifecycle.h"

static BOOL connected(const common_rpc_connection *state)
{
    return state && state->binding && state->connection && state->process;
}

DWORD common_rpc_next_native_command(const common_rpc_connection *state,DWORD capacity,
    BYTE *payload,DWORD *bytes,HANDLE *caller_process,HANDLE *execution,
    HANDLE *frontend,DWORD *request,DWORD *caller_generation)
{
    DWORD error=ERROR_INVALID_STATE;
    if(!bytes || !payload || !capacity || capacity>NATIVE_LAUNCH_MAX_BYTES ||
        !caller_process || !execution || !frontend || !request || !caller_generation)
        return ERROR_INVALID_PARAMETER;
    *bytes=*request=*caller_generation=0;
    *caller_process=*execution=*frontend=NULL;
    if(!connected(state))return error;
    RpcTryExcept {
        error=Client_GetNextNativeCommand(state->binding,state->connection,state->process,
            state->generation,capacity,payload,bytes,caller_process,execution,frontend,
            request,caller_generation);
    }
    RpcExcept(1) {error=RpcExceptionCode();}
    RpcEndExcept
    if(error) {
        if(*caller_process)CloseHandle(*caller_process);
        if(*execution)CloseHandle(*execution);
        if(*frontend)CloseHandle(*frontend);
        *bytes=*request=*caller_generation=0;
        *caller_process=*execution=*frontend=NULL;
    }
    return error;
}

DWORD common_rpc_native_startup_result(const common_rpc_connection *state,
    DWORD caller_generation,DWORD request,DWORD status,HANDLE target,HANDLE receipt)
{
    RPC_ASYNC_STATE async;
    DWORD error=ERROR_INVALID_STATE;
    if(!connected(state))return error;
    error=common_rpc_begin_async(&async);if(error)return error;
    RpcTryExcept {
        Client_NativeStartupResult(&async,state->binding,state->connection,state->process,
            state->generation,caller_generation,request,status,target,receipt);
    }
    RpcExcept(1) {error=RpcExceptionCode();}
    RpcEndExcept
    return common_rpc_end_async(&async,error,NULL,OPENNT_FRONTEND_LIFECYCLE_TIMEOUT_MS);
}

DWORD common_rpc_complete_native_command(const common_rpc_connection *state,
    DWORD request,DWORD exit_code,DWORD io_error,DWORD io_flags)
{
    RPC_ASYNC_STATE async;
    DWORD error=ERROR_INVALID_STATE;
    if(!connected(state))return error;
    error=common_rpc_begin_async(&async);if(error)return error;
    RpcTryExcept {
        Client_CompleteWorkerChannel(&async,state->binding,state->connection,state->process,
            state->generation,request,exit_code,io_error,io_flags);
    }
    RpcExcept(1) {error=RpcExceptionCode();}
    RpcEndExcept
    return common_rpc_end_async(&async,error,NULL,OPENNT_FRONTEND_LIFECYCLE_TIMEOUT_MS);
}

DWORD common_rpc_submit_native_request(const common_rpc_connection *state,DWORD operation,HANDLE frontend,
    DWORD bytes,BYTE *payload,HANDLE *target,HANDLE *receipt,DWORD *request)
{
    RPC_ASYNC_STATE async;
    DWORD error=ERROR_INVALID_STATE;
    if(!target || !receipt || !request ||
        (operation!=OPENNT_BASE_NATIVE_REQUEST_LAUNCH &&
         operation!=OPENNT_BASE_NATIVE_REQUEST_PARENT_RESUME))return ERROR_INVALID_PARAMETER;
    *target=*receipt=NULL;*request=0;
    if(!connected(state))return error;
    error=common_rpc_begin_async(&async);if(error)return error;
    RpcTryExcept {
        Client_SubmitNativeRequest(&async,state->binding,state->connection,state->process,
            state->generation,operation,frontend,bytes,payload,target,receipt,request);
    }
    RpcExcept(1){error=RpcExceptionCode();}
    RpcEndExcept
    error=common_rpc_end_async(&async,error,NULL,OPENNT_FRONTEND_LIFECYCLE_TIMEOUT_MS);
    if(error) {
        if(*target)CloseHandle(*target);
        if(*receipt)CloseHandle(*receipt);
        *target=*receipt=NULL;*request=0;
    }
    return error;
}

DWORD common_rpc_finish_native_request(const common_rpc_connection *state,DWORD request,
    DWORD *exit_code,DWORD *target_completed)
{
    RPC_ASYNC_STATE async;
    DWORD error=ERROR_INVALID_STATE,reply=ERROR_INVALID_STATE;
    if(!exit_code || !target_completed || !request)return ERROR_INVALID_PARAMETER;
    *exit_code=*target_completed=0;
    if(!connected(state))return error;
    error=common_rpc_begin_async(&async);if(error)return error;
    RpcTryExcept {
        Client_FinishNativeRequest(&async,state->binding,state->connection,state->process,
            state->generation,request,exit_code,target_completed);
    }
    RpcExcept(1){error=RpcExceptionCode();}
    RpcEndExcept
    if(!error)error=common_rpc_finish_async(&async,async.u.hEvent,NULL,
        OPENNT_FRONTEND_LIFECYCLE_TIMEOUT_MS,&reply);
    CloseHandle(async.u.hEvent);
    /* A real application-error reply can still carry a consumed exit result.
     * Only failed transport invalidates those outputs. Never replay consumption. */
    if(error){*exit_code=*target_completed=0;return error;}
    if(*target_completed>1){*exit_code=*target_completed=0;return ERROR_INVALID_DATA;}
    return reply;
}
