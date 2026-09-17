/* Smallest public-RPC replacement for the unavailable NT4 CSR port.  This
 * file owns no BaseSrv policy: it authenticates/binds one local process and
 * forwards only operations with an implemented copied endpoint. */
#include <windows.h>
#include <rpc.h>
#include <stdlib.h>
#include <stdio.h>
#include "service.h"
#include "basesrv-exe/transport/rpc_security.h"
#include "basesrv-exe/transport/vdm_receipt.h"
#include <base_client.h>
#include <base_command.h>
#include <base_rpc_client.h>
#include "product-abi/version.h" /* Shared metadata, no product behavior. */

typedef struct OPENNT_BASE_RPC_CLIENT {
    RPC_BINDING_HANDLE binding;
    VDM_CONNECTION connection;
    HANDLE process;
    HANDLE server;
    HANDLE stop;
    HANDLE watcher;
    ULONG generation;
} OPENNT_BASE_RPC_CLIENT;

static OPENNT_BASE_RPC_CLIENT client;
static LONG request_id;
static HANDLE parent_event_handle;
static ULONG parent_receipt;
/* Get delivers this source-shaped worker wait through a typed attachment.
 * ExitVDM later receives only the original broker receipt, so retain the
 * worker-local duplicate here until the original ExitVDM completion says to
 * close it.  This is one VDM client's single ConsoleRecord wait, not a task
 * scheduler or a reusable-worker policy. */
static HANDLE worker_wait_event;

/* Owner-approved standalone failure containment, not guest termination or
 * BaseSrv scheduling. Never reconnect a live command to a replacement server.
 * TerminateProcess avoids deadlocking in CRT/DLL teardown while other threads
 * are inside guest or RPC code after the service has disappeared. */
static DWORD WINAPI broker_lifetime_watch(void *context)
{
    OPENNT_BASE_RPC_CLIENT *state=context;
    HANDLE waits[2]={state->stop,state->server};
    DWORD result=WaitForMultipleObjects(2,waits,FALSE,INFINITE);
    if (result!=WAIT_OBJECT_0) {
        /* Do not write redirected stderr here: a full pipe can itself block
         * forever. The process exit status is the failure witness. */
        TerminateProcess(GetCurrentProcess(),RPC_S_SERVER_UNAVAILABLE);
    }
    return 0;
}

static BOOL receipt_seen(const ULONG receipts[3],ULONG limit,ULONG receipt)
{
    ULONG index;
    for (index=0;index<limit;++index) if (receipts[index]==receipt) return TRUE;
    return FALSE;
}

/* A retained original BaseCheckVDM record can describe an inherited standard
 * stream using the caller's exact handle value.  On current Windows that
 * value is not necessarily an NT4 pseudo Console handle, so identify the
 * caller's three live standard handles before the RPC attachment boundary.
 * This deliberately does not treat a same-type file/pipe as Console. */
static BOOL current_standard_console(HANDLE value,DWORD standard_id)
{
    HANDLE current=GetStdHandle(standard_id);
    DWORD mode;

    return value!=NULL && value==current &&
        GetConsoleMode(current,&mode);
}

/* Default-off adapter-boundary trace.  It records only three Boolean masks,
 * never a native handle, copied command byte, or guest address. */
static void trace_standard_stream_classification(DWORD present,DWORD original,
    DWORD current)
{
    char path[MAX_PATH],line[128];
    DWORD bytes,written;
    HANDLE file;
    int length;

    bytes=GetEnvironmentVariableA("MVDM_BASESRV_TRACE_PATH",path,sizeof(path));
    if (!bytes || bytes>=sizeof(path)) return;
    length=snprintf(line,sizeof(line),
        "BASECLIENT-STREAMS present=%X original-console=%X current-console=%X state=copied\r\n",
        (unsigned int)present,(unsigned int)original,(unsigned int)current);
    if (length<=0 || (size_t)length>=sizeof(line)) return;
    file=CreateFileA(path,FILE_APPEND_DATA,FILE_SHARE_READ,NULL,OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,NULL);
    if (file==INVALID_HANDLE_VALUE) return;
    (void)WriteFile(file,line,(DWORD)length,&written,NULL);
    CloseHandle(file);
}

/* Default-off lifecycle observation at the finite BaseClient transport
 * boundary.  This deliberately records only original ExitVDM branch/result
 * scalars: no handle value, command payload, or guest state crosses it. */
static void trace_exit_vdm(BOOL is_wow,DWORD error,ULONG close_worker_wait)
{
    char path[MAX_PATH],line[128];
    DWORD bytes,written;
    HANDLE file;
    int length;

    bytes=GetEnvironmentVariableA("MVDM_BASESRV_TRACE_PATH",path,sizeof(path));
    if (!bytes || bytes>=sizeof(path)) return;
    length=snprintf(line,sizeof(line),
        "BASECLIENT-EXIT wow=%u error=%08lX close-worker-wait=%lu\r\n",
        is_wow ? 1u : 0u,(unsigned long)error,(unsigned long)close_worker_wait);
    if (length<=0 || (size_t)length>=sizeof(line)) return;
    file=CreateFileA(path,FILE_APPEND_DATA,FILE_SHARE_READ,NULL,OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,NULL);
    if (file==INVALID_HANDLE_VALUE) return;
    (void)WriteFile(file,line,(DWORD)length,&written,NULL);
    CloseHandle(file);
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
    DWORD standard_ids[3]={STD_INPUT_HANDLE,STD_OUTPUT_HANDLE,STD_ERROR_HANDLE};
    DWORD present_mask=0u,original_console_mask=0u,current_console_mask=0u;
    ULONG index,previous;
    DWORD type;
    DWORD error=ERROR_SUCCESS;
    if (!receipts) return ERROR_INVALID_PARAMETER;
    receipts[0]=receipts[1]=receipts[2]=0;
    for (index=0;index<3;++index) {
        BOOL original_console;
        BOOL current_console;
        if (values[index]) present_mask|=1u<<index;
        original_console=OpenNtBaseIsConsoleHandle(values[index]);
        current_console=current_standard_console(values[index],standard_ids[index]);
        if (original_console) original_console_mask|=1u<<index;
        if (current_console) current_console_mask|=1u<<index;
        /* Original BaseCheckVDM/CSR represents the interactive Console via
         * its ConsoleRecord, not through the three redirected standard-stream
         * fields.  The modern Console exposes ordinary Win32 handles, so the
         * cross-process carrier must recognize them before attempting a file
         * attachment.  Pipes and files deliberately remain untouched: they
         * are the original redirection contract and retain their typed
         * receipts below. */
        if (original_console || current_console) {
            values[index]=NULL;
            continue;
        }
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
    trace_standard_stream_classification(present_mask,original_console_mask,
        current_console_mask);
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
    HANDLE parent_event=NULL,*parent_events=NULL;
    ULONG parent_event_count=0,parent_receipt_value=0,replyBytes=0;
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
            wireBytes,wire,&parent_event_count,&parent_events,&parent_receipt_value,&replyBytes,reply);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    if (!error && replyBytes==sizeof(reply) && parent_event_count<=1 &&
        (!parent_event_count || parent_events)) {
        if (parent_event_count) parent_event=parent_events[0];
        applied=OpenNtBaseApplyCheckReply(reply,(uint32_t)replyBytes,client.generation,request,base);
    }
    if (applied && base->u.CheckVDM.VDMState==VDM_PRESENT_AND_READY &&
        parent_event && parent_receipt_value) {
        base->u.CheckVDM.WaitObjectForParent=parent_event;
        parent_event_handle=parent_event;parent_receipt=parent_receipt_value;
        parent_event=NULL;
    } else if (applied && (parent_event || parent_receipt_value)) {
        applied=FALSE;
    }
done:
    if (parent_events) MIDL_user_free(parent_events);
    if (parent_event) CloseHandle(parent_event);
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
        } else {
            /* The scalar reply explicitly says whether original BaseSrv
             * returned a standard stream.  With no typed attachment it is
             * either absent (must become NULL) or already inherited by this
             * worker from the suspended launch; never leave the caller's
             * pre-RPC stack residue in these original result fields. */
            base->u.GetNextVDMCommand.StdIn=
                base->u.GetNextVDMCommand.StdIn ? GetStdHandle(STD_INPUT_HANDLE) : NULL;
            base->u.GetNextVDMCommand.StdOut=
                base->u.GetNextVDMCommand.StdOut ? GetStdHandle(STD_OUTPUT_HANDLE) : NULL;
            base->u.GetNextVDMCommand.StdErr=
                base->u.GetNextVDMCommand.StdErr ? GetStdHandle(STD_ERROR_HANDLE) : NULL;
        }
        if (applied && wait_event) {
            /* Original srvvdm.c reuses its ConsoleRecord event across Get
             * calls. RPC supplies another local duplicate on each reply;
             * retain the first until ExitVDM, not a one-delivery restriction. */
            if (worker_wait_event) {
                CloseHandle(wait_event);
                base->u.GetNextVDMCommand.WaitObjectForVDM=worker_wait_event;
            } else worker_wait_event=wait_event;
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

static NTSTATUS exit_command(PCSR_API_MSG message,ULONG length)
{
    PBASE_EXIT_VDM_MSG exit_message=(PBASE_EXIT_VDM_MSG)&message->u.ApiMessageData;
    DWORD error=ERROR_INVALID_PARAMETER;
    ULONG close_worker_wait=0;
    BOOL is_wow;

    if (length!=sizeof(*exit_message)) goto done;
    is_wow=exit_message->ConsoleHandle==(HANDLE)-1;
    RpcTryExcept {
        error=Client_Exit(client.binding,client.connection,client.process,client.generation,
            is_wow ? 1u : 0u,is_wow ? exit_message->iWowTask : 0u,&close_worker_wait);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    trace_exit_vdm(is_wow,error,close_worker_wait);
    if (error || close_worker_wait>1u || (close_worker_wait && !worker_wait_event)) goto done;
    exit_message->WaitObjectForVDM=close_worker_wait ? worker_wait_event : NULL;
    if (close_worker_wait) worker_wait_event=NULL;
    message->ReturnValue=STATUS_SUCCESS;
    return STATUS_SUCCESS;
done:
    SetLastError(error ? error : ERROR_INVALID_DATA);
    message->ReturnValue=STATUS_UNSUCCESSFUL;
    return STATUS_UNSUCCESSFUL;
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

/* UNKNOWN_IF also occurs while a healthy-version endpoint drains. Query the
 * same authenticated local endpoint before calling it a version mismatch.
 * This never admits work or relaxes the subsequent Connect identity check. */
static DWORD classify_missing_interface(RPC_BINDING_HANDLE binding)
{
    RPC_IF_ID expected;
    RPC_IF_ID_VECTOR *interfaces=NULL;
    RPC_STATUS status,uuid_status;
    unsigned int index;
    DWORD result=RPC_S_SERVER_UNAVAILABLE;
    status=RpcIfInqId(Client_vdm_service_v3_0_c_ifspec,&expected);
    if (status) return status;
    status=RpcMgmtInqIfIds(binding,&interfaces);
    if (status) return status;
    for (index=0;index<interfaces->Count;++index) {
        RPC_IF_ID *peer=interfaces->IfId[index];
        if (UuidEqual(&peer->Uuid,&expected.Uuid,&uuid_status) &&
            (peer->VersMajor!=expected.VersMajor || peer->VersMinor!=expected.VersMinor)) {
            result=ERROR_REVISION_MISMATCH;
            break;
        }
    }
    RpcIfIdVectorFree(&interfaces);
    return result;
}

DWORD OpenNtBaseClientConnectCurrent(void)
{
    static const unsigned char expected[APP_VERSION_BYTES]=APP_VERSION;
    unsigned char server_version[APP_VERSION_BYTES]={0};
    ULONG server_protocol=0;
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
        error=Client_Connect(client.binding,client.process,APP_PROTOCOL_VERSION,
            (unsigned char *)expected,&server_protocol,server_version,&connection,&generation);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    if (error==RPC_S_UNKNOWN_IF) error=classify_missing_interface(client.binding);
    /* A confirmed legacy RPC major cannot decode this Connect signature. */
    if (error==RPC_S_PROCNUM_OUT_OF_RANGE ||
        (error==ERROR_REVISION_MISMATCH && !server_protocol)) {
        fprintf(stderr,"ntvdm client: version mismatch: local protocol=%u app=%s; broker RPC interface incompatible\n",
            APP_PROTOCOL_VERSION,APP_VERSION);
        error=ERROR_REVISION_MISMATCH;
    } else if (error==ERROR_REVISION_MISMATCH || (!error &&
        (server_protocol!=APP_PROTOCOL_VERSION || memcmp(server_version,expected,sizeof(expected))))) {
        fprintf(stderr,"ntvdm client: version mismatch: local protocol=%u app=%s; broker protocol=%lu app=%.32s\n",
            APP_PROTOCOL_VERSION,APP_VERSION,server_protocol,(const char *)server_version);
        error=ERROR_REVISION_MISMATCH;
    }
    /* Retain any returned context solely for cleanup on a rejected response;
     * it never becomes available to original BaseClient task operations. */
    if (connection) { client.connection=connection; client.generation=generation; }
    if (!error && (!connection || !generation)) error=ERROR_INVALID_DATA;
    if (error) goto done;
    RpcTryExcept {
        error=Client_BrokerProcess(client.binding,client.connection,client.process,
            client.generation,&client.server);
    }
    RpcExcept(1) { error=RpcExceptionCode(); }
    RpcEndExcept
    if (error) goto done;
    if (!client.server || WaitForSingleObject(client.server,0)!=WAIT_TIMEOUT) {
        error=RPC_S_SERVER_UNAVAILABLE; goto done;
    }
    error=ERROR_SUCCESS;
done:
    if (text) RpcStringFreeW(&text);
    if (error) OpenNtBaseClientDisconnectCurrent();
    return error;
}

DWORD OpenNtBaseClientWatchBroker(void)
{
    if (!client.server) return ERROR_INVALID_STATE;
    if (client.watcher) return ERROR_ALREADY_EXISTS;
    client.stop=CreateEventW(NULL,TRUE,FALSE,NULL);
    if (!client.stop) return GetLastError();
    client.watcher=CreateThread(NULL,0,broker_lifetime_watch,&client,0,NULL);
    if (!client.watcher) {
        DWORD error=GetLastError();
        CloseHandle(client.stop);client.stop=NULL;
        return error;
    }
    return ERROR_SUCCESS;
}

void OpenNtBaseClientDisconnectCurrent(void)
{
    VDM_CONNECTION connection;
    if (client.watcher) {
        SetEvent(client.stop);
        WaitForSingleObject(client.watcher,INFINITE);
        CloseHandle(client.watcher);
    }
    if (client.stop) CloseHandle(client.stop);
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
    if (client.server) CloseHandle(client.server);
    if (client.binding) RpcBindingFree(&client.binding);
    if (worker_wait_event) CloseHandle(worker_wait_event);
    ZeroMemory(&client,sizeof(client));
    worker_wait_event=NULL;
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
    if (number==CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,BasepExitVDM))
        return exit_command(message,length);
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
