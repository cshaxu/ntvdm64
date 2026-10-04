/* NTSRV-private native commands; project adaptation,
 * not an original OpenNT mirror. Physical separation only: existing function
 * bodies, state authority and lock/resource contracts are preserved. */
#include <service_internal.h>

static void service_delete_win32record(OPENNT_BASE_WIN32RECORD *record);
static BOOL service_launcher_connected(OPENNT_BASE_SERVICE *service,DWORD generation);
static DWORD service_next_win32record(OPENNT_BASE_CONNECTION *connection,DWORD *request);
static void service_query_native_image(DWORD process_id,WCHAR image[OPENNT_BASE_WORKER_IMAGE_CHARS]);
static OPENNT_BASE_WIN32RECORD *service_native_result_record(OPENNT_BASE_CONNECTION *caller,
    DWORD request,OPENNT_BASE_CONNECTION **owner);

static void service_delete_win32record(OPENNT_BASE_WIN32RECORD *record)
{
    if (!record) return;
    RemoveEntryList(&record->link);
    /* Callback only signals a service event; it never takes the service lock
     * or retains this record, so draining under that lock is safe. */
    if(record->gui_wait)UnregisterWaitEx(record->gui_wait,INVALID_HANDLE_VALUE);
    if(record->gui_process)CloseHandle(record->gui_process);
    if(record->receipt)CloseHandle(record->receipt);
    HeapFree(GetProcessHeap(),0,record);
}

void service_clear_pending_win32record(OPENNT_BASE_CONNECTION *connection)
{
    if (connection->pending_win32record) {
        HeapFree(GetProcessHeap(),0,connection->pending_win32record);
        connection->pending_win32record=NULL;
    }
}

void service_clear_win32records(OPENNT_BASE_CONNECTION *connection)
{
    while (!IsListEmpty(&connection->win32records)) {
        OPENNT_BASE_WIN32RECORD *record=CONTAINING_RECORD(connection->win32records.Flink,
            OPENNT_BASE_WIN32RECORD,link);
        LIST_ENTRY *entry;
        BOOL retained=FALSE;
        /* Preserve a direct result before worker rundown destroys its record.
         * Transfer the existing record to its authenticated launcher, not a
         * second registry, single-slot cache or target termination policy. */
        if(record->receipt) {
            for(entry=connection->service->connections.Flink;
                entry!=&connection->service->connections;entry=entry->Flink) {
                OPENNT_BASE_CONNECTION *launcher=CONTAINING_RECORD(entry,
                    OPENNT_BASE_CONNECTION,service_link);
                if(launcher==connection ||
                    launcher->process.SequenceNumber!=record->launcher_generation)continue;
                if(!record->completed) {
                    record->completed=TRUE;record->exit_code=0;
                    record->completion_error=ERROR_PROCESS_ABORTED;
                }
                RemoveEntryList(&record->link);
                InsertTailList(&launcher->win32records,&record->link);
                retained=TRUE;
                break;
            }
            (void)SetEvent(record->receipt);
        }
        if(!retained)service_delete_win32record(record);
    }
    connection->native_inflight=0;
}

static BOOL service_launcher_connected(OPENNT_BASE_SERVICE *service,DWORD generation)
{
    LIST_ENTRY *link;
    for(link=service->connections.Flink;link!=&service->connections;link=link->Flink) {
        OPENNT_BASE_CONNECTION *item=CONTAINING_RECORD(link,OPENNT_BASE_CONNECTION,service_link);
        if(item->process.SequenceNumber==generation)return TRUE;
    }
    return FALSE;
}

void service_release_launcher_results(OPENNT_BASE_SERVICE *service,DWORD generation)
{
    LIST_ENTRY *link;
    LIST_ENTRY *gui_link=service->gui_records.Flink;
    while(gui_link!=&service->gui_records) {
        OPENNT_BASE_WIN32RECORD *record=CONTAINING_RECORD(gui_link,OPENNT_BASE_WIN32RECORD,link);
        gui_link=gui_link->Flink;
        if(record->completed && record->launcher_generation==generation)
            service_delete_win32record(record);
    }
    for(link=service->connections.Flink;link!=&service->connections;link=link->Flink) {
        OPENNT_BASE_CONNECTION *worker=CONTAINING_RECORD(link,OPENNT_BASE_CONNECTION,service_link);
        LIST_ENTRY *entry=worker->win32records.Flink;
        while(entry!=&worker->win32records) {
            OPENNT_BASE_WIN32RECORD *record=CONTAINING_RECORD(entry,OPENNT_BASE_WIN32RECORD,link);
            entry=entry->Flink;
            if(record->completed && record->launcher_generation==generation)
                service_delete_win32record(record);
        }
    }
}

static VOID CALLBACK service_gui_exit(PVOID context,BOOLEAN fired)
{
    OPENNT_BASE_SERVICE *service=context;
    UNREFERENCED_PARAMETER(fired);
    (void)SetEvent(service->frontend_lifetime_changed);
}

BOOL service_prune_gui_records(OPENNT_BASE_SERVICE *service)
{
    LIST_ENTRY *entry=service->gui_records.Flink;
    BOOL changed=FALSE;
    while(entry!=&service->gui_records) {
        OPENNT_BASE_WIN32RECORD *record=CONTAINING_RECORD(entry,OPENNT_BASE_WIN32RECORD,link);
        entry=entry->Flink;
        if(!record->completed && WaitForSingleObject(record->gui_process,0)==WAIT_OBJECT_0) {
            record->completion_error=GetExitCodeProcess(record->gui_process,&record->exit_code) ?
                ERROR_SUCCESS : GetLastError();
            record->completed=TRUE;
            (void)SetEvent(record->receipt);
            changed=TRUE;
        }
        if(record->completed && !service_launcher_connected(service,record->launcher_generation)) {
            service_delete_win32record(record);changed=TRUE;
        }
    }
    return changed;
}

static DWORD service_next_win32record(OPENNT_BASE_CONNECTION *connection,DWORD *request)
{
    if(!connection || !request || connection->next_win32record==MAXDWORD)
        return ERROR_ARITHMETIC_OVERFLOW;
    if(!++connection->next_win32record)++connection->next_win32record;
    *request=connection->next_win32record;
    return ERROR_SUCCESS;
}

static void service_query_native_image(DWORD process_id,WCHAR image[OPENNT_BASE_WORKER_IMAGE_CHARS])
{
    HANDLE process;
    DWORD characters=OPENNT_BASE_WORKER_IMAGE_CHARS;
    if(!image)return;
    image[0]=L'\0';
    process=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,FALSE,process_id);
    if(process) {
        (void)QueryFullProcessImageNameW(process,0,image,&characters);
        CloseHandle(process);
    }
    if(!image[0])lstrcpynW(image,L"<UNKNOWN>",OPENNT_BASE_WORKER_IMAGE_CHARS);
}

void service_clear_frontend_channel(OPENNT_BASE_CONNECTION *connection)
{
    if(connection->native_command_payload)HeapFree(GetProcessHeap(),0,connection->native_command_payload);
    connection->native_command_payload=NULL;connection->native_command_bytes=0;
    connection->native_command_pending=FALSE;
    if (connection->frontend_execution) CloseHandle(connection->frontend_execution);
    if (connection->channel_frontend) CloseHandle(connection->channel_frontend);
    connection->frontend_execution=NULL;
    connection->channel_frontend=NULL;connection->channel_worker_generation=0;
    connection->frontend_channel_root=0;
    service_clear_pending_win32record(connection);
}


DWORD OpenNtBaseServiceCompleteWorkerChannel(OPENNT_BASE_CONNECTION *connection,DWORD pid,DWORD generation,
    DWORD request,DWORD exit_code)
{
    return OpenNtBaseServiceCompleteNativeRequest(connection,pid,generation,request,exit_code,0,0);
}


DWORD OpenNtBaseServiceCompleteNativeRequest(OPENNT_BASE_CONNECTION *connection,DWORD pid,DWORD generation,
    DWORD request,DWORD exit_code,DWORD io_error,DWORD io_flags)
{
    DWORD error=ERROR_ACCESS_DENIED;
    if(!connection)return ERROR_INVALID_PARAMETER;
    if(io_flags & ~NATIVE_COMPLETION_CONSOLE_EMPTY)return ERROR_INVALID_DATA;
    if(!request && (io_error || io_flags))return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&connection->service->lock);
    if(OpenNtBaseServicePeer(connection,pid,generation) && connection->native_worker) {
        OPENNT_BASE_WIN32RECORD *record=NULL;
        LIST_ENTRY *link;
        if(!request) error=ERROR_SUCCESS; /* I/O resume, not a native task. */
        else for(link=connection->win32records.Flink;link!=&connection->win32records;link=link->Flink) {
            OPENNT_BASE_WIN32RECORD *candidate=CONTAINING_RECORD(link,OPENNT_BASE_WIN32RECORD,link);
            if(candidate->request==request) { record=candidate;break; }
        }
        if(request && !record)error=ERROR_INVALID_STATE;
        else if(request) {
            if(record->completed){error=ERROR_INVALID_STATE;goto done;}
            /* A preflight failure has no bound target; its error was returned
             * on the direct channel, so it has no successful exit receipt. */
            if(record->process_id) {
                record->exit_code=exit_code;
                record->io_error=io_error;
                record->io_flags=io_flags;
                record->completed=TRUE;
                if(!record->receipt || !SetEvent(record->receipt)) {
                    error=record->receipt ? GetLastError() : ERROR_INVALID_STATE;
                    record->completed=FALSE;
                    goto done;
                }
                if(!service_launcher_connected(connection->service,record->launcher_generation))
                    service_delete_win32record(record);
            } else service_delete_win32record(record);
            --connection->native_inflight;
            service_signal_frontend_states(connection->service);error=ERROR_SUCCESS; }
    }
done:
    LeaveCriticalSection(&connection->service->lock);return error;
}



DWORD OpenNtBaseServiceBindNativeTarget(OPENNT_BASE_CONNECTION *connection,DWORD pid,
    DWORD generation,DWORD request,HANDLE target,HANDLE receipt)
{
    DWORD error=ERROR_ACCESS_DENIED,target_pid=0;
    LIST_ENTRY *link;
    if(!connection || !request || !target || !receipt)return error;
    target_pid=GetProcessId(target);
    if(!target_pid)return GetLastError();
    EnterCriticalSection(&connection->service->lock);
    if(OpenNtBaseServicePeer(connection,pid,generation) && connection->native_worker) {
        for(link=connection->win32records.Flink;link!=&connection->win32records;link=link->Flink) {
            OPENNT_BASE_WIN32RECORD *record=CONTAINING_RECORD(link,OPENNT_BASE_WIN32RECORD,link);
            if(record->request==request) {
                if(record->receipt ||
                    (record->process_id && record->process_id!=target_pid))error=ERROR_INVALID_STATE;
                else {
                    /* S24 owns only direct identity and receipt.  The target
                     * is still created suspended, so this registration occurs
                     * before it can execute; Job-based descendant observation
                     * is deliberately a later monitor-only S and must not
                     * decide whether this direct command may resume. */
                    HANDLE owned_receipt=NULL,owned_target=NULL,owned_wait=NULL;
                    if(!DuplicateHandle(GetCurrentProcess(),receipt,GetCurrentProcess(),&owned_receipt,
                        EVENT_MODIFY_STATE,FALSE,0))error=GetLastError();
                    else if(record->gui && !DuplicateHandle(GetCurrentProcess(),target,GetCurrentProcess(),&owned_target,
                        SYNCHRONIZE|PROCESS_QUERY_LIMITED_INFORMATION|PROCESS_TERMINATE,FALSE,0))error=GetLastError();
                    else if(record->gui && !RegisterWaitForSingleObject(&owned_wait,owned_target,
                        service_gui_exit,connection->service,INFINITE,WT_EXECUTEONLYONCE))error=GetLastError();
                    else {
                        record->receipt=owned_receipt;owned_receipt=NULL;
                        record->gui_process=owned_target;owned_target=NULL;
                        record->gui_wait=owned_wait;owned_wait=NULL;
                        record->process_id=target_pid;
                        record->worker_generation=generation;
                        service_query_native_image(target_pid,record->image);
                        service_signal_frontend_states(connection->service);
                        error=ERROR_SUCCESS;
                    }
                    if(owned_wait)UnregisterWaitEx(owned_wait,INVALID_HANDLE_VALUE);
                    if(owned_receipt)CloseHandle(owned_receipt);
                    if(owned_target)CloseHandle(owned_target);
                }
                break;
            }
        }
    }
    LeaveCriticalSection(&connection->service->lock);
    return error;
}


DWORD OpenNtBaseServiceNativeExitCode(OPENNT_BASE_CONNECTION *launcher,DWORD pid,
    DWORD generation,DWORD request,DWORD *exit_code)
{
    OPENNT_BASE_SERVICE *service;
    LIST_ENTRY *link;
    DWORD error=ERROR_NOT_FOUND;
    if(!launcher || !request || !exit_code)return ERROR_INVALID_PARAMETER;
    service=launcher->service;
    EnterCriticalSection(&service->lock);
    if(!OpenNtBaseServicePeer(launcher,pid,generation) || launcher->native_worker ||
        launcher->process.fVDM){error=ERROR_ACCESS_DENIED;goto done;}
    for(link=service->gui_records.Flink;link!=&service->gui_records;link=link->Flink) {
        OPENNT_BASE_WIN32RECORD *record=CONTAINING_RECORD(link,OPENNT_BASE_WIN32RECORD,link);
        if(record->request!=request || record->launcher_generation!=generation)continue;
        if(!record->completed){error=ERROR_NOT_READY;goto done;}
        *exit_code=record->exit_code;error=record->completion_error;
        service_delete_win32record(record);goto done;
    }
    for(link=service->connections.Flink;link!=&service->connections;link=link->Flink) {
        OPENNT_BASE_CONNECTION *worker=CONTAINING_RECORD(link,OPENNT_BASE_CONNECTION,service_link);
        LIST_ENTRY *entry;
        /* Rundown-retained results live on the existing launcher record list;
         * active requests still belong exclusively to their native worker. */
        if(!worker->native_worker && worker!=launcher)continue;
        for(entry=worker->win32records.Flink;entry!=&worker->win32records;entry=entry->Flink) {
            OPENNT_BASE_WIN32RECORD *record=CONTAINING_RECORD(entry,OPENNT_BASE_WIN32RECORD,link);
            if(record->request!=request || record->launcher_generation!=generation)continue;
            if(!record->completed){error=ERROR_NOT_READY;goto done;}
            *exit_code=record->exit_code;
            error=record->completion_error;
            service_delete_win32record(record);
            goto done;
        }
    }
done:
    LeaveCriticalSection(&service->lock);
    return error;
}


DWORD service_queue_native_command(OPENNT_BASE_CONNECTION *connection,DWORD pid,
    DWORD generation,HANDLE capability,const WCHAR image[OPENNT_BASE_WORKER_IMAGE_CHARS],
    DWORD bytes,const BYTE *payload)
{
    HANDLE root_process=NULL,execution=NULL,worker=NULL,frontend=NULL;
    DWORD root_generation=0,worker_generation=0,error;
    OPENNT_BASE_WIN32RECORD *record=NULL;
    BYTE *owned_payload=NULL;
    LIST_ENTRY *link;
    if (!connection || !image) return ERROR_ACCESS_DENIED;
    EnterCriticalSection(&connection->service->lock);
    error=ERROR_ACCESS_DENIED;
    if (!OpenNtBaseServicePeer(connection,pid,generation) || connection->process.fVDM || connection->native_worker) goto done;
    if (connection->native_command_pending) { error=ERROR_BUSY;goto done; }
    if(bytes>NATIVE_LAUNCH_MAX_BYTES || (bytes && !payload)) {error=ERROR_INVALID_PARAMETER;goto done;}
    if(bytes) {
        owned_payload=HeapAlloc(GetProcessHeap(),0,bytes);
        if(!owned_payload){error=ERROR_NOT_ENOUGH_MEMORY;goto done;}
        CopyMemory(owned_payload,payload,bytes);
    }
    if(image[0]) {
        SIZE_T characters=0;
        while(characters<OPENNT_BASE_WORKER_IMAGE_CHARS && image[characters])++characters;
        if(characters==OPENNT_BASE_WORKER_IMAGE_CHARS) { error=ERROR_INVALID_DATA;goto done; }
        record=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*record));
        if(!record) { error=ERROR_NOT_ENOUGH_MEMORY;goto done; }
        record->launcher_generation=generation;
        record->gui=capability==NULL;
        lstrcpynW(record->image,image,OPENNT_BASE_WORKER_IMAGE_CHARS);
    }
    if(capability) {
        error=OpenNtBaseServiceRetainFrontendRoot(connection,pid,generation,capability,
            &root_process,&root_generation);
        if (error) goto done;
        error=OpenNtBaseServiceAcquireConsoleContext(connection,pid,generation,capability,&execution);
        if (error) goto done;
    } else if(!record || !bytes) {error=ERROR_INVALID_PARAMETER;goto done;}
    {
        /* The existing admission selects the process. Neither an arbitrary
         * PID nor frontend membership can nominate an execution endpoint. */
        error=OpenNtBaseServiceRetainCommandWorker(connection,pid,generation,&worker);
        if (error) goto done;
        for (link=connection->service->connections.Flink;link!=&connection->service->connections;link=link->Flink) {
            OPENNT_BASE_CONNECTION *candidate=CONTAINING_RECORD(link,OPENNT_BASE_CONNECTION,service_link);
            if (candidate->native_worker &&
                GetProcessId(candidate->process.ProcessHandle)==GetProcessId(worker)) {
                worker_generation=candidate->process.SequenceNumber;break;
            }
        }
        if (!worker_generation) { error=ERROR_NOT_READY;goto done; }
        if (capability && !DuplicateHandle(GetCurrentProcess(),capability,GetCurrentProcess(),&frontend,
                SYNCHRONIZE,FALSE,0)) { error=GetLastError();goto done; }
    }
    error=ERROR_ACCESS_DENIED;
    for (link=connection->service->connections.Flink;link!=&connection->service->connections;link=link->Flink) {
        OPENNT_BASE_CONNECTION *root=CONTAINING_RECORD(link,OPENNT_BASE_CONNECTION,service_link);
        if (capability && root->process.SequenceNumber!=root_generation) continue;
        if(capability) {
            HANDLE origin=NULL;
            error=service_acquire_console_context(root,connection->console,worker_generation,&origin);
            if(error)goto done;
            CloseHandle(execution);execution=origin;
        }
        connection->native_command_payload=owned_payload;owned_payload=NULL;
        connection->native_command_bytes=bytes;connection->native_command_pending=TRUE;
        connection->frontend_execution=execution;execution=NULL;
        connection->frontend_channel_root=root_generation;
        connection->channel_worker_generation=worker_generation;
        connection->channel_frontend=frontend;frontend=NULL;
        connection->pending_win32record=record;record=NULL;
        WakeAllConditionVariable(&connection->service->frontend_changed);
        service_signal_frontend_states(connection->service);
        error=ERROR_SUCCESS;break;
    }
done:
    if (root_process) CloseHandle(root_process);
    if (worker) CloseHandle(worker);
    if (frontend) CloseHandle(frontend);
    if (execution) CloseHandle(execution);
    if (record) HeapFree(GetProcessHeap(),0,record);
    if (owned_payload) HeapFree(GetProcessHeap(),0,owned_payload);
    LeaveCriticalSection(&connection->service->lock);
    return error;
}


/* Locate only this authenticated launcher's direct record. Caller holds lock;
 * rundown may have moved a completed record onto the launcher's own list. */
static OPENNT_BASE_WIN32RECORD *service_native_result_record(OPENNT_BASE_CONNECTION *caller,
    DWORD request,OPENNT_BASE_CONNECTION **owner)
{
    LIST_ENTRY *link,*entry;
    if(owner)*owner=NULL;
    for(entry=caller->service->gui_records.Flink;entry!=&caller->service->gui_records;entry=entry->Flink) {
        OPENNT_BASE_WIN32RECORD *record=CONTAINING_RECORD(entry,OPENNT_BASE_WIN32RECORD,link);
        if(record->request==request && record->launcher_generation==caller->process.SequenceNumber)
            return record;
    }
    for(link=caller->service->connections.Flink;link!=&caller->service->connections;link=link->Flink) {
        OPENNT_BASE_CONNECTION *worker=CONTAINING_RECORD(link,OPENNT_BASE_CONNECTION,service_link);
        if(!worker->native_worker && worker!=caller)continue;
        for(entry=worker->win32records.Flink;entry!=&worker->win32records;entry=entry->Flink) {
            OPENNT_BASE_WIN32RECORD *record=CONTAINING_RECORD(entry,OPENNT_BASE_WIN32RECORD,link);
            if(record->request==request && record->launcher_generation==caller->process.SequenceNumber) {
                if(owner)*owner=worker;
                return record;
            }
        }
    }
    return NULL;
}


DWORD OpenNtBaseServiceNativeStartupResult(OPENNT_BASE_CONNECTION *worker,DWORD pid,DWORD generation,
    DWORD caller_generation,DWORD request,DWORD status,HANDLE target,HANDLE receipt)
{
    DWORD error=ERROR_ACCESS_DENIED;
    LIST_ENTRY *link;
    HANDLE owned_target=NULL,owned_receipt=NULL;
    if(!worker || !caller_generation)return error;
    EnterCriticalSection(&worker->service->lock);
    if(!OpenNtBaseServicePeer(worker,pid,generation) || !worker->native_worker)goto done;
    for(link=worker->service->connections.Flink;link!=&worker->service->connections;link=link->Flink) {
        OPENNT_BASE_CONNECTION *caller=CONTAINING_RECORD(link,OPENNT_BASE_CONNECTION,service_link);
        OPENNT_BASE_WIN32RECORD *record=NULL;
        SERVICE_COMPARE_HANDLES compare;
        if(caller->process.SequenceNumber!=caller_generation)continue;
        if(!caller->native_start_event || caller->native_start_worker!=generation ||
            caller->native_start_request!=request)goto done;
        if(caller->native_start_reported){error=ERROR_ALREADY_EXISTS;goto done;}
        if(status || !request) {
            if(target || receipt){error=ERROR_INVALID_DATA;goto done;}
        } else {
            record=service_native_result_record(caller,request,NULL);
            compare=(SERVICE_COMPARE_HANDLES)GetProcAddress(GetModuleHandleW(L"kernelbase.dll"),"CompareObjectHandles");
            if(!record || record->startup_delivered || !compare || !target || !receipt ||
                record->process_id!=GetProcessId(target) || !compare(record->receipt,receipt)) {
                error=ERROR_INVALID_DATA;goto done;
            }
            if(!DuplicateHandle(GetCurrentProcess(),target,GetCurrentProcess(),&owned_target,
                    SYNCHRONIZE|PROCESS_QUERY_LIMITED_INFORMATION,FALSE,0) ||
                !DuplicateHandle(GetCurrentProcess(),receipt,GetCurrentProcess(),&owned_receipt,
                    SYNCHRONIZE,FALSE,0)){error=GetLastError();goto done;}
            if(record->gui) {
                /* The actual target was bound while suspended. Its exit wait
                 * must already exist before the worker resumes it. */
                if(!record->gui_process || !record->gui_wait){error=ERROR_INVALID_STATE;goto done;}
            }
        }
        caller->native_start_status=status;
        caller->native_start_reported=TRUE;
        if(!SetEvent(caller->native_start_event)) {
            error=GetLastError();caller->native_start_reported=FALSE;
            if(record && record->gui_wait) {
                UnregisterWaitEx(record->gui_wait,INVALID_HANDLE_VALUE);
                record->gui_wait=NULL;
            }
            goto done;
        }
        if(record && record->gui) {
            /* Commit request release only after startup notification succeeds.
             * The caller re-enters under this same lock before consuming it.
             * Transfer the existing record, not a second task/receipt. */
            RemoveEntryList(&record->link);
            InsertTailList(&worker->service->gui_records,&record->link);
            --worker->native_inflight;
            service_signal_frontend_states(worker->service);
        }
        caller->native_start_target=owned_target;owned_target=NULL;
        caller->native_start_receipt=owned_receipt;owned_receipt=NULL;
        error=ERROR_SUCCESS;break;
    }
done:
    LeaveCriticalSection(&worker->service->lock);
    if(owned_target)CloseHandle(owned_target);if(owned_receipt)CloseHandle(owned_receipt);
    return error;
}


/* Caller holds the service lock. Completion is original DOS receipt state;
 * origin is the existing inherited context, never physical PID membership. */
DWORD service_prepare_parent_resume(OPENNT_BASE_CONNECTION *caller,
    DWORD root_generation,BOOL *native_parent)
{
    LIST_ENTRY *link;
    *native_parent=FALSE;
    if(!caller->dos_completion_read || !caller->execution_worker_generation)
        return ERROR_INVALID_STATE;
    for(link=caller->service->connections.Flink;link!=&caller->service->connections;link=link->Flink) {
        OPENNT_BASE_CONNECTION *parent=CONTAINING_RECORD(link,OPENNT_BASE_CONNECTION,service_link);
        if(parent->process.SequenceNumber!=caller->execution_worker_generation)continue;
        if(parent->worker_failed || WaitForSingleObject(parent->process.ProcessHandle,0)!=WAIT_TIMEOUT)
            return ERROR_PROCESS_ABORTED;
        if(parent->native_worker) {
            if(parent->native_root!=root_generation || !parent->native_inflight)
                return ERROR_INVALID_STATE;
            caller->selected_native_generation=parent->process.SequenceNumber;
            *native_parent=TRUE;return ERROR_SUCCESS;
        }
        return parent->process.fVDM && !parent->wow ? ERROR_SUCCESS : ERROR_ACCESS_DENIED;
    }
    return ERROR_PROCESS_ABORTED;
}

DWORD OpenNtBaseServiceSubmitNativeRequest(OPENNT_BASE_CONNECTION *caller,DWORD pid,
    DWORD generation,HANDLE frontend,DWORD bytes,BYTE *payload,
    HANDLE *target,HANDLE *receipt,DWORD *request)
{
    WCHAR image[OPENNT_BASE_WORKER_IMAGE_CHARS]={0};
    HANDLE startup=NULL,worker=NULL;
    HANDLE root=NULL;
    DWORD root_generation=0;
    DWORD error;
    if(!target || !receipt || !request)return ERROR_INVALID_PARAMETER;
    *target=*receipt=NULL;*request=0;
    if(!caller || !OpenNtBaseServicePeer(caller,pid,generation))return ERROR_ACCESS_DENIED;
    /* Authenticate the root before interpreting launch data or selecting a
     * worker. The copied request never supplies a worker pipe or its owner. */
    if(frontend) {
        error=OpenNtBaseServiceRetainFrontendRoot(caller,pid,generation,frontend,&root,&root_generation);
        if(error)return error;
        CloseHandle(root);
    } else if(!bytes)return ERROR_INVALID_PARAMETER;
    if(!bytes) {
        BOOL native_parent=FALSE;
        if(payload)return ERROR_INVALID_PARAMETER;
        EnterCriticalSection(&caller->service->lock);
        error=service_prepare_parent_resume(caller,root_generation,&native_parent);
        if(!error && !native_parent)caller->dos_completion_read=FALSE;
        LeaveCriticalSection(&caller->service->lock);
        if(error || !native_parent)return error;
    }
    if(bytes) {
        run16_native_launch_packet packet;WCHAR *strings[4];
        error=run16_native_launch_unpack(payload,bytes,&packet,strings);
        if(error)return error;
        if(packet.capabilities[0] || packet.capabilities[1] ||
            wcslen(strings[0])>=ARRAYSIZE(image))return ERROR_INVALID_DATA;
        lstrcpynW(image,*strings[0] ? strings[0] : strings[1],ARRAYSIZE(image));
    } else if(payload)return ERROR_INVALID_PARAMETER;
    error=OpenNtBaseServiceRetainCommandWorker(caller,pid,generation,&worker);
    if(error)return error;
    startup=CreateEventW(NULL,TRUE,FALSE,NULL);
    if(!startup){error=GetLastError();goto done;}
    EnterCriticalSection(&caller->service->lock);
    if(caller->native_start_event)error=ERROR_BUSY;
    else {
        error=service_queue_native_command(caller,pid,generation,frontend,image,bytes,payload);
        if(!error) {
            caller->native_start_event=startup;
            caller->native_start_worker=caller->channel_worker_generation;
            caller->native_start_request=MAXDWORD;caller->native_start_reported=FALSE;
        }
    }
    LeaveCriticalSection(&caller->service->lock);
    if(error)goto done;
    {
        HANDLE waits[3]={startup,worker,caller->process.ProcessHandle};
        DWORD wait=WaitForMultipleObjects(3,waits,FALSE,10000);
        if(wait!=WAIT_OBJECT_0) {
            error=wait==WAIT_TIMEOUT ? ERROR_TIMEOUT : wait==WAIT_FAILED ? GetLastError() : ERROR_PROCESS_ABORTED;
            goto done;
        }
    }
    EnterCriticalSection(&caller->service->lock);
    {
        OPENNT_BASE_WIN32RECORD *record;
        error=caller->native_start_reported ? caller->native_start_status : ERROR_INVALID_STATE;
        if(!error && bytes) {
            record=service_native_result_record(caller,caller->native_start_request,NULL);
            if(!record || record->startup_delivered || !caller->native_start_target || !caller->native_start_receipt)
                error=ERROR_INVALID_DATA;
            else {
            record->startup_delivered=TRUE;
            *target=caller->native_start_target;caller->native_start_target=NULL;
            *receipt=caller->native_start_receipt;caller->native_start_receipt=NULL;
            *request=caller->native_start_request;
            }
        }
    }
    LeaveCriticalSection(&caller->service->lock);
done:
    EnterCriticalSection(&caller->service->lock);
    if(!error && !bytes)caller->dos_completion_read=FALSE;
    if(startup && caller->native_start_event==startup) {
        caller->native_start_event=NULL;caller->native_start_worker=0;
        caller->native_start_request=0;caller->native_start_reported=FALSE;
        if(caller->native_start_target)CloseHandle(caller->native_start_target);
        if(caller->native_start_receipt)CloseHandle(caller->native_start_receipt);
        caller->native_start_target=caller->native_start_receipt=NULL;
        /* Only an undelivered command is rolled back here; running targets
         * and their existing direct completion records are never killed. */
        if(error)service_clear_frontend_channel(caller);
    }
    LeaveCriticalSection(&caller->service->lock);
    if(startup)CloseHandle(startup);if(worker)CloseHandle(worker);
    return error;
}


DWORD OpenNtBaseServiceFinishNativeRequest(OPENNT_BASE_CONNECTION *caller,DWORD pid,
    DWORD generation,DWORD request,DWORD *exit_code,DWORD *target_completed)
{
    DWORD error,io_error=0,io_flags=0,idle_worker=0;
    if(!exit_code || !target_completed)return ERROR_INVALID_PARAMETER;
    *exit_code=*target_completed=0;
    if(!caller || !request)return ERROR_INVALID_PARAMETER;
    if(!OpenNtBaseServicePeer(caller,pid,generation))return ERROR_ACCESS_DENIED;
    EnterCriticalSection(&caller->service->lock);
    {
        OPENNT_BASE_CONNECTION *owner=NULL;
        OPENNT_BASE_WIN32RECORD *record=service_native_result_record(caller,request,&owner);
        if(!record)error=ERROR_NOT_FOUND;
        else if(!record->completed)error=ERROR_NOT_READY;
        else if(record->completion_error)error=ERROR_SUCCESS;
        else {
            error=ERROR_SUCCESS;io_error=record->io_error;io_flags=record->io_flags;
            idle_worker=owner && owner->native_worker ? owner->process.SequenceNumber : 0;
        }
    }
    LeaveCriticalSection(&caller->service->lock);
    if(error)goto done;
    /* Consuming the broker result is the one-time gate, including worker death.
     * Never demand a reply from a failed worker to report its broker failure. */
    error=OpenNtBaseServiceNativeExitCode(caller,pid,generation,request,exit_code);
    if(error)goto done;
    /* Only successfully consuming this caller's real completion grants the
     * Console-return decision. A final-I/O error does not undo target exit;
     * worker failure, forged/stale receipts and races never set this flag. */
    *target_completed=TRUE;
    if(io_error){error=io_error;goto done;}
    if(idle_worker && (io_flags & NATIVE_COMPLETION_CONSOLE_EMPTY)) {
        EnterCriticalSection(&caller->service->lock);
        service_retire_completed_root(caller,idle_worker);
        LeaveCriticalSection(&caller->service->lock);
    }
done:
    return error;
}


DWORD service_take_native_command(OPENNT_BASE_CONNECTION *root,DWORD pid,
    DWORD generation,DWORD capacity,BYTE *payload,DWORD *bytes,
    HANDLE *caller_process,HANDLE *execution,HANDLE *frontend,DWORD *request,DWORD *caller_generation)
{
    LIST_ENTRY *link;
    DWORD error=ERROR_ACCESS_DENIED;
    if (!payload || !bytes || !caller_generation || !caller_process || !execution || !frontend || !request) return ERROR_INVALID_PARAMETER;
    *caller_process=NULL;*execution=NULL;*frontend=NULL;*request=0;
    *bytes=0;*caller_generation=0;
    if (!root) return error;
    EnterCriticalSection(&root->service->lock);
    if (!OpenNtBaseServicePeer(root,pid,generation) ||
        !root->native_worker) goto done;
    for (link=root->service->connections.Flink;link!=&root->service->connections;link=link->Flink) {
        OPENNT_BASE_CONNECTION *caller=CONTAINING_RECORD(link,OPENNT_BASE_CONNECTION,service_link);
        if (caller->channel_worker_generation!=generation) continue;
        if (WaitForSingleObject(caller->process.ProcessHandle,0)!=WAIT_TIMEOUT) {
            service_clear_frontend_channel(caller);continue;
        }
        /* A short output buffer leaves command and completion ownership intact. */
        if(caller->native_command_bytes>capacity) {
            error=ERROR_INSUFFICIENT_BUFFER;goto done;
        }
        if(caller->pending_win32record && (root->native_inflight==MAXDWORD ||
            (caller->channel_frontend && root->native_inflight && root->native_activity_root!=caller->frontend_channel_root))) {
            error=ERROR_BUSY;goto done;
        }
        /* The visible Console identity belongs to the authenticated
         * frontend root, not to this short-lived direct launcher. */
        if(caller->channel_frontend) {
            OPENNT_BASE_CONNECTION *frontend=NULL;
            LIST_ENTRY *root_link;
            for(root_link=root->service->connections.Flink;
                root_link!=&root->service->connections;root_link=root_link->Flink) {
                OPENNT_BASE_CONNECTION *candidate=CONTAINING_RECORD(root_link,
                    OPENNT_BASE_CONNECTION,service_link);
                if(candidate->process.SequenceNumber==caller->frontend_channel_root &&
                    candidate->frontend_capability &&
                    WaitForSingleObject(candidate->process.ProcessHandle,0)==WAIT_TIMEOUT) {
                    frontend=candidate;break;
                }
            }
            if(!frontend || frontend->console!=caller->console ||
                !frontend->console_member_count) {error=ERROR_PIPE_NOT_CONNECTED;goto done;}
            error=service_copy_execution_console_members(root,frontend);
        } else error=ERROR_SUCCESS;
        if(error)goto done;
        /* The pinned sender process permits only this direct channel's finite
         * stream/capability exchange, never an arbitrary broker duplication API. */
        if (!DuplicateHandle(GetCurrentProcess(),caller->process.ProcessHandle,GetCurrentProcess(),
                caller_process,SYNCHRONIZE|PROCESS_QUERY_LIMITED_INFORMATION|PROCESS_DUP_HANDLE,FALSE,0) ||
            (caller->channel_frontend && (!DuplicateHandle(GetCurrentProcess(),caller->frontend_execution,GetCurrentProcess(),
                execution,SYNCHRONIZE,FALSE,0) ||
            !DuplicateHandle(GetCurrentProcess(),caller->channel_frontend,GetCurrentProcess(),
                frontend,SYNCHRONIZE,FALSE,0)))) { error=GetLastError();goto done; }
        if(caller->pending_win32record) {
            error=service_next_win32record(root,&caller->pending_win32record->request);
            if(error)goto done;
        }
        if(caller->channel_frontend) {
            error=service_authorize_worker_io(root,pid);
            if(error)goto done;
        }
        if(caller->pending_win32record) {
            InsertTailList(&root->win32records,&caller->pending_win32record->link);
            *request=caller->pending_win32record->request;
            caller->pending_win32record=NULL;
            ++root->native_inflight;
            if(caller->channel_frontend)root->native_activity_root=caller->frontend_channel_root;
        }
        if(caller->native_start_event)caller->native_start_request=*request;
        if(caller->native_command_bytes)CopyMemory(payload,caller->native_command_payload,caller->native_command_bytes);
        *bytes=caller->native_command_bytes;
        *caller_generation=caller->process.SequenceNumber;
        service_clear_frontend_channel(caller);
        service_signal_frontend_states(root->service);
        error=ERROR_SUCCESS;goto done;
    }
    error=ERROR_NOT_FOUND;
done:
    if (error) {
        if (*caller_process) CloseHandle(*caller_process);
        if (*execution) CloseHandle(*execution);
        if (*frontend) { CloseHandle(*frontend);*frontend=NULL; }
        *caller_process=NULL;*execution=NULL;
        *request=0;*bytes=0;*caller_generation=0;
    }
    LeaveCriticalSection(&root->service->lock);
    return error;
}


DWORD OpenNtBaseServiceGetNextNativeCommand(OPENNT_BASE_CONNECTION *connection,DWORD pid,
    DWORD generation,DWORD capacity,BYTE *payload,DWORD *bytes,HANDLE *caller_process,
    HANDLE *execution,HANDLE *frontend,DWORD *request,DWORD *caller_generation)
{
    DWORD error;
    if (!connection) return ERROR_ACCESS_DENIED;
    if(!payload || !capacity || capacity>NATIVE_LAUNCH_MAX_BYTES || !bytes ||
        !caller_process || !execution || !frontend || !request || !caller_generation)return ERROR_INVALID_PARAMETER;
    *bytes=*caller_generation=0;
    *caller_process=*execution=*frontend=NULL;*request=0;
    EnterCriticalSection(&connection->service->lock);
    for (;;) {
        LIST_ENTRY *link;
        if(!OpenNtBaseServicePeer(connection,pid,generation)){error=ERROR_ACCESS_DENIED;break;}
        /* Close is authoritative even before the first presentation binding;
         * never deliver another queued command after this broker instruction. */
        for(link=connection->service->worker_watches.Flink;
            link!=&connection->service->worker_watches;link=link->Flink) {
            OPENNT_BASE_WORKER_WATCH *watch=CONTAINING_RECORD(link,OPENNT_BASE_WORKER_WATCH,link);
            if(watch->process.SequenceNumber==generation &&
                WaitForSingleObject(watch->shutdown,0)==WAIT_OBJECT_0) {
                error=ERROR_CANCELLED;goto done;
            }
        }
        error=service_take_native_command(connection,pid,generation,capacity,payload,bytes,
            caller_process,execution,frontend,request,caller_generation);
        if (error!=ERROR_NOT_FOUND) break;
        /* Same arrival/rundown condition as worker frontend attachment. This
         * waits for a capability, not for task completion or a scheduler. */
        if (!SleepConditionVariableCS(&connection->service->frontend_changed,
                &connection->service->lock,INFINITE)) { error=GetLastError();break; }
    }
done:
    LeaveCriticalSection(&connection->service->lock);
    return error;
}
