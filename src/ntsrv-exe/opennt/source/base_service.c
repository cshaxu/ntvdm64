/* NTSRV-private base service; project adaptation,
 * not an original OpenNT mirror. Physical separation only: existing function
 * bodies, state authority and lock/resource contracts are preserved. */
#include <service_internal.h>

static NTSTATUS service_stream_deliver_to_connection(OPENNT_BASE_CONNECTION *connection,
    HANDLE stream,uint32_t *receipt);
static NTSTATUS service_stream_deliver(void *context,HANDLE stream,uint32_t *receipt);
static NTSTATUS service_worker_stream_deliver(void *context,HANDLE stream,uint32_t *receipt);
static NTSTATUS service_wait_revoke(void *context,uint32_t receipt);
static NTSTATUS service_duplicate_resource(void *context,HANDLE source_process,HANDLE source,
    HANDLE target_process,PHANDLE target,ACCESS_MASK access,ULONG attributes,ULONG options);
static NTSTATUS service_close_resource(void *context,HANDLE handle);
static void service_resources_release(OPENNT_BASE_SERVICE_RESOURCES *scope);
static DWORD service_wait_resolve(OPENNT_BASE_CONNECTION *connection,DWORD generation,
    HANDLE receipt,DWORD role,HANDLE *event);
static DWORD service_allocate_console(OPENNT_BASE_SERVICE *service,HANDLE *console);

/* Called under the service lock before either original ExitVDM or process
 * rundown destroys records. Both paths must preserve the same parent result. */
void service_preserve_parent_results(OPENNT_BASE_SERVICE *service,
    HANDLE console,BOOL wow)
{
    LIST_ENTRY *entry;
    /* Original cleanup wakes waiters and removes records; a later original
     * exit-code query then returns zero for a missing record. Preserve that
     * body, but classify an unsignalled task as failed BEFORE removing it.
     * These are original notification events (non-consuming zero wait). */
    for (entry=service->connections.Flink;
         entry!=&service->connections;entry=entry->Flink) {
        OPENNT_BASE_CONNECTION *parent=CONTAINING_RECORD(entry,OPENNT_BASE_CONNECTION,service_link);
        if (!parent->process.fVDM && parent->wow==wow &&
            parent->console==console && parent->parent_wait && !parent->parent_is_worker_exit) {
            DWORD wait=WaitForSingleObject(parent->parent_wait,0);
            if (wait==WAIT_TIMEOUT) parent->worker_failed=TRUE;
            else if (wait==WAIT_OBJECT_0 && parent->parent_receipt) {
                DWORD code;
                uint32_t receipt=parent->parent_receipt;
                /* Original cleanup frees DOS records. Collect a completed
                 * original result before that cleanup, retaining only its
                 * authenticated reply until the direct parent receives it. */
                if (!OpenNtBaseServiceExitCode(parent,
                        (DWORD)(ULONG_PTR)parent->process.ClientId.UniqueProcess,
                        parent->process.SequenceNumber,receipt,&code) && code!=STILL_ACTIVE) {
                    parent->completed_receipt=receipt;
                    parent->completed_exit_code=code;
                }
            }
        }
    }
}

NTSTATUS service_wait_deliver(void *context,HANDLE event,uint32_t *receipt)
{
    OPENNT_BASE_SERVICE_RESOURCES *scope=context;
    DWORD error;
    if (!scope || !event || !receipt) return STATUS_INVALID_PARAMETER;
    /* Original DOS records compare hWaitForParent across one Console list.
     * Independent connection counters must not give a nested parent the
     * same identity as its still-active outer parent. The service lock is
     * held by every original dispatch reaching this finite delivery seam. */
    if (scope->connection->streams.issued < scope->connection->service->next_wait_receipt)
        scope->connection->streams.issued=scope->connection->service->next_wait_receipt;
    error=broker_vdm_receipt_accept(&scope->connection->streams,scope->role,event,receipt);
    if (!error) scope->connection->service->next_wait_receipt=*receipt;
    return error ? STATUS_INVALID_HANDLE : STATUS_SUCCESS;
}

static NTSTATUS service_stream_deliver_to_connection(OPENNT_BASE_CONNECTION *connection,
    HANDLE stream,uint32_t *receipt)
{
    DWORD error;
    if (!connection || !connection->reservation || !stream || !receipt)
        return STATUS_INVALID_PARAMETER;
    /* Update precedes worker Connect.  The reservation is the only finite
     * owner spanning that interval; srvvdm.c retains alias comparison. */
    error=OpenNtBaseReservationAcceptStream(connection->service->reservations,
        connection->reservation,stream,receipt);
    return error ? STATUS_INVALID_HANDLE : STATUS_SUCCESS;
}

static NTSTATUS service_stream_deliver(void *context,HANDLE stream,uint32_t *receipt)
{
    OPENNT_BASE_SERVICE_RESOURCES *scope=context;
    return scope ? service_stream_deliver_to_connection(scope->connection,stream,receipt) :
        STATUS_INVALID_PARAMETER;
}

static NTSTATUS service_worker_stream_deliver(void *context,HANDLE stream,uint32_t *receipt)
{
    return service_stream_deliver_to_connection((OPENNT_BASE_CONNECTION *)context,stream,receipt);
}

static NTSTATUS service_wait_revoke(void *context,uint32_t receipt)
{
    OPENNT_BASE_SERVICE_RESOURCES *scope=context;
    DWORD error;
    if (!scope) return STATUS_INVALID_PARAMETER;
    error=broker_vdm_receipt_revoke(&scope->connection->streams,
        scope->connection->process.SequenceNumber,receipt);
    return error ? STATUS_INVALID_HANDLE : STATUS_SUCCESS;
}

static NTSTATUS service_duplicate_resource(void *context,HANDLE source_process,HANDLE source,
    HANDLE target_process,PHANDLE target,ACCESS_MASK access,ULONG attributes,ULONG options)
{
    OPENNT_BASE_SERVICE_RESOURCES *scope=context;
    OPENNT_BASE_STREAM_BINDING streams;
    OPENNT_BASE_CONNECTION *worker_connection=NULL;
    LIST_ENTRY *entry;
    if (!scope) return STATUS_INVALID_PARAMETER;
    /* BaseSrvUpdateDOSEntry duplicates the newly created VDM process into
     * BaseSrv.  In NT4 its source was a raw handle in the launcher's CSR
     * namespace.  Here that value cannot cross RPC; use only the live worker
     * retained from this launcher's authenticated reservation. */
    if (source==NtCurrentProcess() && target_process==NtCurrentProcess() && target &&
        !access && !attributes &&
        options==DUPLICATE_SAME_ACCESS) {
        if (scope->worker && DuplicateHandle(GetCurrentProcess(),scope->worker,
                target_process,target,0,FALSE,DUPLICATE_SAME_ACCESS))
            return STATUS_SUCCESS;
        return STATUS_ACCESS_DENIED;
    }
    /* The retained srvvdm.c body asks to duplicate a source-shaped standard
     * handle into its VDM process. */
    /* A present VDM receives another launcher's command.  Retained srvvdm.c
     * has selected the target process already; resolve that process back to
     * its authenticated worker connection, then retain the new launcher's
     * stream in the target worker reservation.  The launcher cannot own this
     * delivery: it has no worker reservation and exits after its parent wait.
     */
    if (source && target && !access && attributes==OBJ_INHERIT &&
        options==DUPLICATE_SAME_ACCESS && target_process) {
        for (entry=scope->connection->service->connections.Flink;
                entry!=&scope->connection->service->connections;entry=entry->Flink) {
            OPENNT_BASE_CONNECTION *candidate=CONTAINING_RECORD(entry,
                OPENNT_BASE_CONNECTION,service_link);
            if (candidate->process.fVDM && candidate->reservation &&
                candidate->console==scope->connection->console) {
                worker_connection=candidate;
                break;
            }
        }
        if (worker_connection) {
            ZeroMemory(&streams,sizeof(streams));
            streams.source_process=source_process;
            streams.target_process=target_process;
            streams.source_receipts=&scope->connection->streams;
            streams.source_generation=scope->connection->process.SequenceNumber;
            streams.context=worker_connection;
            streams.deliver=service_worker_stream_deliver;
            return OpenNtBaseDuplicateStream(&streams,source_process,source,target_process,
                target,access,attributes,options);
        }
    }
    if (scope->connection->reservation && source && target && !access &&
        attributes==OBJ_INHERIT && options==DUPLICATE_SAME_ACCESS) {
        ZeroMemory(&streams,sizeof(streams));
        streams.source_process=source_process;
        streams.target_process=target_process;
        streams.source_receipts=&scope->connection->streams;
        streams.source_generation=scope->connection->process.SequenceNumber;
        streams.context=scope;
        streams.deliver=service_stream_deliver;
        return OpenNtBaseDuplicateStream(&streams,source_process,source,target_process,
            target,access,attributes,options);
    }
    return OpenNtBaseDuplicateWait(&scope->wait,source_process,source,target_process,
        target,access,attributes,options);
}

static NTSTATUS service_close_resource(void *context,HANDLE handle)
{
    OPENNT_BASE_SERVICE_RESOURCES *scope=context;
    if (!scope || !handle) return STATUS_INVALID_HANDLE;
    if (handle==scope->wait.local_event) return OpenNtBaseCloseWait(&scope->wait,handle);
    if (scope->connection->reservation &&
        !OpenNtBaseReservationRevokeStream(scope->connection->service->reservations,
            scope->connection->reservation,(uint32_t)(ULONG_PTR)handle)) return STATUS_SUCCESS;
    if (broker_vdm_receipt_revoke(&scope->connection->streams,
            scope->connection->process.SequenceNumber,(uint32_t)(ULONG_PTR)handle)==ERROR_SUCCESS)
        return STATUS_SUCCESS;
    return CloseHandle(handle) ? STATUS_SUCCESS : STATUS_INVALID_HANDLE;
}

void service_resources_init(OPENNT_BASE_SERVICE_RESOURCES *scope,
    OPENNT_BASE_CONNECTION *connection,DWORD role)
{
    ZeroMemory(scope,sizeof(*scope));
    scope->connection=connection; scope->role=role;
    scope->wait.target_process=connection->process.ProcessHandle;
    scope->wait.context=scope; scope->wait.deliver=service_wait_deliver;
    scope->wait.revoke=service_wait_revoke;
    scope->binding.Context=scope; scope->binding.Duplicate=service_duplicate_resource;
    scope->binding.Close=service_close_resource;
}

static void service_resources_release(OPENNT_BASE_SERVICE_RESOURCES *scope)
{
    if (scope && scope->worker) CloseHandle(scope->worker);
}

static DWORD service_wait_resolve(OPENNT_BASE_CONNECTION *connection,DWORD generation,
    HANDLE receipt,DWORD role,HANDLE *event)
{
    if (!receipt) { *event=NULL; return ERROR_SUCCESS; }
    return broker_vdm_receipt_resolve(&connection->streams,generation,(uint32_t)(ULONG_PTR)receipt,
        role,event);
}

/* RPC rundown replaces only unavailable CSR transport. Original UndoCreation
 * still owns record/stream cleanup. A claimed VDM is no longer the launcher's
 * child resource to kill: its guest lifetime and process watch remain intact. */
void service_abandon_launch(OPENNT_BASE_CONNECTION *connection)
{
    BOOL claimed=FALSE;
    if (connection->process.fVDM || connection->native_worker) return;
    if (connection->reservation)
        claimed=OpenNtBaseReservationAbandon(connection->service->reservations,connection->reservation);
    if (claimed)
        OpenNtBaseReservationCollectAbandoned(connection->service->reservations,connection->reservation);
    if (connection->pending_creation && !claimed) {
        BASE_API_MSG message={0};
        CSR_THREAD thread={0};
        PCSR_THREAD previous_thread;
        OPENNT_BASE_PROCESS_REGISTRY *previous_registry;
        OPENNT_BASE_SERVICE_RESOURCES resources;
        const OPENNT_BASE_RESOURCE_BINDING *previous;
        message.u.UpdateVDMEntry.ConsoleHandle=connection->wow ? (HANDLE)-1 :
            (connection->task ? NULL : connection->console);
        message.u.UpdateVDMEntry.iTask=connection->task;
        message.u.UpdateVDMEntry.BinaryType=connection->wow ? BINARY_TYPE_WIN16 : BINARY_TYPE_DOS;
        message.u.UpdateVDMEntry.EntryIndex=UPDATE_VDM_UNDO_CREATION;
        message.u.UpdateVDMEntry.VDMCreationState=connection->registered_worker ?
            VDM_FULLY_CREATED : VDM_PARTIALLY_CREATED;
        service_resources_init(&resources,connection,BROKER_VDM_PARENT_WAIT);
        thread.Process=&connection->process;
        thread.ClientId.UniqueProcess=connection->process.ClientId.UniqueProcess;
        previous_thread=OpenNtBaseBindServerRequestThread(&thread);
        previous_registry=OpenNtBaseBindProcessRegistry(&connection->service->registry);
        previous=OpenNtBaseBindResources(&resources.binding);
        OpenNtBaseDispatchOperation((PCSR_API_MSG)&message,BROKER_VDM_UPDATE,sizeof(message.u.UpdateVDMEntry));
        OpenNtBaseBindResources(previous);
        OpenNtBaseBindProcessRegistry(previous_registry);
        OpenNtBaseBindServerRequestThread(previous_thread);
        service_resources_release(&resources);
}
    if (connection->reservation && !claimed) {
        OpenNtBaseReservationRelease(connection->service->reservations,connection->reservation,
            (DWORD)connection->process.ClientId.UniqueProcess,connection->process.SequenceNumber);
        connection->reservation=0;
    }
    connection->pending_creation=FALSE;
}


DWORD OpenNtBaseServiceFirst(OPENNT_BASE_CONNECTION *connection,DWORD pid,DWORD generation,DWORD *first)
{
    BASE_API_MSG message={0};
    CSR_THREAD thread={0};
    PCSR_THREAD previousThread;
    OPENNT_BASE_PROCESS_REGISTRY *previousRegistry;
    NTSTATUS status;
    if (!connection || !first) return ERROR_INVALID_PARAMETER;
    *first=0;
    if (!OpenNtBaseServicePeer(connection,pid,generation)) return ERROR_ACCESS_DENIED;
    EnterCriticalSection(&connection->service->lock);
    thread.Process=&connection->process;
    thread.ClientId.UniqueProcess=connection->process.ClientId.UniqueProcess;
    previousThread=OpenNtBaseBindServerRequestThread(&thread);
    previousRegistry=OpenNtBaseBindProcessRegistry(&connection->service->registry);
    status=OpenNtBaseDispatchOperation((PCSR_API_MSG)&message,BROKER_VDM_IS_FIRST,sizeof(message.u.IsFirstVDM));
    OpenNtBaseBindProcessRegistry(previousRegistry);
    OpenNtBaseBindServerRequestThread(previousThread);
    LeaveCriticalSection(&connection->service->lock);
    if (status) return RtlNtStatusToDosError(status);
    *first=message.u.IsFirstVDM.FirstVDM;
    return 0;
}


DWORD OpenNtBaseServiceRegisterWowExec(OPENNT_BASE_CONNECTION *connection,
    DWORD pid,DWORD generation,DWORD window)
{
    BASE_API_MSG message={0};
    CSR_THREAD thread={0};
    PCSR_THREAD previous_thread;
    OPENNT_BASE_PROCESS_REGISTRY *previous_registry;
    DWORD window_pid=0;
    NTSTATUS status;
    if (!connection || !window) return ERROR_INVALID_PARAMETER;
    if (!OpenNtBaseServicePeer(connection,pid,generation)) return ERROR_ACCESS_DENIED;
    EnterCriticalSection(&connection->service->lock);
    /* A copied HWND is only an untrusted identity, never authority. Only the
     * reservation-bound WOW worker may register its own native window. The
     * original server retains the window/thread/process-sequence policy. */
    if (!connection->process.fVDM || !connection->wow ||
            !GetWindowThreadProcessId((HWND)(ULONG_PTR)window,&window_pid) ||
            window_pid!=pid) {
        LeaveCriticalSection(&connection->service->lock);
        return ERROR_ACCESS_DENIED;
    }
    message.u.RegisterWowExec.hwndWowExec=(HWND)(ULONG_PTR)window;
    thread.Process=&connection->process;
    thread.ClientId.UniqueProcess=connection->process.ClientId.UniqueProcess;
    previous_thread=OpenNtBaseBindServerRequestThread(&thread);
    previous_registry=OpenNtBaseBindProcessRegistry(&connection->service->registry);
    status=OpenNtBaseDispatchOperation((PCSR_API_MSG)&message,BROKER_VDM_WOWEXEC,
        sizeof(message.u.RegisterWowExec));
    OpenNtBaseBindProcessRegistry(previous_registry);
    OpenNtBaseBindServerRequestThread(previous_thread);
    LeaveCriticalSection(&connection->service->lock);
    return status ? RtlNtStatusToDosError(status) : ERROR_SUCCESS;
}


/* A no-console DOS record receives its actual Console identity only when the
 * reserved worker makes its original ASKING_FOR_PIF request.  This finite
 * local key is the transport replacement for that unavailable handle; it is
 * not a second record list or worker scheduler. */
static DWORD service_allocate_console(OPENNT_BASE_SERVICE *service,HANDLE *console)
{
    DWORD error=ERROR_SUCCESS;
    if (!service || !console) return ERROR_INVALID_PARAMETER;
    *console=NULL;
    EnterCriticalSection(&service->lock);
    if (!++service->next_console || service->next_console==MAXDWORD)
        error=ERROR_ARITHMETIC_OVERFLOW;
    else
        *console=(HANDLE)(ULONG_PTR)service->next_console;
    LeaveCriticalSection(&service->lock);
    return error;
}


DWORD OpenNtBaseServiceWowStarted(OPENNT_BASE_CONNECTION *connection,DWORD pid,
    DWORD generation,ULONG task)
{
    OPENNT_BASE_SERVICE *service;
    PWOWRECORD record;
    LIST_ENTRY *entry;
    DWORD error=ERROR_ACCESS_DENIED;
    if (!connection) return error;
    service=connection->service;
    EnterCriticalSection(&service->lock);
    if (!OpenNtBaseServicePeer(connection,pid,generation) || !connection->wow ||
        !connection->process.fVDM || !WOWHead || WOWHead->SequenceNumber!=generation)
        goto done;
    for (record=WOWHead->WOWRecord;record;record=record->WOWRecordNext)
        if (record->iTask==task) break;
    if (!record || !record->fDispatched) { error=ERROR_INVALID_STATE; goto done; }
    /* Task IDs wrap. Match the original parent receipt too, never a stale
     * launcher with the same numeric task or a freed record address. */
    error=ERROR_SUCCESS;
    for (entry=service->connections.Flink;entry!=&service->connections;entry=entry->Flink) {
        OPENNT_BASE_CONNECTION *parent=CONTAINING_RECORD(entry,OPENNT_BASE_CONNECTION,service_link);
        if (!parent->process.fVDM && parent->wow && parent->task==task &&
            parent->parent_receipt && parent->parent_receipt==(ULONG_PTR)record->hWaitForParentServer) {
            if (parent->wow_start_event && !SetEvent(parent->wow_start_event)) {
                error=GetLastError(); break;
            }
            parent->wow_started=TRUE;
            break;
        }
    }
done:
    LeaveCriticalSection(&service->lock);
    return error;
}


DWORD OpenNtBaseServiceWowStartup(OPENNT_BASE_CONNECTION *connection,DWORD pid,
    DWORD generation,DWORD parent_receipt,HANDLE *event,BOOL *started)
{
    DWORD error=ERROR_ACCESS_DENIED;
    if (!event || !started) return ERROR_INVALID_PARAMETER;
    *event=NULL; *started=FALSE;
    if (!connection) return error;
    EnterCriticalSection(&connection->service->lock);
    if (!OpenNtBaseServicePeer(connection,pid,generation) || connection->process.fVDM ||
        !connection->wow || !parent_receipt || parent_receipt!=connection->parent_receipt)
        goto done;
    if (!connection->wow_start_event)
        connection->wow_start_event=CreateEventW(NULL,TRUE,connection->wow_started,NULL);
    if (!connection->wow_start_event || !DuplicateHandle(GetCurrentProcess(),
        connection->wow_start_event,GetCurrentProcess(),event,SYNCHRONIZE,FALSE,0)) {
        error=GetLastError(); goto done;
    }
    *started=connection->wow_started;
    error=ERROR_SUCCESS;
done:
    LeaveCriticalSection(&connection->service->lock);
    return error;
}


DWORD OpenNtBaseServiceCheck(OPENNT_BASE_CONNECTION *connection,DWORD pid,DWORD generation,
    void *input,uint32_t bytes,void *output,uint32_t capacity,uint32_t *required,
    HANDLE *parent_event,uint32_t *parent_receipt)
{
    BASE_API_MSG message={0};
    STARTUPINFOA startup;
    CSR_THREAD thread={0};
    PCSR_THREAD previousThread;
    OPENNT_BASE_PROCESS_REGISTRY *previousRegistry;
    const OPENNT_BASE_INTERACTIVE_SCOPE *previousInteractive;
    OPENNT_BASE_SERVICE_RESOURCES resources;
    const OPENNT_BASE_RESOURCE_BINDING *previousResources;
    uint32_t request;
    uint32_t needed=0;
    NTSTATUS status;
    HANDLE separate_console=NULL;
    OPENNT_BASE_CONSOLE_CONTEXT *separate_context=NULL;
    BOOL separate_dos=FALSE;
    BOOL console_record_exists=FALSE;
    if (required) *required=0;
    if (!parent_event || !parent_receipt) return ERROR_INVALID_PARAMETER;
    *parent_event=NULL;*parent_receipt=0;
    if (!connection || !required || !OpenNtBaseServicePeer(connection,pid,generation) ||
        !OpenNtBaseDecodeCheckCommand(input,bytes,generation,&message,&startup,&request))
        return ERROR_INVALID_PARAMETER;
    if(connection->selected_native_generation)return ERROR_INVALID_STATE;
    /* CheckVDM publishes an original record.  Reject a short reply before
     * dispatch so a caller retry cannot submit/consume the command twice. */
    if (!OpenNtBaseEncodeCheckReply(&message,request,generation,NULL,0,&needed))
        return ERROR_INVALID_PARAMETER;
    *required=needed;
    if (!output || capacity<needed) return ERROR_INSUFFICIENT_BUFFER;
    if (message.u.CheckVDM.ConsoleHandle==OPENNT_BASE_CONSOLE_EXISTING) {
        DWORD bind=service_bind_existing_console(connection);
        if (bind) return bind;
        /* VDM_READY is not worker readiness.  Reuse only a worker that has
         * already reached its original GetNextVDMCommand wait.  Otherwise
         * preserve resident COMMAND and let CheckDOS create its original
         * hConsole==0/DosSesId record for a separate worker. */
        if (message.u.CheckVDM.BinaryType==BINARY_TYPE_DOS &&
            !BaseSrvDOSWorkerWaitPending(connection->console,&console_record_exists) &&
            console_record_exists) {
            message.u.CheckVDM.ConsoleHandle=NULL;
        } else {
            message.u.CheckVDM.ConsoleHandle=connection->console;
        }
    }
    /* Both an initially detached launcher and a busy resident Console use
     * original CheckDOS's null-Console/session-id path. Reserve the same
     * transport identity for either, without changing the source request. */
    if (message.u.CheckVDM.BinaryType==BINARY_TYPE_DOS &&
        !message.u.CheckVDM.ConsoleHandle) {
        DWORD allocate=service_allocate_console(connection->service,&separate_console);
        if (allocate) return allocate;
        separate_dos=TRUE;
    }
    EnterCriticalSection(&connection->service->lock);
    if(separate_dos && connection->retained_frontend_root) {
        LIST_ENTRY *link;
        OPENNT_BASE_CONNECTION *root=NULL;
        for(link=connection->service->connections.Flink;
            link!=&connection->service->connections;link=link->Flink) {
            OPENNT_BASE_CONNECTION *candidate=CONTAINING_RECORD(link,
                OPENNT_BASE_CONNECTION,service_link);
            if(candidate->process.SequenceNumber==connection->retained_frontend_root &&
                candidate->frontend_capability && !candidate->frontend_closing &&
                WaitForSingleObject(candidate->process.ProcessHandle,0)==WAIT_TIMEOUT) {
                root=candidate;break;
            }
        }
        if(!root) {LeaveCriticalSection(&connection->service->lock);return ERROR_PIPE_NOT_CONNECTED;}
        /* Prepare the association before publishing the source record, so an
         * allocation failure cannot strand an admitted task. The new Console
         * is service-allocated, never an identity supplied by the launcher. */
        separate_context=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*separate_context));
        if(!separate_context) {LeaveCriticalSection(&connection->service->lock);return ERROR_NOT_ENOUGH_MEMORY;}
        separate_context->capability=CreateEventW(NULL,TRUE,FALSE,NULL);
        if(!separate_context->capability) {
            DWORD error=GetLastError();HeapFree(GetProcessHeap(),0,separate_context);
            LeaveCriticalSection(&connection->service->lock);return error;
        }
        separate_context->root=root;separate_context->console=separate_console;
        InsertTailList(&connection->service->console_contexts,&separate_context->link);
    }
    thread.Process=&connection->process;
    thread.ClientId.UniqueProcess=connection->process.ClientId.UniqueProcess;
    service_resources_init(&resources,connection,BROKER_VDM_PARENT_WAIT);
    previousThread=OpenNtBaseBindServerRequestThread(&thread);
    previousRegistry=OpenNtBaseBindProcessRegistry(&connection->service->registry);
    previousInteractive=OpenNtBaseBindInteractiveScope(&connection->service->interactive);
    previousResources=OpenNtBaseBindResources(&resources.binding);
    status=OpenNtBaseDispatchOperation((PCSR_API_MSG)&message,BROKER_VDM_CHECK,
        sizeof(message.u.CheckVDM));
    OpenNtBaseBindResources(previousResources);
    OpenNtBaseBindInteractiveScope(previousInteractive);
    OpenNtBaseBindProcessRegistry(previousRegistry);
    OpenNtBaseBindServerRequestThread(previousThread);
    service_resources_release(&resources);
    if (!status && NT_SUCCESS((NTSTATUS)message.ReturnValue)) {
        connection->wow=message.u.CheckVDM.BinaryType==BINARY_TYPE_WIN16;
        connection->pending_vdm_binary=message.u.CheckVDM.BinaryType;
        if (connection->wow) connection->task=message.u.CheckVDM.iTask;
        connection->wow_started=FALSE;
        if (connection->wow_start_event) CloseHandle(connection->wow_start_event);
        connection->wow_start_event=NULL;
    }
    if (!status && NT_SUCCESS((NTSTATUS)message.ReturnValue) &&
        message.u.CheckVDM.VDMState==VDM_NOT_PRESENT) {
        connection->pending_creation=TRUE;
        connection->pending_standard_streams[0]=
            (uint32_t)(ULONG_PTR)message.u.CheckVDM.StdIn;
        connection->pending_standard_streams[1]=
            (uint32_t)(ULONG_PTR)message.u.CheckVDM.StdOut;
        connection->pending_standard_streams[2]=
            (uint32_t)(ULONG_PTR)message.u.CheckVDM.StdErr;
        connection->task=message.u.CheckVDM.iTask;
        connection->registered_worker=FALSE;
        connection->worker_failed=FALSE;
        connection->parent_wait=NULL;
        connection->parent_receipt=connection->completed_receipt=0;
        connection->dos_completion_read=FALSE;
    }
    /* Once source has published the no-console record, the launcher and its
     * reservation must use the new identity.  The worker's first PIF request
     * then binds this record, not the resident COMMAND ConsoleRecord. */
    if (!status && NT_SUCCESS((NTSTATUS)message.ReturnValue) && separate_dos &&
        message.u.CheckVDM.VDMState==VDM_NOT_PRESENT)
        connection->console=separate_console;
    else if(separate_context)service_delete_console_context(separate_context);
    if (!status && NT_SUCCESS((NTSTATUS)message.ReturnValue) &&
        message.u.CheckVDM.VDMState==VDM_PRESENT_AND_READY) {
        HANDLE event=NULL;
        service_capture_checked_management_label(connection->service,connection->console,
            &message.u.CheckVDM);
        if (!service_wait_resolve(connection,generation,message.u.CheckVDM.WaitObjectForParent,
                BROKER_VDM_PARENT_WAIT,&event)) {
            connection->parent_wait=event;
            connection->parent_receipt=(uint32_t)(ULONG_PTR)message.u.CheckVDM.WaitObjectForParent;
            connection->completed_receipt=0;
            connection->dos_completion_read=FALSE;
            connection->worker_failed=FALSE;
        }
    }
    LeaveCriticalSection(&connection->service->lock);
    if (status && !message.ReturnValue) message.ReturnValue=status;
if (!OpenNtBaseEncodeCheckReply(&message,request,generation,output,capacity,required))
        return ERROR_INVALID_PARAMETER;
    if (NT_SUCCESS((NTSTATUS)message.ReturnValue) &&
        message.u.CheckVDM.VDMState==VDM_PRESENT_AND_READY) {
        *parent_receipt=(uint32_t)(ULONG_PTR)message.u.CheckVDM.WaitObjectForParent;
        return service_wait_resolve(connection,generation,message.u.CheckVDM.WaitObjectForParent,
            BROKER_VDM_PARENT_WAIT,parent_event);
    }
    return ERROR_SUCCESS;
}


DWORD OpenNtBaseServiceUpdate(OPENNT_BASE_CONNECTION *connection,DWORD pid,DWORD generation,
    const void *input,uint32_t bytes,void *output,uint32_t capacity,uint32_t *required,
    HANDLE *parent_event,uint32_t *parent_receipt)
{
    BASE_API_MSG message={0};
    CSR_THREAD thread={0};
    PCSR_THREAD previous_thread;
    OPENNT_BASE_PROCESS_REGISTRY *previous_registry;
    OPENNT_BASE_SERVICE_RESOURCES resources;
    const OPENNT_BASE_RESOURCE_BINDING *previous_resources;
    uint32_t request,needed=0;
    NTSTATUS status;
    DWORD error;
    if (required) *required=0;
    if (!parent_event || !parent_receipt) return ERROR_INVALID_PARAMETER;
    *parent_event=NULL;*parent_receipt=0;
    if (!connection || !OpenNtBaseServicePeer(connection,pid,generation) ||
        !OpenNtBaseDecodeUpdateCommand(input,bytes,generation,&message,&request) ||
        !OpenNtBaseEncodeUpdateReply(&message,request,generation,NULL,0,&needed))
        return ERROR_INVALID_PARAMETER;
    *required=needed;
    if (!output || capacity<needed) return ERROR_INSUFFICIENT_BUFFER;
    EnterCriticalSection(&connection->service->lock);
    if (message.u.UpdateVDMEntry.BinaryType==BINARY_TYPE_WIN16)
        message.u.UpdateVDMEntry.ConsoleHandle=(HANDLE)-1;
    else if (message.u.UpdateVDMEntry.iTask)
        message.u.UpdateVDMEntry.ConsoleHandle=NULL;
    else if (connection->console)
        message.u.UpdateVDMEntry.ConsoleHandle=connection->console;
    else { error=ERROR_INVALID_HANDLE; goto done; }
    if (message.u.UpdateVDMEntry.EntryIndex==UPDATE_VDM_PROCESS_HANDLE)
        message.u.UpdateVDMEntry.VDMProcessHandle=NtCurrentProcess();
    service_resources_init(&resources,connection,BROKER_VDM_PARENT_WAIT);
    if (message.u.UpdateVDMEntry.EntryIndex==UPDATE_VDM_PROCESS_HANDLE) {
        error=OpenNtBaseReservationRetainWorker(connection->service->reservations,
            connection->reservation,pid,generation,&resources.worker);
        if (error) goto done;
    }
    thread.Process=&connection->process;
    thread.ClientId.UniqueProcess=connection->process.ClientId.UniqueProcess;
    previous_thread=OpenNtBaseBindServerRequestThread(&thread);
    previous_registry=OpenNtBaseBindProcessRegistry(&connection->service->registry);
    previous_resources=OpenNtBaseBindResources(&resources.binding);
    status=OpenNtBaseDispatchOperation((PCSR_API_MSG)&message,BROKER_VDM_UPDATE,
        sizeof(message.u.UpdateVDMEntry));
    OpenNtBaseBindResources(previous_resources);
    OpenNtBaseBindProcessRegistry(previous_registry);
    OpenNtBaseBindServerRequestThread(previous_thread);
    service_resources_release(&resources);
    if (status && !message.ReturnValue) message.ReturnValue=status;
    if (!OpenNtBaseEncodeUpdateReply(&message,request,generation,output,capacity,required)) {
        error=ERROR_INVALID_DATA; goto done;
    }
    if (NT_SUCCESS((NTSTATUS)message.ReturnValue)) {
        if (message.u.UpdateVDMEntry.EntryIndex==UPDATE_VDM_PROCESS_HANDLE) {
            DWORD index,previous;
            uint32_t standard_streams[3];
            connection->registered_worker=TRUE;
            /* The original source process, not BaseSrv, owns its standard
             * handles. BaseSrvDupStandardHandles has copied each source
             * receipt into the reservation's worker-delivery receipt. Close
             * only the source carrier: the reservation copy remains live
             * until the worker consumes it through GetNextVDMCommand. */
            CopyMemory(standard_streams,connection->pending_standard_streams,
                sizeof(standard_streams));
            for (index=0;index<3;++index) {
                uint32_t receipt=standard_streams[index];
                for (previous=0;previous<index;++previous)
                    if (standard_streams[previous]==receipt) break;
                if (receipt && previous==index)
                    (void)broker_vdm_receipt_revoke(&connection->streams,
                        generation,receipt);
            }
            ZeroMemory(connection->pending_standard_streams,
                sizeof(connection->pending_standard_streams));
        }
        if (message.u.UpdateVDMEntry.EntryIndex==UPDATE_VDM_UNDO_CREATION)
            connection->pending_creation=FALSE;
    }
    *parent_receipt=(uint32_t)(ULONG_PTR)message.u.UpdateVDMEntry.WaitObjectForParent;
    error=service_wait_resolve(connection,generation,message.u.UpdateVDMEntry.WaitObjectForParent,
        BROKER_VDM_PARENT_WAIT,parent_event);
    if (error) goto done;
    connection->parent_wait=*parent_event;
    connection->parent_receipt=*parent_receipt;connection->completed_receipt=0;
    connection->dos_completion_read=FALSE;
    error=ERROR_SUCCESS;
done:
    LeaveCriticalSection(&connection->service->lock);
    return error;
}


DWORD OpenNtBaseServiceExitCode(OPENNT_BASE_CONNECTION *connection,DWORD pid,DWORD generation,
    uint32_t parent_receipt,DWORD *exit_code)
{
    BASE_API_MSG message={0};
    CSR_THREAD thread={0};
    PCSR_THREAD previous_thread;
    OPENNT_BASE_PROCESS_REGISTRY *previous_registry;
    NTSTATUS status;
    DWORD error;
    if (!connection || !exit_code || !parent_receipt ||
        !OpenNtBaseServicePeer(connection,pid,generation)) return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&connection->service->lock);
    if (connection->worker_failed) { error=ERROR_PROCESS_ABORTED; goto done; }
    if (connection->completed_receipt==parent_receipt) {
        *exit_code=connection->completed_exit_code;
        connection->completed_receipt=0;
        connection->dos_completion_read=!connection->wow;
        error=ERROR_SUCCESS;goto done;
    }
    /* Original BaseSrvGetVDMExitCode accepts the shared-WOW sentinel and
     * returns its source-defined zero result without looking up a DOS
     * ConsoleRecord.  The standalone connection likewise has no DOS Console
     * for WOW; preserve that original branch rather than rejecting a normal
     * WOW worker completion as ERROR_INVALID_HANDLE. */
    if (!connection->console && !connection->wow) { error=ERROR_INVALID_HANDLE; goto done; }
    message.u.GetVDMExitCode.ConsoleHandle=connection->wow ? (HANDLE)-1 : connection->console;
    /* srvvdm.c owns this exact receipt-shaped hWaitForParent value. */
    message.u.GetVDMExitCode.hParent=(HANDLE)(ULONG_PTR)parent_receipt;
    thread.Process=&connection->process;
    thread.ClientId.UniqueProcess=connection->process.ClientId.UniqueProcess;
    previous_thread=OpenNtBaseBindServerRequestThread(&thread);
    previous_registry=OpenNtBaseBindProcessRegistry(&connection->service->registry);
    status=OpenNtBaseDispatchOperation((PCSR_API_MSG)&message,BROKER_VDM_EXIT_CODE,
        sizeof(message.u.GetVDMExitCode));
    OpenNtBaseBindProcessRegistry(previous_registry);
    OpenNtBaseBindServerRequestThread(previous_thread);
    if (status && !message.ReturnValue) message.ReturnValue=status;
    if (!NT_SUCCESS((NTSTATUS)message.ReturnValue)) { error=RtlNtStatusToDosError((NTSTATUS)message.ReturnValue); goto done; }
    *exit_code=(DWORD)message.u.GetVDMExitCode.ExitCode;
    if (parent_receipt==connection->parent_receipt && *exit_code!=STILL_ACTIVE) {
        connection->parent_receipt=0;
        connection->dos_completion_read=!connection->wow;
    }
    error=ERROR_SUCCESS;
done:
    LeaveCriticalSection(&connection->service->lock);
    return error;
}


DWORD OpenNtBaseServiceReenter(OPENNT_BASE_CONNECTION *connection,DWORD pid,DWORD generation,
    uint32_t increment)
{
    BASE_API_MSG message={0};
    CSR_THREAD thread={0};
    PCSR_THREAD previous_thread;
    OPENNT_BASE_PROCESS_REGISTRY *previous_registry;
    NTSTATUS status;
    DWORD error;

    if (increment!=INCREMENT_REENTER_COUNT && increment!=DECREMENT_REENTER_COUNT)
        return ERROR_INVALID_PARAMETER;
    if (!connection || !OpenNtBaseServicePeer(connection,pid,generation))
        return ERROR_ACCESS_DENIED;
    EnterCriticalSection(&connection->service->lock);
    if (!connection->console) { error=ERROR_INVALID_HANDLE; goto done; }
    message.u.SetReenterCount.ConsoleHandle=connection->console;
    message.u.SetReenterCount.fIncDec=increment;
    thread.Process=&connection->process;
    thread.ClientId.UniqueProcess=connection->process.ClientId.UniqueProcess;
    previous_thread=OpenNtBaseBindServerRequestThread(&thread);
    previous_registry=OpenNtBaseBindProcessRegistry(&connection->service->registry);
    status=OpenNtBaseDispatchOperation((PCSR_API_MSG)&message,BROKER_VDM_REENTER,
        sizeof(message.u.SetReenterCount));
    OpenNtBaseBindProcessRegistry(previous_registry);
    OpenNtBaseBindServerRequestThread(previous_thread);
    if (status && !message.ReturnValue) message.ReturnValue=status;
    error=NT_SUCCESS((NTSTATUS)message.ReturnValue) ? ERROR_SUCCESS :
        RtlNtStatusToDosError((NTSTATUS)message.ReturnValue);
done:
    LeaveCriticalSection(&connection->service->lock);
    return error;
}


DWORD OpenNtBaseServiceGet(OPENNT_BASE_CONNECTION *connection,DWORD pid,DWORD generation,
    const void *input,uint32_t bytes,void **output,uint32_t *output_bytes,HANDLE *wait_event,
    HANDLE standard[3],ULONG *standard_count)
{
    BASE_API_MSG message={0};
    OPENNT_BASE_GET_COMMAND state={0};
    CSR_THREAD thread={0};
    PCSR_THREAD previous_thread;
    OPENNT_BASE_PROCESS_REGISTRY *previous_registry;
    OPENNT_BASE_SERVICE_RESOURCES resources;
    const OPENNT_BASE_RESOURCE_BINDING *previous_resources;
    NTSTATUS status;
    DWORD error;
    if (!output || !output_bytes || !wait_event || !standard || !standard_count)
        return ERROR_INVALID_PARAMETER;
    *output=NULL; *output_bytes=0; *wait_event=NULL;
    standard[0]=standard[1]=standard[2]=NULL;*standard_count=0;
    if (!connection || !OpenNtBaseServicePeer(connection,pid,generation))
        return ERROR_ACCESS_DENIED;
    error=OpenNtBasePrepareGetCommand(input,bytes,generation,&message,&state);
    if (error) return error;
EnterCriticalSection(&connection->service->lock);
    /* The original client sends -1 for the shared-WOW PIF/acquisition path;
     * that selects BaseSrv's WOW record and deliberately has no DOS Console.
     * Every other request retains the reservation's service-local Console
     * identity, never a copied native handle. */
    if (message.u.GetNextVDMCommand.VDMState & ASKING_FOR_WOW_BINARY)
        message.u.GetNextVDMCommand.ConsoleHandle=OPENNT_BASE_CONSOLE_WOW;
    else {
        if (!connection->console) { error=ERROR_INVALID_HANDLE; goto done; }
        message.u.GetNextVDMCommand.ConsoleHandle=connection->console;
    }
    service_resources_init(&resources,connection,BROKER_VDM_WORKER_WAIT);
    thread.Process=&connection->process;
    thread.ClientId.UniqueProcess=connection->process.ClientId.UniqueProcess;
    previous_thread=OpenNtBaseBindServerRequestThread(&thread);
    previous_registry=OpenNtBaseBindProcessRegistry(&connection->service->registry);
    previous_resources=OpenNtBaseBindResources(&resources.binding);
    /* Capture project-only WOW display labels before the original GetNext
     * frees VDMINFO. The original record still owns dispatch/completion. */
    if(message.u.GetNextVDMCommand.VDMState & ASKING_FOR_WOW_BINARY)
        service_capture_wow_management_labels(connection);
    status=OpenNtBaseDispatchOperation((PCSR_API_MSG)&message,BROKER_VDM_GET_NEXT,
        sizeof(message.u.GetNextVDMCommand));
    if(connection->wow)service_capture_wow_management_labels(connection);
    OpenNtBaseBindResources(previous_resources);
    OpenNtBaseBindProcessRegistry(previous_registry);
    OpenNtBaseBindServerRequestThread(previous_thread);
    if (status && !message.ReturnValue) message.ReturnValue=status;
if (!OpenNtBaseFinishGetCommand(&message,&state)) { error=ERROR_INVALID_DATA; goto done; }
    if (message.u.GetNextVDMCommand.StdIn || message.u.GetNextVDMCommand.StdOut ||
        message.u.GetNextVDMCommand.StdErr) {
        HANDLE ids[3]={message.u.GetNextVDMCommand.StdIn,message.u.GetNextVDMCommand.StdOut,
            message.u.GetNextVDMCommand.StdErr};
        DWORD index;
        for (index=0;index<3;++index) if (ids[index]) {
            DWORD previous;
            for (previous=0;previous<index;++previous)
                if (ids[previous]==ids[index]) break;
            if (previous<index) standard[index]=standard[previous];
            else {
                uint32_t receipt=(uint32_t)(ULONG_PTR)ids[index];
                /* A first worker receives its streams through the launch
                 * reservation.  Retained srvvdm.c sends streams for a
                 * present-VDM re-entry directly to that existing worker's
                 * authenticated connection.  Both are the same typed
                 * receipt contract; choose the original delivery owner,
                 * never reinterpret the scalar receipt as a HANDLE. */
                error=OpenNtBaseReservationResolveStream(connection->service->reservations,
                    connection->reservation,receipt,&standard[index]);
                if (error==ERROR_NOT_FOUND)
                    error=broker_vdm_receipt_take(&connection->streams,generation,receipt,
                        BROKER_VDM_STDIN+index,&standard[index]);
                if (error) { error=ERROR_INVALID_HANDLE;goto done; }
            }
        }
        *standard_count=3;
    }
    /* This is an original target event, held by the console record. It crosses
     * process boundaries only as a typed RPC event attachment, never in the
     * copied VDM command record. */
    error=service_wait_resolve(connection,generation,message.u.GetNextVDMCommand.WaitObjectForVDM,
        BROKER_VDM_WORKER_WAIT,wait_event);
    if (error) goto done;
    /* Original GetNext decides when a DOS execution/reentry or parent resume
     * is available. Grant its transport phase only after that result, never
     * from an idle worker's request to acquire the frontend. */
    /* srvvdm.c returns STATUS_NO_MEMORY with no wait object for its ordinary
     * RETURN_ON_NO_COMMAND resume. The original client clears CmdSize and
     * cmdExec32 resumes its parent on that result. Do not require a success
     * command payload to authorize the parent's transport phase. The original
     * return value, execution and task completion stay unchanged. */
    if(!*wait_event && (message.ReturnValue==STATUS_SUCCESS ||
        (message.ReturnValue==STATUS_NO_MEMORY &&
         (message.u.GetNextVDMCommand.VDMState & RETURN_ON_NO_COMMAND))) && connection->process.fVDM &&
        !connection->wow && !(message.u.GetNextVDMCommand.VDMState &
            (ASKING_FOR_ENVIRONMENT|ASKING_FOR_PIF|ASKING_FOR_WOW_BINARY))) {
        error=service_authorize_worker_io(connection,pid);
        /* Redirected/nonfrontend workers have no I/O association to grant. */
        if(error==ERROR_NOT_FOUND)error=ERROR_SUCCESS;
        if(error)goto done;
    }
    *output=state.reply; *output_bytes=state.reply_bytes; state.reply=NULL;
    error=ERROR_SUCCESS;
done:
    LeaveCriticalSection(&connection->service->lock);
    if (error) {
        DWORD index;
        for (index=0;index<3;++index) if (standard[index]) {
            DWORD previous;
            for (previous=0;previous<index;++previous)
                if (standard[previous]==standard[index]) break;
            if (previous==index) CloseHandle(standard[index]);
            standard[index]=NULL;
        }
        *standard_count=0;
    }
    OpenNtBaseReleaseGetCommand(&state);
    return error;
}


DWORD OpenNtBaseServiceExit(OPENNT_BASE_CONNECTION *connection,DWORD pid,DWORD generation,
    BOOL is_wow,ULONG wow_task,BOOL *close_worker_wait)
{
    BASE_API_MSG message={0};
    CSR_THREAD thread={0};
    PCSR_THREAD previous_thread;
    OPENNT_BASE_PROCESS_REGISTRY *previous_registry;
    NTSTATUS status;
    DWORD error=ERROR_SUCCESS;
    HANDLE worker_wait;

    if (!close_worker_wait) return ERROR_INVALID_PARAMETER;
    *close_worker_wait=FALSE;
    if (!connection || !OpenNtBaseServicePeer(connection,pid,generation))
        return ERROR_ACCESS_DENIED;
    /* ExitVDM's branch is selected by the original ConsoleHandle sentinel.
     * Do not let a copied client bit switch an authenticated DOS connection
     * to WOW (or vice versa). */
    if (!!is_wow!=!!connection->wow) return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&connection->service->lock);
    if (is_wow) {
        message.u.ExitVDM.ConsoleHandle=(HANDLE)-1;
        message.u.ExitVDM.iWowTask=wow_task;
    } else {
        if (!connection->console) { error=ERROR_INVALID_HANDLE; goto done; }
        message.u.ExitVDM.ConsoleHandle=connection->console;
        message.u.ExitVDM.iWowTask=0;
        if (connection->process.fVDM)
            service_preserve_parent_results(connection->service,connection->console,FALSE);
    }
    thread.Process=&connection->process;
    thread.ClientId.UniqueProcess=connection->process.ClientId.UniqueProcess;
    previous_thread=OpenNtBaseBindServerRequestThread(&thread);
    previous_registry=OpenNtBaseBindProcessRegistry(&connection->service->registry);
    status=OpenNtBaseDispatchOperation((PCSR_API_MSG)&message,BROKER_VDM_EXIT,
        sizeof(message.u.ExitVDM));
    if(connection->wow)service_capture_wow_management_labels(connection);
    OpenNtBaseBindProcessRegistry(previous_registry);
    OpenNtBaseBindServerRequestThread(previous_thread);
    if (status && !message.ReturnValue) message.ReturnValue=status;
    if (!NT_SUCCESS((NTSTATUS)message.ReturnValue)) {
        error=RtlNtStatusToDosError((NTSTATUS)message.ReturnValue); goto done;
    }
    /* BaseSrvExitDOSTask returns the same client-side wait identity it had
     * published through Get.  Here it is a broker receipt, never a native
     * handle.  Revoke the broker copy only after original record cleanup;
     * the worker still owns its typed local duplicate and closes it below. */
    worker_wait=message.u.ExitVDM.WaitObjectForVDM;
    if (worker_wait) {
        error=broker_vdm_receipt_revoke(&connection->streams,generation,
            (uint32_t)(ULONG_PTR)worker_wait);
        if (error) goto done;
        *close_worker_wait=TRUE;
    }
done:
    LeaveCriticalSection(&connection->service->lock);
    return error;
}


void OpenNtBaseServiceReleaseCommandReply(void *reply)
{
    if (reply) HeapFree(GetProcessHeap(),0,reply);
}
