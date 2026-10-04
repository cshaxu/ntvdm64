/* NTSRV-private frontend registry; project adaptation,
 * not an original OpenNT mirror. Physical separation only: existing function
 * bodies, state authority and lock/resource contracts are preserved. */
#include <service_internal.h>
#include "common/protocol/frontend_protocol.h"
#include "common/system_root.h"


static void service_console_return_ack(OPENNT_BASE_CONNECTION *root);
static VOID CALLBACK service_frontend_exited(PVOID context,BOOLEAN fired);
static DWORD service_attach_frontend(OPENNT_BASE_CONNECTION *connection,
    HANDLE worker,HANDLE pipe,HANDLE ready);

/* Association lookup is service-private. A pipe lease never creates a task. */
static OPENNT_FRONTEND_ROUTE *service_worker_io_route(OPENNT_BASE_CONNECTION *connection,DWORD pid)
{
    LIST_ENTRY *link;
    for(link=connection->service->frontend_routes.Flink;
        link!=&connection->service->frontend_routes;link=link->Flink) {
        OPENNT_FRONTEND_ROUTE *route=CONTAINING_RECORD(link,OPENNT_FRONTEND_ROUTE,link);
        if(GetProcessId(route->worker)==pid)return route;
    }
    return NULL;
}
/* Caller holds the service lock and has admitted an execution/resume phase.
 * A transport acquisition never authorizes another worker's release. Native
 * parents cannot intercept an ordinary Windows child's CreateProcess, so the
 * admitted child/resume is their broker notification to finish local I/O. */
DWORD service_authorize_worker_io(OPENNT_BASE_CONNECTION *worker,DWORD pid)
{
    OPENNT_FRONTEND_ROUTE *route=service_worker_io_route(worker,pid);
    LIST_ENTRY *link;
    if(!route)return ERROR_NOT_FOUND;
    if(!route->root || route->root->frontend_closing)
        return ERROR_PIPE_NOT_CONNECTED;
    if(route->io_releasing)return ERROR_BUSY;
    if(route->root->frontend_io_route && route->root->frontend_io_route!=route) {
        DWORD owner=GetProcessId(route->root->frontend_io_route->worker);
        for(link=worker->service->connections.Flink;
            link!=&worker->service->connections;link=link->Flink) {
            OPENNT_BASE_CONNECTION *current=CONTAINING_RECORD(link,OPENNT_BASE_CONNECTION,service_link);
            if(GetProcessId(current->process.ProcessHandle)==owner && current->native_worker &&
                current->worker_io_release && !SetEvent(current->worker_io_release))
                return GetLastError();
        }
    }
    route->io_requested=TRUE;
    WakeAllConditionVariable(&worker->service->frontend_changed);
    return ERROR_SUCCESS;
}
static void service_finish_io_release(OPENNT_FRONTEND_ROUTE *route)
{
    if(!route->io_worker_closed || !route->io_frontend_closed)return;
    route->root->frontend_io_route=NULL;
    route->io_requested=route->io_releasing=route->delivered=FALSE;
    WakeAllConditionVariable(&route->root->service->frontend_changed);
    service_signal_frontend_states(route->root->service);
}
DWORD OpenNtBaseServiceWorkerIoTransition(OPENNT_BASE_CONNECTION *connection,DWORD pid,
    DWORD generation,DWORD action)
{
    OPENNT_FRONTEND_ROUTE *route;
    OPENNT_BASE_CONNECTION *root;
    ULONGLONG deadline=GetTickCount64()+FRONTEND_STARTUP_DEADLINE_MS;
    DWORD error=ERROR_ACCESS_DENIED;
    if(!connection)return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&connection->service->lock);
    if(!OpenNtBaseServicePeer(connection,pid,generation) ||
        (!connection->process.fVDM && !connection->native_worker))goto done;
    /* Window-only workers have no character route. Their normal startup
     * skips this operation; an erroneous request is not an admission. */
    if(connection->wow)goto done;
    for(;;) {
        ULONGLONG now;
        route=service_worker_io_route(connection,pid);
        if(!route || !(root=route->root) || root->frontend_closing ||
            WaitForSingleObject(root->process.ProcessHandle,0)!=WAIT_TIMEOUT) {
            error=ERROR_PIPE_NOT_CONNECTED;break;
        }
        if(action==WORKER_IO_ACQUIRE) {
            if(!route->io_requested){error=ERROR_ACCESS_DENIED;break;}
            if(root->frontend_io_route==route && !route->io_releasing) {
                error=ERROR_SUCCESS;break;
            }
            if(!root->frontend_io_route) {
                route->io_releasing=route->io_worker_closed=route->io_frontend_closed=FALSE;
                root->frontend_io_route=route;
                error=service_refresh_frontend_work(root);
                if(error){root->frontend_io_route=NULL;route->io_requested=FALSE;break;}
                error=ERROR_SUCCESS;break;
            }
            /* An admitted phase waits for the old owner's acknowledged
             * release. It cannot evict that owner by asking for a pipe. */
        } else {
            if(action==WORKER_IO_RELEASED &&
                route->io_worker_closed && route->io_frontend_closed) {
                error=ERROR_SUCCESS;break;
            }
            if(root->frontend_io_route!=route) {
                error=ERROR_INVALID_STATE;break;
            }
            if(action==WORKER_IO_RELEASE_BEGIN) {
                route->io_releasing=TRUE;
                error=service_refresh_frontend_work(root);break;
            }
            if(action!=WORKER_IO_RELEASED || !route->io_releasing) {
                error=ERROR_INVALID_PARAMETER;break;
            }
            route->io_worker_closed=TRUE;
            service_finish_io_release(route);
            if(root->frontend_io_route!=route){error=ERROR_SUCCESS;break;}
        }
        now=GetTickCount64();
        if(now>=deadline){error=ERROR_TIMEOUT;break;}
        if(!SleepConditionVariableCS(&connection->service->frontend_changed,
            &connection->service->lock,(DWORD)(deadline-now))) {
            error=GetLastError();break;
        }
        /* Re-resolve after every wake: rundown may have removed the route. */
    }
done:
    LeaveCriticalSection(&connection->service->lock);
    return error;
}
DWORD OpenNtBaseServiceFrontendIoDisconnected(OPENNT_BASE_CONNECTION *root,DWORD pid,
    DWORD generation)
{
    OPENNT_FRONTEND_ROUTE *route;
    DWORD error=ERROR_ACCESS_DENIED;
    if(!root)return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&root->service->lock);
    if(!OpenNtBaseServicePeer(root,pid,generation) || !root->frontend_capability)goto done;
    route=root->frontend_io_route;
    if(!route){error=ERROR_INVALID_STATE;goto done;}
    /* Unexpected EOF cannot be treated as a successful ownership release. */
    if(!route->io_releasing){error=ERROR_PIPE_NOT_CONNECTED;goto done;}
    route->io_frontend_closed=TRUE;
    service_finish_io_release(route);
    error=ERROR_SUCCESS;
done:
    LeaveCriticalSection(&root->service->lock);
    return error;
}

/* The event is a projection, not a queue. All writers and consumers hold the
 * existing service lock. Decided joins wait on frontend_changed for a lease;
 * they are not work for the frontend's authentication pump. */
DWORD service_refresh_frontend_work(OPENNT_BASE_CONNECTION *root)
{
    LIST_ENTRY *link;
    BOOL pending;
    DWORD error;
    if(!root || !root->frontend_capability)return ERROR_SUCCESS;
    pending=(root->frontend_join_caller && !root->frontend_join_decision) ||
        (root->frontend_io_route && root->frontend_io_route->io_releasing &&
         !root->frontend_io_route->io_frontend_closed);
    for(link=root->service->frontend_routes.Flink;
        !pending && link!=&root->service->frontend_routes;link=link->Flink) {
        OPENNT_FRONTEND_ROUTE *route=CONTAINING_RECORD(link,OPENNT_FRONTEND_ROUTE,link);
        pending=route==root->frontend_io_route && route->io_requested && !route->pipe &&
            !route->delivered && WaitForSingleObject(route->worker,0)==WAIT_TIMEOUT;
    }
    if(pending ? SetEvent(root->frontend_capability) : ResetEvent(root->frontend_capability))
        return ERROR_SUCCESS;
    error=GetLastError();
    /* A broken notification cannot authorize another successful acquisition.
     * Use the existing broker-owned failure/retirement path, not retries. */
    root->frontend_closing=TRUE;
    service_signal_frontend_states(root->service);
    WakeAllConditionVariable(&root->service->frontend_changed);
    return error;
}

void service_delete_console_context(OPENNT_BASE_CONSOLE_CONTEXT *context)
{
    RemoveEntryList(&context->link);
    CloseHandle(context->capability);
    HeapFree(GetProcessHeap(),0,context);
}

/* Called under the service lock. An execution Console can differ from the
 * visible root: original CheckDOS allocates a separate DosSessionId when it
 * cannot use the shared Console record. Only broker-authenticated contexts
 * establish that association; matching a PID/member count does not. */
BOOL service_root_console_matches(OPENNT_BASE_CONNECTION *root,HANDLE console)
{
    LIST_ENTRY *link;
    if(root->console==console)return TRUE;
    for(link=root->service->console_contexts.Flink;
        link!=&root->service->console_contexts;link=link->Flink) {
        OPENNT_BASE_CONSOLE_CONTEXT *context=CONTAINING_RECORD(link,
            OPENNT_BASE_CONSOLE_CONTEXT,link);
        if(context->root==root && context->console==console)return TRUE;
    }
    return FALSE;
}

void service_release_console_identities(OPENNT_BASE_CONNECTION *connection)
{
    DWORD index;
    if(connection->console_member_processes) {
        for(index=0;index<connection->console_member_count;++index)
            if(connection->console_member_processes[index])
                CloseHandle(connection->console_member_processes[index]);
        HeapFree(GetProcessHeap(),0,connection->console_member_processes);
    }
    if(connection->execution_console_processes) {
        for(index=0;index<connection->execution_console_member_count;++index)
            if(connection->execution_console_processes[index])
                CloseHandle(connection->execution_console_processes[index]);
        HeapFree(GetProcessHeap(),0,connection->execution_console_processes);
    }
    if(connection->console_members)HeapFree(GetProcessHeap(),0,connection->console_members);
    if(connection->execution_console_members)
        HeapFree(GetProcessHeap(),0,connection->execution_console_members);
}

DWORD service_copy_execution_console_members(OPENNT_BASE_CONNECTION *destination,
    const OPENNT_BASE_CONNECTION *source)
{
    DWORD *copy=NULL,index;
    HANDLE *processes=NULL;
    if (!destination || !source) return ERROR_INVALID_PARAMETER;
    if (source->console_member_count) {
        copy=HeapAlloc(GetProcessHeap(),0,source->console_member_count*sizeof(*copy));
        if (!copy) return ERROR_NOT_ENOUGH_MEMORY;
        processes=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,
            source->console_member_count*sizeof(*processes));
        if(!processes){HeapFree(GetProcessHeap(),0,copy);return ERROR_NOT_ENOUGH_MEMORY;}
        memcpy(copy,source->console_members,source->console_member_count*sizeof(*copy));
        for(index=0;index<source->console_member_count;++index) {
            if(source->console_member_processes[index] &&
                !DuplicateHandle(GetCurrentProcess(),source->console_member_processes[index],
                    GetCurrentProcess(),&processes[index],0,FALSE,DUPLICATE_SAME_ACCESS)) {
                DWORD error=GetLastError(),undo;
                for(undo=0;undo<index;++undo)if(processes[undo])CloseHandle(processes[undo]);
                HeapFree(GetProcessHeap(),0,processes);HeapFree(GetProcessHeap(),0,copy);
                return error;
            }
        }
    }
    if(destination->execution_console_processes) {
        for(index=0;index<destination->execution_console_member_count;++index)
            if(destination->execution_console_processes[index])
                CloseHandle(destination->execution_console_processes[index]);
        HeapFree(GetProcessHeap(),0,destination->execution_console_processes);
    }
    if (destination->execution_console_members)
        HeapFree(GetProcessHeap(),0,destination->execution_console_members);
    destination->execution_console_members=copy;
    destination->execution_console_processes=processes;
    destination->execution_console_member_count=source->console_member_count;
    return ERROR_SUCCESS;
}

void service_delete_frontend(OPENNT_FRONTEND_ROUTE *route)
{
    OPENNT_BASE_CONNECTION *root=route->root;
    if(root && root->frontend_io_route==route)root->frontend_io_route=NULL;
    RemoveEntryList(&route->link);
    if (route->pipe) CloseHandle(route->pipe);
    if (route->ready) CloseHandle(route->ready);
    CloseHandle(route->worker);
    HeapFree(GetProcessHeap(),0,route);
    /* Cleanup has no caller result; notification failure is handled by the
     * helper's explicit broker retirement, never hidden as successful work. */
    (void)service_refresh_frontend_work(root);
    if(root)WakeAllConditionVariable(&root->service->frontend_changed);
}

void service_clear_frontend(OPENNT_BASE_CONNECTION *connection)
{
    LIST_ENTRY *link=connection->service->frontend_routes.Flink;
    LIST_ENTRY *context_link=connection->service->console_contexts.Flink;
    LIST_ENTRY *caller_link;
    service_clear_frontend_channel(connection);
    for (caller_link=connection->service->connections.Flink;
         caller_link!=&connection->service->connections;caller_link=caller_link->Flink) {
        OPENNT_BASE_CONNECTION *caller=CONTAINING_RECORD(caller_link,OPENNT_BASE_CONNECTION,service_link);
        if (caller->frontend_channel_root==connection->process.SequenceNumber ||
            caller->channel_worker_generation==connection->process.SequenceNumber)
            service_clear_frontend_channel(caller);
    }
    /* Do not clear native_root here. The worker's authenticated root process
     * is its Console-session lifetime; only worker rundown releases that
     * association, so a new root cannot adopt the old hidden Console. */
    while (context_link!=&connection->service->console_contexts) {
        OPENNT_BASE_CONSOLE_CONTEXT *context=CONTAINING_RECORD(context_link,
            OPENNT_BASE_CONSOLE_CONTEXT,link);
        context_link=context_link->Flink;
        if (context->root==connection) service_delete_console_context(context);
    }
    while (link!=&connection->service->frontend_routes) {
        OPENNT_FRONTEND_ROUTE *route=CONTAINING_RECORD(link,OPENNT_FRONTEND_ROUTE,link);
        link=link->Flink;
        if (route->root==connection || (!route->native_worker && !route->pipe && !route->delivered &&
            !route->io_worker_closed && !route->io_frontend_closed &&
            route->request==connection->process.SequenceNumber)) {
            if (route->delivered || route->native_worker) service_delete_frontend(route);
            else {
                OPENNT_BASE_CONNECTION *root=route->root;
                /* Retain only the selected worker identity until its exit:
                 * a waiter must observe cancellation, not await a new root. */
                route->root=NULL;
                if (route->pipe) { CloseHandle(route->pipe);route->pipe=NULL; }
                if (route->ready) { CloseHandle(route->ready);route->ready=NULL; }
                (void)service_refresh_frontend_work(root);
            }
        }
    }
    WakeAllConditionVariable(&connection->service->frontend_changed);
    service_signal_frontend_states(connection->service);
}

/* Called under the service lock on new-client admission. A startup canceled
 * before worker Connect has no worker watch. Keep its cancellation marker
 * while that exact process is live; reclaim it after exit, without a reaper
 * or PID-based ownership inference. */
void service_prune_cancelled_frontends(OPENNT_BASE_SERVICE *service)
{
    LIST_ENTRY *link=service->frontend_routes.Flink;
    while (link!=&service->frontend_routes) {
        OPENNT_FRONTEND_ROUTE *route=CONTAINING_RECORD(link,OPENNT_FRONTEND_ROUTE,link);
        link=link->Flink;
        if (!route->root && WaitForSingleObject(route->worker,0)==WAIT_OBJECT_0)
            service_delete_frontend(route);
    }
}


DWORD OpenNtBaseServiceRegisterFrontendRoot(OPENNT_BASE_CONNECTION *connection,
    DWORD pid,DWORD generation,HANDLE capability)
{
    SERVICE_COMPARE_HANDLES compare;
    SERVICE_QUERY_OBJECT query;
    EVENT_BASIC_INFORMATION event_info;
    struct { UNICODE_STRING name; WCHAR buffer[256]; } object_name;
    LIST_ENTRY *link;
    DWORD error=ERROR_SUCCESS;
    BOOL granted=FALSE;
    if (!connection) return ERROR_ACCESS_DENIED;
    compare=(SERVICE_COMPARE_HANDLES)GetProcAddress(GetModuleHandleW(L"kernelbase.dll"),
        "CompareObjectHandles");
    query=(SERVICE_QUERY_OBJECT)GetProcAddress(GetModuleHandleW(L"ntdll.dll"),"NtQueryObject");
    if (!compare || !query) return ERROR_CALL_NOT_IMPLEMENTED;
    EnterCriticalSection(&connection->service->lock);
    if (!OpenNtBaseServicePeer(connection,pid,generation) || connection->process.fVDM) {
        error=ERROR_ACCESS_DENIED;goto done;
    }
    if (connection->frontend_capability) { error=ERROR_ALREADY_EXISTS;goto done; }
    /* Only the broker's exact created process and inherited unnamed event
     * may register a product root. Possessing some other event is not a grant. */
    for(link=connection->service->connections.Flink;
        link!=&connection->service->connections;link=link->Flink) {
        OPENNT_BASE_CONNECTION *creator=CONTAINING_RECORD(link,OPENNT_BASE_CONNECTION,service_link);
        if(creator->frontend_starting && creator->frontend_start_process &&
            creator->frontend_start_capability &&
            compare(creator->frontend_start_process,connection->process.ProcessHandle) &&
            compare(creator->frontend_start_capability,capability)){granted=TRUE;break;}
    }
    if(!granted){error=ERROR_ACCESS_DENIED;goto done;}
    /* A discoverable named event would turn the lease into a public name.
     * Only the broker-created inherited unnamed event is this capability. */
    ZeroMemory(&object_name,sizeof(object_name));
    if (NtQueryEvent(capability,EventBasicInformation,&event_info,sizeof(event_info),NULL)<0 ||
        event_info.EventType!=NotificationEvent ||
        query(capability,1,&object_name,sizeof(object_name),NULL)<0 || object_name.name.Length) {
        error=ERROR_INVALID_PARAMETER;goto done;
    }
    for (link=connection->service->connections.Flink;
         link!=&connection->service->connections;link=link->Flink) {
        OPENNT_BASE_CONNECTION *root=CONTAINING_RECORD(link,OPENNT_BASE_CONNECTION,service_link);
        if (root->frontend_capability && compare(root->frontend_capability,capability)) {
            error=ERROR_ALREADY_EXISTS;goto done;
        }
    }
    if (!DuplicateHandle(GetCurrentProcess(),capability,GetCurrentProcess(),
        &connection->frontend_capability,SYNCHRONIZE|EVENT_MODIFY_STATE,FALSE,0)) error=GetLastError();
    else if (!(connection->frontend_state_changed=CreateEventW(NULL,FALSE,FALSE,NULL))) {
        error=GetLastError();
        CloseHandle(connection->frontend_capability);
        connection->frontend_capability=NULL;
    }
done:
    LeaveCriticalSection(&connection->service->lock);
    return error;
}


/* The HWND identifies a local Console only for serialized selection. The
 * root must independently confirm each joining PID before any capability
 * leaves the broker. Root registration itself is authenticated as before. */
DWORD OpenNtBaseServiceAcquireFrontendRoot(OPENNT_BASE_CONNECTION *caller,DWORD pid,
    DWORD generation,uint64_t console_window,DWORD *create_root,HANDLE *process,
    HANDLE *capability,HANDLE *retire,HANDLE *restored)
{
    OPENNT_BASE_SERVICE *service;
    ULONGLONG deadline=GetTickCount64()+10000;
    DWORD error=ERROR_ACCESS_DENIED;
    if(!caller || !console_window || !create_root || !process || !capability || !retire || !restored)
        return ERROR_INVALID_PARAMETER;
    *create_root=0;*process=*capability=*retire=*restored=NULL;
    service=caller->service;
    EnterCriticalSection(&service->lock);
    for(;;) {
        OPENNT_BASE_CONNECTION *root=NULL,*reservation=NULL;
        LIST_ENTRY *link;
        DWORD remaining;
        if(!OpenNtBaseServicePeer(caller,pid,generation) || caller->process.fVDM ||
            caller->native_worker || WaitForSingleObject(caller->process.ProcessHandle,0)!=WAIT_TIMEOUT)
            {error=ERROR_ACCESS_DENIED;break;}
        for(link=service->connections.Flink;link!=&service->connections;link=link->Flink) {
            OPENNT_BASE_CONNECTION *item=CONTAINING_RECORD(link,OPENNT_BASE_CONNECTION,service_link);
            if(item->frontend_console_window==console_window && item->frontend_capability &&
                !item->frontend_closing &&
                WaitForSingleObject(item->process.ProcessHandle,0)==WAIT_TIMEOUT) {
                if(root){error=ERROR_INVALID_STATE;goto done;}
                root=item;
            }
            if(item->frontend_reserved_window==console_window)reservation=item;
        }
        if(root && root->frontend_retire && root->frontend_restored) {
            if(caller->retained_frontend_root &&
                caller->retained_frontend_root!=root->process.SequenceNumber)
                {error=ERROR_ACCESS_DENIED;break;}
            if(!root->frontend_join_caller) {
                if(++service->next_frontend_join==0)++service->next_frontend_join;
                root->frontend_join_nonce=service->next_frontend_join;
                root->frontend_join_pid=pid;
                root->frontend_join_decision=0;
                root->frontend_join_caller=caller;
                error=service_refresh_frontend_work(root);
                if(error)break;
            }
            if(root->frontend_join_caller==caller) {
                if(root->frontend_join_decision<0){error=ERROR_ACCESS_DENIED;break;}
                if(root->frontend_join_decision>0 && root->frontend_idle) {
                    if(!DuplicateHandle(GetCurrentProcess(),root->process.ProcessHandle,
                        GetCurrentProcess(),process,PROCESS_QUERY_LIMITED_INFORMATION|SYNCHRONIZE,FALSE,0) ||
                       !DuplicateHandle(GetCurrentProcess(),root->frontend_capability,
                        GetCurrentProcess(),capability,SYNCHRONIZE,FALSE,0) ||
                       !DuplicateHandle(GetCurrentProcess(),root->frontend_retire,
                        GetCurrentProcess(),retire,EVENT_MODIFY_STATE,FALSE,0) ||
                       !DuplicateHandle(GetCurrentProcess(),root->frontend_restored,
                        GetCurrentProcess(),restored,SYNCHRONIZE,FALSE,0)) {
                        error=GetLastError();break;
                    }
                    if(!ResetEvent(root->frontend_retire) || !ResetEvent(root->frontend_restored))
                        {error=GetLastError();break;}
                    root->frontend_idle=FALSE;
                    root->frontend_admission_deadline=service_root_has_worker(root) ? 0 :
                        GetTickCount64()+FRONTEND_STARTUP_DEADLINE_MS;
                    root->frontend_creator_generation=generation;
                    caller->retained_frontend_root=root->process.SequenceNumber;
                    /* Match RetainFrontendRoot: the authenticated reusable
                     * root also establishes the caller's Console identity. */
                    if(!caller->console)caller->console=root->console;
                    error=ERROR_SUCCESS;break;
                }
            }
        } else if(!root && !reservation) {
            caller->frontend_reserved_window=console_window;
            *create_root=1;error=ERROR_SUCCESS;break;
        }
        remaining=deadline>GetTickCount64() ?
            (DWORD)(deadline-GetTickCount64()) : 0;
        if(!remaining){error=ERROR_TIMEOUT;break;}
        if(!SleepConditionVariableCS(&service->frontend_changed,&service->lock,remaining) &&
            GetLastError()!=ERROR_TIMEOUT){error=GetLastError();break;}
    }
done:
    /* A join belongs to this RPC call, never to an abandoned launcher. */
    {
        LIST_ENTRY *link;
        for(link=service->connections.Flink;link!=&service->connections;link=link->Flink) {
            OPENNT_BASE_CONNECTION *item=CONTAINING_RECORD(link,OPENNT_BASE_CONNECTION,service_link);
            if(item->frontend_join_caller==caller){
                item->frontend_join_caller=NULL;item->frontend_join_nonce=0;
                item->frontend_join_pid=0;item->frontend_join_decision=0;
                {
                    DWORD notification_error=service_refresh_frontend_work(item);
                    if(!error)error=notification_error;
                }
            }
        }
        WakeAllConditionVariable(&service->frontend_changed);
    }
    LeaveCriticalSection(&service->lock);
    if(error){
        if(*process)CloseHandle(*process);if(*capability)CloseHandle(*capability);
        if(*retire)CloseHandle(*retire);if(*restored)CloseHandle(*restored);
        *process=*capability=*retire=*restored=NULL;*create_root=0;
    }
    return error;
}


/* Reuses the verified restricted-inheritance bootstrap, now entirely inside
 * the broker. No caller-selected image, command, PID or Console HANDLE. */
DWORD broker_frontend_admit(OPENNT_BASE_CONNECTION *caller,DWORD pid,DWORD generation,
    HANDLE process,HANDLE capability,HANDLE retire,HANDLE restored,HANDLE startup_result)
{
    DWORD error=ERROR_ACCESS_DENIED;
    if(!caller || !process || !capability || !retire || !restored)
        return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&caller->service->lock);
    if(OpenNtBaseServicePeer(caller,pid,generation)) {
        if(caller->frontend_start_process)error=ERROR_BUSY;
        else if(WaitForSingleObject(process,0)!=WAIT_TIMEOUT)error=ERROR_PROCESS_ABORTED;
        else {
            caller->frontend_start_process=process;
            caller->frontend_start_capability=capability;
            caller->frontend_start_retire=retire;
            caller->frontend_start_restored=restored;
            caller->frontend_start_result=startup_result;
            caller->frontend_start_status=0;
            caller->frontend_start_reported=FALSE;
            caller->frontend_starting=TRUE;
            error=ERROR_SUCCESS;
        }
    }
    LeaveCriticalSection(&caller->service->lock);
    return error;
}

void broker_frontend_clear_admission(OPENNT_BASE_CONNECTION *caller)
{
    EnterCriticalSection(&caller->service->lock);
    caller->frontend_start_process=NULL;
    caller->frontend_start_capability=NULL;
    caller->frontend_start_retire=NULL;
    caller->frontend_start_restored=NULL;
    caller->frontend_start_result=NULL;
    caller->frontend_start_status=0;
    caller->frontend_start_reported=FALSE;
    caller->frontend_starting=FALSE;
    LeaveCriticalSection(&caller->service->lock);
}


DWORD OpenNtBaseServiceFrontendStartupResult(OPENNT_BASE_CONNECTION *root,DWORD pid,
    DWORD generation,HANDLE capability,DWORD status)
{
    LIST_ENTRY *link;
    DWORD error=ERROR_ACCESS_DENIED;
    SERVICE_COMPARE_HANDLES compare=(SERVICE_COMPARE_HANDLES)GetProcAddress(
        GetModuleHandleW(L"kernelbase.dll"),"CompareObjectHandles");
    if(!root || !capability)return ERROR_INVALID_PARAMETER;
    if(!compare)return ERROR_CALL_NOT_IMPLEMENTED;
    EnterCriticalSection(&root->service->lock);
    if(!OpenNtBaseServicePeer(root,pid,generation))goto done;
    for(link=root->service->connections.Flink;link!=&root->service->connections;link=link->Flink) {
        OPENNT_BASE_CONNECTION *creator=CONTAINING_RECORD(link,OPENNT_BASE_CONNECTION,service_link);
        if(!creator->frontend_starting || !creator->frontend_start_process ||
            !creator->frontend_start_capability || !creator->frontend_start_result ||
            !compare(creator->frontend_start_process,root->process.ProcessHandle) ||
            !compare(creator->frontend_start_capability,capability))continue;
        if(creator->frontend_start_reported){error=ERROR_ALREADY_EXISTS;break;}
        /* Failure may precede root registration; success must prove the
         * registered Console lease, not merely possession of an event. */
        if(!status && (!root->frontend_capability || !root->frontend_console_window ||
            !compare(root->frontend_capability,capability)))break;
        creator->frontend_start_status=status;
        if(!SetEvent(creator->frontend_start_result)){error=GetLastError();break;}
        creator->frontend_start_reported=TRUE;
        error=ERROR_SUCCESS;break;
    }
done:
    LeaveCriticalSection(&root->service->lock);
    return error;
}


DWORD OpenNtBaseServiceStartFrontend(OPENNT_BASE_CONNECTION *caller,DWORD pid,
    DWORD generation,uint64_t window,BOOL borrowed,HANDLE *root,HANDLE *capability,HANDLE *restored)
{
    WCHAR image[MAX_PATH],command[1024];
    HANDLE creator=NULL;
    HANDLE notification=NULL,retire=NULL,ack=NULL,event=NULL,timer=NULL,verified=NULL;
    HANDLE inherited[4],unused_retire=NULL;
    SECURITY_ATTRIBUTES security={sizeof(security),NULL,TRUE};
    STARTUPINFOEXW startup={0};PROCESS_INFORMATION process={0};
    LARGE_INTEGER due;SIZE_T bytes=0;
    DWORD error,create=0,root_generation=0;
    BOOL attributes=FALSE,accepted=FALSE;
    if(!caller || !window || borrowed>1 || !root || !capability || !restored)
        return ERROR_INVALID_PARAMETER;
    *root=*capability=*restored=NULL;
    EnterCriticalSection(&caller->service->lock);
    if(!OpenNtBaseServicePeer(caller,pid,generation) || caller->process.fVDM ||
        caller->native_worker || caller->frontend_capability)error=ERROR_ACCESS_DENIED;
    else if(caller->frontend_starting)error=ERROR_BUSY;
    else {
        caller->frontend_starting=TRUE;
        caller->frontend_return_pending=caller->frontend_return_complete=FALSE;
        error=ERROR_SUCCESS;
    }
    LeaveCriticalSection(&caller->service->lock);
    if(error)return error;
    error=OpenNtBaseServiceAcquireFrontendRoot(caller,pid,generation,window,&create,
        root,capability,&unused_retire,restored);
    if(unused_retire)CloseHandle(unused_retire);
    if(error || !create)goto done;
    error=OpenNtBaseServiceRetainPeer(caller,pid,generation,&verified);
    if(error)goto done;
    if(!DuplicateHandle(GetCurrentProcess(),verified,GetCurrentProcess(),&creator,
        PROCESS_QUERY_LIMITED_INFORMATION|SYNCHRONIZE,TRUE,0)){error=GetLastError();goto done;}
    CloseHandle(verified);verified=NULL;
    error=common_product_path_w(L"ntcon.exe",image,ARRAYSIZE(image));
    if(error)goto done;
    notification=CreateEventW(&security,TRUE,FALSE,NULL);
    retire=CreateEventW(&security,TRUE,FALSE,NULL);
    ack=CreateEventW(&security,TRUE,FALSE,NULL);
    event=CreateEventW(NULL,TRUE,FALSE,NULL);
    if(!notification || !retire || !ack || !event){error=GetLastError();goto done;}
    InitializeProcThreadAttributeList(NULL,1,0,&bytes);
    startup.lpAttributeList=HeapAlloc(GetProcessHeap(),0,bytes);
    if(!startup.lpAttributeList){error=ERROR_NOT_ENOUGH_MEMORY;goto done;}
    if(!InitializeProcThreadAttributeList(startup.lpAttributeList,1,0,&bytes))
        {error=GetLastError();goto done;}
    attributes=TRUE;
    inherited[0]=creator;inherited[1]=notification;
    inherited[2]=retire;inherited[3]=ack;
    if(!UpdateProcThreadAttribute(startup.lpAttributeList,0,PROC_THREAD_ATTRIBUTE_HANDLE_LIST,
        inherited,sizeof(inherited),NULL,NULL)){error=GetLastError();goto done;}
    startup.StartupInfo.cb=sizeof(startup);
    if(swprintf_s(command,ARRAYSIZE(command),L"\"%ls\" --session %Ix %Ix %Ix %Ix %u %I64x",
        image,(UINT_PTR)creator,(UINT_PTR)notification,(UINT_PTR)retire,
        (UINT_PTR)ack,borrowed!=FALSE,window)<0){error=ERROR_FILENAME_EXCED_RANGE;goto done;}
    if(!CreateProcessW(image,command,NULL,NULL,TRUE,
        CREATE_SUSPENDED|EXTENDED_STARTUPINFO_PRESENT|DETACHED_PROCESS,NULL,NULL,
        &startup.StartupInfo,&process)){error=GetLastError();goto done;}
    error=broker_frontend_admit(caller,pid,generation,process.hProcess,notification,retire,ack,event);
    if(error)goto done;
    if(ResumeThread(process.hThread)==(DWORD)-1){error=GetLastError();goto done;}
    CloseHandle(process.hThread);process.hThread=NULL;
    timer=CreateWaitableTimerW(NULL,TRUE,NULL);
    if(!timer){error=GetLastError();goto done;}
    due.QuadPart=-10LL*1000*10000;
    if(!SetWaitableTimer(timer,&due,0,NULL,NULL,FALSE)){error=GetLastError();goto done;}
    {
        HANDLE waits[4]={event,process.hProcess,creator,timer};
        DWORD wait=WaitForMultipleObjects(ARRAYSIZE(waits),waits,FALSE,INFINITE);
        if(wait==WAIT_OBJECT_0) {
            EnterCriticalSection(&caller->service->lock);
            error=caller->frontend_start_reported ? caller->frontend_start_status : ERROR_INVALID_STATE;
            LeaveCriticalSection(&caller->service->lock);
        }else if(wait==WAIT_OBJECT_0+3)error=ERROR_TIMEOUT;
        else if(wait==WAIT_FAILED)error=GetLastError();
        else error=ERROR_PROCESS_ABORTED;
    }
    if(error && WaitForSingleObject(process.hProcess,0)==WAIT_OBJECT_0) {
        DWORD status;
        if(GetExitCodeProcess(process.hProcess,&status) && status)error=status;
    }
    if(error)goto done;
    error=OpenNtBaseServiceRetainFrontendRoot(caller,pid,generation,notification,&verified,&root_generation);
    if(error)goto done;
    if(GetProcessId(verified)!=process.dwProcessId){error=ERROR_ACCESS_DENIED;goto done;}
    if(!DuplicateHandle(GetCurrentProcess(),notification,GetCurrentProcess(),capability,
        SYNCHRONIZE,FALSE,0) || !DuplicateHandle(GetCurrentProcess(),ack,GetCurrentProcess(),restored,
        SYNCHRONIZE,FALSE,0)){error=GetLastError();goto done;}
    *root=verified;verified=NULL;accepted=TRUE;
done:
    broker_frontend_clear_admission(caller);
    if(!accepted && process.hProcess) {
        /* No task can be submitted until this RPC returns successfully. */
        TerminateProcess(process.hProcess,error);
        WaitForSingleObject(process.hProcess,INFINITE);
    }
    if(error){
        (void)OpenNtBaseServiceCancelFrontendRootReservation(caller,pid,generation);
        if(*root)CloseHandle(*root);if(*capability)CloseHandle(*capability);
        if(*restored)CloseHandle(*restored);
        *root=*capability=*restored=NULL;
    }
    if(process.hThread)CloseHandle(process.hThread);
    if(process.hProcess)CloseHandle(process.hProcess);
    if(verified)CloseHandle(verified);if(creator)CloseHandle(creator);
    if(notification)CloseHandle(notification);if(retire)CloseHandle(retire);if(ack)CloseHandle(ack);
    if(timer)CloseHandle(timer);if(event)CloseHandle(event);
    if(attributes)DeleteProcThreadAttributeList(startup.lpAttributeList);
    if(startup.lpAttributeList)HeapFree(GetProcessHeap(),0,startup.lpAttributeList);
    return error;
}


DWORD OpenNtBaseServiceReturnFrontendConsole(OPENNT_BASE_CONNECTION *caller,DWORD pid,DWORD generation)
{
    DWORD error=ERROR_ACCESS_DENIED;LIST_ENTRY *link;
    if(!caller)return error;
    EnterCriticalSection(&caller->service->lock);
    if(!OpenNtBaseServicePeer(caller,pid,generation))goto done;
    /* An independent DOS/native completion can retire its otherwise unused root
     * before the launcher reaches this call. Its restoration acknowledgement
     * belongs to the launcher lease and survives frontend rundown. */
    if(caller->frontend_return_complete){error=ERROR_SUCCESS;goto done;}
    for(link=caller->service->connections.Flink;link!=&caller->service->connections;link=link->Flink) {
        OPENNT_BASE_CONNECTION *root=CONTAINING_RECORD(link,OPENNT_BASE_CONNECTION,service_link);
        if(root->process.SequenceNumber!=caller->retained_frontend_root)continue;
        if(root->frontend_creator_generation!=generation || !root->frontend_retire)break;
        if(SetEvent(root->frontend_retire)) {
            caller->frontend_return_pending=TRUE;
            error=ERROR_SUCCESS;
        }else error=GetLastError();
        break;
    }
done:
    LeaveCriticalSection(&caller->service->lock);return error;
}

/* A restoration belongs to the admitted launcher lease, not to the lifetime
 * of a reusable root event. Preserve its acknowledgement if another launcher
 * subsequently reacquires the root and resets that event. service lock held. */
static void service_console_return_ack(OPENNT_BASE_CONNECTION *root)
{
    LIST_ENTRY *link;
    for(link=root->service->connections.Flink;link!=&root->service->connections;link=link->Flink) {
        OPENNT_BASE_CONNECTION *caller=CONTAINING_RECORD(link,OPENNT_BASE_CONNECTION,service_link);
        if(caller->retained_frontend_root==root->process.SequenceNumber &&
            caller->process.SequenceNumber==root->frontend_creator_generation &&
            caller->frontend_return_pending)caller->frontend_return_complete=TRUE;
    }
    WakeAllConditionVariable(&root->service->frontend_changed);
}

DWORD OpenNtBaseServiceWaitFrontendConsoleRestored(OPENNT_BASE_CONNECTION *caller,DWORD pid,DWORD generation)
{
    DWORD error=ERROR_ACCESS_DENIED;
    OPENNT_BASE_SERVICE *service;
    if(!caller)return error;
    service=caller->service;
    EnterCriticalSection(&service->lock);
    for(;;) {
        LIST_ENTRY *link;OPENNT_BASE_CONNECTION *root=NULL;
        if(!OpenNtBaseServicePeer(caller,pid,generation) || !caller->frontend_return_pending)break;
        if(caller->frontend_return_complete){error=ERROR_SUCCESS;break;}
        for(link=service->connections.Flink;link!=&service->connections;link=link->Flink) {
            OPENNT_BASE_CONNECTION *item=CONTAINING_RECORD(link,OPENNT_BASE_CONNECTION,service_link);
            if(item->process.SequenceNumber==caller->retained_frontend_root){root=item;break;}
        }
        if(!root || WaitForSingleObject(root->process.ProcessHandle,0)!=WAIT_TIMEOUT)
            {error=ERROR_PIPE_NOT_CONNECTED;break;}
        if(!SleepConditionVariableCS(&service->frontend_changed,&service->lock,INFINITE))
            {error=GetLastError();break;}
    }
    LeaveCriticalSection(&service->lock);return error;
}

DWORD OpenNtBaseServiceFrontendConsoleRestored(OPENNT_BASE_CONNECTION *root,DWORD pid,DWORD generation)
{
    DWORD error=ERROR_ACCESS_DENIED;
    if(!root)return error;
    EnterCriticalSection(&root->service->lock);
    if(OpenNtBaseServicePeer(root,pid,generation) && root->frontend_capability && root->frontend_restored) {
        error=SetEvent(root->frontend_restored) ? ERROR_SUCCESS : GetLastError();
        if(!error)service_console_return_ack(root);
    }
    LeaveCriticalSection(&root->service->lock);return error;
}


DWORD OpenNtBaseServiceCancelFrontendRootReservation(OPENNT_BASE_CONNECTION *caller,
    DWORD pid,DWORD generation)
{
    DWORD error=ERROR_ACCESS_DENIED;
    if(!caller)return error;
    EnterCriticalSection(&caller->service->lock);
    if(OpenNtBaseServicePeer(caller,pid,generation) && caller->frontend_reserved_window) {
        caller->frontend_reserved_window=0;
        WakeAllConditionVariable(&caller->service->frontend_changed);
        error=ERROR_SUCCESS;
    }
    LeaveCriticalSection(&caller->service->lock);
    return error;
}


static VOID CALLBACK service_frontend_exited(PVOID context,BOOLEAN fired)
{
    OPENNT_BASE_SERVICE *service=context;
    (void)fired;
    (void)SetEvent(service->frontend_lifetime_changed);
}


DWORD OpenNtBaseServiceRegisterFrontendLease(OPENNT_BASE_CONNECTION *root,DWORD pid,
    DWORD generation,uint64_t console_window,DWORD creator_pid,BOOL borrowed,
    HANDLE retire,HANDLE restored)
{
    OPENNT_BASE_CONNECTION *creator=NULL;
    LIST_ENTRY *link;
    DWORD error=ERROR_ACCESS_DENIED;
    EVENT_BASIC_INFORMATION info;
    SERVICE_COMPARE_HANDLES compare=(SERVICE_COMPARE_HANDLES)GetProcAddress(
        GetModuleHandleW(L"kernelbase.dll"),"CompareObjectHandles");
    if(!root || !console_window || !creator_pid || borrowed>1)return ERROR_INVALID_PARAMETER;
    if(!compare)return ERROR_CALL_NOT_IMPLEMENTED;
    if(NtQueryEvent(retire,EventBasicInformation,&info,sizeof(info),NULL)<0 ||
        info.EventType!=NotificationEvent ||
        NtQueryEvent(restored,EventBasicInformation,&info,sizeof(info),NULL)<0 ||
        info.EventType!=NotificationEvent)return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&root->service->lock);
    if(!OpenNtBaseServicePeer(root,pid,generation) || !root->frontend_capability ||
        root->frontend_console_window || root->process.fVDM)goto done;
    for(link=root->service->connections.Flink;link!=&root->service->connections;link=link->Flink) {
        OPENNT_BASE_CONNECTION *item=CONTAINING_RECORD(link,OPENNT_BASE_CONNECTION,service_link);
        if(item!=root && item->frontend_console_window==console_window &&
            item->frontend_capability && !item->frontend_closing &&
            WaitForSingleObject(item->process.ProcessHandle,0)==WAIT_TIMEOUT)
            {error=ERROR_ALREADY_EXISTS;goto done;}
        if(item->frontend_reserved_window==console_window &&
            (DWORD)item->process.ClientId.UniqueProcess==creator_pid &&
            WaitForSingleObject(item->process.ProcessHandle,0)==WAIT_TIMEOUT)creator=item;
    }
    if(!creator)goto done;
    /* A production bootstrap is accepted only from the exact suspended
     * process created by NTSRV for this authenticated caller's reservation.
     * A raw PID or possession of an arbitrary event does not grant this. */
    if(!creator->frontend_starting || !creator->frontend_start_process ||
        !creator->frontend_start_retire || !creator->frontend_start_restored ||
        !compare(creator->frontend_start_process,root->process.ProcessHandle) ||
        !compare(creator->frontend_start_retire,retire) ||
        !compare(creator->frontend_start_restored,restored))goto done;
    if(!DuplicateHandle(GetCurrentProcess(),retire,GetCurrentProcess(),&root->frontend_retire,
            EVENT_MODIFY_STATE|SYNCHRONIZE,FALSE,0) ||
       !DuplicateHandle(GetCurrentProcess(),restored,GetCurrentProcess(),&root->frontend_restored,
            EVENT_MODIFY_STATE|SYNCHRONIZE,FALSE,0)){
        error=GetLastError();
        if(root->frontend_retire){CloseHandle(root->frontend_retire);root->frontend_retire=NULL;}
        goto done;
    }
    root->frontend_console_window=console_window;
    root->frontend_borrowed=borrowed;
    root->frontend_creator_generation=creator->process.SequenceNumber;
    root->frontend_admission_deadline=GetTickCount64()+FRONTEND_STARTUP_DEADLINE_MS;
    root->frontend_workerless_deadline=root->frontend_admission_deadline;
    creator->retained_frontend_root=generation;
    /* Registry removal closes ProcessHandle before context disposal. The
     * registered wait therefore owns a separate copy until joined below. */
    if(!DuplicateHandle(GetCurrentProcess(),root->process.ProcessHandle,GetCurrentProcess(),
        &root->frontend_exit_process,SYNCHRONIZE,FALSE,0) ||
       !RegisterWaitForSingleObject(&root->frontend_exit_watch,root->frontend_exit_process,
        service_frontend_exited,root->service,INFINITE,WT_EXECUTEONLYONCE)) {
        error=GetLastError();
        root->frontend_console_window=0;
        root->frontend_creator_generation=0;
        root->frontend_admission_deadline=0;
        creator->retained_frontend_root=0;
        CloseHandle(root->frontend_retire);root->frontend_retire=NULL;
        CloseHandle(root->frontend_restored);root->frontend_restored=NULL;
        if(root->frontend_exit_process){CloseHandle(root->frontend_exit_process);root->frontend_exit_process=NULL;}
        goto done;
    }
    creator->frontend_reserved_window=0;
    service_signal_frontend_states(root->service);
    WakeAllConditionVariable(&root->service->frontend_changed);
    error=ERROR_SUCCESS;
done:
    LeaveCriticalSection(&root->service->lock);
    return error;
}


DWORD OpenNtBaseServiceFrontendJoinCandidate(OPENNT_BASE_CONNECTION *root,DWORD pid,
    DWORD generation,DWORD *nonce,DWORD *candidate_pid)
{
    DWORD error=ERROR_NOT_FOUND;
    if(!root || !nonce || !candidate_pid)return ERROR_INVALID_PARAMETER;
    *nonce=*candidate_pid=0;
    EnterCriticalSection(&root->service->lock);
    if(!OpenNtBaseServicePeer(root,pid,generation) || !root->frontend_capability)
        error=ERROR_ACCESS_DENIED;
    else if(root->frontend_join_caller && !root->frontend_join_decision) {
        *nonce=root->frontend_join_nonce;*candidate_pid=root->frontend_join_pid;
        error=ERROR_SUCCESS;
    }
    LeaveCriticalSection(&root->service->lock);
    return error;
}

DWORD OpenNtBaseServiceFrontendJoinDecision(OPENNT_BASE_CONNECTION *root,DWORD pid,
    DWORD generation,DWORD nonce,BOOL same_console)
{
    DWORD error=ERROR_ACCESS_DENIED;
    if(!root || !nonce)return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&root->service->lock);
    if(OpenNtBaseServicePeer(root,pid,generation) && root->frontend_capability &&
        root->frontend_join_caller && root->frontend_join_nonce==nonce &&
        !root->frontend_join_decision) {
        root->frontend_join_decision=same_console ? 1 : -1;
        error=service_refresh_frontend_work(root);
        WakeAllConditionVariable(&root->service->frontend_changed);
    }
    LeaveCriticalSection(&root->service->lock);
    return error;
}

DWORD OpenNtBaseServiceFrontendLeaseReady(OPENNT_BASE_CONNECTION *root,DWORD pid,DWORD generation)
{
    DWORD error=ERROR_ACCESS_DENIED;
    if(!root)return error;
    EnterCriticalSection(&root->service->lock);
    if(OpenNtBaseServicePeer(root,pid,generation) && root->frontend_capability &&
        !root->frontend_closing && root->frontend_restored) {
        if(SetEvent(root->frontend_restored)) {
            service_console_return_ack(root);
            /* A leased root returns the visible Console, not the worker
             * channels. Keep delivered routes for resident worker reuse. */
            root->frontend_idle=TRUE;
            service_signal_frontend_states(root->service);
            WakeAllConditionVariable(&root->service->frontend_changed);
            error=ERROR_SUCCESS;
        }else error=GetLastError();
    }
    LeaveCriticalSection(&root->service->lock);
    return error;
}


DWORD OpenNtBaseServiceRetainFrontendRoot(OPENNT_BASE_CONNECTION *connection,
    DWORD pid,DWORD generation,HANDLE capability,HANDLE *process,DWORD *root_generation)
{
    SERVICE_COMPARE_HANDLES compare;
    LIST_ENTRY *link;
    DWORD error=ERROR_ACCESS_DENIED;
    if (!process || !root_generation) return ERROR_INVALID_PARAMETER;
    *process=NULL;*root_generation=0;
    if (!connection) return ERROR_ACCESS_DENIED;
    compare=(SERVICE_COMPARE_HANDLES)GetProcAddress(GetModuleHandleW(L"kernelbase.dll"),
        "CompareObjectHandles");
    if (!compare) return ERROR_CALL_NOT_IMPLEMENTED;
    EnterCriticalSection(&connection->service->lock);
    if (!OpenNtBaseServicePeer(connection,pid,generation)) goto done;
    for (link=connection->service->connections.Flink;
         link!=&connection->service->connections;link=link->Flink) {
        OPENNT_BASE_CONNECTION *root=CONTAINING_RECORD(link,OPENNT_BASE_CONNECTION,service_link);
        if (!root->frontend_capability || !compare(root->frontend_capability,capability)) continue;
        if (root->frontend_closing || WaitForSingleObject(root->process.ProcessHandle,0)!=WAIT_TIMEOUT) {
            error=ERROR_PIPE_NOT_CONNECTED;goto done;
        }
        if (connection->console && connection->console!=root->console &&
            !connection->process.fVDM && !connection->native_worker &&
            !connection->frontend_capability) {
            LIST_ENTRY *context_link;
            BOOL belongs_to_root=FALSE;
            for(context_link=connection->service->console_contexts.Flink;
                context_link!=&connection->service->console_contexts;
                context_link=context_link->Flink) {
                OPENNT_BASE_CONSOLE_CONTEXT *context=CONTAINING_RECORD(context_link,
                    OPENNT_BASE_CONSOLE_CONTEXT,link);
                if(context->root==root && context->console==connection->console)
                    {belongs_to_root=TRUE;break;}
            }
            if(!belongs_to_root) {error=ERROR_ACCESS_DENIED;goto done;}
        }
        if (!DuplicateHandle(GetCurrentProcess(),root->process.ProcessHandle,
            GetCurrentProcess(),process,PROCESS_QUERY_LIMITED_INFORMATION|SYNCHRONIZE,FALSE,0)) {
            error=GetLastError();goto done;
        }
        *root_generation=root->process.SequenceNumber;
        /* A launcher without a separately authenticated execution context
         * uses the registered root's visible Console. Workers and nested
         * launchers already bound to a hidden execution Console keep that
         * distinct identity. */
        if (!connection->process.fVDM && !connection->native_worker &&
            !connection->frontend_capability) {
            if(connection->retained_frontend_root &&
                connection->retained_frontend_root!=root->process.SequenceNumber) {
                CloseHandle(*process);*process=NULL;*root_generation=0;
                error=ERROR_ACCESS_DENIED;goto done;
            }
            connection->retained_frontend_root=root->process.SequenceNumber;
        }
        if (!connection->console && !connection->process.fVDM &&
            !connection->native_worker && !connection->frontend_capability)
            connection->console=root->console;
        error=ERROR_SUCCESS;
        break;
    }
done:
    LeaveCriticalSection(&connection->service->lock);
    return error;
}


DWORD OpenNtBaseServiceAcquireConsoleContext(OPENNT_BASE_CONNECTION *connection,
    DWORD pid,DWORD generation,HANDLE frontend,HANDLE *capability)
{
    OPENNT_BASE_SERVICE *service;
    OPENNT_BASE_CONNECTION *root=NULL;
    OPENNT_BASE_CONSOLE_CONTEXT *context=NULL;
    HANDLE root_process=NULL;
    DWORD root_generation=0,error;
    LIST_ENTRY *link;
    BOOL created=FALSE;
    if (!capability) return ERROR_INVALID_PARAMETER;
    *capability=NULL;
    if (!connection) return ERROR_ACCESS_DENIED;
    service=connection->service;
    error=OpenNtBaseServiceRetainFrontendRoot(connection,pid,generation,frontend,
        &root_process,&root_generation);
    if (error) return error;
    /* Capture real execution membership separately from frontend identity.
     * This runs before the helper changes its physical Console attachment. */
    if (connection->wow) error=ERROR_ACCESS_DENIED;
    else if (connection->process.fVDM && !connection->console) error=ERROR_NOT_READY;
    else error=service_bind_existing_console(connection);
    EnterCriticalSection(&service->lock);
    if (error) goto done;
    if (!OpenNtBaseServicePeer(connection,pid,generation)) { error=ERROR_ACCESS_DENIED;goto done; }
    for (link=service->connections.Flink;link!=&service->connections;link=link->Flink) {
        OPENNT_BASE_CONNECTION *candidate=CONTAINING_RECORD(link,OPENNT_BASE_CONNECTION,service_link);
        if (candidate->process.SequenceNumber==root_generation &&
            (DWORD)candidate->process.ClientId.UniqueProcess==GetProcessId(root_process) &&
            candidate->frontend_capability) { root=candidate;break; }
    }
    if (!root || root->frontend_closing || WaitForSingleObject(root_process,0)!=WAIT_TIMEOUT) {
        error=ERROR_PIPE_NOT_CONNECTED;goto done;
    }
    for (link=service->console_contexts.Flink;link!=&service->console_contexts;link=link->Flink) {
        OPENNT_BASE_CONSOLE_CONTEXT *candidate=CONTAINING_RECORD(link,OPENNT_BASE_CONSOLE_CONTEXT,link);
        if (candidate->root==root && candidate->console==connection->console) { context=candidate;break; }
    }
    if (!context) {
        context=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*context));
        if (!context) { error=ERROR_NOT_ENOUGH_MEMORY;goto done; }
        context->capability=CreateEventW(NULL,TRUE,FALSE,NULL);
        if (!context->capability) { error=GetLastError();HeapFree(GetProcessHeap(),0,context);goto done; }
        context->root=root;context->console=connection->console;
        InsertTailList(&service->console_contexts,&context->link);
        created=TRUE;
    }
    if (!DuplicateHandle(GetCurrentProcess(),context->capability,GetCurrentProcess(),
        capability,SYNCHRONIZE,FALSE,0)) {
        error=GetLastError();
        if (created) service_delete_console_context(context);
    }
done:
    LeaveCriticalSection(&service->lock);
    CloseHandle(root_process);
    return error;
}


DWORD OpenNtBaseServiceBindConsoleContext(OPENNT_BASE_CONNECTION *connection,
    DWORD pid,DWORD generation,HANDLE capability)
{
    SERVICE_COMPARE_HANDLES compare;
    LIST_ENTRY *link;
    DWORD error=ERROR_ACCESS_DENIED;
    if (!connection) return error;
    compare=(SERVICE_COMPARE_HANDLES)GetProcAddress(GetModuleHandleW(L"kernelbase.dll"),"CompareObjectHandles");
    if (!compare) return ERROR_CALL_NOT_IMPLEMENTED;
    EnterCriticalSection(&connection->service->lock);
    if (!OpenNtBaseServicePeer(connection,pid,generation) || connection->process.fVDM ||
        connection->wow || connection->task || connection->reservation ||
        connection->pending_creation || connection->parent_wait || connection->selected_native_generation) goto done;
    for (link=connection->service->console_contexts.Flink;
         link!=&connection->service->console_contexts;link=link->Flink) {
        OPENNT_BASE_CONSOLE_CONTEXT *context=CONTAINING_RECORD(link,OPENNT_BASE_CONSOLE_CONTEXT,link);
        if (!compare(context->capability,capability)) continue;
        if (context->root->frontend_closing ||
            WaitForSingleObject(context->root->process.ProcessHandle,0)!=WAIT_TIMEOUT) {
            error=ERROR_PIPE_NOT_CONNECTED;break;
        }
        if (connection->console && connection->console!=context->console) break;
        connection->console=context->console;
        error=ERROR_SUCCESS;break;
    }
done:
    LeaveCriticalSection(&connection->service->lock);
    return error;
}


/* Called under the service lock, with an original-command-selected worker.
 * The route belongs to the root, not to the command submitting descendant. */
static DWORD service_attach_frontend(OPENNT_BASE_CONNECTION *connection,
    HANDLE worker,HANDLE pipe,HANDLE ready)
{
    OPENNT_FRONTEND_ROUTE *route=connection->frontend_io_route;
    EVENT_BASIC_INFORMATION event_info;
    HANDLE local_pipe=NULL,local_ready=NULL;
    DWORD error,flags;
    if(connection->frontend_closing) return ERROR_PIPE_NOT_CONNECTED;
    if(!route || route->root!=connection || !route->io_requested || route->io_releasing ||
        GetProcessId(route->worker)!=GetProcessId(worker))return ERROR_ACCESS_DENIED;
    if(route->pipe || route->ready || route->delivered)return ERROR_ALREADY_EXISTS;
    if (NtQueryEvent(ready,EventBasicInformation,&event_info,sizeof(event_info),NULL)<0 ||
        event_info.EventType!=NotificationEvent) { error=ERROR_INVALID_PARAMETER;goto done; }
    if (GetFileType(pipe)!=FILE_TYPE_PIPE ||
        !GetNamedPipeInfo(pipe,&flags,NULL,NULL,NULL) || (flags&PIPE_TYPE_MESSAGE)) {
        error=ERROR_INVALID_PARAMETER;goto done;
    }
    if (!DuplicateHandle(GetCurrentProcess(),ready,GetCurrentProcess(),&local_ready,
            SYNCHRONIZE,FALSE,0)) { error=GetLastError();goto done; }
    if (!DuplicateHandle(GetCurrentProcess(),pipe,GetCurrentProcess(),&local_pipe,
            0,FALSE,DUPLICATE_SAME_ACCESS)) { error=GetLastError();goto done; }
    route->pipe=local_pipe;local_pipe=NULL;
    route->ready=local_ready;local_ready=NULL;
    WakeAllConditionVariable(&connection->service->frontend_changed);
    service_signal_frontend_states(connection->service);
    error=ERROR_SUCCESS;
done:
    if(local_ready)CloseHandle(local_ready);
    if(local_pipe)CloseHandle(local_pipe);
    return error;
}


DWORD OpenNtBaseServiceAttachFrontend(OPENNT_BASE_CONNECTION *connection,DWORD pid,
    DWORD generation,HANDLE pipe,HANDLE ready)
{
    HANDLE worker=NULL;
    DWORD error;
    if (!connection) return ERROR_ACCESS_DENIED;
    EnterCriticalSection(&connection->service->lock);
    error=OpenNtBaseServiceRetainCommandWorker(connection,pid,generation,&worker);
    if (!error) error=service_attach_frontend(connection,worker,pipe,ready);
    if (worker) CloseHandle(worker);
    LeaveCriticalSection(&connection->service->lock);
    return error;
}


DWORD OpenNtBaseServiceRequestFrontend(OPENNT_BASE_CONNECTION *connection,DWORD pid,
    DWORD generation,HANDLE capability)
{
    HANDLE root_process=NULL,worker=NULL;
    DWORD root_generation=0,error;
    LIST_ENTRY *link;
    if (!connection) return ERROR_ACCESS_DENIED;
    EnterCriticalSection(&connection->service->lock);
    error=OpenNtBaseServiceRetainFrontendRoot(connection,pid,generation,capability,
        &root_process,&root_generation);
    if (error) goto done;
    error=OpenNtBaseServiceRetainCommandWorker(connection,pid,generation,&worker);
    if (error) goto done;
    /* This records a logical association only. WorkerIoTransition separately
     * authorizes its physical connection, after the current lease is closed. */
    for (link=connection->service->frontend_routes.Flink;
         link!=&connection->service->frontend_routes;link=link->Flink) {
        OPENNT_FRONTEND_ROUTE *route=CONTAINING_RECORD(link,OPENNT_FRONTEND_ROUTE,link);
        if (GetProcessId(route->worker)==GetProcessId(worker)) {
            if (!route->root) error=ERROR_PIPE_NOT_CONNECTED;
            else if (route->root->process.SequenceNumber==root_generation)
                error=route->pipe || route->delivered ? ERROR_ALREADY_EXISTS : ERROR_SUCCESS;
            else error=ERROR_ACCESS_DENIED;
            if((!error || error==ERROR_ALREADY_EXISTS) && !route->native_worker) {
                DWORD authorization=service_authorize_worker_io(connection,GetProcessId(worker));
                if(authorization)error=authorization;
            }
            if(!error || error==ERROR_ALREADY_EXISTS)
                service_bind_management_root(connection->service,worker,route->root);
            goto done;
        }
    }
    error=ERROR_ACCESS_DENIED;
    for (link=connection->service->connections.Flink;
         link!=&connection->service->connections;link=link->Flink) {
        OPENNT_BASE_CONNECTION *root=CONTAINING_RECORD(link,OPENNT_BASE_CONNECTION,service_link);
        OPENNT_FRONTEND_ROUTE *pending;
        if (root->process.SequenceNumber!=root_generation) continue;
        pending=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*pending));
        if (!pending) { error=ERROR_NOT_ENOUGH_MEMORY;break; }
        pending->root=root;pending->worker=worker;worker=NULL;
        pending->request=generation;
        pending->native_worker=connection->selected_native_generation!=0 ||
            connection->reservation_kind==OPENNT_BASE_WORKER_NATIVE;
        InsertTailList(&connection->service->frontend_routes,&pending->link);
        connection->frontend_request_root=0;
        error=pending->native_worker ? ERROR_SUCCESS :
            service_authorize_worker_io(connection,GetProcessId(pending->worker));
        if(!error)error=service_refresh_frontend_work(root);
        if(error) {
            connection->frontend_request_root=0;
            service_delete_frontend(pending);
            break;
        }
        service_signal_frontend_states(connection->service);
        error=ERROR_SUCCESS;
        service_bind_management_root(connection->service,pending->worker,root);
        break;
    }
done:
    if (root_process) CloseHandle(root_process);
    if (worker) CloseHandle(worker);
    LeaveCriticalSection(&connection->service->lock);
    return error;
}


DWORD OpenNtBaseServiceFrontendStateChanged(OPENNT_BASE_CONNECTION *root,DWORD pid,
    DWORD generation,HANDLE *state_changed)
{
    DWORD error=ERROR_ACCESS_DENIED;
    if(!state_changed)return ERROR_INVALID_PARAMETER;
    *state_changed=NULL;
    if(!root)return error;
    EnterCriticalSection(&root->service->lock);
    if(OpenNtBaseServicePeer(root,pid,generation) && root->frontend_capability &&
        root->frontend_state_changed) {
        if(!DuplicateHandle(GetCurrentProcess(),root->frontend_state_changed,GetCurrentProcess(),
            state_changed,SYNCHRONIZE,FALSE,0)) error=GetLastError();
        else error=ERROR_SUCCESS;
    }
    LeaveCriticalSection(&root->service->lock);
    return error;
}


DWORD OpenNtBaseServiceFrontendRequest(OPENNT_BASE_CONNECTION *root,DWORD pid,
    DWORD generation,DWORD *request,HANDLE *worker)
{
    LIST_ENTRY *link;
    DWORD error=ERROR_NOT_FOUND;
    if (!request || !worker) return ERROR_INVALID_PARAMETER;
    *request=0;*worker=NULL;
    if (!root) return ERROR_ACCESS_DENIED;
    EnterCriticalSection(&root->service->lock);
    if (!OpenNtBaseServicePeer(root,pid,generation) || !root->frontend_capability) {
        error=ERROR_ACCESS_DENIED;goto done;
    }
    /* A successful zero request with no endpoint is the broker's explicit
     * disconnect instruction, not EOF-based ownership inference. The current
     * route remains reserved until both endpoints acknowledge closure. */
    if(root->frontend_io_route && root->frontend_io_route->io_releasing &&
        !root->frontend_io_route->io_frontend_closed) {
        error=ERROR_SUCCESS;goto done;
    }
    /* Independent native worker attachment is not a pending DOS record.
     * The originating launcher can already have returned its direct result. */
    for(link=root->service->frontend_routes.Flink;link!=&root->service->frontend_routes;link=link->Flink) {
        OPENNT_FRONTEND_ROUTE *route=CONTAINING_RECORD(link,OPENNT_FRONTEND_ROUTE,link);
        if(route!=root->frontend_io_route || !route->io_requested || route->pipe || route->delivered ||
            WaitForSingleObject(route->worker,0)!=WAIT_TIMEOUT)continue;
        error=DuplicateHandle(GetCurrentProcess(),route->worker,GetCurrentProcess(),worker,
            PROCESS_QUERY_LIMITED_INFORMATION|SYNCHRONIZE,FALSE,0) ? ERROR_SUCCESS : GetLastError();
        if(!error)*request=route->request;
        goto done;
    }
    error=service_refresh_frontend_work(root);
    if(!error)error=ERROR_NOT_FOUND;
done:
    LeaveCriticalSection(&root->service->lock);
    return error;
}


DWORD OpenNtBaseServiceAttachFrontendRequest(OPENNT_BASE_CONNECTION *root,DWORD pid,
    DWORD generation,DWORD request,HANDLE pipe,HANDLE ready)
{
    LIST_ENTRY *link;
    HANDLE worker=NULL;
    DWORD error=ERROR_ACCESS_DENIED;
    if (!root) return error;
    EnterCriticalSection(&root->service->lock);
    if (!OpenNtBaseServicePeer(root,pid,generation) || !root->frontend_capability) {
        goto done;
    }
    for(link=root->service->frontend_routes.Flink;link!=&root->service->frontend_routes;link=link->Flink) {
        OPENNT_FRONTEND_ROUTE *route=CONTAINING_RECORD(link,OPENNT_FRONTEND_ROUTE,link);
        if(route!=root->frontend_io_route || route->request!=request ||
            !route->io_requested || route->io_releasing)continue;
        error=service_attach_frontend(root,route->worker,pipe,ready);
        if(!error || error==ERROR_ALREADY_EXISTS) {
            LIST_ENTRY *caller_link;
            for(caller_link=root->service->connections.Flink;caller_link!=&root->service->connections;caller_link=caller_link->Flink) {
                OPENNT_BASE_CONNECTION *caller=CONTAINING_RECORD(caller_link,OPENNT_BASE_CONNECTION,service_link);
                if(caller->process.SequenceNumber==request && caller->frontend_request_root==generation)
                    caller->frontend_request_root=0;
            }
        }
        goto done;
    }
done:
    if (worker) CloseHandle(worker);
    if(OpenNtBaseServicePeer(root,pid,generation) && root->frontend_capability) {
        DWORD notification_error=service_refresh_frontend_work(root);
        if(!error || error==ERROR_ALREADY_EXISTS) {
            if(notification_error)error=notification_error;
        }
    }
    LeaveCriticalSection(&root->service->lock);
    return error;
}


DWORD OpenNtBaseServiceWorkerFrontendCapability(OPENNT_BASE_CONNECTION *connection,
    DWORD pid,DWORD generation,HANDLE *capability)
{
    LIST_ENTRY *link;
    DWORD error=ERROR_NOT_FOUND;
    if (!capability) return ERROR_INVALID_PARAMETER;
    *capability=NULL;
    if (!connection) return ERROR_ACCESS_DENIED;
    EnterCriticalSection(&connection->service->lock);
    if (!OpenNtBaseServicePeer(connection,pid,generation) ||
        (!connection->process.fVDM && !connection->native_worker) || connection->wow) {
        error=ERROR_ACCESS_DENIED;goto done;
    }
    for (link=connection->service->frontend_routes.Flink;
         link!=&connection->service->frontend_routes;link=link->Flink) {
        OPENNT_FRONTEND_ROUTE *route=CONTAINING_RECORD(link,OPENNT_FRONTEND_ROUTE,link);
        if (GetProcessId(route->worker)!=pid) continue;
        if (!route->root || !route->root->frontend_capability ||
            WaitForSingleObject(route->root->process.ProcessHandle,0)!=WAIT_TIMEOUT) {
            error=ERROR_PIPE_NOT_CONNECTED;goto done;
        }
        error=DuplicateHandle(GetCurrentProcess(),route->root->frontend_capability,
            GetCurrentProcess(),capability,SYNCHRONIZE,FALSE,0) ? ERROR_SUCCESS : GetLastError();
        break;
    }
done:
    LeaveCriticalSection(&connection->service->lock);
    return error;
}


DWORD OpenNtBaseServiceTakeFrontend(OPENNT_BASE_CONNECTION *connection,DWORD pid,
    DWORD generation,HANDLE *pipe,HANDLE *frontend,DWORD *frontend_generation,HANDLE *ready)
{
    LIST_ENTRY *link;
    DWORD error=ERROR_NOT_READY;
    if (!pipe || !frontend || !frontend_generation || !ready) return ERROR_INVALID_PARAMETER;
    *pipe=NULL; *frontend=NULL; *frontend_generation=0;*ready=NULL;
    if (!connection) return ERROR_ACCESS_DENIED;
    EnterCriticalSection(&connection->service->lock);
    if (!OpenNtBaseServicePeer(connection,pid,generation)) {
        error=ERROR_ACCESS_DENIED; goto done;
    }
    if (connection->wow) { error=ERROR_ACCESS_DENIED; goto done; }
    if (!connection->process.fVDM && !connection->native_worker) {
        error=ERROR_ACCESS_DENIED; goto done;
    }
    for (link=connection->service->frontend_routes.Flink;
         link!=&connection->service->frontend_routes;link=link->Flink) {
        OPENNT_FRONTEND_ROUTE *route=CONTAINING_RECORD(link,OPENNT_FRONTEND_ROUTE,link);
        OPENNT_BASE_CONNECTION *root=route->root;
        if (GetProcessId(route->worker)!=pid) continue;
        if (!root || WaitForSingleObject(root->process.ProcessHandle,0)!=WAIT_TIMEOUT) {
            error=ERROR_PIPE_NOT_CONNECTED; goto done;
        }
        if(route!=root->frontend_io_route || !route->io_requested || route->io_releasing) {
            error=ERROR_NOT_READY;goto done;
        }
        if (!route->pipe) { error=route->delivered ? ERROR_ALREADY_EXISTS : ERROR_NOT_READY; goto done; }
        if (!DuplicateHandle(GetCurrentProcess(),root->process.ProcessHandle,
                GetCurrentProcess(),frontend,PROCESS_QUERY_LIMITED_INFORMATION|SYNCHRONIZE,FALSE,0)) {
            error=GetLastError(); goto done;
        }
        *pipe=route->pipe; route->pipe=NULL;
        *ready=route->ready;route->ready=NULL;
        route->delivered=TRUE;
        *frontend_generation=root->process.SequenceNumber;
        error=ERROR_SUCCESS;
        break;
    }
done:
    LeaveCriticalSection(&connection->service->lock);
    return error;
}


DWORD OpenNtBaseServiceWaitFrontend(OPENNT_BASE_CONNECTION *connection,DWORD pid,
    DWORD generation,HANDLE *pipe,HANDLE *frontend,DWORD *frontend_generation,HANDLE *ready)
{
    DWORD error;
    if (!connection) return ERROR_ACCESS_DENIED;
    EnterCriticalSection(&connection->service->lock);
    for (;;) {
        error=OpenNtBaseServiceTakeFrontend(connection,pid,generation,
            pipe,frontend,frontend_generation,ready);
        if (error!=ERROR_NOT_READY) break;
        /* Only capability arrival is awaited. Original Get already selected
         * the command; no guest command is queued, replayed or selected here.
         * Already delivered is terminal: a route cannot be consumed twice. */
        if (!SleepConditionVariableCS(&connection->service->frontend_changed,
                &connection->service->lock,INFINITE)) { error=GetLastError();break; }
    }
    LeaveCriticalSection(&connection->service->lock);
    return error;
}


/* Reintroduce only the modern transport for the original ConsoleHandle
 * discriminator.  srvvdm.c still decides whether that ConsoleRecord is
 * READY/BUSY and performs all command-record mutation. */
DWORD service_bind_existing_console(OPENNT_BASE_CONNECTION *connection)
{
    OPENNT_BASE_SERVICE *service;
    SERVICE_COMPARE_HANDLES compare;
    HANDLE selected=NULL;
    DWORD index,error=0;
    LIST_ENTRY *entry;
    if (!connection) return ERROR_INVALID_PARAMETER;
    compare=(SERVICE_COMPARE_HANDLES)GetProcAddress(GetModuleHandleW(L"kernelbase.dll"),
        "CompareObjectHandles");
    if (!compare) return ERROR_CALL_NOT_IMPLEMENTED;
    service=connection->service;
    EnterCriticalSection(&service->lock);
    if (connection->console) { LeaveCriticalSection(&service->lock); return ERROR_SUCCESS; }
    /* Only NTCON roots report the visible Console membership. A launcher
     * inherits its authenticated root's logical Console when it retains the
     * root capability; membership itself grants no worker or task authority. */
    for (entry=service->connections.Flink;entry!=&service->connections;entry=entry->Flink) {
        OPENNT_BASE_CONNECTION *other=CONTAINING_RECORD(entry,OPENNT_BASE_CONNECTION,service_link);
        const DWORD *members=other->console_members;
        const HANDLE *processes=other->console_member_processes;
        DWORD count=other->console_member_count;
        BOOL shared=FALSE;
        if (other==connection || !other->console ||
            (!other->frontend_capability && !other->native_worker &&
                !other->process.fVDM)) continue;
        if (other->execution_console_member_count) {
            members=other->execution_console_members;
            processes=other->execution_console_processes;
            count=other->execution_console_member_count;
        }
        if (WaitForSingleObject(other->process.ProcessHandle,0)!=WAIT_TIMEOUT) continue;
        for (index=0;index<connection->console_member_count;++index) {
            DWORD member;
            for(member=0;member<count;++member)
                if(connection->console_members[index]==members[member] &&
                    connection->console_member_processes && processes &&
                    connection->console_member_processes[index] && processes[member] &&
                    WaitForSingleObject(connection->console_member_processes[index],0)==WAIT_TIMEOUT &&
                    WaitForSingleObject(processes[member],0)==WAIT_TIMEOUT &&
                    compare(connection->console_member_processes[index],processes[member]))
                    { shared=TRUE;break; }
            if(shared)break;
        }
        /* A resident original DOS worker remains attached to its Console
         * after the short-lived frontend root retires. Match its actual live
         * process object in the new root's sample, not a reusable PID value. */
        if(!shared && other->process.fVDM && !other->wow &&
            connection->console_member_processes) {
            for(index=0;index<connection->console_member_count;++index)
                if(connection->console_members[index]==
                        (DWORD)other->process.ClientId.UniqueProcess &&
                    connection->console_member_processes[index] &&
                    WaitForSingleObject(connection->console_member_processes[index],0)==WAIT_TIMEOUT &&
                    compare(connection->console_member_processes[index],
                        other->process.ProcessHandle)) {shared=TRUE;break;}
        }
        if (!shared) continue;
        if (selected && selected!=other->console) { error=ERROR_RETRY;break; }
        selected=other->console;
    }
    if (!error) {
        if (selected) connection->console=selected;
        else if (!++service->next_console || service->next_console==MAXDWORD) error=ERROR_ARITHMETIC_OVERFLOW;
        else connection->console=(HANDLE)(ULONG_PTR)service->next_console;
    }
    LeaveCriticalSection(&service->lock);
    return error;
}


DWORD OpenNtBaseServiceReportConsoleMembers(OPENNT_BASE_CONNECTION *connection,
    DWORD pid,DWORD generation,DWORD count,const DWORD *members)
{
    DWORD *copy=NULL,error=ERROR_ACCESS_DENIED,index,other;
    HANDLE *processes=NULL;
    if (!count || count>4096 || !members) return ERROR_INVALID_PARAMETER;
    copy=HeapAlloc(GetProcessHeap(),0,count*sizeof(*copy));
    processes=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,count*sizeof(*processes));
    if (!copy || !processes) {error=ERROR_NOT_ENOUGH_MEMORY;goto done;}
    for(index=0;index<count;++index) {
        if(!members[index]) {error=ERROR_INVALID_DATA;goto done;}
        for(other=0;other<index;++other)
            if(members[index]==members[other]) {error=ERROR_INVALID_DATA;goto done;}
        copy[index]=members[index];
        if(connection && members[index]==pid) {
            if(!DuplicateHandle(GetCurrentProcess(),connection->process.ProcessHandle,
                GetCurrentProcess(),&processes[index],SYNCHRONIZE|
                PROCESS_QUERY_LIMITED_INFORMATION,FALSE,0)) {error=GetLastError();goto done;}
        } else {
            /* Every claimed peer must be a live, pinned process object.
             * A PID which has disappeared or cannot be authenticated may
             * not influence reuse through a partial sample. */
            processes[index]=OpenProcess(SYNCHRONIZE|PROCESS_QUERY_LIMITED_INFORMATION,
                FALSE,members[index]);
            if(!processes[index]) {error=GetLastError();goto done;}
        }
    }
    if (!connection) goto done;
    EnterCriticalSection(&connection->service->lock);
    if (OpenNtBaseServicePeer(connection,pid,generation) &&
        connection->frontend_capability && !connection->process.fVDM &&
        !connection->native_worker &&
        WaitForSingleObject(connection->process.ProcessHandle,0)==WAIT_TIMEOUT) {
        BOOL self=FALSE;
        for(index=0;index<count;++index)
            if(copy[index]==(DWORD)connection->process.ClientId.UniqueProcess &&
                processes[index] && GetProcessId(processes[index])==copy[index])
                {self=TRUE;break;}
        if(connection->console_members)error=ERROR_ALREADY_EXISTS;
        else if(self) {
            connection->console_members=copy;connection->console_member_processes=processes;
            connection->console_member_count=count;copy=NULL;processes=NULL;
            error=ERROR_SUCCESS;
        } else error=ERROR_INVALID_DATA;
    }
    LeaveCriticalSection(&connection->service->lock);
    if(!error)error=service_bind_existing_console(connection);
done:
    if(processes) {
        for(index=0;index<count;++index)if(processes[index])CloseHandle(processes[index]);
        HeapFree(GetProcessHeap(),0,processes);
    }
    if(copy)HeapFree(GetProcessHeap(),0,copy);
    return error;
}
