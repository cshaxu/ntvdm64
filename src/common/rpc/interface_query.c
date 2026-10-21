#include "interface_query.h"
#include "async_call.h"
#include "rpc_management.h"

DWORD common_rpc_classify_interface(RPC_BINDING_HANDLE binding,
    const RPC_IF_ID *expected,ULONGLONG deadline)
{
    RPC_ASYNC_STATE call;
    management_if_vector interfaces=NULL;
    DWORD error,reply=ERROR_INVALID_STATE,index,result=RPC_S_SERVER_UNAVAILABLE;
    RPC_STATUS uuid_status;
    if(!binding || !expected)return ERROR_INVALID_PARAMETER;
    if(!common_rpc_remaining_ms(deadline))return ERROR_TIMEOUT;
    error=common_rpc_begin_async(&call);if(error)return error;
    RpcTryExcept {management_inquire_interfaces(&call,binding,&interfaces,&reply);}
    RpcExcept(1){error=RpcExceptionCode();}
    RpcEndExcept
    /* This standard operation has a void return and a separate status output. */
    if(!error)error=common_rpc_finish_async(&call,call.u.hEvent,NULL,
        common_rpc_remaining_ms(deadline),NULL);
    CloseHandle(call.u.hEvent);
    if(!error)error=reply;
    if(!error && !interfaces)error=ERROR_INVALID_DATA;
    if(!error)for(index=0;index<interfaces->Count;++index){
        management_if_id *peer=interfaces->IfId[index];
        if(!peer){error=ERROR_INVALID_DATA;break;}
        if(UuidEqual((UUID *)&peer->Uuid,(UUID *)&expected->Uuid,&uuid_status) &&
            (peer->VersMajor!=expected->VersMajor || peer->VersMinor!=expected->VersMinor)){
            result=ERROR_REVISION_MISMATCH;break;
        }
    }
    /* Outputs may arrive even when cancellation wins. They are not public-API
     * allocations: release with the generated stub's allocator, after drain. */
    if(interfaces){
        for(index=0;index<interfaces->Count;++index)MIDL_user_free(interfaces->IfId[index]);
        MIDL_user_free(interfaces);
    }
    return error ? error : result;
}
