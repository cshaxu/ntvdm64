/* NTSRV-private lifecycle; project adaptation,
 * not an original OpenNT mirror. Physical separation only: existing function
 * bodies, state authority and lock/resource contracts are preserved. */
#include <service_internal.h>

static ULONGLONG service_root_retirement_deadline(OPENNT_BASE_CONNECTION *root,ULONGLONG now);

typedef struct service_frontend_shutdown {
    HANDLE process;
    HANDLE shutdown;
} service_frontend_shutdown;

/* Service lock held. Capture only authenticated non-WOW workers bound to this
 * frontend root. A frontend deletion is Console-close, never Worker-close. */
static DWORD service_capture_frontend_shutdowns(OPENNT_BASE_SERVICE *service,
    OPENNT_BASE_CONNECTION *root,service_frontend_shutdown **stops,DWORD *count)
{
    LIST_ENTRY *link;
    service_frontend_shutdown *items=NULL;
    DWORD capacity=0,index=0,error=ERROR_SUCCESS;
    if(!service || !root || !stops || !count)return ERROR_INVALID_PARAMETER;
    *stops=NULL;*count=0;
    for(link=service->worker_watches.Flink;link!=&service->worker_watches;link=link->Flink) {
        OPENNT_BASE_WORKER_WATCH *watch=CONTAINING_RECORD(link,OPENNT_BASE_WORKER_WATCH,link);
        if(!watch->wow && !watch->native_gui_launcher &&
            watch->frontend_root_generation==root->process.SequenceNumber &&
            WaitForSingleObject(watch->process.ProcessHandle,0)==WAIT_TIMEOUT)++capacity;
    }
    if(!capacity)return ERROR_SUCCESS;
    items=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,capacity*sizeof(*items));
    if(!items)return ERROR_NOT_ENOUGH_MEMORY;
    for(link=service->worker_watches.Flink;link!=&service->worker_watches;link=link->Flink) {
        OPENNT_BASE_WORKER_WATCH *watch=CONTAINING_RECORD(link,OPENNT_BASE_WORKER_WATCH,link);
        if(watch->wow || watch->native_gui_launcher ||
            watch->frontend_root_generation!=root->process.SequenceNumber ||
            WaitForSingleObject(watch->process.ProcessHandle,0)!=WAIT_TIMEOUT)continue;
        if(!watch->console_close) { error=ERROR_NOT_READY;break; }
        if(!DuplicateHandle(GetCurrentProcess(),watch->process.ProcessHandle,GetCurrentProcess(),
            &items[index].process,SYNCHRONIZE,FALSE,0) ||
            !DuplicateHandle(GetCurrentProcess(),watch->console_close,GetCurrentProcess(),
            &items[index].shutdown,EVENT_MODIFY_STATE,FALSE,0)) {
            error=GetLastError();break;
        }
        /* The root's drain flag is the admission barrier.  Do not repurpose
         * the management-termination bit: this is an orderly frontend close,
         * and its existing shutdown path remains the sole worker exit path. */
        ++index;
    }
    if(error) {
        while(index) {--index;CloseHandle(items[index].process);CloseHandle(items[index].shutdown);}
        HeapFree(GetProcessHeap(),0,items);return error;
    }
    *stops=items;*count=index;return ERROR_SUCCESS;
}

static void service_release_frontend_shutdowns(service_frontend_shutdown *stops,DWORD count)
{
    if(!stops)return;
    while(count) {--count;CloseHandle(stops[count].process);CloseHandle(stops[count].shutdown);}
    HeapFree(GetProcessHeap(),0,stops);
}

/* The final worker-side ResetConsoleState must precede NTCON's restore of the
 * same borrowed Console.  Use the existing worker process watches as the
 * completion proof; do not add delay/retry policy or alter guest code. */
static DWORD service_shutdown_frontend_workers(service_frontend_shutdown *stops,DWORD count)
{
    DWORD index,error=ERROR_SUCCESS;
    ULONGLONG deadline=GetTickCount64()+10000u;
    for(index=0;index<count;++index)
        if(!SetEvent(stops[index].shutdown))return GetLastError();
    for(index=0;index<count;++index) {
        ULONGLONG now=GetTickCount64();
        DWORD wait=now>=deadline ? 0u : (DWORD)(deadline-now);
        DWORD status=WaitForSingleObject(stops[index].process,wait);
        if(status!=WAIT_OBJECT_0) {error=status==WAIT_FAILED ? GetLastError() : ERROR_TIMEOUT;break;}
    }
    return error;
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

static DWORD service_terminate_worker(OPENNT_BASE_SERVICE *service,uint32_t process_id,
    const OPENNT_BASE_MANAGEMENT_KEY *key)
{
    LIST_ENTRY *link;
    HANDLE process=NULL,shutdown=NULL;
    DWORD error=ERROR_NOT_FOUND;
    if (!service || (!process_id && !key)) return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&service->lock);
    if(key && (key->instance!=service->management_epoch || key->category!=MANAGEMENT_WORKER ||
        !key->generation || key->object)) {error=ERROR_INVALID_HANDLE;goto unlock;}
    for (link=service->worker_watches.Flink;link!=&service->worker_watches;link=link->Flink) {
        OPENNT_BASE_WORKER_WATCH *watch=CONTAINING_RECORD(link,OPENNT_BASE_WORKER_WATCH,link);
        OPENNT_BASE_CONNECTION *worker=NULL;
        LIST_ENTRY *connection_link;
        if(key ? watch->process.SequenceNumber!=key->generation :
            (DWORD)(ULONG_PTR)watch->process.ClientId.UniqueProcess!=process_id)continue;
        process_id=(DWORD)(ULONG_PTR)watch->process.ClientId.UniqueProcess;
        for(connection_link=service->connections.Flink;
            connection_link!=&service->connections;connection_link=connection_link->Flink) {
            OPENNT_BASE_CONNECTION *candidate=CONTAINING_RECORD(connection_link,
                OPENNT_BASE_CONNECTION,service_link);
            if(candidate->process.SequenceNumber==watch->process.SequenceNumber) {
                worker=candidate;break;
            }
        }
        /* A native carrier exposes its worker-base receiver only after native
         * backend registration.  VDM workers have their original lifecycle
         * receiver as part of the authenticated reservation/update path, so
         * its shutdown event remains valid during that startup interval. */
        if(!worker || (worker->native_worker && !worker->registered_worker))
            {error=ERROR_NOT_READY;break;}
        if (watch->termination_requested) { error=ERROR_BUSY; break; }
        if (!DuplicateHandle(GetCurrentProcess(),watch->process.ProcessHandle,
                GetCurrentProcess(),&process,SYNCHRONIZE,FALSE,0) ||
            !DuplicateHandle(GetCurrentProcess(),watch->shutdown,GetCurrentProcess(),&shutdown,
                EVENT_MODIFY_STATE,FALSE,0)) {
            error=GetLastError(); break;
        }
        watch->termination_requested=TRUE;
        error=ERROR_SUCCESS;
        break;
    }
unlock:
    LeaveCriticalSection(&service->lock);
    if(!error && !SetEvent(shutdown))error=GetLastError();
    if(!error && WaitForSingleObject(process,10000)!=WAIT_OBJECT_0)error=ERROR_TIMEOUT;
    if(shutdown){CloseHandle(shutdown);shutdown=NULL;}
    if(error) {if(process)CloseHandle(process);return error;}
    CloseHandle(process);
    return error;
}

/* Local compatibility entry for the existing owner fixtures, not wire identity. */
DWORD OpenNtBaseServiceTerminateWorker(OPENNT_BASE_SERVICE *service,uint32_t process_id)
{
    return service_terminate_worker(service,process_id,NULL);
}

DWORD OpenNtBaseServiceCloseManagementNode(OPENNT_BASE_SERVICE *service,
    const OPENNT_BASE_MANAGEMENT_KEY *key)
{
    LIST_ENTRY *link;
    HANDLE target=NULL;
    service_frontend_shutdown *stops=NULL;
    DWORD stop_count=0;
    BOOL frontend=FALSE;
    DWORD error=ERROR_NOT_FOUND;
    if(!service || !key)return ERROR_INVALID_PARAMETER;
    if(key->category==MANAGEMENT_WORKER)return service_terminate_worker(service,0,key);
    EnterCriticalSection(&service->lock);
    if(key->instance!=service->management_epoch || !key->generation) {
        error=ERROR_INVALID_HANDLE;goto done;
    }
    if(key->category==MANAGEMENT_FRONTEND && !key->object) {
        for(link=service->connections.Flink;link!=&service->connections;link=link->Flink) {
            OPENNT_BASE_CONNECTION *root=CONTAINING_RECORD(link,OPENNT_BASE_CONNECTION,service_link);
            if(root->process.SequenceNumber!=key->generation || !root->frontend_capability)continue;
            if(root->frontend_closing || root->frontend_close_draining){error=ERROR_BUSY;break;}
            if(WaitForSingleObject(root->process.ProcessHandle,0)!=WAIT_TIMEOUT)break;
            if(!DuplicateHandle(GetCurrentProcess(),root->process.ProcessHandle,GetCurrentProcess(),
                &target,SYNCHRONIZE,FALSE,0)){error=GetLastError();break;}
            error=service_capture_frontend_shutdowns(service,root,&stops,&stop_count);
            if(error) {CloseHandle(target);target=NULL;break;}
            /* Keep NTCON alive while the worker performs its original final
             * Console reset.  The drain bit is the admission barrier; only
             * after the worker process is gone do we publish closing. */
            root->frontend_close_draining=stop_count ? TRUE : FALSE;
            if(!stop_count)root->frontend_closing=TRUE;
            /* Do not wake NTCON while a worker still owns the final Console
             * reset.  Its ordinary request loop would otherwise observe the
             * admission barrier as a broken route and exit too early. */
            if(!stop_count)service_signal_frontend_states(service);
            WakeAllConditionVariable(&service->frontend_changed);
            frontend=TRUE;error=ERROR_SUCCESS;break;
        }
    } else if(key->category==MANAGEMENT_GUI_TARGET && key->object) {
        /* Keep management-key validation honest, but no GUI observation ever
         * grants an operation over the real process. */
        for(link=service->gui_records.Flink;link!=&service->gui_records;link=link->Flink) {
            OPENNT_BASE_WIN32RECORD *record=CONTAINING_RECORD(link,OPENNT_BASE_WIN32RECORD,link);
            if(record->worker_generation==key->generation && record->request==key->object) {
                error=ERROR_NOT_SUPPORTED;break;
            }
        }
    }
    else error=ERROR_NOT_SUPPORTED;
done:
    LeaveCriticalSection(&service->lock);
    /* Wait/termination cannot hold the registry lock needed for rundown.
     * A pinned original process object, never a reopened PID, is targeted. */
    if(!error) {
        if(frontend) {
            error=service_shutdown_frontend_workers(stops,stop_count);
            if(!error && stop_count) {
                EnterCriticalSection(&service->lock);
                /* The root stayed live while draining, so this is the only
                 * point at which NTCON may restore the borrowed Console. */
                for(link=service->connections.Flink;link!=&service->connections;link=link->Flink) {
                    OPENNT_BASE_CONNECTION *root=CONTAINING_RECORD(link,OPENNT_BASE_CONNECTION,service_link);
                    if(root->process.SequenceNumber==key->generation) {
                        root->frontend_close_draining=FALSE;
                        root->frontend_closing=TRUE;
                        service_signal_frontend_states(service);
                        WakeAllConditionVariable(&service->frontend_changed);
                        break;
                    }
                }
                LeaveCriticalSection(&service->lock);
            }
            if(!error) {
                DWORD wait=WaitForSingleObject(target,10000);
                if(wait!=WAIT_OBJECT_0)error=wait==WAIT_FAILED ? GetLastError() : ERROR_TIMEOUT;
            }
        } else if(!TerminateProcess(target,ERROR_CANCELLED))error=GetLastError();
    }
    service_release_frontend_shutdowns(stops,stop_count);
    if(target)CloseHandle(target);
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
        /* Resident association survives a closed I/O pipe. Only an admitted,
         * not-yet-delivered connection pins the Console-return barrier. */
        if(route->io_requested && !route->delivered)pending_count=1;
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
        if(watch->frontend_root_generation==generation)return TRUE;
    }
    return FALSE;
}

static ULONGLONG service_root_retirement_deadline(OPENNT_BASE_CONNECTION *root,ULONGLONG now)
{
    LIST_ENTRY *link;
    ULONGLONG deadline;
    if(!root->frontend_capability || !root->frontend_console_window || root->frontend_closing || root->frontend_close_draining ||
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

/* The root process is NTCON, so its continued attachment must never count as
 * proof that a borrowed user Console still exists.  The identity is fixed at
 * authenticated root registration: later processes cannot revive an orphaned
 * frontend merely by attaching to the Console object NTCON kept alive. */
static BOOL service_root_borrowed_console_lost(const OPENNT_BASE_CONNECTION *root)
{
    DWORD index,root_pid;
    if(!root || !root->frontend_borrowed || !root->frontend_console_window ||
        !root->console_members || !root->console_member_processes ||
        !root->console_member_count)return FALSE;
    root_pid=(DWORD)(ULONG_PTR)root->process.ClientId.UniqueProcess;
    for(index=0;index<root->console_member_count;++index) {
        HANDLE member=root->console_member_processes[index];
        DWORD wait;
        if(!member)return FALSE;
        if(root->console_members[index]==root_pid && GetProcessId(member)==root_pid)continue;
        wait=WaitForSingleObject(member,0);
        if(wait==WAIT_TIMEOUT || wait==WAIT_FAILED)return FALSE;
    }
    return TRUE;
}

HANDLE OpenNtBaseServiceFrontendLifetimeChanged(OPENNT_BASE_SERVICE *service)
{
    return service ? service->frontend_lifetime_changed : NULL;
}

DWORD OpenNtBaseServiceNextFrontendDeadline(OPENNT_BASE_SERVICE *service,ULONGLONG *deadline)
{
    return service_next_frontend_deadline_at(service,GetTickCount64(),deadline);
}

/* Explicit time is private to the policy owner. Fixtures advance the same
 * decision path, not a shortened timer or replacement lifecycle policy. */
DWORD service_next_frontend_deadline_at(OPENNT_BASE_SERVICE *service,ULONGLONG now,ULONGLONG *deadline)
{
    LIST_ENTRY *link;
    if(!service || !deadline)return ERROR_INVALID_PARAMETER;
    *deadline=0;
    EnterCriticalSection(&service->lock);
    for(link=service->connections.Flink;link!=&service->connections;link=link->Flink) {
        OPENNT_BASE_CONNECTION *root=CONTAINING_RECORD(link,OPENNT_BASE_CONNECTION,service_link);
        ULONGLONG due=service_root_retirement_deadline(root,now);
        if(due && (!*deadline || due<*deadline))*deadline=due;
    }
    LeaveCriticalSection(&service->lock);
    return ERROR_SUCCESS;
}

DWORD OpenNtBaseServiceRetireExpiredFrontends(OPENNT_BASE_SERVICE *service)
{
    return service_retire_expired_frontends_at(service,GetTickCount64());
}

DWORD service_retire_expired_frontends_at(OPENNT_BASE_SERVICE *service,ULONGLONG now)
{
    LIST_ENTRY *link;
    BOOL changed=FALSE,gui_changed;
    if(!service)return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&service->lock);
    gui_changed=service_prune_gui_records(service);
    service_observation_prune(service);
    for(link=service->connections.Flink;link!=&service->connections;link=link->Flink) {
        OPENNT_BASE_CONNECTION *root=CONTAINING_RECORD(link,OPENNT_BASE_CONNECTION,service_link);
        if(service_root_borrowed_console_lost(root)) {
            root->frontend_closing=TRUE;
            changed=TRUE;
            continue;
        }
        ULONGLONG deadline=service_root_retirement_deadline(root,now);
        if(deadline && deadline<=now) {
            root->frontend_closing=TRUE;
            changed=TRUE;
        }
    }
    /* Only NTSRV translates lost frontend ownership into worker shutdown.
     * An admitted GUI-only carrier has no character frontend by design;
     * like shared WOW, it remains resident rather than timing out as startup. */
    for(link=service->worker_watches.Flink;link!=&service->worker_watches;link=link->Flink) {
        OPENNT_BASE_WORKER_WATCH *watch=CONTAINING_RECORD(link,OPENNT_BASE_WORKER_WATCH,link);
        LIST_ENTRY *root_link;
        BOOL attached=FALSE;
        if(watch->wow || !watch->frontend_root_generation)continue;
        for(root_link=service->connections.Flink;root_link!=&service->connections;root_link=root_link->Flink) {
            OPENNT_BASE_CONNECTION *root=CONTAINING_RECORD(root_link,OPENNT_BASE_CONNECTION,service_link);
            if(!root->frontend_capability)continue;
            if(watch->frontend_root_generation==root->process.SequenceNumber) {
                if(!root->frontend_closing && !root->frontend_close_draining && WaitForSingleObject(root->process.ProcessHandle,0)==WAIT_TIMEOUT)
                    attached=TRUE;
            }
        }
        if(!attached && watch->shutdown &&
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
                if(watch->frontend_root_generation==root->process.SequenceNumber){pending=TRUE;break;}
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
            if(other->native_worker && service_worker_root(other)==root->process.SequenceNumber &&
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
