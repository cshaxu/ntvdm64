/* NTSRV-private lifecycle; project adaptation,
 * not an original OpenNT mirror. Physical separation only: existing function
 * bodies, state authority and lock/resource contracts are preserved. */
#include <service_internal.h>

static ULONGLONG service_root_retirement_deadline(OPENNT_BASE_CONNECTION *root,ULONGLONG now);
static BOOL service_root_workerless(OPENNT_BASE_CONNECTION *root);

static ULONGLONG service_unbound_native_deadline(OPENNT_BASE_WORKER_WATCH *watch,ULONGLONG now)
{
    LIST_ENTRY *link;
    if(watch->kind!=OPENNT_BASE_WORKER_NATIVE || watch->frontend_associated ||
        WaitForSingleObject(watch->shutdown,0)!=WAIT_TIMEOUT)return 0;
    for(link=watch->service->connections.Flink;link!=&watch->service->connections;link=link->Flink) {
        OPENNT_BASE_CONNECTION *worker=CONTAINING_RECORD(link,OPENNT_BASE_CONNECTION,service_link);
        if(worker->process.SequenceNumber!=watch->process.SequenceNumber)continue;
        if(worker->native_root || worker->native_inflight) {
            watch->unbound_native_deadline=0;return 0;
        }
        break;
    }
    if(!watch->unbound_native_deadline)
        watch->unbound_native_deadline=now+FRONTEND_STARTUP_DEADLINE_MS;
    return watch->unbound_native_deadline;
}

void service_signal_frontend_states(OPENNT_BASE_SERVICE *service)
{
    LIST_ENTRY *link;
    if(service->frontend_lifetime_changed)
        (void)SetEvent(service->frontend_lifetime_changed);
    for(link=service->connections.Flink;link!=&service->connections;link=link->Flink) {
        OPENNT_BASE_CONNECTION *root=CONTAINING_RECORD(link,OPENNT_BASE_CONNECTION,service_link);
        if(root->frontend_state_changed) (void)SetEvent(root->frontend_state_changed);
    }
}

BOOL OpenNtBaseServiceIsEmpty(OPENNT_BASE_SERVICE *service)
{
    BOOL empty;
    if (!service) return FALSE;
    EnterCriticalSection(&service->lock);
    empty=IsListEmpty(&service->worker_watches) && IsListEmpty(&service->gui_records) && OpenNtBaseProcessRegistryIsEmpty(&service->registry) &&
        OpenNtBaseReservationsIsEmpty(service->reservations);
    LeaveCriticalSection(&service->lock);
    return empty;
}

DWORD OpenNtBaseServiceTerminateWorker(OPENNT_BASE_SERVICE *service,uint32_t process_id)
{
    LIST_ENTRY *link;
    HANDLE process=NULL,native_stop=NULL,native_closed=NULL;
    DWORD error=ERROR_NOT_FOUND;
    if (!service || !process_id) return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&service->lock);
    for (link=service->worker_watches.Flink;link!=&service->worker_watches;link=link->Flink) {
        OPENNT_BASE_WORKER_WATCH *watch=CONTAINING_RECORD(link,OPENNT_BASE_WORKER_WATCH,link);
        if ((DWORD)(ULONG_PTR)watch->process.ClientId.UniqueProcess!=process_id) continue;
        /* Native close must close its Console session, not only the carrier.
         * Until its control binding is registered, fail explicitly. */
        if(watch->kind==OPENNT_BASE_WORKER_NATIVE) {
            LIST_ENTRY *entry;error=ERROR_NOT_READY;
            for(entry=service->connections.Flink;entry!=&service->connections;entry=entry->Flink) {
                OPENNT_BASE_CONNECTION *native=CONTAINING_RECORD(entry,OPENNT_BASE_CONNECTION,service_link);
                if((DWORD)(ULONG_PTR)native->process.ClientId.UniqueProcess!=process_id ||
                    !native->native_worker)continue;
                if(!native->native_stop || !native->native_closed)break;
                if(!DuplicateHandle(GetCurrentProcess(),watch->process.ProcessHandle,GetCurrentProcess(),&process,
                    SYNCHRONIZE,FALSE,0) ||
                    !DuplicateHandle(GetCurrentProcess(),native->native_stop,GetCurrentProcess(),&native_stop,
                    EVENT_MODIFY_STATE,FALSE,0) ||
                    !DuplicateHandle(GetCurrentProcess(),native->native_closed,GetCurrentProcess(),&native_closed,
                    SYNCHRONIZE,FALSE,0))error=GetLastError();
                else {watch->termination_requested=TRUE;error=ERROR_SUCCESS;}
                break;
            }
            break;
        }
        if (watch->termination_requested) { error=ERROR_BUSY; break; }
        if (!DuplicateHandle(GetCurrentProcess(),watch->process.ProcessHandle,
                GetCurrentProcess(),&process,PROCESS_TERMINATE|SYNCHRONIZE,FALSE,0)) {
            error=GetLastError(); break;
        }
        watch->termination_requested=TRUE;
        error=ERROR_SUCCESS;
        break;
    }
    LeaveCriticalSection(&service->lock);
    if(native_stop && native_closed && !error) {
        /* Session owner must close its Console and acknowledge it. Carrier death
         * alone cannot prove that attached clients were actually closed. */
        if(!SetEvent(native_stop))error=GetLastError();
        else if(WaitForSingleObject(native_closed,10000)!=WAIT_OBJECT_0)error=ERROR_TIMEOUT;
        else if(WaitForSingleObject(process,5000)!=WAIT_OBJECT_0)error=ERROR_TIMEOUT;
    } else if(!error && !TerminateProcess(process,ERROR_CANCELLED))error=GetLastError();
    if(native_stop)CloseHandle(native_stop);if(native_closed)CloseHandle(native_closed);
    if(error) {if(process)CloseHandle(process);return error;}
    CloseHandle(process);
    return error;
}


DWORD OpenNtBaseServiceFrontendUsage(OPENNT_BASE_CONNECTION *root,DWORD pid,
    DWORD generation,DWORD *pending,DWORD *tasks)
{
    LIST_ENTRY *link,*watch_link;
    DWORD error=ERROR_ACCESS_DENIED,pending_count=0,task_count=0;
    if(!root || !pending || !tasks)return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&root->service->lock);
    if(!OpenNtBaseServicePeer(root,pid,generation) || !root->frontend_capability)goto done;
    for(link=root->service->connections.Flink;link!=&root->service->connections;link=link->Flink){
        OPENNT_BASE_CONNECTION *caller=CONTAINING_RECORD(link,OPENNT_BASE_CONNECTION,service_link);
        if(caller->frontend_request_root==generation || caller->frontend_channel_root==generation)
            pending_count=1;
        if(caller->native_activity_root==generation && caller->native_inflight &&
            WaitForSingleObject(caller->process.ProcessHandle,0)==WAIT_TIMEOUT)
            pending_count=1;
        /* Management Win32Records do not add a separate frontend pin. */
    }
    /* A route pins the actual selected worker, not the submitting launcher.
     * The original DOSRecord chain survives launcher disconnection. Read it
     * under its original lock; do not synthesize an execution registry. */
    (void)RtlEnterCriticalSection(&BaseSrvDOSCriticalSection);
    for(link=root->service->frontend_routes.Flink;link!=&root->service->frontend_routes;link=link->Flink){
        OPENNT_FRONTEND_ROUTE *route=CONTAINING_RECORD(link,OPENNT_FRONTEND_ROUTE,link);
        if(route->root!=root || WaitForSingleObject(route->worker,0)!=WAIT_TIMEOUT)continue;
        if(!route->delivered)pending_count=1;
        for(watch_link=root->service->worker_watches.Flink;watch_link!=&root->service->worker_watches;watch_link=watch_link->Flink){
            OPENNT_BASE_WORKER_WATCH *watch=CONTAINING_RECORD(watch_link,OPENNT_BASE_WORKER_WATCH,link);
            PCONSOLERECORD console;
            if(watch->kind!=OPENNT_BASE_WORKER_DOS || GetProcessId(watch->process.ProcessHandle)!=GetProcessId(route->worker))continue;
            for(console=DOSHead;console;console=console->Next){
                PDOSRECORD record;
                if(console->SequenceNumber!=watch->process.SequenceNumber)continue;
                for(record=console->DOSRecord;record;record=record->DOSRecordNext)
                    if(record->VDMState==VDM_BUSY || record->VDMState==VDM_TO_TAKE_A_COMMAND)++task_count;
                break;
            }
            break;
        }
    }
    RtlLeaveCriticalSection(&BaseSrvDOSCriticalSection);
    *pending=pending_count;*tasks=task_count;error=ERROR_SUCCESS;
done:
    LeaveCriticalSection(&root->service->lock);
    return error;
}


DWORD OpenNtBaseServiceRetireWorkerlessFrontend(OPENNT_BASE_CONNECTION *root,DWORD pid,
    DWORD generation,DWORD *retired)
{
    DWORD error=ERROR_ACCESS_DENIED;
    if(!root || !retired)return ERROR_INVALID_PARAMETER;
    *retired=0;
    EnterCriticalSection(&root->service->lock);
    if(OpenNtBaseServicePeer(root,pid,generation) && root->frontend_capability) {
        *retired=root->frontend_closing ? 1u : 0u;
        error=ERROR_SUCCESS;
    }
    LeaveCriticalSection(&root->service->lock);
    return error;
}


/* The broker alone owns the worker census and bounded startup admission.
 * This is a Console-root lifetime check, never a DOS/native task scheduler. */
BOOL service_root_has_worker(OPENNT_BASE_CONNECTION *root)
{
    LIST_ENTRY *link;
    OPENNT_BASE_SERVICE *service=root->service;
    DWORD generation=root->process.SequenceNumber;
    for(link=service->worker_watches.Flink;link!=&service->worker_watches;link=link->Flink) {
        OPENNT_BASE_WORKER_WATCH *watch=CONTAINING_RECORD(link,OPENNT_BASE_WORKER_WATCH,link);
        if(watch->wow || WaitForSingleObject(watch->process.ProcessHandle,0)!=WAIT_TIMEOUT)
            continue;
        if(watch->kind==OPENNT_BASE_WORKER_DOS && service_root_console_matches(root,watch->console))
            return TRUE;
        if(watch->kind==OPENNT_BASE_WORKER_NATIVE) {
            LIST_ENTRY *connection_link;
            for(connection_link=service->connections.Flink;
                connection_link!=&service->connections;connection_link=connection_link->Flink) {
                OPENNT_BASE_CONNECTION *worker=CONTAINING_RECORD(connection_link,
                    OPENNT_BASE_CONNECTION,service_link);
                if(worker->process.SequenceNumber==watch->process.SequenceNumber &&
                    (worker->native_root==generation ||
                    (!worker->native_root && watch->console==root->console)))return TRUE;
            }
        }
    }
    return FALSE;
}

static ULONGLONG service_root_retirement_deadline(OPENNT_BASE_CONNECTION *root,ULONGLONG now)
{
    LIST_ENTRY *link;
    ULONGLONG deadline;
    if(!root->frontend_capability || !root->frontend_console_window || root->frontend_closing ||
        WaitForSingleObject(root->process.ProcessHandle,0)!=WAIT_TIMEOUT)return 0;
    if(service_root_has_worker(root)) {
        root->frontend_workerless_deadline=0;
        return 0;
    }
    /* Start once on loss of the last worker; rechecks do not renew the grace.
     * A newly associated worker cancels it. NTCON owns no local deadline. */
    if(!root->frontend_workerless_deadline)
        root->frontend_workerless_deadline=now+FRONTEND_STARTUP_DEADLINE_MS;
    deadline=root->frontend_workerless_deadline;
    /* An already admitted startup remains bounded and authenticated. */
    if(root->frontend_admission_deadline>deadline)
        for(link=root->service->connections.Flink;link!=&root->service->connections;link=link->Flink) {
            OPENNT_BASE_CONNECTION *caller=CONTAINING_RECORD(link,OPENNT_BASE_CONNECTION,service_link);
            if(caller->process.SequenceNumber==root->frontend_creator_generation &&
                WaitForSingleObject(caller->process.ProcessHandle,0)==WAIT_TIMEOUT)
                return root->frontend_admission_deadline;
        }
    return deadline;
}

static BOOL service_root_workerless(OPENNT_BASE_CONNECTION *root)
{
    ULONGLONG now=GetTickCount64(),deadline=service_root_retirement_deadline(root,now);
    return deadline && deadline<=now;
}

HANDLE OpenNtBaseServiceFrontendLifetimeChanged(OPENNT_BASE_SERVICE *service)
{
    return service ? service->frontend_lifetime_changed : NULL;
}

DWORD OpenNtBaseServiceNextFrontendDeadline(OPENNT_BASE_SERVICE *service,ULONGLONG *deadline)
{
    LIST_ENTRY *link;
    ULONGLONG now;
    if(!service || !deadline)return ERROR_INVALID_PARAMETER;
    *deadline=0;now=GetTickCount64();
    EnterCriticalSection(&service->lock);
    for(link=service->connections.Flink;link!=&service->connections;link=link->Flink) {
        OPENNT_BASE_CONNECTION *root=CONTAINING_RECORD(link,OPENNT_BASE_CONNECTION,service_link);
        ULONGLONG due=service_root_retirement_deadline(root,now);
        if(due && (!*deadline || due<*deadline))*deadline=due;
    }
    for(link=service->worker_watches.Flink;link!=&service->worker_watches;link=link->Flink) {
        OPENNT_BASE_WORKER_WATCH *watch=CONTAINING_RECORD(link,OPENNT_BASE_WORKER_WATCH,link);
        ULONGLONG due=service_unbound_native_deadline(watch,now);
        if(due && (!*deadline || due<*deadline))*deadline=due;
    }
    LeaveCriticalSection(&service->lock);
    return ERROR_SUCCESS;
}

DWORD OpenNtBaseServiceRetireExpiredFrontends(OPENNT_BASE_SERVICE *service)
{
    LIST_ENTRY *link;
    BOOL changed=FALSE,gui_changed;
    if(!service)return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&service->lock);
    gui_changed=service_prune_gui_records(service);
    for(link=service->connections.Flink;link!=&service->connections;link=link->Flink) {
        OPENNT_BASE_CONNECTION *root=CONTAINING_RECORD(link,OPENNT_BASE_CONNECTION,service_link);
        if(service_root_workerless(root)) {
            root->frontend_closing=TRUE;
            changed=TRUE;
        }
    }
    /* Only NTSRV translates lost frontend ownership into worker shutdown.
     * A live worker which has never been associated is still in startup. */
    for(link=service->worker_watches.Flink;link!=&service->worker_watches;link=link->Flink) {
        OPENNT_BASE_WORKER_WATCH *watch=CONTAINING_RECORD(link,OPENNT_BASE_WORKER_WATCH,link);
        LIST_ENTRY *root_link;
        BOOL attached=FALSE;
        DWORD native_root=0;
        if(watch->wow)continue;
        {
            ULONGLONG now=GetTickCount64(),due=service_unbound_native_deadline(watch,now);
            if(due && due<=now) {(void)SetEvent(watch->shutdown);changed=TRUE;}
        }
        if(watch->kind==OPENNT_BASE_WORKER_NATIVE)
            for(root_link=service->connections.Flink;root_link!=&service->connections;root_link=root_link->Flink) {
                OPENNT_BASE_CONNECTION *worker=CONTAINING_RECORD(root_link,OPENNT_BASE_CONNECTION,service_link);
                if(worker->process.SequenceNumber==watch->process.SequenceNumber){native_root=worker->native_root;break;}
            }
        for(root_link=service->connections.Flink;root_link!=&service->connections;root_link=root_link->Flink) {
            OPENNT_BASE_CONNECTION *root=CONTAINING_RECORD(root_link,OPENNT_BASE_CONNECTION,service_link);
            if(!root->frontend_capability)continue;
            if((native_root && native_root==root->process.SequenceNumber) ||
                (!native_root && service_root_console_matches(root,watch->console))) {
                watch->frontend_associated=TRUE;
                if(!root->frontend_closing && WaitForSingleObject(root->process.ProcessHandle,0)==WAIT_TIMEOUT)
                    attached=TRUE;
            }
        }
        if(watch->frontend_associated && !attached && watch->shutdown &&
            WaitForSingleObject(watch->shutdown,0)==WAIT_TIMEOUT) {
            (void)SetEvent(watch->shutdown);
            changed=TRUE; /* Also wake pre-binding GetNextCommand. */
        }
    }
    if(changed) {
        service_signal_frontend_states(service);
        WakeAllConditionVariable(&service->frontend_changed);
    }
    LeaveCriticalSection(&service->lock);
    if(gui_changed && service->empty_notify && OpenNtBaseServiceIsEmpty(service))
        service->empty_notify(service->empty_notify_context);
    return ERROR_SUCCESS;
}


DWORD OpenNtBaseServiceRetireFrontend(OPENNT_BASE_CONNECTION *root,DWORD pid,DWORD generation)
{
    DWORD pending=0,tasks=0,error;
    if(!root)return ERROR_ACCESS_DENIED;
    EnterCriticalSection(&root->service->lock);
    error=OpenNtBaseServiceFrontendUsage(root,pid,generation,&pending,&tasks);
    if(!error){
        if(pending || tasks)error=ERROR_BUSY;
        else { root->frontend_closing=TRUE;service_signal_frontend_states(root->service); }
    }
    LeaveCriticalSection(&root->service->lock);
    return error;
}



/* Service lock held. The owner-specific boundary proves completion before
 * entering this shared Console-return/retirement path: original nonzero
 * DosSesId worker exit, or native final I/O acknowledgement with an empty
 * backend Console. Native passes its idle worker generation; DOS passes zero.
 * PIF CloseOnExit is never reimplemented here. */
void service_retire_completed_root(OPENNT_BASE_CONNECTION *parent,DWORD idle_worker)
{
    LIST_ENTRY *link,*other_link;
    for(link=parent->service->connections.Flink;
        link!=&parent->service->connections;link=link->Flink) {
        OPENNT_BASE_CONNECTION *root=CONTAINING_RECORD(link,OPENNT_BASE_CONNECTION,service_link);
        BOOL pending=FALSE;
        if(root->process.SequenceNumber!=parent->retained_frontend_root ||
            !root->frontend_capability || root->frontend_closing ||
            !service_root_console_matches(root,parent->console))continue;
        if(idle_worker) {
            LIST_ENTRY *watch_link;
            /* Native CloseOnExit applies only to this launcher's self-created
             * Console. A nested request never retires its parent's frontend. */
            if(root->frontend_borrowed || root->frontend_creator_generation!=parent->process.SequenceNumber)continue;
            for(watch_link=parent->service->worker_watches.Flink;
                watch_link!=&parent->service->worker_watches;watch_link=watch_link->Flink) {
                OPENNT_BASE_WORKER_WATCH *watch=CONTAINING_RECORD(watch_link,OPENNT_BASE_WORKER_WATCH,link);
                if(watch->wow || WaitForSingleObject(watch->process.ProcessHandle,0)!=WAIT_TIMEOUT)continue;
                if(watch->process.SequenceNumber==idle_worker)continue;
                if(service_root_console_matches(root,watch->console)){pending=TRUE;break;}
            }
            if(pending)continue;
        } else if(service_root_has_worker(root))continue;
        for(other_link=parent->service->connections.Flink;
            other_link!=&parent->service->connections;other_link=other_link->Flink) {
            OPENNT_BASE_CONNECTION *other=CONTAINING_RECORD(other_link,OPENNT_BASE_CONNECTION,service_link);
            if(other==parent || other->retained_frontend_root!=root->process.SequenceNumber)continue;
            if((other->pending_creation && !other->registered_worker) ||
                (other->parent_wait && WaitForSingleObject(other->parent_wait,0)==WAIT_TIMEOUT) ||
                other->frontend_request_root==root->process.SequenceNumber ||
                other->frontend_channel_root==root->process.SequenceNumber) {
                pending=TRUE;break;
            }
        }
        if(idle_worker)for(other_link=parent->service->connections.Flink;
            other_link!=&parent->service->connections;other_link=other_link->Flink) {
            OPENNT_BASE_CONNECTION *other=CONTAINING_RECORD(other_link,OPENNT_BASE_CONNECTION,service_link);
            if(other->native_worker && other->native_root==root->process.SequenceNumber &&
                (other->native_inflight || !IsListEmpty(&other->win32records))) {pending=TRUE;break;}
        }
        if(pending)continue;
        /* Arm the existing return acknowledgement before the shutdown
         * notification. The launcher must not return before Console cleanup. */
        if(root->frontend_creator_generation==parent->process.SequenceNumber && root->frontend_retire)
            parent->frontend_return_pending=TRUE;
        root->frontend_closing=TRUE;
        service_signal_frontend_states(parent->service);
        WakeAllConditionVariable(&parent->service->frontend_changed);
        break;
    }
}
