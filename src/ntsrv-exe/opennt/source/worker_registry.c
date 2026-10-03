/* NTSRV-private worker registry; project adaptation,
 * not an original OpenNT mirror. Physical separation only: existing function
 * bodies, state authority and lock/resource contracts are preserved. */
#include <service_internal.h>

typedef struct service_vdm_admission {
    OPENNT_BASE_CONNECTION *connection;
    DWORD pid,generation,binary;
    HANDLE frontend,parent,parent_export;
    uint32_t receipt;
    BOOL registered;
} service_vdm_admission;

static BOOL service_native_root_selectable(OPENNT_BASE_SERVICE *service,
    DWORD worker_generation,DWORD requested_root);
static VOID CALLBACK service_vdm_process_completed(void *context,BOOLEAN fired);
static DWORD service_vdm_update(service_vdm_admission *state,BOOL undo);
static DWORD service_vdm_admit(void *context,HANDLE process);
/* A resident native worker may be selected only while its original
 * authenticated frontend root is still an open character session. A new
 * root in the same Windows Console is not a replacement for that root. */
static BOOL service_native_root_selectable(OPENNT_BASE_SERVICE *service,
    DWORD worker_generation,DWORD requested_root)
{
    LIST_ENTRY *link,*root_link;
    for(link=service->connections.Flink;link!=&service->connections;link=link->Flink) {
        OPENNT_BASE_CONNECTION *worker=CONTAINING_RECORD(link,OPENNT_BASE_CONNECTION,service_link);
        if(worker->process.SequenceNumber!=worker_generation || !worker->native_worker)
            continue;
        if(!worker->native_root)return TRUE; /* First direct admission. */
        if(!requested_root || worker->native_root!=requested_root)return FALSE;
        for(root_link=service->connections.Flink;root_link!=&service->connections;
            root_link=root_link->Flink) {
            OPENNT_BASE_CONNECTION *root=CONTAINING_RECORD(root_link,OPENNT_BASE_CONNECTION,service_link);
            if(root->process.SequenceNumber==worker->native_root && root->frontend_capability &&
                !root->frontend_closing &&
                WaitForSingleObject(root->process.ProcessHandle,0)==WAIT_TIMEOUT)return TRUE;
        }
        return FALSE;
    }
    /* Prepared worker not connected yet: there is no root to lose. Its
     * reservation/registration checks still gate actual command delivery. */
    return TRUE;
}

void service_signal_worker_states(OPENNT_BASE_SERVICE *service)
{
    LIST_ENTRY *link;
    for(link=service->connections.Flink;link!=&service->connections;link=link->Flink) {
        OPENNT_BASE_CONNECTION *caller=CONTAINING_RECORD(link,OPENNT_BASE_CONNECTION,service_link);
        if(caller->worker_state_changed)(void)SetEvent(caller->worker_state_changed);
    }
}

VOID CALLBACK service_worker_terminated(PVOID context,BOOLEAN fired)
{
    OPENNT_BASE_WORKER_WATCH *watch=context;
    OPENNT_BASE_CONNECTION *connection=NULL;
    LIST_ENTRY *entry;
    OPENNT_BASE_EMPTY_NOTIFY notify=NULL;
    void *notify_context=NULL;
    (void)fired;
    if (!watch || !watch->service) return;
    EnterCriticalSection(&watch->service->lock);
    if(watch->kind!=OPENNT_BASE_WORKER_NATIVE)
        service_preserve_parent_results(watch->service,watch->console,watch->wow);
    /* Equivalent to the selected BaseClientDisconnectRoutine: a one-shot
     * authenticated process-exit signal, never queue polling or a reaper. */
    entry=watch->service->frontend_routes.Flink;
    while (entry!=&watch->service->frontend_routes) {
        OPENNT_FRONTEND_ROUTE *route=CONTAINING_RECORD(entry,OPENNT_FRONTEND_ROUTE,link);
        entry=entry->Flink;
        if (GetProcessId(route->worker)==GetProcessId(watch->process.ProcessHandle))
            service_delete_frontend(route);
    }
    if(watch->kind!=OPENNT_BASE_WORKER_NATIVE)BaseSrvCleanupVDMResources(&watch->process);
    for (entry=watch->service->connections.Flink;
         entry!=&watch->service->connections;entry=entry->Flink) {
        OPENNT_BASE_CONNECTION *caller=CONTAINING_RECORD(entry,OPENNT_BASE_CONNECTION,service_link);
        if (caller->channel_worker_generation==watch->process.SequenceNumber)
            service_clear_frontend_channel(caller);
    }
    (void)OpenNtBaseReservationReleaseWorker(watch->service->reservations,watch->reservation,
        (DWORD)(ULONG_PTR)watch->process.ClientId.UniqueProcess,watch->process.SequenceNumber);
    /* CSR removes the dead process during disconnect rundown.  A standalone
     * RPC context can outlive an abruptly killed client, so detach its local
     * registration now; keep its opaque context until rundown to avoid UAF. */
    for (entry=watch->service->connections.Flink;
         entry!=&watch->service->connections;entry=entry->Flink) {
        OPENNT_BASE_CONNECTION *candidate=CONTAINING_RECORD(entry,OPENNT_BASE_CONNECTION,service_link);
        if (candidate->process.SequenceNumber==watch->process.SequenceNumber) {
            connection=candidate;
            break;
        }
    }
    if (connection) {
        (void)OpenNtBaseRemoveProcess(&watch->service->registry,&connection->process);
        RemoveEntryList(&connection->service_link);
        service_clear_pending_win32record(connection);
        service_clear_win32records(connection);
        broker_vdm_receipts_drain(&connection->streams);
        connection->retired=TRUE;
        InsertTailList(&watch->service->retired_connections,&connection->retired_link);
    }
    RemoveEntryList(&watch->link);
    WakeAllConditionVariable(&watch->service->frontend_changed);
    service_signal_frontend_states(watch->service);
    service_signal_worker_states(watch->service);
    notify=watch->service->empty_notify;
    notify_context=watch->service->empty_notify_context;
    LeaveCriticalSection(&watch->service->lock);
service_clear_management_labels(watch);
    CloseHandle(watch->process.ProcessHandle);
    if(watch->shutdown)CloseHandle(watch->shutdown);
    HeapFree(GetProcessHeap(),0,watch);
    if (notify) notify(notify_context);
}


DWORD OpenNtBaseServiceRegisterNativeBackend(OPENNT_BASE_CONNECTION *connection,
    DWORD pid,DWORD generation,HANDLE frontend,HANDLE stop,HANDLE closed)
{
    HANDLE root=NULL,stop_copy=NULL,closed_copy=NULL;DWORD root_generation=0,error;
    LIST_ENTRY *link;EVENT_BASIC_INFORMATION info;
    SERVICE_COMPARE_HANDLES compare=(SERVICE_COMPARE_HANDLES)GetProcAddress(
        GetModuleHandleW(L"kernelbase.dll"),"CompareObjectHandles");
    SERVICE_QUERY_OBJECT query=(SERVICE_QUERY_OBJECT)GetProcAddress(GetModuleHandleW(L"ntdll.dll"),"NtQueryObject");
    struct {UNICODE_STRING name;WCHAR buffer[256];} object_name;
    HANDLE events[2];DWORD index;
    if(!connection)return ERROR_ACCESS_DENIED;
    if(!compare || !query)return ERROR_CALL_NOT_IMPLEMENTED;
    EnterCriticalSection(&connection->service->lock);
    /* Only the process which claimed the authenticated native reservation may
     * become a native worker.  A connected launcher/root is not sufficient. */
    if(!connection->native_worker ||
        connection->reservation_kind!=OPENNT_BASE_WORKER_NATIVE ||
        connection->frontend_capability || connection->process.fVDM)
        {error=ERROR_INVALID_STATE;goto done;}
    error=OpenNtBaseServiceRetainFrontendRoot(connection,pid,generation,frontend,&root,&root_generation);
    if(error)goto done;
    if(connection->native_root) {
        if(connection->native_root==root_generation) {error=ERROR_INVALID_STATE;goto done;}
        error=ERROR_PIPE_NOT_CONNECTED;goto done;
    }
    if(compare(stop,closed) || compare(stop,frontend) || compare(closed,frontend))
        {error=ERROR_INVALID_PARAMETER;goto done;}
    events[0]=stop;events[1]=closed;
    for(index=0;index<2;++index) {
        ZeroMemory(&object_name,sizeof(object_name));
        if(NtQueryEvent(events[index],EventBasicInformation,&info,sizeof(info),NULL)<0 ||
            info.EventType!=NotificationEvent || info.EventState ||
            query(events[index],1,&object_name,sizeof(object_name),NULL)<0 || object_name.name.Length)
            {error=ERROR_INVALID_PARAMETER;goto done;}
    }
    for(link=connection->service->connections.Flink;link!=&connection->service->connections;link=link->Flink) {
        OPENNT_BASE_CONNECTION *other=CONTAINING_RECORD(link,OPENNT_BASE_CONNECTION,service_link);
        if(other->native_root==root_generation && WaitForSingleObject(other->process.ProcessHandle,0)==WAIT_TIMEOUT)
            {error=ERROR_ALREADY_EXISTS;goto done;}
    }
    if(!DuplicateHandle(GetCurrentProcess(),stop,GetCurrentProcess(),&stop_copy,
        SYNCHRONIZE|EVENT_MODIFY_STATE,FALSE,0) ||
        !DuplicateHandle(GetCurrentProcess(),closed,GetCurrentProcess(),&closed_copy,SYNCHRONIZE,FALSE,0))
        {error=GetLastError();goto done;}
    connection->native_root=root_generation;
    {
        LIST_ENTRY *root_link;
        for(root_link=connection->service->connections.Flink;
            root_link!=&connection->service->connections;root_link=root_link->Flink) {
            OPENNT_BASE_CONNECTION *registered_root=CONTAINING_RECORD(root_link,
                OPENNT_BASE_CONNECTION,service_link);
            if(registered_root->process.SequenceNumber==root_generation)
                {registered_root->frontend_admission_deadline=0;break;}
        }
    }
    if(connection->native_stop)CloseHandle(connection->native_stop);
    if(connection->native_closed)CloseHandle(connection->native_closed);
    connection->native_stop=stop_copy;connection->native_closed=closed_copy;
    stop_copy=closed_copy=NULL;service_signal_frontend_states(connection->service);error=0;
done:
    if(root)CloseHandle(root);if(stop_copy)CloseHandle(stop_copy);if(closed_copy)CloseHandle(closed_copy);
    LeaveCriticalSection(&connection->service->lock);return error;
}


DWORD OpenNtBaseServiceRetainCommandWorker(OPENNT_BASE_CONNECTION *connection,
    DWORD pid,DWORD generation,HANDLE *worker)
{
    OPENNT_BASE_SERVICE *service;
    LIST_ENTRY *link;
    HANDLE selected=NULL,retained=NULL;
    DWORD error=ERROR_NOT_READY;
    if (!worker) return ERROR_INVALID_PARAMETER;
    *worker=NULL;
    if (!connection) return ERROR_ACCESS_DENIED;
    service=connection->service;
    EnterCriticalSection(&service->lock);
    if (!OpenNtBaseServicePeer(connection,pid,generation)) {
        error=ERROR_ACCESS_DENIED; goto done;
    }
    /* Membership alone is not a grant. Original UpdateDOS deliberately omits
     * the pair event for a nonzero DosSesId: that caller waits on its worker
     * process. Require completed Check/Update plus its authenticated reservation
     * in that case; never fabricate a parent event or accept an unprepared peer. */
    if (connection->wow || connection->process.fVDM || connection->native_worker || connection->worker_failed) goto done;
    if (connection->selected_native_generation) {
        for (link=service->worker_watches.Flink;link!=&service->worker_watches;link=link->Flink) {
            OPENNT_BASE_WORKER_WATCH *watch=CONTAINING_RECORD(link,OPENNT_BASE_WORKER_WATCH,link);
            if (watch->kind==OPENNT_BASE_WORKER_NATIVE && watch->console==connection->console &&
                watch->process.SequenceNumber==connection->selected_native_generation) {
                selected=watch->process.ProcessHandle;break;
            }
        }
        /* Never silently choose a replacement after this worker's death. */
        goto retain_selected;
    }
    if (connection->reservation_kind==OPENNT_BASE_WORKER_NATIVE) {
        /* Only the authenticated creator's live prepared reservation grants
         * this first native attachment. Console membership is not a grant;
         * reuse by another request needs its own admitted task reference. */
        if (!connection->reservation) goto done;
    } else if (connection->parent_wait) {
        if (WaitForSingleObject(connection->parent_wait,0)!=WAIT_TIMEOUT) goto done;
    } else if (!connection->task || !connection->registered_worker || !connection->reservation)
        goto done;
    if (connection->reservation) {
        error=OpenNtBaseReservationRetainWorker(service->reservations,
            connection->reservation,pid,generation,&retained);
        if (error) goto done;
        selected=retained;
    } else {
        for (link=service->worker_watches.Flink;link!=&service->worker_watches;link=link->Flink) {
            OPENNT_BASE_WORKER_WATCH *watch=CONTAINING_RECORD(link,OPENNT_BASE_WORKER_WATCH,link);
            if (watch->kind==OPENNT_BASE_WORKER_DOS && watch->console==connection->console) {
                if (selected) { error=ERROR_INVALID_DATA; goto done; }
                selected=watch->process.ProcessHandle;
            }
        }
    }
retain_selected:
    if (!selected || WaitForSingleObject(selected,0)!=WAIT_TIMEOUT) {
        error=ERROR_PROCESS_ABORTED; goto done;
    }
    if (!DuplicateHandle(GetCurrentProcess(),selected,GetCurrentProcess(),worker,
            PROCESS_QUERY_LIMITED_INFORMATION|SYNCHRONIZE,FALSE,0))
        error=GetLastError();
    else error=ERROR_SUCCESS;
done:
    if (retained) CloseHandle(retained);
    LeaveCriticalSection(&service->lock);
    return error;
}


DWORD OpenNtBaseServiceWorkerShutdownEvent(OPENNT_BASE_CONNECTION *worker,DWORD pid,
    DWORD generation,HANDLE *shutdown)
{
    LIST_ENTRY *link;
    DWORD error=ERROR_ACCESS_DENIED;
    if(!worker || !shutdown)return ERROR_INVALID_PARAMETER;
    *shutdown=NULL;
    EnterCriticalSection(&worker->service->lock);
    if(OpenNtBaseServicePeer(worker,pid,generation))
        for(link=worker->service->worker_watches.Flink;link!=&worker->service->worker_watches;link=link->Flink) {
            OPENNT_BASE_WORKER_WATCH *watch=CONTAINING_RECORD(link,OPENNT_BASE_WORKER_WATCH,link);
            if(watch->process.SequenceNumber!=generation)continue;
            error=DuplicateHandle(GetCurrentProcess(),watch->shutdown,GetCurrentProcess(),shutdown,
                SYNCHRONIZE,FALSE,0) ? ERROR_SUCCESS : GetLastError();
            break;
        }
    LeaveCriticalSection(&worker->service->lock);
    return error;
}


DWORD OpenNtBaseServiceWorkerStateChanged(OPENNT_BASE_CONNECTION *caller,DWORD pid,
    DWORD generation,HANDLE *state_changed)
{
    DWORD error=ERROR_ACCESS_DENIED;
    if(!state_changed)return ERROR_INVALID_PARAMETER;
    *state_changed=NULL;
    if(!caller)return error;
    EnterCriticalSection(&caller->service->lock);
    if(OpenNtBaseServicePeer(caller,pid,generation) && !caller->native_worker &&
        !caller->process.fVDM) {
        if(!caller->worker_state_changed)
            caller->worker_state_changed=CreateEventW(NULL,FALSE,FALSE,NULL);
        if(!caller->worker_state_changed)error=GetLastError();
        else if(!DuplicateHandle(GetCurrentProcess(),caller->worker_state_changed,
            GetCurrentProcess(),state_changed,SYNCHRONIZE,FALSE,0))error=GetLastError();
        else error=ERROR_SUCCESS;
    }
    LeaveCriticalSection(&caller->service->lock);
    return error;
}


DWORD OpenNtBaseServiceCreateReservation(OPENNT_BASE_CONNECTION *connection,DWORD pid,
    DWORD generation,ULONG task,uint64_t *reservation)
{
    if (!connection || !reservation) return ERROR_INVALID_PARAMETER;
    if (!OpenNtBaseServicePeer(connection,pid,generation)) return ERROR_ACCESS_DENIED;
    if (!connection->console && !connection->wow) return ERROR_INVALID_HANDLE;
    DWORD error=OpenNtBaseReservationCreate(connection->service->reservations,pid,generation,
        task,connection->console,connection->wow,reservation);
    if (!error) connection->reservation=*reservation;
    return error;
}


DWORD OpenNtBaseServiceCreateNativeReservation(OPENNT_BASE_CONNECTION *connection,
    DWORD pid,DWORD generation,uint64_t *reservation)
{
    DWORD error;
    if (!connection || !reservation) return ERROR_INVALID_PARAMETER;
    *reservation=0;
    if (!OpenNtBaseServicePeer(connection,pid,generation)) return ERROR_ACCESS_DENIED;
    /* Reuse the execution-Console discriminator, not a frontend capability.
     * No CheckVDM/DOS record is manufactured for a native worker. */
    error=service_bind_existing_console(connection);
    if (error) return error;
    EnterCriticalSection(&connection->service->lock);
    if (connection->reservation || connection->pending_creation || connection->selected_native_generation ||
            connection->process.fVDM || connection->native_worker || connection->wow)
        error=ERROR_INVALID_STATE;
    else {
        error=OpenNtBaseReservationCreateKind(connection->service->reservations,
            pid,generation,0,connection->console,OPENNT_BASE_WORKER_NATIVE,reservation);
        if (!error) {
            connection->reservation=*reservation;
            connection->reservation_kind=OPENNT_BASE_WORKER_NATIVE;
            service_signal_worker_states(connection->service);
        }
    }
    LeaveCriticalSection(&connection->service->lock);
    return error;
}


DWORD OpenNtBaseServiceSelectNativeWorker(OPENNT_BASE_CONNECTION *connection,
    DWORD pid,DWORD generation,HANDLE *worker)
{
    OPENNT_BASE_SERVICE *service;OPENNT_BASE_WORKER_WATCH *selected=NULL;
    LIST_ENTRY *link;DWORD error;
    if(!worker)return ERROR_INVALID_PARAMETER;
    *worker=NULL;
    if(!connection || !OpenNtBaseServicePeer(connection,pid,generation))return ERROR_ACCESS_DENIED;
    error=service_bind_existing_console(connection);
    if(error)return error;
    service=connection->service;
    EnterCriticalSection(&service->lock);
    if(!OpenNtBaseServicePeer(connection,pid,generation)) {error=ERROR_ACCESS_DENIED;goto done;}
    if(connection->selected_native_generation) {
        error=OpenNtBaseServiceRetainCommandWorker(connection,pid,generation,worker);goto done;
    }
    if(connection->process.fVDM || connection->native_worker || connection->wow ||
        (!connection->dos_completion_read && (connection->pending_creation ||
            connection->reservation || connection->task || connection->parent_wait))) {
        error=ERROR_INVALID_STATE;goto done;
    }
    error=ERROR_NOT_FOUND;
    for(link=service->worker_watches.Flink;link!=&service->worker_watches;link=link->Flink) {
        OPENNT_BASE_WORKER_WATCH *watch=CONTAINING_RECORD(link,OPENNT_BASE_WORKER_WATCH,link);
        if(watch->kind!=OPENNT_BASE_WORKER_NATIVE || watch->console!=connection->console ||
            watch->termination_requested || WaitForSingleObject(watch->process.ProcessHandle,0)!=WAIT_TIMEOUT ||
            !service_native_root_selectable(service,watch->process.SequenceNumber,
                connection->retained_frontend_root))continue;
        if(selected) {error=ERROR_INVALID_DATA;goto done;}
        selected=watch;
    }
    if(selected) {
        if(!DuplicateHandle(GetCurrentProcess(),selected->process.ProcessHandle,GetCurrentProcess(),worker,
            PROCESS_QUERY_LIMITED_INFORMATION|SYNCHRONIZE,FALSE,0))error=GetLastError();
        else {connection->selected_native_generation=selected->process.SequenceNumber;error=ERROR_SUCCESS;}
    }
done:
    LeaveCriticalSection(&service->lock);return error;
}


DWORD OpenNtBaseServicePrepareWorker(OPENNT_BASE_CONNECTION *connection,DWORD pid,DWORD generation,
    uint64_t reservation,HANDLE worker)
{
    if (!connection) return ERROR_INVALID_PARAMETER;
    if (!OpenNtBaseServicePeer(connection,pid,generation)) return ERROR_ACCESS_DENIED;
    return OpenNtBaseReservationPrepareWorker(connection->service->reservations,reservation,
        pid,generation,worker);
}


/* Covers both the pre-Connect failure interval and original worker-exit
 * completion. The registered worker watch still owns original cleanup;
 * this connection-owned wait only completes its launcher's direct receipt. */
static VOID CALLBACK service_vdm_process_completed(void *context,BOOLEAN fired)
{
    OPENNT_BASE_CONNECTION *parent=context;DWORD code=0;
    (void)fired;
    EnterCriticalSection(&parent->service->lock);
    if(!parent->vdm_exit_cancelled && parent->parent_wait && parent->parent_receipt) {
        if(parent->parent_is_worker_exit && GetExitCodeProcess(parent->vdm_exit_process,&code)) {
            parent->completed_receipt=parent->parent_receipt;
            parent->completed_exit_code=code;parent->worker_failed=FALSE;
            if(parent->pending_vdm_binary==BINARY_TYPE_DOS && parent->task)
                service_retire_completed_root(parent,0);
            (void)SetEvent(parent->parent_wait);
        } else if(WaitForSingleObject(parent->parent_wait,0)==WAIT_TIMEOUT) {
            parent->worker_failed=TRUE;
            (void)SetEvent(parent->parent_wait);
        }
    }
    LeaveCriticalSection(&parent->service->lock);
}


/* Call the existing source-shaped Update adapter; do not duplicate srvvdm's
 * registration or undo policy in the product creator. */
static DWORD service_vdm_update(service_vdm_admission *state,BOOL undo)
{
    BASE_API_MSG message={0};BYTE input[128],output[32];uint32_t bytes=0,required=0;
    DWORD error;
    message.u.UpdateVDMEntry.EntryIndex=undo ? UPDATE_VDM_UNDO_CREATION : UPDATE_VDM_PROCESS_HANDLE;
    message.u.UpdateVDMEntry.BinaryType=state->binary;
    message.u.UpdateVDMEntry.iTask=state->connection->task;
    if(undo)message.u.UpdateVDMEntry.VDMCreationState=state->registered ? VDM_FULLY_CREATED : VDM_PARTIALLY_CREATED;
    if(!OpenNtBaseEncodeUpdateCommand(&message,1,state->generation,input,sizeof(input),&bytes))
        return ERROR_INVALID_DATA;
    error=OpenNtBaseServiceUpdate(state->connection,state->pid,state->generation,
        input,bytes,output,sizeof(output),&required,&state->parent,&state->receipt);
    if(!error && (!OpenNtBaseApplyUpdateReply(output,required,state->generation,1,&message)))
        error=ERROR_INVALID_DATA;
    if(!error && !NT_SUCCESS((NTSTATUS)message.ReturnValue))
        error=RtlNtStatusToDosError((NTSTATUS)message.ReturnValue);
    return error;
}

static DWORD service_vdm_admit(void *context,HANDLE process)
{
    service_vdm_admission *state=context;DWORD error;
    error=service_vdm_update(state,FALSE);
    if(error)return error;
    state->registered=TRUE;
    EnterCriticalSection(&state->connection->service->lock);
    if(!state->parent) {
        HANDLE event;
        OPENNT_BASE_SERVICE_RESOURCES resources;
        if(state->binary!=BINARY_TYPE_DOS || !state->connection->task) {
            error=ERROR_INVALID_DATA;goto registered;
        }
        /* srvvdm deliberately returned NULL: keep its worker-exit semantics,
         * with a broker completion receipt instead of exporting that process
         * as the launcher's wait target. The original DOS record is untouched. */
        event=CreateEventW(NULL,TRUE,FALSE,NULL);
        if(!event){error=GetLastError();goto registered;}
        service_resources_init(&resources,state->connection,BROKER_VDM_PARENT_WAIT);
        error=service_wait_deliver(&resources,event,&state->receipt)==STATUS_SUCCESS ?
            ERROR_SUCCESS : ERROR_NOT_ENOUGH_MEMORY;
        CloseHandle(event);
        if(error)goto registered;
        error=broker_vdm_receipt_resolve(&state->connection->streams,state->generation,
            state->receipt,BROKER_VDM_PARENT_WAIT,&state->parent);
        if(error)goto registered;
        state->connection->parent_wait=state->parent;
        state->connection->parent_receipt=state->receipt;
        state->connection->parent_is_worker_exit=TRUE;
    }
    if(state->connection->vdm_exit_watch || state->connection->vdm_exit_process) {
        error=ERROR_INVALID_STATE;goto registered;
    }
    state->connection->vdm_exit_cancelled=FALSE;
    if(!DuplicateHandle(GetCurrentProcess(),process,GetCurrentProcess(),
        &state->connection->vdm_exit_process,PROCESS_QUERY_LIMITED_INFORMATION|SYNCHRONIZE,FALSE,0) ||
        !RegisterWaitForSingleObject(&state->connection->vdm_exit_watch,
            state->connection->vdm_exit_process,service_vdm_process_completed,
            state->connection,INFINITE,WT_EXECUTEONLYONCE))error=GetLastError();
registered:
    LeaveCriticalSection(&state->connection->service->lock);
    if(error)return error;
    /* Update returns a borrowed receipt-table event. Typed RPC consumes its
     * exported handle, just as Server_Update's export_handles does. Keep the
     * table's event intact and prepare its transport duplicate before Resume. */
    if(state->parent && !DuplicateHandle(GetCurrentProcess(),state->parent,
        GetCurrentProcess(),&state->parent_export,0,FALSE,DUPLICATE_SAME_ACCESS))
        return GetLastError();
    if(state->binary==BINARY_TYPE_DOS) {
        error=OpenNtBaseServiceRequestFrontend(state->connection,state->pid,
            state->generation,state->frontend);
        if(error==ERROR_ALREADY_EXISTS)error=ERROR_SUCCESS;
    }
    return error;
}

DWORD OpenNtBaseServiceStartVdmWorker(OPENNT_BASE_CONNECTION *connection,DWORD pid,
    DWORD generation,DWORD characters,const WCHAR *environment,DWORD show,HANDLE frontend,
    HANDLE *worker,HANDLE *parent,DWORD *receipt)
{
    WCHAR image[MAX_PATH],*slash;CHAR ansi[MAX_PATH],kernel[MAX_PATH],*ansi_slash;
    OPENNT_BASE_VDM_CONFIG config;
    const OPENNT_BASE_VDM_CONFIG *previous;
    UNICODE_STRING command={0};ULONG size=0;
    STARTUPINFOW startup={sizeof(startup)};
    service_vdm_admission admission={0};uint64_t reservation=0;
    DWORD error,length,flags;
    if(!worker || !parent || !receipt)return ERROR_INVALID_PARAMETER;
    *worker=*parent=NULL;*receipt=0;
    if(!connection || !OpenNtBaseServicePeer(connection,pid,generation))return ERROR_ACCESS_DENIED;
    if(!environment || characters<2 || characters>65536 || show>SW_FORCEMINIMIZE ||
        environment[characters-1] || environment[characters-2])return ERROR_INVALID_PARAMETER;
    /* Serialize only admission mutation, never registration/creation waits. */
    EnterCriticalSection(&connection->service->lock);
    if(!connection->pending_creation || connection->vdm_starting || connection->reservation ||
        connection->process.fVDM || connection->native_worker ||
        (connection->pending_vdm_binary!=BINARY_TYPE_DOS &&
         connection->pending_vdm_binary!=BINARY_TYPE_WIN16 &&
         connection->pending_vdm_binary!=BINARY_TYPE_SEPWOW))error=ERROR_INVALID_STATE;
    else {
        connection->vdm_starting=TRUE;error=0;
        admission.connection=connection;admission.pid=pid;admission.generation=generation;
        admission.binary=connection->pending_vdm_binary;admission.frontend=frontend;
    }
    LeaveCriticalSection(&connection->service->lock);
    if(error)return error;
    error=OpenNtBaseServiceCreateReservation(connection,pid,generation,connection->task,&reservation);
    if(error)goto done;
    length=GetModuleFileNameW(NULL,image,ARRAYSIZE(image));
    if(!length || length>=ARRAYSIZE(image) || !(slash=wcsrchr(image,L'\\')) ||
        wcscpy_s(slash+1,ARRAYSIZE(image)-(size_t)(slash+1-image),L"ntvdm.exe"))
        {error=ERROR_BAD_PATHNAME;goto done;}
    if(!WideCharToMultiByte(CP_ACP,0,image,-1,ansi,sizeof(ansi),NULL,NULL))
        {error=GetLastError();goto done;}
    ansi_slash=strrchr(ansi,'\\');
    if(!ansi_slash || sprintf_s(kernel,sizeof(kernel),"%.*s\\system32\\krnl386",
        (int)(ansi_slash-ansi),ansi)<=0 || !OpenNtBaseInitializeVdmConfig(&config,ansi,kernel))
        {error=ERROR_BAD_PATHNAME;goto done;}
    previous=OpenNtBaseBindVdmConfig(&config);
    if(!BaseGetVdmConfigInfo(image,admission.binary==BINARY_TYPE_DOS ? connection->task : 0,
        admission.binary,&command,&size))error=GetLastError();
    (void)OpenNtBaseBindVdmConfig(previous);
    if(error)goto done;
    startup.dwFlags=STARTF_USESHOWWINDOW;startup.wShowWindow=(WORD)show;
    flags=CREATE_UNICODE_ENVIRONMENT |
        (admission.binary==BINARY_TYPE_DOS ? DETACHED_PROCESS : CREATE_NO_WINDOW);
    error=broker_worker_start_admitted(connection,pid,generation,reservation,image,
        command.Buffer,(void *)environment,flags,&startup,service_vdm_admit,&admission,worker);
    if(!error){*parent=admission.parent_export;admission.parent_export=NULL;*receipt=admission.receipt;}
done:
    if(error) {
        EnterCriticalSection(&connection->service->lock);
        connection->vdm_exit_cancelled=TRUE;
        LeaveCriticalSection(&connection->service->lock);
        if(connection->vdm_exit_watch) {
            (void)UnregisterWaitEx(connection->vdm_exit_watch,INVALID_HANDLE_VALUE);
            connection->vdm_exit_watch=NULL;
        }
        if(connection->vdm_exit_process) {
            CloseHandle(connection->vdm_exit_process);connection->vdm_exit_process=NULL;
        }
        (void)service_vdm_update(&admission,TRUE);
        if(reservation)(void)OpenNtBaseServiceReleaseReservation(connection,pid,generation,reservation);
    }
    if(command.Buffer)RtlFreeUnicodeString(&command);
    if(admission.parent_export)CloseHandle(admission.parent_export);
    EnterCriticalSection(&connection->service->lock);
    connection->vdm_starting=FALSE;
    LeaveCriticalSection(&connection->service->lock);
    return error;
}


DWORD OpenNtBaseServiceStartNativeWorker(OPENNT_BASE_CONNECTION *connection,DWORD pid,
    DWORD generation,HANDLE *worker)
{
    WCHAR image[MAX_PATH],command[MAX_PATH+3],*slash;
    STARTUPINFOW startup={sizeof(startup)};
    uint64_t reservation=0;DWORD length,error;
    if(!worker)return ERROR_INVALID_PARAMETER;
    *worker=NULL;
    error=OpenNtBaseServiceSelectNativeWorker(connection,pid,generation,worker);
    if(error!=ERROR_NOT_FOUND)return error;
    error=OpenNtBaseServiceCreateNativeReservation(connection,pid,generation,&reservation);
    if(error)return error;
    /* Same-package product worker only. No remote executable/flags command. */
    length=GetModuleFileNameW(NULL,image,ARRAYSIZE(image));
    if(!length || length>=ARRAYSIZE(image) || !(slash=wcsrchr(image,L'\\')))
        {error=ERROR_BAD_PATHNAME;goto done;}
    if(wcscpy_s(slash+1,ARRAYSIZE(image)-(size_t)(slash+1-image),L"ntvwm.exe") ||
        swprintf_s(command,ARRAYSIZE(command),L"\"%ls\"",image)<0)
        {error=ERROR_FILENAME_EXCED_RANGE;goto done;}
    startup.dwFlags=STARTF_USESHOWWINDOW;startup.wShowWindow=SW_HIDE;
    error=broker_worker_start(connection,pid,generation,reservation,image,command,NULL,
        CREATE_NEW_CONSOLE,&startup,worker);
done:
    if(error)(void)OpenNtBaseServiceReleaseReservation(connection,pid,generation,reservation);
    return error;
}


DWORD OpenNtBaseServiceReleaseReservation(OPENNT_BASE_CONNECTION *connection,DWORD pid,
    DWORD generation,uint64_t reservation)
{
    DWORD error;
    if (!connection) return ERROR_INVALID_PARAMETER;
    if (!OpenNtBaseServicePeer(connection,pid,generation)) return ERROR_ACCESS_DENIED;
    EnterCriticalSection(&connection->service->lock);
    error=OpenNtBaseReservationRelease(connection->service->reservations,reservation,pid,generation);
    if (!error) {
        connection->reservation=0; connection->pending_creation=FALSE;
        connection->reservation_kind=OPENNT_BASE_WORKER_DOS;
        connection->parent_wait=NULL; connection->worker_failed=FALSE;
        connection->parent_is_worker_exit=FALSE;
        connection->parent_receipt=connection->completed_receipt=0;
        connection->dos_completion_read=FALSE;
        service_signal_worker_states(connection->service);
    }
    LeaveCriticalSection(&connection->service->lock);
    return error;
}


BOOL OpenNtBaseServiceWorkerReservation(OPENNT_BASE_CONNECTION *connection,uint64_t *reservation,
    ULONG *task,HANDLE *console)
{
    if (!connection || !connection->reservation || !reservation || !task || !console) return FALSE;
    *reservation=connection->reservation;*task=connection->task;*console=connection->console;
    return TRUE;
}
