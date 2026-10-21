/* Existing project management exchanges, not original VDM record policy. */
#include "management.h"
#include "common/protocol/version.h"
#include "async_call.h"
#include "common/protocol/frontend_lifecycle.h"

static const unsigned char app_version[APP_VERSION_BYTES]=APP_VERSION;

DWORD common_rpc_observe_native_creation_bounded(const common_rpc_management *state,HANDLE child,
    DWORD flags,uint64_t *node)
{
    RPC_ASYNC_STATE async={0};hyper identity=0;DWORD error;
    if(!node)return ERROR_INVALID_PARAMETER;
    *node=0;
    if(!state || !state->binding || !state->process || !child)return ERROR_INVALID_STATE;
    error=common_rpc_begin_async(&async);if(error)return error;
    RpcTryExcept {
        Client_ObserveNativeCreationAsync(&async,state->binding,state->process,APP_PROTOCOL_VERSION,
            (unsigned char *)app_version,child,flags,&identity);
    }
    RpcExcept(1) {error=RpcExceptionCode();}
    RpcEndExcept
    error=common_rpc_end_async(&async,error,NULL,INFINITE);
    if(!error)*node=(uint64_t)identity;
    return error;
}

DWORD common_rpc_worker_task_trace(const common_rpc_management *state,const DTASKMGR_KEY *key,
    ULONG *coverage,ULONG *count,WORKER_TRACE_NODE **items)
{
    RPC_ASYNC_STATE async;
    DWORD error=ERROR_INVALID_STATE;
    ULONG flags=0,actual=0;
    WORKER_TRACE_NODE *result=NULL;
    if(!coverage || !count || !items)return ERROR_INVALID_PARAMETER;
    *coverage=0;*count=0;*items=NULL;
    if(!key)return ERROR_INVALID_PARAMETER;
    if(!state || !state->binding || !state->process)return error;
    error=common_rpc_begin_async(&async);if(error)return error;
    RpcTryExcept {
        Client_WorkerTaskTrace(&async,state->binding,state->process,APP_PROTOCOL_VERSION,
            (unsigned char *)app_version,(DTASKMGR_KEY *)key,&flags,&actual,&result);
    }
    RpcExcept(1) {error=RpcExceptionCode();}
    RpcEndExcept
    error=common_rpc_end_async(&async,error,NULL,INFINITE);
    if(error){if(result)MIDL_user_free(result);return error;}
    *coverage=flags;*count=actual;*items=result;
    return ERROR_SUCCESS;
}

DWORD common_rpc_task_snapshot(const common_rpc_management *state,
    ULONG *count,DTASKMGR_WORKER **items)
{
    RPC_ASYNC_STATE async;
    ULONG result_count=0;
    DTASKMGR_WORKER *result=NULL;
    DWORD error=ERROR_INVALID_STATE;
    if (!count || !items) return ERROR_INVALID_PARAMETER;
    *count=0;*items=NULL;
    if (!state || !state->binding || !state->process) return error;
    error=common_rpc_begin_async(&async);if(error)return error;
    RpcTryExcept {
        Client_TaskSnapshot(&async,state->binding,state->process,APP_PROTOCOL_VERSION,
            (unsigned char *)app_version,&result_count,&result);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    error=common_rpc_end_async(&async,error,NULL,INFINITE);
    if (error) {
        if (result) MIDL_user_free(result);
        return error;
    }
    *count=result_count;*items=result;
    return ERROR_SUCCESS;
}

DWORD common_rpc_close_management_node(const common_rpc_management *state,const DTASKMGR_KEY *key)
{
    RPC_ASYNC_STATE async;DWORD error=ERROR_INVALID_STATE;
    if (!state || !state->binding || !state->process) return error;
    if(!key)return ERROR_INVALID_PARAMETER;
    error=common_rpc_begin_async(&async);if(error)return error;
    RpcTryExcept {
        Client_CloseManagementNode(&async,state->binding,state->process,APP_PROTOCOL_VERSION,
            (unsigned char *)app_version,(DTASKMGR_KEY *)key);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    return common_rpc_end_async(&async,error,NULL,INFINITE);
}
