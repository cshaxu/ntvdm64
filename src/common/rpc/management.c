/* Existing project management exchanges, not original VDM record policy. */
#include "management.h"
#include "common/protocol/version.h"

static const unsigned char app_version[APP_VERSION_BYTES]=APP_VERSION;

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
