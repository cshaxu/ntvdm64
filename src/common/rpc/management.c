/* Existing project management exchanges, not original VDM record policy. */
#include "management.h"
#include "common/protocol/version.h"
#include <rpcasync.h>

static const unsigned char app_version[APP_VERSION_BYTES]=APP_VERSION;

DWORD common_rpc_observe_native_creation_bounded(const common_rpc_management *state,HANDLE child,
    DWORD flags,uint64_t *node)
{
    RPC_ASYNC_STATE async={0};hyper identity=0;DWORD reply=ERROR_INVALID_STATE,error,wait;
    HANDLE completed=NULL;BOOL issued=FALSE;
    if(!node)return ERROR_INVALID_PARAMETER;
    *node=0;
    if(!state || !state->binding || !state->process || !child)return ERROR_INVALID_STATE;
    error=RpcAsyncInitializeHandle(&async,sizeof(async));if(error)return error;
    completed=CreateEventW(NULL,TRUE,FALSE,NULL);if(!completed)return GetLastError();
    async.NotificationType=RpcNotificationTypeEvent;async.u.hEvent=completed;
    RpcTryExcept {
        Client_ObserveNativeCreationAsync(&async,state->binding,state->process,APP_PROTOCOL_VERSION,
            (unsigned char *)app_version,child,flags,&identity);
        issued=TRUE;error=ERROR_SUCCESS;
    }
    RpcExcept(1) {error=RpcExceptionCode();}
    RpcEndExcept
    if(issued) {
        wait=WaitForSingleObject(completed,250);
        if(wait!=WAIT_OBJECT_0) {
            /* Abortive cancellation completes locally without waiting for
             * the server's reply. State/inputs still live until notification. */
            error=RpcAsyncCancelCall(&async,TRUE);
            /* Even cancellation failure cannot justify freeing pending call
             * state. Drain its actual notification, then complete it. */
            if(WaitForSingleObject(completed,INFINITE)!=WAIT_OBJECT_0)
                error=GetLastError();
        }
        {DWORD completion=RpcAsyncCompleteCall(&async,&reply);if(!error)error=completion;}
        if(!error)error=reply;
    }
    CloseHandle(completed);
    if(!error)*node=(uint64_t)identity;
    return error;
}

DWORD common_rpc_worker_task_trace(const common_rpc_management *state,const DTASKMGR_KEY *key,
    ULONG *coverage,ULONG *count,WORKER_TRACE_NODE **items)
{
    DWORD error=ERROR_INVALID_STATE;
    ULONG flags=0,actual=0;
    WORKER_TRACE_NODE *result=NULL;
    if(!coverage || !count || !items)return ERROR_INVALID_PARAMETER;
    *coverage=0;*count=0;*items=NULL;
    if(!key)return ERROR_INVALID_PARAMETER;
    if(!state || !state->binding || !state->process)return error;
    RpcTryExcept {
        error=Client_WorkerTaskTrace(state->binding,state->process,APP_PROTOCOL_VERSION,
            (unsigned char *)app_version,(DTASKMGR_KEY *)key,&flags,&actual,&result);
    }
    RpcExcept(1) {error=RpcExceptionCode();}
    RpcEndExcept
    if(error){if(result)MIDL_user_free(result);return error;}
    *coverage=flags;*count=actual;*items=result;
    return ERROR_SUCCESS;
}

DWORD common_rpc_task_snapshot(const common_rpc_management *state,
    ULONG *count,DTASKMGR_WORKER **items)
{
    ULONG result_count=0;
    DTASKMGR_WORKER *result=NULL;
    DWORD error=ERROR_INVALID_STATE;
    if (!count || !items) return ERROR_INVALID_PARAMETER;
    *count=0;*items=NULL;
    if (!state || !state->binding || !state->process) return error;
    RpcTryExcept {
        error=Client_TaskSnapshot(state->binding,state->process,APP_PROTOCOL_VERSION,
            (unsigned char *)app_version,&result_count,&result);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    if (error) {
        if (result) MIDL_user_free(result);
        return error;
    }
    *count=result_count;*items=result;
    return ERROR_SUCCESS;
}

DWORD common_rpc_close_management_node(const common_rpc_management *state,const DTASKMGR_KEY *key)
{
    DWORD error=ERROR_INVALID_STATE;
    if (!state || !state->binding || !state->process) return error;
    if(!key)return ERROR_INVALID_PARAMETER;
    RpcTryExcept {
        error=Client_CloseManagementNode(state->binding,state->process,APP_PROTOCOL_VERSION,
            (unsigned char *)app_version,(DTASKMGR_KEY *)key);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    return error;
}
