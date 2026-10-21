/* Launch/execution extension of the NTSRV-owned RPC client.
 * NTCON does not link this translation unit. Connection and shutdown remain
 * core-owned; these calls borrow the same authenticated connection. */
#include <windows.h>
#include <rpc.h>
#include "service.h"
#include <base_client.h>
#include <base_command.h>
#include <base_service.h>
#include <base_rpc_client.h>
#include <base_rpc_client_internal.h>
#include <base_rpc_client_launch_style.h>
#include "ntsrv-exe/transport/vdm_receipt.h"
#include "common/rpc/async_call.h"
#include "common/protocol/frontend_lifecycle.h"

static struct {
    DWORD request_window_style;
    DWORD dos_window_style;
    DWORD separate_wow_style;
    BOOL separate_wow_style_valid;
    uint64_t observation_direct; /* Captured on actual Get delivery, not a selector. */
    DWORD (*command_ready)(void *);
    void *command_context;
} launch;
static LONG request_id;
static HANDLE parent_event_handle;
static ULONG parent_receipt;
/* Get delivers this source-shaped worker wait through a typed attachment.
 * ExitVDM later receives only the original broker receipt, so retain the
 * worker-local duplicate here until the original ExitVDM completion says to
 * close it.  This is one VDM client's single ConsoleRecord wait, not a task
 * scheduler or a reusable-worker policy. */
static HANDLE worker_wait_event;

static void reset_launch(void)
{
    if(worker_wait_event)CloseHandle(worker_wait_event);
    worker_wait_event=NULL;
    parent_event_handle=NULL;
    parent_receipt=0;
    ZeroMemory(&launch,sizeof(launch));
}

DWORD OpenNtBaseClientSetWindowStyle(DWORD style)
{
    (void)OpenNtBaseClientExecutionConnection(reset_launch);
    if(!WINDOW_STYLE_VALID(style))return ERROR_INVALID_PARAMETER;
    launch.request_window_style=style;return ERROR_SUCCESS;
}

DWORD OpenNtBaseClientDosWindowStyle(void)
{
    (void)OpenNtBaseClientExecutionConnection(reset_launch);
    /* Read on the guest execution thread when COMMAND captures its inputs.
     * The detached CreateProcess thread consumes that copy, never this value.
     * A successful parent resume restores the policy before the next capture. */
    return launch.dos_window_style;
}

DWORD OpenNtBaseClientSeparateWowStyle(DWORD *style)
{
    (void)OpenNtBaseClientExecutionConnection(reset_launch);
    if(!style)return ERROR_INVALID_PARAMETER;
    if(!launch.separate_wow_style_valid)return ERROR_INVALID_STATE;
    *style=launch.separate_wow_style;return ERROR_SUCCESS;
}

uint64_t OpenNtBaseClientObservationDirect(void)
{
    (void)OpenNtBaseClientExecutionConnection(reset_launch);
    /* Read only by the same original guest execution thread as GetNext. */
    return launch.observation_direct;
}

void OpenNtBaseClientSetCommandBinding(DWORD (*ready)(void *),void *context)
{
    (void)OpenNtBaseClientExecutionConnection(reset_launch);
    launch.command_ready=ready;launch.command_context=context;
}

static BOOL receipt_seen(const ULONG receipts[3],ULONG limit,ULONG receipt)
{
    ULONG index;
    for (index=0;index<limit;++index) if (receipts[index]==receipt) return TRUE;
    return FALSE;
}

/* Roll back each distinct attachment once; terminal transport failure leaves
 * the authenticated connection rundown responsible for service resources. */
static void revoke_streams(const common_rpc_connection *client,const ULONG receipts[3])
{
    ULONG index;
    for(index=0;!OpenNtBaseClientFailure(0) && index<3;++index) {
        RPC_ASYNC_STATE async;DWORD error;
        if(!receipts[index] || receipt_seen(receipts,index,receipts[index]))continue;
        error=common_rpc_begin_async(&async);if(error)break;
        RpcTryExcept {Client_RevokeStream(&async,client->binding,client->connection,
            client->process,client->generation,receipts[index]);}
        RpcExcept(1){error=RpcExceptionCode();}
        RpcEndExcept
        (void)OpenNtBaseClientFailure(common_rpc_end_async(&async,error,NULL,
            OPENNT_FRONTEND_LIFECYCLE_TIMEOUT_MS));
    }
}

/* Recognize only the caller's exact standard Console handle, not an
 * unrelated same-type file/pipe; preserve the original stream contract. */
static BOOL current_standard_console(HANDLE value,DWORD standard_id)
{
    HANDLE current=GetStdHandle(standard_id);
    DWORD mode;

    return value!=NULL && value==current &&
        GetConsoleMode(current,&mode);
}

static NTSTATUS rpc_failure(DWORD error)
{
    error=OpenNtBaseClientFailure(error);
    SetLastError(error);
    if(OpenNtBaseClientFailure(0)==ERROR_INVALID_HANDLE)return STATUS_PORT_DISCONNECTED;
    return STATUS_UNSUCCESSFUL;
}

/* BaseCheckVDM remains the classifier/record publisher. Its standard fields
 * cross RPC as typed attachments, preserving stdout/stderr aliasing. */
static DWORD attach_standard_streams(PBASE_API_MSG message,ULONG receipts[3])
{
    common_rpc_connection client=OpenNtBaseClientExecutionConnection(reset_launch);
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
        {
            RPC_ASYNC_STATE async;
            error=OpenNtBaseClientFailure(0);
            if(!error)error=common_rpc_begin_async(&async);
            if(!error) {
                RpcTryExcept {
                    if (type==FILE_TYPE_PIPE)
                        Client_AttachPipe(&async,client.binding,client.connection,client.process,
                            client.generation,roles[index],values[index],&receipts[index]);
                    else
                        Client_AttachFile(&async,client.binding,client.connection,client.process,
                            client.generation,roles[index],values[index],&receipts[index]);
                }
                RpcExcept(1) { error=RpcExceptionCode(); }
                RpcEndExcept
                error=OpenNtBaseClientFailure(common_rpc_end_async(&async,error,NULL,
                    OPENNT_FRONTEND_LIFECYCLE_TIMEOUT_MS));
            }
        }
        if (error || !receipts[index]) break;
    }
    if (error || index!=3) {
        error=OpenNtBaseClientFailure(error ? error : ERROR_INVALID_HANDLE);
        revoke_streams(&client,receipts);
        receipts[0]=receipts[1]=receipts[2]=0;
        return error ? error : ERROR_INVALID_HANDLE;
    }
    return ERROR_SUCCESS;
}

/* Returned attachments may arrive even when cancellation wins the reply.
 * Transfer consumed handles by clearing their slot; close every other copy. */
static void release_handle_array(HANDLE *values,ULONG count)
{
    ULONG index;
    if(!values)return;
    for(index=0;index<count;++index)if(values[index])CloseHandle(values[index]);
    MIDL_user_free(values);
}

static NTSTATUS check_command(PCSR_API_MSG message,ULONG length)
{
    common_rpc_connection client=OpenNtBaseClientExecutionConnection(reset_launch);
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
        !OpenNtBaseEncodeCheckCommand(&forwarded,request,client.generation,wire,wireBytes,&wireBytes)
        ||
        !OpenNtBaseWriteCheckStyle(wire,wireBytes,launch.request_window_style))
        goto done;
    {
        RPC_ASYNC_STATE async;
        error=OpenNtBaseClientFailure(0);
        if(!error)error=common_rpc_begin_async(&async);
        if(!error) {
            RpcTryExcept {
                Client_Check(&async,client.binding,client.connection,client.process,client.generation,
                    wireBytes,wire,&parent_event_count,&parent_events,&parent_receipt_value,&replyBytes,reply);
            }
            RpcExcept(1) { error=RpcExceptionCode(); }
            RpcEndExcept
            error=OpenNtBaseClientFailure(common_rpc_end_async(&async,error,NULL,
                OPENNT_FRONTEND_LIFECYCLE_TIMEOUT_MS));
        }
    }
    if (!error && replyBytes==sizeof(reply) && parent_event_count<=1 &&
        (!parent_event_count || parent_events)) {
        if (parent_event_count) {parent_event=parent_events[0];parent_events[0]=NULL;}
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
    release_handle_array(parent_events,parent_event_count);
    if (parent_event) CloseHandle(parent_event);
    if (wire) HeapFree(GetProcessHeap(),0,wire);
    if (error || !applied) {
        error=OpenNtBaseClientFailure(error ? error : ERROR_INVALID_DATA);
        revoke_streams(&client,streams);
        message->ReturnValue=rpc_failure(error);
        return message->ReturnValue;
    }
    return (NTSTATUS)message->ReturnValue;
}

static NTSTATUS get_command(PCSR_API_MSG message,ULONG length)
{
    common_rpc_connection client=OpenNtBaseClientExecutionConnection(reset_launch);
    PBASE_API_MSG base=(PBASE_API_MSG)message;
    uint32_t request=(uint32_t)InterlockedIncrement(&request_id),wire_bytes=0;
    unsigned char *reply=NULL;
    void *wire=NULL;
    HANDLE wait_event=NULL;
    HANDLE *wait_events=NULL;
    HANDLE *pipe_handles=NULL;
    HANDLE *file_handles=NULL;
    ULONG wait_event_count=0;
    ULONG pipe_mask=0,pipe_count=0,file_mask=0,file_count=0;
    ULONG reply_bytes=0;
    DWORD error=ERROR_INVALID_DATA;
    BOOL applied=FALSE;
    uint64_t observation_direct=0;
    uint32_t window_style=WINDOW_STYLE_OFF,window_style_present=0;
    DWORD requested_state=base->u.GetNextVDMCommand.VDMState;
    if (!request) request=(uint32_t)InterlockedIncrement(&request_id);
    if (length!=sizeof(BASE_GET_NEXT_VDM_COMMAND_MSG) ||
        !OpenNtBaseEncodeGetCommand(base,request,client.generation,NULL,0,&wire_bytes) ||
        !(wire=HeapAlloc(GetProcessHeap(),0,wire_bytes)) ||
        !OpenNtBaseEncodeGetCommand(base,request,client.generation,wire,wire_bytes,&wire_bytes))
        goto done;
    {
        RPC_ASYNC_STATE async;
        error=OpenNtBaseClientFailure(0);
        if(!error)error=common_rpc_begin_async(&async);
        if(!error) {
            RpcTryExcept {
                Client_Get(&async,client.binding,client.connection,client.process,client.generation,
                    wire_bytes,wire,&wait_event_count,&wait_events,
                    &pipe_mask,&pipe_count,&pipe_handles,&file_mask,&file_count,&file_handles,
                    &reply_bytes,&reply);
            }
            RpcExcept(1) { error=RpcExceptionCode(); }
            RpcEndExcept
            error=OpenNtBaseClientFailure(common_rpc_end_async(&async,error,NULL,
                OPENNT_FRONTEND_LIFECYCLE_TIMEOUT_MS));
        }
    }
    if (!error && wait_event_count<=1 && (!wait_event_count || wait_events) &&
        pipe_count<=3 && (!pipe_count || pipe_handles) &&
        file_count<=3 && (!file_count || file_handles) && reply && reply_bytes) {
        if (wait_event_count) {wait_event=wait_events[0];wait_events[0]=NULL;}
        applied=OpenNtBaseApplyGetCommand(reply,(uint32_t)reply_bytes,client.generation,request,base);
        if(applied)(void)OpenNtBaseReadGetObservation(reply,reply_bytes,client.generation,request,&observation_direct);
        if(applied)applied=OpenNtBaseReadGetStyle(reply,reply_bytes,client.generation,request,
            &window_style,&window_style_present);
    }
    if (applied) {
        base->u.GetNextVDMCommand.WaitObjectForVDM=wait_event;
        if (pipe_count || file_count) {
            HANDLE streams[3]={NULL,NULL,NULL};
            ULONG stream_mask=pipe_mask|file_mask;
            ULONG pipe_index=0,file_index=0,index;
            if ((pipe_mask&file_mask) || (pipe_mask&~7u) || (file_mask&~7u) ||
                (!!base->u.GetNextVDMCommand.StdIn != !!(stream_mask&1u)) ||
                (!!base->u.GetNextVDMCommand.StdOut != !!(stream_mask&2u)) ||
                (!!base->u.GetNextVDMCommand.StdErr != !!(stream_mask&4u))) {
                applied=FALSE;
            } else {
                for (index=0;index<3;++index) {
                    if (pipe_mask&(1u<<index)) streams[index]=pipe_handles[pipe_index++];
                    else if (file_mask&(1u<<index)) streams[index]=file_handles[file_index++];
                }
                if (pipe_index!=pipe_count || file_index!=file_count) { applied=FALSE; goto done; }
                for (index=0;index<3;++index) if (streams[index] &&
                    !SetHandleInformation(streams[index],HANDLE_FLAG_INHERIT,HANDLE_FLAG_INHERIT)) {
                    error=GetLastError(); applied=FALSE; break;
                }
                if (!applied) goto done;
                base->u.GetNextVDMCommand.StdIn=streams[0];
                base->u.GetNextVDMCommand.StdOut=streams[1];
                base->u.GetNextVDMCommand.StdErr=streams[2];
                for (index=0;index<pipe_count;++index) pipe_handles[index]=NULL;
                for (index=0;index<file_count;++index) file_handles[index]=NULL;
            }
        } else {
            /* A declared standard stream is carried only as a typed RPC
             * attachment.  The scalar reply contains presence bits, never a
             * usable HANDLE value, so reject a truncated/malformed reply. */
            if (base->u.GetNextVDMCommand.StdIn ||
                base->u.GetNextVDMCommand.StdOut ||
                base->u.GetNextVDMCommand.StdErr) {
                applied=FALSE;
            } else {
                base->u.GetNextVDMCommand.StdIn=NULL;
                base->u.GetNextVDMCommand.StdOut=NULL;
                base->u.GetNextVDMCommand.StdErr=NULL;
            }
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
        if(applied)wait_event=NULL;
    }
done:
    release_handle_array(wait_events,wait_event_count);
    release_handle_array(pipe_handles,pipe_count);
    release_handle_array(file_handles,file_count);
    if (wait_event) CloseHandle(wait_event);
    if (reply) MIDL_user_free(reply);
    if (wire) HeapFree(GetProcessHeap(),0,wire);
    if (error || !applied) {
        message->ReturnValue=rpc_failure(error ? error : ERROR_INVALID_DATA);
        return message->ReturnValue;
    }
    if((message->ReturnValue==STATUS_SUCCESS || (message->ReturnValue==STATUS_NO_MEMORY &&
        (base->u.GetNextVDMCommand.VDMState & RETURN_ON_NO_COMMAND))) &&
        !base->u.GetNextVDMCommand.WaitObjectForVDM &&
        (base->u.GetNextVDMCommand.CmdLen || (base->u.GetNextVDMCommand.VDMState & RETURN_ON_NO_COMMAND)) &&
        !(requested_state &
            (ASKING_FOR_ENVIRONMENT|ASKING_FOR_PIF|ASKING_FOR_WOW_BINARY))
        ) {
        launch.observation_direct=observation_direct;
        if(!window_style_present) {
            SetLastError(ERROR_INVALID_DATA);message->ReturnValue=(ULONG)STATUS_UNSUCCESSFUL;
            return STATUS_UNSUCCESSFUL;
        }
        launch.dos_window_style=window_style;
        if(requested_state & ASKING_FOR_SEPWOW_BINARY) {
            launch.separate_wow_style=window_style;
            launch.separate_wow_style_valid=TRUE;
        }
    }
    /* Separate WOW initializes its bootstrap task before asking for the
     * first application command. Capture policy on its original PIF reply. */
    if(message->ReturnValue==STATUS_SUCCESS &&
        (requested_state & (ASKING_FOR_PIF|ASKING_FOR_SEPWOW_BINARY))==
            (ASKING_FOR_PIF|ASKING_FOR_SEPWOW_BINARY)) {
        if(!window_style_present) {
            SetLastError(ERROR_INVALID_DATA);message->ReturnValue=(ULONG)STATUS_UNSUCCESSFUL;
            return STATUS_UNSUCCESSFUL;
        }
        launch.separate_wow_style=window_style;
        launch.separate_wow_style_valid=TRUE;
    }
    if (message->ReturnValue==STATUS_SUCCESS && launch.command_ready &&
        !base->u.GetNextVDMCommand.WaitObjectForVDM && base->u.GetNextVDMCommand.CmdLen &&
        !(base->u.GetNextVDMCommand.VDMState & ASKING_FOR_ENVIRONMENT)) {
        error=launch.command_ready(launch.command_context);
        if (error) {
            SetLastError(error);message->ReturnValue=(ULONG)STATUS_UNSUCCESSFUL;
            return STATUS_UNSUCCESSFUL;
        }
    }
    return (NTSTATUS)message->ReturnValue;
}

static NTSTATUS exit_command(PCSR_API_MSG message,ULONG length)
{
    common_rpc_connection client=OpenNtBaseClientExecutionConnection(reset_launch);
    PBASE_EXIT_VDM_MSG exit_message=(PBASE_EXIT_VDM_MSG)&message->u.ApiMessageData;
    DWORD error=ERROR_INVALID_PARAMETER;
    ULONG close_worker_wait=0;
    BOOL is_wow;

    if (length!=sizeof(*exit_message)) goto done;
    is_wow=exit_message->ConsoleHandle==(HANDLE)-1;
    {
        RPC_ASYNC_STATE async;
        error=OpenNtBaseClientFailure(0);
        if(!error)error=common_rpc_begin_async(&async);
        if(!error) {
            RpcTryExcept {
                Client_Exit(&async,client.binding,client.connection,client.process,client.generation,
                    is_wow ? 1u : 0u,is_wow ? exit_message->iWowTask : 0u,&close_worker_wait);
            }
            RpcExcept(1) { error=RpcExceptionCode(); }
            RpcEndExcept
            error=OpenNtBaseClientFailure(common_rpc_end_async(&async,error,NULL,
                OPENNT_FRONTEND_LIFECYCLE_TIMEOUT_MS));
        }
    }
    if (error || close_worker_wait>1u || (close_worker_wait && !worker_wait_event)) goto done;
    exit_message->WaitObjectForVDM=close_worker_wait ? worker_wait_event : NULL;
    if (close_worker_wait) worker_wait_event=NULL;
    message->ReturnValue=STATUS_SUCCESS;
    return STATUS_SUCCESS;
done:
    message->ReturnValue=rpc_failure(error ? error : ERROR_INVALID_DATA);
    return message->ReturnValue;
}

static NTSTATUS update_command(PCSR_API_MSG message,ULONG length)
{
    common_rpc_connection client=OpenNtBaseClientExecutionConnection(reset_launch);
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
    {
        RPC_ASYNC_STATE async;
        error=OpenNtBaseClientFailure(0);
        if(!error)error=common_rpc_begin_async(&async);
        if(!error) {
            RpcTryExcept {
                Client_Update(&async,client.binding,client.connection,client.process,client.generation,
                    wire_bytes,wire,&parent_event_count,&parent_events,&receipt,&reply_bytes,reply);
            }
            RpcExcept(1) { error=RpcExceptionCode(); }
            RpcEndExcept
            error=OpenNtBaseClientFailure(common_rpc_end_async(&async,error,NULL,
                OPENNT_FRONTEND_LIFECYCLE_TIMEOUT_MS));
        }
    }
    if (!error && reply_bytes==sizeof(reply) && parent_event_count<=1 &&
        (!parent_event_count || parent_events)) {
        if (parent_event_count) {parent_event=parent_events[0];parent_events[0]=NULL;}
        applied=OpenNtBaseApplyUpdateReply(reply,(uint32_t)reply_bytes,client.generation,request,base);
    }
    if (applied) {
        base->u.UpdateVDMEntry.WaitObjectForParent=parent_event;
        parent_event_handle=parent_event;
        parent_receipt=receipt;
        parent_event=NULL;
    }
done:
    release_handle_array(parent_events,parent_event_count);
    if (parent_event) CloseHandle(parent_event);
    if (wire) HeapFree(GetProcessHeap(),0,wire);
    if (error || !applied) {
        message->ReturnValue=rpc_failure(error ? error : ERROR_INVALID_DATA);
        return message->ReturnValue;
    }
    return (NTSTATUS)message->ReturnValue;
}

static NTSTATUS exit_code_command(PCSR_API_MSG message,ULONG length)
{
    common_rpc_connection client=OpenNtBaseClientExecutionConnection(reset_launch);
    PBASE_GET_VDM_EXIT_CODE_MSG exit_message=(PBASE_GET_VDM_EXIT_CODE_MSG)&message->u.ApiMessageData;
    DWORD error=ERROR_INVALID_PARAMETER;
    ULONG exit_code=0;
    if (length!=sizeof(*exit_message) || exit_message->hParent!=parent_event_handle || !parent_receipt)
        goto done;
    {
        RPC_ASYNC_STATE async;
        error=OpenNtBaseClientFailure(0);
        if(!error)error=common_rpc_begin_async(&async);
        if(!error) {
            RpcTryExcept {
                Client_ExitCode(&async,client.binding,client.connection,client.process,client.generation,
                    parent_receipt,&exit_code);
            }
            RpcExcept(1) { error=RpcExceptionCode(); }
            RpcEndExcept
            error=OpenNtBaseClientFailure(common_rpc_end_async(&async,error,NULL,
                OPENNT_FRONTEND_LIFECYCLE_TIMEOUT_MS));
        }
    }
    if (!error) {
        exit_message->ExitCode=exit_code;
        parent_event_handle=NULL;parent_receipt=0;
        message->ReturnValue=STATUS_SUCCESS;
        return STATUS_SUCCESS;
    }
done:
    message->ReturnValue=rpc_failure(error);
    return message->ReturnValue;
}

static NTSTATUS reenter_command(PCSR_API_MSG message,ULONG length)
{
    common_rpc_connection client=OpenNtBaseClientExecutionConnection(reset_launch);
    PBASE_SET_REENTER_COUNT_MSG reenter=(PBASE_SET_REENTER_COUNT_MSG)&message->u.ApiMessageData;
    DWORD error=ERROR_INVALID_PARAMETER;

    if (length!=sizeof(*reenter) ||
        (reenter->fIncDec!=INCREMENT_REENTER_COUNT &&
         reenter->fIncDec!=DECREMENT_REENTER_COUNT)) goto done;
    {
        RPC_ASYNC_STATE async;
        error=OpenNtBaseClientFailure(0);
        if(!error)error=common_rpc_begin_async(&async);
        if(!error) {
            RpcTryExcept {
                Client_Reenter(&async,client.binding,client.connection,client.process,client.generation,
                    reenter->fIncDec);
            }
            RpcExcept(1) { error=RpcExceptionCode(); }
            RpcEndExcept
            error=OpenNtBaseClientFailure(common_rpc_end_async(&async,error,NULL,
                OPENNT_FRONTEND_LIFECYCLE_TIMEOUT_MS));
        }
    }
    if (!error) {
        message->ReturnValue=STATUS_SUCCESS;
        return STATUS_SUCCESS;
    }
done:
    message->ReturnValue=rpc_failure(error);
    return message->ReturnValue;
}

DWORD OpenNtBaseClientStartVdmWorker(PCWSTR environment,DWORD characters,DWORD show,
    HANDLE frontend,HANDLE *worker,HANDLE *parent)
{
    RPC_ASYNC_STATE async;
    common_rpc_connection client=OpenNtBaseClientExecutionConnection(reset_launch);
    DWORD error=ERROR_INVALID_STATE;ULONG receipt=0;
    if(!worker || !parent)return ERROR_INVALID_PARAMETER;
    *worker=*parent=NULL;
    if(!client.connection || !client.binding || !client.process)return error;
    error=OpenNtBaseClientFailure(0);if(error)return error;
    error=common_rpc_begin_async(&async);if(error)return error;
    RpcTryExcept {
        Client_StartVdmWorker(&async,client.binding,client.connection,client.process,
            client.generation,characters,(WCHAR *)environment,show,frontend,
            worker,parent,&receipt);
    }
    RpcExcept(1){error=RpcExceptionCode();}
    RpcEndExcept
    error=OpenNtBaseClientFailure(common_rpc_end_async(&async,error,NULL,OPENNT_FRONTEND_LIFECYCLE_TIMEOUT_MS));
    if(!error && (!*worker || (!!*parent != !!receipt)))error=ERROR_INVALID_DATA;
    if(error) {
        if(*worker)CloseHandle(*worker);if(*parent)CloseHandle(*parent);
        *worker=*parent=NULL;
    } else {
        /* Same process-local receipt mapping as original Update's RPC facade. */
        parent_event_handle=*parent;parent_receipt=receipt;
    }
    return error;
}

DWORD WINAPI OpenNtBaseClientWowStyle(ULONG task,DWORD *style)
{
    common_rpc_connection client=OpenNtBaseClientExecutionConnection(reset_launch);
    DWORD error=ERROR_INVALID_STATE;
    if(!style)return ERROR_INVALID_PARAMETER;
    *style=WINDOW_STYLE_OFF;
    if(!client.connection)return error;
    {
        RPC_ASYNC_STATE async;
        error=OpenNtBaseClientFailure(0);
        if(!error)error=common_rpc_begin_async(&async);
        if(!error) {
            RpcTryExcept {
                Client_WowStyle(&async,client.binding,client.connection,client.process,
                    client.generation,task,style);
            }
            RpcExcept(1){error=RpcExceptionCode();}
            RpcEndExcept
            error=OpenNtBaseClientFailure(common_rpc_end_async(&async,error,NULL,
                OPENNT_FRONTEND_LIFECYCLE_TIMEOUT_MS));
        }
    }
    if(!error && !WINDOW_STYLE_VALID(*style))error=ERROR_INVALID_DATA;
    return error;
}

DWORD WINAPI OpenNtBaseClientWowStarted(ULONG task)
{
    common_rpc_connection client=OpenNtBaseClientExecutionConnection(reset_launch);
    DWORD error=ERROR_INVALID_STATE;
    if (!client.connection) return error;
    {
        RPC_ASYNC_STATE async;
        error=OpenNtBaseClientFailure(0);
        if(!error)error=common_rpc_begin_async(&async);
        if(!error) {
            RpcTryExcept {
                Client_WowStarted(&async,client.binding,client.connection,client.process,
                    client.generation,task);
            }
            RpcExcept(1) { error=RpcExceptionCode(); }
            RpcEndExcept
            error=OpenNtBaseClientFailure(common_rpc_end_async(&async,error,NULL,
                OPENNT_FRONTEND_LIFECYCLE_TIMEOUT_MS));
        }
    }
    return error;
}

DWORD OpenNtBaseClientWowStartup(HANDLE parent,HANDLE *event,BOOL *started)
{
    common_rpc_connection client=OpenNtBaseClientExecutionConnection(reset_launch);
    DWORD error=ERROR_INVALID_STATE;
    ULONG value=0;
    if (!event || !started) return ERROR_INVALID_PARAMETER;
    *event=NULL; *started=FALSE;
    if (!client.connection) return error;
    if (!parent || parent!=parent_event_handle || !parent_receipt)
        return ERROR_INVALID_HANDLE;
    {
        RPC_ASYNC_STATE async;
        error=OpenNtBaseClientFailure(0);
        if(!error)error=common_rpc_begin_async(&async);
        if(!error) {
            RpcTryExcept {
                Client_WowStartup(&async,client.binding,client.connection,client.process,
                    client.generation,parent_receipt,event,&value);
            }
            RpcExcept(1) { error=RpcExceptionCode(); }
            RpcEndExcept
            error=OpenNtBaseClientFailure(common_rpc_end_async(&async,error,NULL,
                OPENNT_FRONTEND_LIFECYCLE_TIMEOUT_MS));
        }
    }
    if (!error && (!*event || value>1)) error=ERROR_INVALID_DATA;
    if (error && *event) { CloseHandle(*event); *event=NULL; }
    if (!error) *started=value!=0;
    return OpenNtBaseClientFailure(error);
}

NTSTATUS NTAPI OpenNtBaseClientCallServer(PCSR_API_MSG message,
    PCSR_CAPTURE_HEADER capture,CSR_API_NUMBER number,ULONG length)
{
    common_rpc_connection client=OpenNtBaseClientExecutionConnection(reset_launch);
    ULONG first=0;
    DWORD error=ERROR_NOT_SUPPORTED;
    (void)capture;
    if (!message || !client.connection || !client.binding || !client.process)
        return STATUS_UNSUCCESSFUL;
    error=OpenNtBaseClientFailure(0);
    if(error){message->ReturnValue=rpc_failure(error);return message->ReturnValue;}
    if (number==CSR_MAKE_API_NUMBER(BASESRV_SERVERDLL_INDEX,BasepRegisterWowExec)) {
        if (length!=sizeof(BASE_REGISTER_WOWEXEC_MSG)) {
            message->ReturnValue=STATUS_UNSUCCESSFUL;
            return STATUS_UNSUCCESSFUL;
        }
        {
            RPC_ASYNC_STATE async;
            error=OpenNtBaseClientFailure(0);
            if(!error)error=common_rpc_begin_async(&async);
            if(!error) {
                RpcTryExcept {
                    Client_RegisterWowExec(&async,client.binding,client.connection,client.process,
                        client.generation,(ULONG)(ULONG_PTR)
                            ((PBASE_API_MSG)message)->u.RegisterWowExec.hwndWowExec);
                }
                RpcExcept(1) { error=RpcExceptionCode(); }
                RpcEndExcept
                error=OpenNtBaseClientFailure(common_rpc_end_async(&async,error,NULL,
                    OPENNT_FRONTEND_LIFECYCLE_TIMEOUT_MS));
            }
        }
        message->ReturnValue=error ? rpc_failure(error) : STATUS_SUCCESS;
        return (NTSTATUS)message->ReturnValue;
    }
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
    {
        RPC_ASYNC_STATE async;
        error=OpenNtBaseClientFailure(0);
        if(!error)error=common_rpc_begin_async(&async);
        if(!error) {
            RpcTryExcept {
                Client_First(&async,client.binding,client.connection,client.process,
                    client.generation,&first);
            }
            RpcExcept(1) { error=RpcExceptionCode(); }
            RpcEndExcept
            error=OpenNtBaseClientFailure(common_rpc_end_async(&async,error,NULL,
                OPENNT_FRONTEND_LIFECYCLE_TIMEOUT_MS));
        }
    }
    if (error) {
        message->ReturnValue=rpc_failure(error);
        return (NTSTATUS)message->ReturnValue;
    }
    ((PBASE_IS_FIRST_VDM_MSG)&message->u.ApiMessageData)->FirstVDM=first;
    message->ReturnValue=STATUS_SUCCESS;
    return STATUS_SUCCESS;
}
