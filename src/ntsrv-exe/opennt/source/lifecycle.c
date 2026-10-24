/* NTSRV-private lifecycle; project adaptation,
 * not an original OpenNT mirror. Physical separation only: existing function
 * bodies, state authority and lock/resource contracts are preserved. */
#include <service_internal.h>
#include "common/protocol/frontend_lifecycle.h"


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

/* The service owns admission identity; the WOW process owns the actual TD and
 * HTASK.  These helpers intentionally retain no mirror of either private WOW
 * object.  A label disappears with its original WOWRECORD, which makes an
 * undelivered request stale rather than redirecting it after iTask reuse. */
static OPENNT_BASE_MANAGEMENT_LABEL *service_wow_task_label(
    OPENNT_BASE_WORKER_WATCH *watch,uint64_t identity,ULONG task)
{
    LIST_ENTRY *link;
    for(link=watch->management_labels.Flink;link!=&watch->management_labels;link=link->Flink) {
        OPENNT_BASE_MANAGEMENT_LABEL *label=CONTAINING_RECORD(link,OPENNT_BASE_MANAGEMENT_LABEL,link);
        if(label->wow_record && label->identity==identity && label->task==task)return label;
    }
    return NULL;
}

/* Only the original shared-WOW record supplies a task identity.  Separate
 * WOW deliberately consumes its one-shot record before WOW32 reaches
 * InitTask, so it remains worker-close-only rather than gaining a fabricated
 * individual-task management row. */
static BOOL service_wow_worker_connection(const OPENNT_BASE_CONNECTION *connection)
{
    return connection && connection->wow;
}

DWORD OpenNtBaseServiceBindWowTask(OPENNT_BASE_CONNECTION *connection,DWORD pid,
    DWORD generation,ULONG task,USHORT htask,uint64_t *identity)
{
    OPENNT_BASE_WORKER_WATCH *watch;LIST_ENTRY *link;DWORD error=ERROR_ACCESS_DENIED;
    if(!identity || !htask)return ERROR_INVALID_PARAMETER;
    *identity=0;
    if(!connection)return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&connection->service->lock);
    if(!OpenNtBaseServicePeer(connection,pid,generation) ||
        !service_wow_worker_connection(connection))goto done;
    watch=service_find_worker_watch(connection->service,generation);
    if(!watch || !watch->wow || watch->close_kind!=WORKER_CLOSE_NONE) {error=ERROR_INVALID_STATE;goto done;}
    service_capture_wow_management_labels(connection);
    error=ERROR_NOT_FOUND;
    if(connection->wow) {
        if(!task){error=ERROR_INVALID_PARAMETER;goto done;}
        for(link=watch->management_labels.Flink;link!=&watch->management_labels;link=link->Flink) {
            OPENNT_BASE_MANAGEMENT_LABEL *label=CONTAINING_RECORD(link,OPENNT_BASE_MANAGEMENT_LABEL,link);
            if(!label->wow_record || label->task!=task)continue;
            if(label->wow_task_bound){error=ERROR_ALREADY_EXISTS;break;}
            label->wow_task_bound=TRUE;
            *identity=label->identity;
            error=ERROR_SUCCESS;
            break;
        }
    }
    if(error==ERROR_NOT_FOUND && connection->wow) {
        /* The worker's TD is already authoritative, but BaseSrv can expose
         * its matching WOWRECORD only after the in-flight GetNext returns.
         * Preserve this authenticated pending label; management remains
         * invisible until management.c resolves it to that original record. */
        if(connection->service->next_management_task==UINT64_MAX) {
            error=ERROR_NOT_ENOUGH_MEMORY;
        } else {
            OPENNT_BASE_MANAGEMENT_LABEL *pending=HeapAlloc(GetProcessHeap(),
                HEAP_ZERO_MEMORY,sizeof(*pending));
            if(!pending) error=ERROR_NOT_ENOUGH_MEMORY;
            else {
                pending->identity=++connection->service->next_management_task;
                pending->task=task;pending->wow_task_bound=TRUE;
                InsertTailList(&watch->management_labels,&pending->link);
                *identity=pending->identity;error=ERROR_SUCCESS;
            }
        }
    }
done:
    LeaveCriticalSection(&connection->service->lock);
    return error;
}

DWORD OpenNtBaseServiceRetireWowTask(OPENNT_BASE_CONNECTION *connection,DWORD pid,
    DWORD generation,uint64_t identity)
{
    OPENNT_BASE_WORKER_WATCH *watch;OPENNT_BASE_MANAGEMENT_LABEL *label;DWORD error=ERROR_ACCESS_DENIED;
    if(!connection || !identity)return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&connection->service->lock);
    if(!OpenNtBaseServicePeer(connection,pid,generation) ||
        !service_wow_worker_connection(connection))goto done;
    watch=service_find_worker_watch(connection->service,generation);
    if(!watch || !watch->wow){error=ERROR_INVALID_STATE;goto done;}
    service_capture_wow_management_labels(connection);
    for(LIST_ENTRY *link=watch->management_labels.Flink;link!=&watch->management_labels;link=link->Flink) {
        label=CONTAINING_RECORD(link,OPENNT_BASE_MANAGEMENT_LABEL,link);
        if(label->identity==identity && label->wow_task_bound) {
            label->wow_task_bound=FALSE;label->wow_task_close_requested=FALSE;
            error=ERROR_SUCCESS;goto done;
        }
    }
    error=ERROR_NOT_FOUND;
done:
    LeaveCriticalSection(&connection->service->lock);
    return error;
}

DWORD OpenNtBaseServiceTakeWowTaskClose(OPENNT_BASE_CONNECTION *connection,DWORD pid,
    DWORD generation,uint64_t *identity)
{
    OPENNT_BASE_WORKER_WATCH *watch;DWORD error=ERROR_ACCESS_DENIED;
    if(!identity)return ERROR_INVALID_PARAMETER;
    *identity=0;
    if(!connection)return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&connection->service->lock);
    if(!OpenNtBaseServicePeer(connection,pid,generation) ||
        !service_wow_worker_connection(connection))goto done;
    watch=service_find_worker_watch(connection->service,generation);
    if(!watch || !watch->wow){error=ERROR_INVALID_STATE;goto done;}
    service_capture_wow_management_labels(connection);
    while(!IsListEmpty(&watch->wow_task_closes)) {
        LIST_ENTRY *entry=RemoveHeadList(&watch->wow_task_closes);
        OPENNT_BASE_WOW_TASK_CLOSE *request=CONTAINING_RECORD(entry,OPENNT_BASE_WOW_TASK_CLOSE,link);
        OPENNT_BASE_MANAGEMENT_LABEL *label=service_wow_task_label(watch,request->identity,request->task);
        if(label && label->wow_task_bound) {*identity=request->identity;error=ERROR_SUCCESS;HeapFree(GetProcessHeap(),0,request);break;}
        HeapFree(GetProcessHeap(),0,request);
    }
    if(IsListEmpty(&watch->wow_task_closes))(void)ResetEvent(watch->wow_task_close);
    if(!*identity && error==ERROR_ACCESS_DENIED)error=ERROR_NOT_FOUND;
done:
    LeaveCriticalSection(&connection->service->lock);
    return error;
}

DWORD OpenNtBaseServiceFailWowTaskClose(OPENNT_BASE_CONNECTION *connection,DWORD pid,
    DWORD generation,uint64_t identity)
{
    OPENNT_BASE_WORKER_WATCH *watch;OPENNT_BASE_MANAGEMENT_LABEL *label=NULL;
    LIST_ENTRY *link;DWORD error=ERROR_ACCESS_DENIED;
    if(!connection || !identity)return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&connection->service->lock);
    if(!OpenNtBaseServicePeer(connection,pid,generation) ||
        !service_wow_worker_connection(connection))goto done;
    watch=service_find_worker_watch(connection->service,generation);
    if(!watch || !watch->wow || watch->close_kind!=WORKER_CLOSE_NONE){error=ERROR_INVALID_STATE;goto done;}
    service_capture_wow_management_labels(connection);
    for(link=watch->management_labels.Flink;link!=&watch->management_labels;link=link->Flink) {
        OPENNT_BASE_MANAGEMENT_LABEL *candidate=CONTAINING_RECORD(link,OPENNT_BASE_MANAGEMENT_LABEL,link);
        if(candidate->identity==identity){label=candidate;break;}
    }
    if(!label || !label->wow_task_bound || !label->wow_task_close_requested){error=ERROR_NOT_FOUND;goto done;}
    /* A local original-entry failure is neither a task completion nor a
     * worker failure.  Reopen the same authenticated row for an explicit
     * later operator request; never immediately self-requeue a manual event. */
    label->wow_task_close_requested=FALSE;
    error=ERROR_SUCCESS;
done:
    LeaveCriticalSection(&connection->service->lock);
    return error;
}

static DWORD service_terminate_worker(OPENNT_BASE_SERVICE *service,uint32_t process_id,
    const OPENNT_BASE_MANAGEMENT_KEY *key)
{
    LIST_ENTRY *link;
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
        /* Connect already owns the authenticated watch and manual-reset
         * instruction. Backend readiness gates execution, not cancellation:
         * startup failure must not need a working frontend to request exit.
         * A receiver attached later observes the same latched instruction. */
        if(!worker){error=ERROR_NOT_READY;break;}
        if (watch->close_kind!=WORKER_CLOSE_NONE) { error=ERROR_SUCCESS; break; }
        if(!watch->shutdown){error=ERROR_NOT_READY;break;}
        /* SetEvent cannot call back or wait. Commit acceptance only after the
         * service-owned instruction is latched, under its lifetime lock. */
        if(!SetEvent(watch->shutdown)){error=GetLastError();break;}
        watch->close_kind=WORKER_CLOSE_WORKER;
        error=ERROR_SUCCESS;
        break;
    }
unlock:
    LeaveCriticalSection(&service->lock);
    /* Acceptance does not wait for a worker containing the requesting task. */
    return error;
}

/* Local compatibility entry for the existing owner fixtures, not wire identity. */
DWORD OpenNtBaseServiceTerminateWorker(OPENNT_BASE_SERVICE *service,uint32_t process_id)
{
    return service_terminate_worker(service,process_id,NULL);
}

DWORD OpenNtBaseServiceRequestParticipantExit(OPENNT_BASE_CONNECTION *participant,DWORD pid,
    DWORD generation,DWORD scope)
{
    OPENNT_BASE_SERVICE *service;
    OPENNT_BASE_MANAGEMENT_KEY key={0};
    OPENNT_BASE_WORKER_WATCH *watch;
    DWORD error=ERROR_ACCESS_DENIED;
    if(!participant)return ERROR_INVALID_PARAMETER;
    service=participant->service;
    EnterCriticalSection(&service->lock);
    if(!OpenNtBaseServicePeer(participant,pid,generation))goto done;
    if(scope!=OPENNT_EXIT_PARTICIPANT && scope!=OPENNT_EXIT_CONSOLE) {
        error=ERROR_INVALID_PARAMETER;goto done;
    }
    key.instance=service->management_epoch;
    key.generation=participant->process.SequenceNumber;
    if(participant->frontend_capability) {
        key.category=MANAGEMENT_FRONTEND;
        error=ERROR_SUCCESS;
        goto done;
    }
    watch=service_find_worker_watch(service,participant->process.SequenceNumber);
    if(!watch)goto done;
    /* A transient GUI launcher has no managed worker teardown. Its startup
     * release is a distinct service result, not a Console/worker command. */
    if(watch->native_gui_launcher){error=ERROR_NOT_SUPPORTED;goto done;}
    key.category=MANAGEMENT_WORKER;
    if(scope==OPENNT_EXIT_CONSOLE) {
        if(watch->wow){error=ERROR_NOT_SUPPORTED;goto done;}
        if(!watch->frontend_root_generation){error=ERROR_NOT_READY;goto done;}
        /* Never accept a caller-supplied frontend identity. The retained
         * authenticated association selects the existing close transaction. */
        key.category=MANAGEMENT_FRONTEND;
        key.generation=watch->frontend_root_generation;
    }
    error=ERROR_SUCCESS;
done:
    LeaveCriticalSection(&service->lock);
    /* Re-resolve by generation, never by PID after dropping the lock. Reuse
     * management arbitration: a pending Console close keeps its quiescence
     * barrier, and repeated intent cannot send a competing worker command. */
    return error ? error : OpenNtBaseServiceCloseManagementNode(service,&key);
}

DWORD OpenNtBaseServiceCloseManagementNode(OPENNT_BASE_SERVICE *service,
    const OPENNT_BASE_MANAGEMENT_KEY *key)
{
    LIST_ENTRY *link;
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
            /* One terminal operation per root generation. A repeated request
             * observes acceptance, never restarts cleanup. */
            if(root->frontend_close_requested){error=ERROR_SUCCESS;break;}
            if(root->frontend_closing){error=ERROR_BUSY;break;}
            if(WaitForSingleObject(root->process.ProcessHandle,0)!=WAIT_TIMEOUT)break;
            root->frontend_close_requested=TRUE;
            root->frontend_close_draining=TRUE;
            root->frontend_close_return_required=TRUE;
            root->frontend_close_returned=FALSE;
            {
                LIST_ENTRY *entry;
                for(entry=service->worker_watches.Flink;entry!=&service->worker_watches;entry=entry->Flink) {
                    OPENNT_BASE_WORKER_WATCH *watch=CONTAINING_RECORD(entry,OPENNT_BASE_WORKER_WATCH,link);
                    if(!watch->wow && !watch->native_gui_launcher &&
                        watch->frontend_root_generation==key->generation &&
                        watch->close_kind==WORKER_CLOSE_NONE)
                        watch->close_kind=WORKER_CLOSE_CONSOLE;
                }
            }
            service_signal_frontend_states(service);
            WakeAllConditionVariable(&service->frontend_changed);
            error=ERROR_SUCCESS;break;
        }
    } else if(key->category==MANAGEMENT_GUI_TARGET && key->object) {
        for(link=service->gui_records.Flink;link!=&service->gui_records;link=link->Flink) {
            OPENNT_BASE_WIN32RECORD *record=CONTAINING_RECORD(link,OPENNT_BASE_WIN32RECORD,link);
            if(record->worker_generation==key->generation && record->request==key->object) {
                error=ERROR_NOT_SUPPORTED;break;
            }
        }
    } else if(key->category==MANAGEMENT_WOW_TASK && key->object) {
        OPENNT_BASE_WORKER_WATCH *watch=service_find_worker_watch(service,key->generation);
        OPENNT_BASE_MANAGEMENT_LABEL *label;
        OPENNT_BASE_WOW_TASK_CLOSE *request;
        if(!watch || !watch->wow || watch->close_kind!=WORKER_CLOSE_NONE) {error=ERROR_NOT_FOUND;goto done;}
        service_capture_initial_management_labels(watch);
        /* The opaque management identity is the only caller input that
         * survives original-record address and iTask reuse.  The original
         * scalar is retained only with the queued local delivery record. */
        label=NULL;
        for(link=watch->management_labels.Flink;link!=&watch->management_labels;link=link->Flink) {
            OPENNT_BASE_MANAGEMENT_LABEL *candidate=CONTAINING_RECORD(link,OPENNT_BASE_MANAGEMENT_LABEL,link);
            if(candidate->wow_record && candidate->identity==key->object){label=candidate;break;}
        }
        /* An extant original WOW row that has not authenticated a local
         * HTASK is visible but deliberately not terminable.  Preserve the
         * management API's existing capability result rather than treating
         * an unbound row as a stale management key. */
        if(!label){error=ERROR_INVALID_HANDLE;goto done;}
        if(!label->wow_task_bound){error=ERROR_NOT_SUPPORTED;goto done;}
        if(label->wow_task_close_requested){error=ERROR_SUCCESS;goto done;}
        request=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*request));
        if(!request){error=ERROR_NOT_ENOUGH_MEMORY;goto done;}
        request->identity=label->identity;request->task=label->task;
        label->wow_task_close_requested=TRUE;
        InsertTailList(&watch->wow_task_closes,&request->link);
        if(!SetEvent(watch->wow_task_close)) {
            RemoveEntryList(&request->link);label->wow_task_close_requested=FALSE;
            HeapFree(GetProcessHeap(),0,request);error=GetLastError();goto done;
        }
        error=ERROR_SUCCESS;
    } else error=ERROR_NOT_SUPPORTED;
done:
    LeaveCriticalSection(&service->lock);
    /* Accepted, not completed. The lifecycle owner advances the retained
     * root operation even if this ordinary management task immediately exits. */
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
        /* The scalar wire shape is retained; only NTCON consumes the
         * management pre-close return action. */
        *retired=root->frontend_closing ? OPENNT_FRONTEND_RETIRE_FINAL_CLOSE :
            (root->frontend_close_draining && root->frontend_close_return_required &&
             !root->frontend_close_returned ? OPENNT_FRONTEND_RETIRE_RETURN_CONSOLE :
             OPENNT_FRONTEND_RETIRE_NONE);
        if(*retired==OPENNT_FRONTEND_RETIRE_NONE && !root->frontend_close_draining &&
            root->frontend_creator_generation &&
            root->frontend_restored && WaitForSingleObject(root->frontend_restored,0)==WAIT_TIMEOUT) {
            LIST_ENTRY *link;BOOL returning=TRUE;DWORD pending=0,tasks=0;
            for(link=root->service->connections.Flink;link!=&root->service->connections;link=link->Flink) {
                OPENNT_BASE_CONNECTION *caller=CONTAINING_RECORD(link,OPENNT_BASE_CONNECTION,service_link);
                if(caller->process.SequenceNumber==root->frontend_creator_generation) {
                    returning=caller->frontend_return_pending ||
                        WaitForSingleObject(caller->process.ProcessHandle,0)!=WAIT_TIMEOUT;
                    break;
                }
            }
            if(returning) {
                error=OpenNtBaseServiceFrontendUsage(root,pid,generation,&pending,&tasks);
                if(error){LeaveCriticalSection(&root->service->lock);return error;}
                if(!pending && !tasks)*retired=OPENNT_FRONTEND_RETIRE_PARK;
            }
        }
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

DWORD OpenNtBaseServiceReconcileFrontends(OPENNT_BASE_SERVICE *service)
{
    LIST_ENTRY *link;
    BOOL changed=FALSE,gui_changed;
    if(!service)return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&service->lock);
    gui_changed=service_prune_gui_records(service);
    service_observation_prune(service);
    for(link=service->connections.Flink;link!=&service->connections;link=link->Flink) {
        OPENNT_BASE_CONNECTION *root=CONTAINING_RECORD(link,OPENNT_BASE_CONNECTION,service_link);
        if(root->frontend_close_draining) {
            BOOL live_worker=FALSE;
            BOOL frontend_live=WaitForSingleObject(root->process.ProcessHandle,0)==WAIT_TIMEOUT;
            LIST_ENTRY *watch_link;
            /* The acknowledgement is the ordering fence.  Before it, retain
             * the transaction but do not touch worker lifetime.  Afterwards
             * retry only Console-close delivery; a lost wake cannot silently
             * turn into worker shutdown or strand the root forever. */
            if(!root->frontend_close_returned && frontend_live)continue;
            /* If NTCON itself has gone away before acknowledging, still use
             * Console-close for the worker. Reopening admission would be
             * wrong; generic worker shutdown would be the wrong command. */
            for(watch_link=service->worker_watches.Flink;
                watch_link!=&service->worker_watches;watch_link=watch_link->Flink) {
                OPENNT_BASE_WORKER_WATCH *watch=CONTAINING_RECORD(watch_link,OPENNT_BASE_WORKER_WATCH,link);
                if(watch->wow || watch->native_gui_launcher ||
                    watch->frontend_root_generation!=root->process.SequenceNumber ||
                    WaitForSingleObject(watch->process.ProcessHandle,0)!=WAIT_TIMEOUT)continue;
                live_worker=TRUE;
                if(watch->close_kind==WORKER_CLOSE_NONE)watch->close_kind=WORKER_CLOSE_CONSOLE;
                /* The first accepted terminal scope survives root rundown.
                 * Console-close encompasses an existing worker-close by
                 * waiting for it, never by dispatching a second close kind. */
                if(watch->close_kind==WORKER_CLOSE_CONSOLE && watch->console_close &&
                    WaitForSingleObject(watch->console_close,0)==WAIT_TIMEOUT) {
                    (void)SetEvent(watch->console_close);
                    changed=TRUE;
                }
            }
            if(!live_worker) {
                root->frontend_close_draining=FALSE;
                root->frontend_close_return_required=FALSE;
                root->frontend_closing=TRUE;
                changed=TRUE;
            }
            continue;
        }
        if(service_root_borrowed_console_lost(root)) {
            root->frontend_closing=TRUE;
            changed=TRUE;
            continue;
        }
    }
    /* Only NTSRV translates lost frontend ownership into Console-close.
     * An admitted GUI-only carrier has no character frontend by design;
     * like shared WOW, it remains resident rather than timing out as startup. */
    for(link=service->worker_watches.Flink;link!=&service->worker_watches;link=link->Flink) {
        OPENNT_BASE_WORKER_WATCH *watch=CONTAINING_RECORD(link,OPENNT_BASE_WORKER_WATCH,link);
        LIST_ENTRY *root_link;
        BOOL attached=FALSE;
        if(watch->wow || !watch->frontend_root_generation ||
            WaitForSingleObject(watch->process.ProcessHandle,0)!=WAIT_TIMEOUT)continue;
        for(root_link=service->connections.Flink;root_link!=&service->connections;root_link=root_link->Flink) {
            OPENNT_BASE_CONNECTION *root=CONTAINING_RECORD(root_link,OPENNT_BASE_CONNECTION,service_link);
            if(!root->frontend_capability)continue;
            if(watch->frontend_root_generation==root->process.SequenceNumber) {
                /* A draining transaction owns Console-close even if its
                 * frontend dies. Do not issue a competing worker-close. */
                if(root->frontend_close_draining ||
                    (!root->frontend_closing && WaitForSingleObject(root->process.ProcessHandle,0)==WAIT_TIMEOUT))
                    attached=TRUE;
            }
        }
        if(!attached) {
            if(watch->close_kind==WORKER_CLOSE_NONE)watch->close_kind=WORKER_CLOSE_CONSOLE;
            if(watch->close_kind==WORKER_CLOSE_CONSOLE && watch->console_close &&
                WaitForSingleObject(watch->console_close,0)==WAIT_TIMEOUT) {
                (void)SetEvent(watch->console_close);
                changed=TRUE; /* Also wake pre-binding GetNextCommand. */
            }
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





/* Service lock held. The owner-specific boundary proves completion before
 * entering this shared Console-return/retirement path: original nonzero
 * DosSesId worker exit, or native final I/O acknowledgement with an empty
 * backend Console. The caller has already consumed its exact completion;
 * PIF CloseOnExit is never reimplemented here. */
void service_retire_completed_root(OPENNT_BASE_CONNECTION *parent)
{
    LIST_ENTRY *link;
    for(link=parent->service->connections.Flink;
        link!=&parent->service->connections;link=link->Flink) {
        OPENNT_BASE_CONNECTION *root=CONTAINING_RECORD(link,OPENNT_BASE_CONNECTION,service_link);
        if(root->process.SequenceNumber!=parent->retained_frontend_root ||
            !root->frontend_capability || root->frontend_closing ||
            !service_root_console_matches(root,parent->console))continue;
        /* A child completion is not a terminal-root completion.  In
         * particular, COMMAND can enter a DOS worker from an active NTVWM
         * carrier (and the reverse); the inner Run16 has the same retained
         * root but did not create it.  Starting a terminal transaction here
         * would freeze the still-live parent before it can consume `exit`.
         * The root creator is the only authority that may start its
         * self-created Console's retirement, for both DOS and native paths. */
        if(root->frontend_borrowed ||
            root->frontend_creator_generation!=parent->process.SequenceNumber)continue;
        /* Completion has one root-owned meaning irrespective of whether its
         * source was DOS or NTVWM-CUI.  Later worker exits re-enter the same
         * broker transaction; no component guesses that another carrier is
         * already idle, and no component contacts Run16 or NTCON directly. */
        root->frontend_terminal_retire=TRUE;
        service_advance_terminal_root_retire(parent->service,root->process.SequenceNumber);
        break;
    }
}

/* Service lock held.  A terminal root transaction becomes frontend close
 * only after every NTSRV-requested text carrier has really exited.  A root
 * can legitimately contain both the original DOS carrier and NTVWM-CUI:
 * both receive the same broker-owned worker-close instruction. */
void service_advance_terminal_root_retire(OPENNT_BASE_SERVICE *service,DWORD root_generation)
{
    LIST_ENTRY *link;
    OPENNT_BASE_CONNECTION *root=NULL;
    BOOL live=FALSE,pending=FALSE;
    if(!service || !root_generation)return;
    for(link=service->connections.Flink;link!=&service->connections;link=link->Flink) {
        OPENNT_BASE_CONNECTION *candidate=CONTAINING_RECORD(link,OPENNT_BASE_CONNECTION,service_link);
        if(candidate->process.SequenceNumber==root_generation) {root=candidate;break;}
    }
    if(!root || !root->frontend_terminal_retire || root->frontend_closing)return;
    for(link=service->connections.Flink;link!=&service->connections;link=link->Flink) {
        OPENNT_BASE_CONNECTION *other=CONTAINING_RECORD(link,OPENNT_BASE_CONNECTION,service_link);
        if(other->retained_frontend_root!=root_generation)continue;
        if((other->pending_creation && !other->registered_worker) ||
            (other->parent_wait && WaitForSingleObject(other->parent_wait,0)==WAIT_TIMEOUT) ||
            other->frontend_request_root==root_generation || other->frontend_channel_root==root_generation) {
            pending=TRUE;break;
        }
    }
    if(pending)return;
    for(link=service->worker_watches.Flink;link!=&service->worker_watches;link=link->Flink) {
        OPENNT_BASE_WORKER_WATCH *watch=CONTAINING_RECORD(link,OPENNT_BASE_WORKER_WATCH,link);
        OPENNT_BASE_CONNECTION *worker=NULL;
        LIST_ENTRY *worker_link;
        if(watch->frontend_root_generation!=root_generation ||
            WaitForSingleObject(watch->process.ProcessHandle,0)!=WAIT_TIMEOUT)continue;
        /* WOW and GUI launchers do not carry this root's text Console. They
         * must not keep a Console-return transaction live merely because an
         * unrelated observation record remains registered. */
        if(watch->wow || watch->native_gui_launcher)continue;
        live=TRUE;
        for(worker_link=service->connections.Flink;worker_link!=&service->connections;worker_link=worker_link->Flink) {
            OPENNT_BASE_CONNECTION *candidate=CONTAINING_RECORD(worker_link,OPENNT_BASE_CONNECTION,service_link);
            if(candidate->process.SequenceNumber==watch->process.SequenceNumber) {worker=candidate;break;}
        }
        /* WOW has no text frontend and a GUI launcher is one-shot.  Neither
         * belongs to this root Console retirement.  The remaining two kinds
         * are text carriers with distinct implementation paths but one
         * NTSRV-owned worker-close protocol.  Returning merely because the
         * DOS carrier is live creates a cycle: a root COMMAND can be waiting
         * for the service to close it while this transaction waits for it to
         * disappear. */
        if(!worker ||
            (watch->kind!=OPENNT_BASE_WORKER_DOS && watch->kind!=OPENNT_BASE_WORKER_NATIVE) ||
            (worker->native_worker && (worker->native_gui_launcher || worker->native_inflight ||
                !IsListEmpty(&worker->win32records))))return;
        if(watch->close_kind==WORKER_CLOSE_NONE) {
            if(!watch->shutdown || !SetEvent(watch->shutdown))return;
            watch->close_kind=WORKER_CLOSE_WORKER;
        }
    }
    if(live)return;
    /* Do not turn a completed terminal transaction into FINAL_CLOSE here.
     * The creator's existing WaitFrontendConsoleRestored request, or its
     * still-live lease, is what lets NTCON park the now-workerless frontend
     * and acknowledge restored Console mode.  Marking root closing first
     * makes NTCON take FINAL_CLOSE instead, so the creator can never receive
     * its restoration acknowledgement. */
    root->frontend_terminal_retire=FALSE;
    service_signal_frontend_states(service);
    WakeAllConditionVariable(&service->frontend_changed);
}
