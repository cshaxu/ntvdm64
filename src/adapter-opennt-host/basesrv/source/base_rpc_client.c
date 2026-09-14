/* Smallest public-RPC replacement for the unavailable NT4 CSR port.  This
 * file owns no BaseSrv policy: it authenticates/binds one local process and
 * forwards only operations with an implemented copied endpoint. */
#include <windows.h>
#include <rpc.h>
#include <stdlib.h>
#include "service.h"
#include "broker/rpc_security.h"
#include "broker/vdm_receipt.h"
#include <base_client.h>
#include <base_command.h>
#include <base_rpc_client.h>

typedef struct OPENNT_BASE_RPC_CLIENT {
    RPC_BINDING_HANDLE binding;
    VDM_CONNECTION connection;
    HANDLE process;
    ULONG generation;
} OPENNT_BASE_RPC_CLIENT;

static OPENNT_BASE_RPC_CLIENT client;
static LONG request_id;
static HANDLE parent_event_handle;
static ULONG parent_receipt;

static BOOL receipt_seen(const ULONG receipts[3],ULONG limit,ULONG receipt)
{
    ULONG index;
    for (index=0;index<limit;++index) if (receipts[index]==receipt) return TRUE;
    return FALSE;
}

/* Generated client stubs own only their transient marshalling buffers. */
void *__RPC_USER MIDL_user_allocate(size_t bytes) { return malloc(bytes); }
void __RPC_USER MIDL_user_free(void *value) { free(value); }

static NTSTATUS rpc_failure(DWORD error)
{
    (void)error;
    return STATUS_UNSUCCESSFUL;
}

/* BaseCheckVDM is still the original classifier and record publisher.  Its
 * three standard fields are the sole resource inputs: replace their local
 * HANDLE spelling with typed attachment receipts before the copied request
 * crosses the broker, preserving stdout/stderr aliases. */
static DWORD attach_standard_streams(PBASE_API_MSG message,ULONG receipts[3])
{
    HANDLE values[3]={message->u.CheckVDM.StdIn,message->u.CheckVDM.StdOut,
        message->u.CheckVDM.StdErr};
    ULONG roles[3]={BROKER_VDM_STDIN,BROKER_VDM_STDOUT,BROKER_VDM_STDERR};
    ULONG index,previous;
    DWORD type;
    DWORD error=ERROR_SUCCESS;
    if (!receipts) return ERROR_INVALID_PARAMETER;
    receipts[0]=receipts[1]=receipts[2]=0;
    for (index=0;index<3;++index) {
        /* COMMAND's native-child startup can carry the original DOS
         * INVALID_HANDLE_VALUE stdin sentinel.  It denotes no inheritable
         * stream, not an OS resource eligible for typed attachment. */
        if (!values[index] || values[index]==INVALID_HANDLE_VALUE) continue;
        SetLastError(ERROR_SUCCESS);
        type=GetFileType(values[index]);
        if (type==FILE_TYPE_UNKNOWN && GetLastError()!=ERROR_SUCCESS) continue;
        for (previous=0;previous<index;++previous) if (values[previous]==values[index]) {
            receipts[index]=receipts[previous]; break;
        }
        if (previous<index) continue;
        RpcTryExcept {
            if (type==FILE_TYPE_PIPE)
                error=Client_AttachPipe(client.binding,client.connection,client.process,
                    client.generation,roles[index],values[index],&receipts[index]);
            else
                error=Client_AttachFile(client.binding,client.connection,client.process,
                    client.generation,roles[index],values[index],&receipts[index]);
        }
        RpcExcept(1) { error=RpcExceptionCode(); }
        RpcEndExcept
        if (error || !receipts[index]) break;
    }
    if (error || index!=3) {
        for (index=0;index<3;++index) if (receipts[index] &&
            !receipt_seen(receipts,index,receipts[index])) {
            RpcTryExcept { (void)Client_RevokeStream(client.binding,client.connection,
                client.process,client.generation,receipts[index]); }
            RpcExcept(1) { }
            RpcEndExcept
        }
        receipts[0]=receipts[1]=receipts[2]=0;
        return error ? error : ERROR_INVALID_HANDLE;
    }
    return ERROR_SUCCESS;
}

static NTSTATUS check_command(PCSR_API_MSG message,ULONG length)
{
    PBASE_API_MSG base=(PBASE_API_MSG)message;
    BASE_API_MSG forwarded;
    unsigned char reply[40];
    void *wire=NULL;
    uint32_t request=(uint32_t)InterlockedIncrement(&request_id),wireBytes=0;
    ULONG replyBytes=0;
    DWORD error=ERROR_INVALID_DATA;
    BOOL applied=FALSE;
    ULONG streams[3]={0,0,0};
    if (!request) request=(uint32_t)InterlockedIncrement(&request_id);
    if (length!=sizeof(BASE_CHECKVDM_MSG) ||
        (error=attach_standard_streams(base,streams))!=ERROR_SUCCESS) goto done;
    forwarded=*base;
    forwarded.u.CheckVDM.StdIn=(HANDLE)(ULONG_PTR)streams[0];
    forwarded.u.CheckVDM.StdOut=(HANDLE)(ULONG_PTR)streams[1];
    forwarded.u.CheckVDM.StdErr=(HANDLE)(ULONG_PTR)streams[2];
    if (!OpenNtBaseEncodeCheckCommand(&forwarded,request,client.generation,NULL,0,&wireBytes) ||
        !(wire=HeapAlloc(GetProcessHeap(),0,wireBytes)) ||
        !OpenNtBaseEncodeCheckCommand(&forwarded,request,client.generation,wire,wireBytes,&wireBytes))
        goto done;
    RpcTryExcept {
        error=Client_Check(client.binding,client.connection,client.process,client.generation,
            wireBytes,wire,&replyBytes,reply);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    if (!error && replyBytes==sizeof(reply))
        applied=OpenNtBaseApplyCheckReply(reply,(uint32_t)replyBytes,client.generation,request,base);
done:
    if (wire) HeapFree(GetProcessHeap(),0,wire);
    if (error || !applied) {
        ULONG index;
        for (index=0;index<3;++index) if (streams[index] &&
            !receipt_seen(streams,index,streams[index])) {
            RpcTryExcept { (void)Client_RevokeStream(client.binding,client.connection,
                client.process,client.generation,streams[index]); }
            RpcExcept(1) { }
            RpcEndExcept
        }
        SetLastError(error ? error : ERROR_INVALID_DATA);
        message->ReturnValue=(ULONG)STATUS_UNSUCCESSFUL;
        return STATUS_UNSUCCESSFUL;
    }
    return (NTSTATUS)message->ReturnValue;
}

static NTSTATUS get_command(PCSR_API_MSG message,ULONG length)
{
    PBASE_API_MSG base=(PBASE_API_MSG)message;
    uint32_t request=(uint32_t)InterlockedIncrement(&request_id),wire_bytes=0;
    unsigned char *reply=NULL;
    void *wire=NULL;
    HANDLE wait_event=NULL;
    HANDLE *wait_events=NULL;
    HANDLE *stream_handles=NULL;
    ULONG wait_event_count=0;
    ULONG stream_count=0;
    ULONG reply_bytes=0;
    DWORD error=ERROR_INVALID_DATA;
    BOOL applied=FALSE;
    if (!request) request=(uint32_t)InterlockedIncrement(&request_id);
    if (length!=sizeof(BASE_GET_NEXT_VDM_COMMAND_MSG) ||
        !OpenNtBaseEncodeGetCommand(base,request,client.generation,NULL,0,&wire_bytes) ||
        !(wire=HeapAlloc(GetProcessHeap(),0,wire_bytes)) ||
        !OpenNtBaseEncodeGetCommand(base,request,client.generation,wire,wire_bytes,&wire_bytes))
        goto done;
    RpcTryExcept {
        error=Client_Get(client.binding,client.connection,client.process,client.generation,
            wire_bytes,wire,&wait_event_count,&wait_events,&stream_count,&stream_handles,
            &reply_bytes,&reply);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    if (!error && wait_event_count<=1 && (!wait_event_count || wait_events) &&
        stream_count<=3 && (!stream_count || stream_handles) && reply && reply_bytes) {
        if (wait_event_count) wait_event=wait_events[0];
        applied=OpenNtBaseApplyGetCommand(reply,(uint32_t)reply_bytes,client.generation,request,base);
    }
    if (applied) {
        base->u.GetNextVDMCommand.WaitObjectForVDM=wait_event;
        if (stream_count) {
            if (stream_count!=3 ||
                (!!base->u.GetNextVDMCommand.StdIn != !!stream_handles[0]) ||
                (!!base->u.GetNextVDMCommand.StdOut != !!stream_handles[1]) ||
                (!!base->u.GetNextVDMCommand.StdErr != !!stream_handles[2])) {
                applied=FALSE;
            } else {
                base->u.GetNextVDMCommand.StdIn=stream_handles[0];
                base->u.GetNextVDMCommand.StdOut=stream_handles[1];
                base->u.GetNextVDMCommand.StdErr=stream_handles[2];
            }
        }
        /* Update may have copied standard streams directly into the already
         * registered suspended worker before it connected.  In that original
         * timing path these are already valid worker-local handles, so Get
         * carries no second typed attachment. */
        wait_event=NULL;
    }
done:
    if (wait_events) MIDL_user_free(wait_events);
    if (stream_handles) MIDL_user_free(stream_handles);
    if (wait_event) CloseHandle(wait_event);
    if (reply) MIDL_user_free(reply);
    if (wire) HeapFree(GetProcessHeap(),0,wire);
    if (error || !applied) {
        SetLastError(error ? error : ERROR_INVALID_DATA);
        message->ReturnValue=(ULONG)STATUS_UNSUCCESSFUL;
        return STATUS_UNSUCCESSFUL;
    }
    return (NTSTATUS)message->ReturnValue;
}

static NTSTATUS update_command(PCSR_API_MSG message,ULONG length)
{
    PBASE_API_MSG base=(PBASE_API_MSG)message;
    unsigned char reply[32];
    void *wire=NULL;
    HANDLE parent_event=NULL,*parent_events=NULL;
    uint32_t request=(uint32_t)InterlockedIncrement(&request_id),wire_bytes=0;
    ULONG parent_event_count=0,reply_bytes=0,receipt=0;
    DWORD error=ERROR_INVALID_DATA;
    BOOL applied=FALSE;
    if (!request) request=(uint32_t)InterlockedIncrement(&request_id);
    if (length!=sizeof(BASE_UPDATE_VDM_ENTRY_MSG) ||
        !OpenNtBaseEncodeUpdateCommand(base,request,client.generation,NULL,0,&wire_bytes) ||
        !(wire=HeapAlloc(GetProcessHeap(),0,wire_bytes)) ||
        !OpenNtBaseEncodeUpdateCommand(base,request,client.generation,wire,wire_bytes,&wire_bytes))
        goto done;
    RpcTryExcept {
        error=Client_Update(client.binding,client.connection,client.process,client.generation,
            wire_bytes,wire,&parent_event_count,&parent_events,&receipt,&reply_bytes,reply);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    if (!error && reply_bytes==sizeof(reply) && parent_event_count<=1 &&
        (!parent_event_count || parent_events)) {
        if (parent_event_count) parent_event=parent_events[0];
        applied=OpenNtBaseApplyUpdateReply(reply,(uint32_t)reply_bytes,client.generation,request,base);
    }
    if (applied) {
        base->u.UpdateVDMEntry.WaitObjectForParent=parent_event;
        parent_event_handle=parent_event;
        parent_receipt=receipt;
        parent_event=NULL;
    }
done:
    if (parent_events) MIDL_user_free(parent_events);
    if (parent_event) CloseHandle(parent_event);
    if (wire) HeapFree(GetProcessHeap(),0,wire);
    if (error || !applied) {
        SetLastError(error ? error : ERROR_INVALID_DATA);
        message->ReturnValue=(ULONG)STATUS_UNSUCCESSFUL;
        return STATUS_UNSUCCESSFUL;
    }
    return (NTSTATUS)message->ReturnValue;
}

static NTSTATUS exit_code_command(PCSR_API_MSG message,ULONG length)
{
    PBASE_GET_VDM_EXIT_CODE_MSG exit_message=(PBASE_GET_VDM_EXIT_CODE_MSG)&message->u.ApiMessageData;
    DWORD error=ERROR_INVALID_PARAMETER;
    ULONG exit_code=0;
    if (length!=sizeof(*exit_message) || exit_message->hParent!=parent_event_handle || !parent_receipt)
        goto done;
    RpcTryExcept {
        error=Client_ExitCode(client.binding,client.connection,client.process,client.generation,
            parent_receipt,&exit_code);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    if (!error) {
        exit_message->ExitCode=exit_code;
        parent_event_handle=NULL;parent_receipt=0;
        message->ReturnValue=STATUS_SUCCESS;
        return STATUS_SUCCESS;
    }
done:
    SetLastError(error);
    message->ReturnValue=STATUS_UNSUCCESSFUL;
    return STATUS_UNSUCCESSFUL;
}

static NTSTATUS reenter_command(PCSR_API_MSG message,ULONG length)
{
    PBASE_SET_REENTER_COUNT_MSG reenter=(PBASE_SET_REENTER_COUNT_MSG)&message->u.ApiMessageData;
    DWORD error=ERROR_INVALID_PARAMETER;

    if (length!=sizeof(*reenter) ||
        (reenter->fIncDec!=INCREMENT_REENTER_COUNT &&
         reenter->fIncDec!=DECREMENT_REENTER_COUNT)) goto done;
    RpcTryExcept {
        error=Client_Reenter(client.binding,client.connection,client.process,client.generation,
            reenter->fIncDec);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    if (!error) {
        message->ReturnValue=STATUS_SUCCESS;
        return STATUS_SUCCESS;
    }
done:
    SetLastError(error);
    message->ReturnValue=STATUS_UNSUCCESSFUL;
    return STATUS_UNSUCCESSFUL;
}

DWORD OpenNtBaseClientConnectCurrent(void)
{
    broker_rpc_scope scope;
    RPC_WSTR text=NULL;
    WCHAR endpoint[128];
    DWORD error=ERROR_GEN_FAILURE;
    ULONG generation=0;
    VDM_CONNECTION connection=NULL;

    if (client.connection) return ERROR_ALREADY_EXISTS;
    if (!broker_rpc_capture_scope(&scope)) return GetLastError();
    wsprintfW(endpoint,L"ntvdm-basesrv-%lu-%08lx-%08lx",scope.session,
        (ULONG)scope.logon.HighPart,scope.logon.LowPart);
    if (RpcStringBindingComposeW(NULL,(RPC_WSTR)L"ncalrpc",NULL,
            (RPC_WSTR)endpoint,NULL,&text)) goto done;
    if (RpcBindingFromStringBindingW(text,&client.binding)) goto done;
    if (RpcBindingSetAuthInfoW(client.binding,NULL,RPC_C_AUTHN_LEVEL_PKT_PRIVACY,
            RPC_C_AUTHN_WINNT,NULL,RPC_C_AUTHZ_NONE)) goto done;
    /* Original BaseSrvUpdateDOSEntry duplicates the authenticated worker's
     * self pseudo-handle; no broader process access is needed. */
    client.process=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION|SYNCHRONIZE|PROCESS_DUP_HANDLE,
        FALSE,GetCurrentProcessId());
    if (!client.process) { error=GetLastError(); goto done; }
    RpcTryExcept {
        error=Client_Connect(client.binding,client.process,&connection,&generation);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    if (error || !connection || !generation) goto done;
    client.connection=connection;
    client.generation=generation;
    error=ERROR_SUCCESS;
done:
    if (text) RpcStringFreeW(&text);
    if (error) OpenNtBaseClientDisconnectCurrent();
    return error;
}

void OpenNtBaseClientDisconnectCurrent(void)
{
    VDM_CONNECTION connection;
    if (client.connection && client.binding && client.process) {
        connection=client.connection;
        RpcTryExcept {
            (void)Client_Disconnect(client.binding,client.process,
                client.generation,&connection);
        }
        RpcExcept(1) { }
        RpcEndExcept
    }
    if (client.process) CloseHandle(client.process);
    if (client.binding) RpcBindingFree(&client.binding);
    ZeroMemory(&client,sizeof(client));
}

DWORD OpenNtBaseClientReserveWorker(ULONG task,uint64_t *reservation)
{
    hyper id=0;
    DWORD error=ERROR_INVALID_STATE;
    if (!reservation) return ERROR_INVALID_PARAMETER;
    *reservation=0;
    if (!client.connection || !client.binding || !client.process) return error;
    RpcTryExcept {
        error=Client_Reserve(client.binding,client.connection,client.process,
            client.generation,task,&id);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    if (!error && id>0) *reservation=(uint64_t)id;
    else if (!error) error=ERROR_INVALID_DATA;
    return error;
}

DWORD OpenNtBaseClientPrepareWorker(uint64_t reservation,HANDLE worker)
{
    DWORD error=ERROR_INVALID_STATE;
    if (!reservation || !worker || worker==INVALID_HANDLE_VALUE) return ERROR_INVALID_PARAMETER;
    if (!client.connection || !client.binding || !client.process) return error;
    RpcTryExcept {
        error=Client_Prepare(client.binding,client.connection,client.process,
            client.generation,(hyper)reservation,worker);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    return error;
}

DWORD OpenNtBaseClientReleaseWorker(uint64_t reservation)
{
    DWORD error=ERROR_INVALID_STATE;
    if (!reservation) return ERROR_INVALID_PARAMETER;
    if (!client.connection || !client.binding || !client.process) return error;
    RpcTryExcept {
        error=Client_Release(client.binding,client.connection,client.process,
            client.generation,(hyper)reservation);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    return error;
}

NTSTATUS NTAPI OpenNtBaseClientCallServer(PCSR_API_MSG message,
    PCSR_CAPTURE_HEADER capture,CSR_API_NUMBER number,ULONG length)
{
    ULONG first=0;
    DWORD error=ERROR_NOT_SUPPORTED;
    (void)capture;
    if (!message || !client.connection || !client.binding || !client.process)
        return STATUS_UNSUCCESSFUL;
    if (number==CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,BasepCheckVDM))
        return check_command(message,length);
    if (number==CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,BasepGetNextVDMCommand))
        return get_command(message,length);
    if (number==CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,BasepUpdateVDMEntry))
        return update_command(message,length);
    if (number==CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,BasepGetVDMExitCode))
        return exit_code_command(message,length);
    if (number==CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,BasepSetReenterCount))
        return reenter_command(message,length);
    if (number!=CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,BasepIsFirstVDM) ||
        length!=sizeof(BASE_IS_FIRST_VDM_MSG)) {
        message->ReturnValue=(ULONG)STATUS_UNSUCCESSFUL;
        return STATUS_UNSUCCESSFUL;
    }
    RpcTryExcept {
        error=Client_First(client.binding,client.connection,client.process,
            client.generation,&first);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    if (error) {
        message->ReturnValue=rpc_failure(error);
        return (NTSTATUS)message->ReturnValue;
    }
    ((PBASE_IS_FIRST_VDM_MSG)&message->u.ApiMessageData)->FirstVDM=first;
    message->ReturnValue=STATUS_SUCCESS;
    return STATUS_SUCCESS;
}
