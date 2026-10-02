/* Original BaseSrv initialization/dispatch plus finite registered caller
 * binding. No test provider, command state machine or CSR runtime. */
#include "basesrv.h"
#include <base_service.h>
#include <base_process.h>
#include <base_dispatch.h>
#include <base_reservation.h>
#include <base_command.h>
#include <base_wait.h>
#include <base_stream.h>
#include <base_interactive.h>
#include <base_event.h>
#include <base_config.h>
#include "ntsrv-exe/transport/vdm_receipt.h"
#include "ntsrv-exe/transport/worker_spawn.h"
#include "ntsrv-exe/transport/native_control.h"
#include "ntsrv-exe/transport/frontend_admission.h"
#include "interface/frontend_bootstrap.h"
#include "interface/native_request_protocol.h"
#include "interface/native_launch.h"
#include <stdio.h>
#define FRONTEND_STARTUP_DEADLINE_MS 10000u
typedef NTSTATUS (*OPENNT_USER_TEST_TOKEN_FOR_INTERACTIVE)(HANDLE,PLUID);
extern OPENNT_USER_TEST_TOKEN_FOR_INTERACTIVE UserTestTokenForInteractive;
/* srvvdm.c owns these source-shaped lists and their lock objects.  They are
 * intentionally not re-declared in a public product ABI: this binding only
 * reads them while constructing a bounded management projection. */
extern PWOWHEAD WOWHead;
extern PCONSOLERECORD DOSHead;
extern RTL_CRITICAL_SECTION BaseSrvWOWCriticalSection;
extern RTL_CRITICAL_SECTION BaseSrvDOSCriticalSection;

struct OPENNT_BASE_SERVICE {
    OPENNT_BASE_PROCESS_REGISTRY registry;
    OPENNT_BASE_RESERVATIONS *reservations;
    CRITICAL_SECTION lock;
    CONDITION_VARIABLE frontend_changed;
    ULONG next_console;
    ULONG next_wait_receipt;
    ULONG next_frontend_join;
    OPENNT_BASE_INTERACTIVE_SCOPE interactive;
    LIST_ENTRY connections;
    LIST_ENTRY retired_connections;
    LIST_ENTRY worker_watches;
    LIST_ENTRY frontend_routes;
    LIST_ENTRY console_contexts;
    HANDLE frontend_lifetime_changed;
    OPENNT_BASE_EMPTY_NOTIFY empty_notify;
    void *empty_notify_context;
    uint64_t management_epoch;
};
struct OPENNT_BASE_CONNECTION {
    OPENNT_BASE_SERVICE *service;
    CSR_PROCESS process;
    broker_vdm_receipts streams;
    uint64_t reservation;
    ULONG task;
    OPENNT_BASE_WORKER_KIND reservation_kind;
    HANDLE console;
    DWORD *console_members;
    HANDLE *console_member_processes; /* Pinned process identities, not task state. */
    DWORD console_member_count;
    /* A resident worker outlives its direct launcher. Retain the
     * authenticated frontend root's visible-Console identity separately
     * from the worker's own hidden Console membership. This is only a local
     * Console discriminator, never task or process authority. */
    DWORD *execution_console_members;
    HANDLE *execution_console_processes;
    DWORD execution_console_member_count;
    BOOL wow;
    BOOL retired;
    BOOL pending_creation;
    ULONG pending_vdm_binary; /* Original Check result, not caller-selected creation. */
    BOOL vdm_starting;
    BOOL registered_worker;
    BOOL native_worker;
    DWORD selected_native_generation; /* Explicit launcher grant, not membership alone. */
    /* The original BaseSrvDupStandardHandles copies these caller streams
     * into the suspended worker during UpdateDOSEntry.  The standalone
     * broker temporarily owns receipt-backed duplicates until that point;
     * it must release them afterwards so a pipe reader can observe EOF. */
    uint32_t pending_standard_streams[3];
    HANDLE parent_wait; /* Borrowed from this connection's receipt table. */
    uint32_t parent_receipt,completed_receipt;
    DWORD completed_exit_code;
    BOOL dos_completion_read;
    BOOL worker_failed;
    /* Only the original nonzero-DosSesId branch completes on real worker exit.
     * The broker transports that existing result; it does not synthesize a DOS
     * task event or add another task registry. Other branches keep srvvdm waits. */
    BOOL parent_is_worker_exit,vdm_exit_cancelled;
    HANDLE vdm_exit_watch,vdm_exit_process;
    HANDLE wow_start_event;
    BOOL wow_started;
    HANDLE frontend_capability; /* Root lease, independent of command lifetime. */
    uint64_t frontend_console_window,frontend_reserved_window;
    HANDLE frontend_retire,frontend_restored;
    HANDLE frontend_start_process; /* Borrowed only during the active Start RPC. */
    HANDLE frontend_start_capability,frontend_start_retire,frontend_start_restored;
    BOOL frontend_starting;
    BOOL frontend_return_pending,frontend_return_complete;
    BOOL frontend_borrowed,frontend_idle;
    ULONGLONG frontend_admission_deadline;
    ULONGLONG frontend_workerless_deadline; /* NTSRV-owned cancellable grace. */
    DWORD frontend_creator_generation;
    HANDLE frontend_exit_watch,frontend_exit_process;
    OPENNT_BASE_CONNECTION *frontend_join_caller;
    DWORD frontend_join_nonce,frontend_join_pid;
    int frontend_join_decision;
    DWORD retained_frontend_root; /* Authenticated launcher association, not a task. */
    HANDLE frontend_state_changed; /* Root-private auto-reset retirement wake. */
    HANDLE worker_state_changed; /* Per-launcher auto-reset worker transition wake. */
    BOOL frontend_closing; /* Admission barrier, never execution/task state. */
    DWORD frontend_request_root; /* Original pending command asks this root for I/O. */
    DWORD frontend_channel_root; /* One pending direct launcher-to-root attachment. */
    HANDLE frontend_channel,frontend_execution;
    DWORD channel_worker_generation; /* Zero selects the retained frontend route. */
    HANDLE channel_frontend; /* Authenticated I/O association for worker delivery. */
    DWORD native_root;
    DWORD native_inflight,native_activity_root;
    /* Project-owned native-text counterpart to BaseSrv's original DOSRECORD
     * projection.  The broker, not the native Console process list, owns
     * logical request depth and the active task label exposed to NTMON. */
    LIST_ENTRY win32records;
    struct OPENNT_BASE_WIN32RECORD *pending_win32record;
    DWORD next_win32record;
    HANDLE native_stop,native_closed;
    LIST_ENTRY service_link;
    LIST_ENTRY retired_link;
};
typedef struct OPENNT_BASE_WIN32RECORD {
    LIST_ENTRY link;
    DWORD request;
    /* Bind only the admitted direct target's actual CreateProcess identity. */
    DWORD process_id;
    DWORD launcher_generation,exit_code,completion_error;
    BOOL completed;
    HANDLE receipt; /* Signalled after NTW32 reports exit and I/O release. */
    HANDLE control,control_worker; /* Broker-owned final I/O acknowledgement. */
    WCHAR image[OPENNT_BASE_WORKER_IMAGE_CHARS];
} OPENNT_BASE_WIN32RECORD;
typedef struct OPENNT_FRONTEND_ROUTE {
    LIST_ENTRY link;
    OPENNT_BASE_CONNECTION *root; /* Removed before this connection is freed. */
    HANDLE worker;
    HANDLE pipe;
    HANDLE ready;
    BOOL delivered;
    BOOL native_worker; /* I/O route survives its native launcher, not its root. */
    DWORD request;
} OPENNT_FRONTEND_ROUTE;
typedef struct OPENNT_BASE_WORKER_WATCH {
    LIST_ENTRY link;
    OPENNT_BASE_SERVICE *service;
    CSR_PROCESS process;
    HANDLE wait;
    HANDLE console;
    BOOL wow;
    uint64_t reservation;
    OPENNT_BASE_WORKER_KIND kind;
    FILETIME started;
    /* The original GetNextVDMCommand body frees VDMINFO immediately after
     * copying it to the worker.  These are labels keyed to surviving original
     * DOSRECORD identities.  They never model execution lifetime: srvvdm.c's
     * DOSRecord chain remains the sole source of task depth and state. */
    LIST_ENTRY management_labels;
    BOOL termination_requested;
    HANDLE shutdown;
    BOOL frontend_associated;
} OPENNT_BASE_WORKER_WATCH;
typedef struct OPENNT_BASE_MANAGEMENT_LABEL {
    LIST_ENTRY link;
    PDOSRECORD record;
    HANDLE parent_wait;
    ULONG task;
    WCHAR image[OPENNT_BASE_WORKER_IMAGE_CHARS];
} OPENNT_BASE_MANAGEMENT_LABEL;
typedef struct OPENNT_BASE_CONSOLE_CONTEXT {
    LIST_ENTRY link;
    OPENNT_BASE_CONNECTION *root; /* Removed under service lock before root free. */
    HANDLE capability,console;
} OPENNT_BASE_CONSOLE_CONTEXT;
static DWORD service_bind_existing_console(OPENNT_BASE_CONNECTION *connection);
static BOOL service_root_has_worker(OPENNT_BASE_CONNECTION *root);
static void service_retire_completed_root(OPENNT_BASE_CONNECTION *parent,DWORD idle_worker);
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
static void service_signal_frontend_states(OPENNT_BASE_SERVICE *service)
{
    LIST_ENTRY *link;
    if(service->frontend_lifetime_changed)
        (void)SetEvent(service->frontend_lifetime_changed);
    for(link=service->connections.Flink;link!=&service->connections;link=link->Flink) {
        OPENNT_BASE_CONNECTION *root=CONTAINING_RECORD(link,OPENNT_BASE_CONNECTION,service_link);
        if(root->frontend_state_changed) (void)SetEvent(root->frontend_state_changed);
    }
}
static void service_signal_worker_states(OPENNT_BASE_SERVICE *service)
{
    LIST_ENTRY *link;
    for(link=service->connections.Flink;link!=&service->connections;link=link->Flink) {
        OPENNT_BASE_CONNECTION *caller=CONTAINING_RECORD(link,OPENNT_BASE_CONNECTION,service_link);
        if(caller->worker_state_changed)(void)SetEvent(caller->worker_state_changed);
    }
}
static void service_delete_console_context(OPENNT_BASE_CONSOLE_CONTEXT *context)
{
    RemoveEntryList(&context->link);
    CloseHandle(context->capability);
    HeapFree(GetProcessHeap(),0,context);
}
/* Called under the service lock. An execution Console can differ from the
 * visible root: original CheckDOS allocates a separate DosSessionId when it
 * cannot use the shared Console record. Only broker-authenticated contexts
 * establish that association; matching a PID/member count does not. */
static BOOL service_root_console_matches(OPENNT_BASE_CONNECTION *root,HANDLE console)
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
typedef struct OPENNT_BASE_SERVICE_RESOURCES {
    OPENNT_BASE_CONNECTION *connection;
    DWORD role;
    HANDLE worker;
    OPENNT_BASE_WAIT_BINDING wait;
    OPENNT_BASE_RESOURCE_BINDING binding;
} OPENNT_BASE_SERVICE_RESOURCES;
static void service_copy_management_record(OPENNT_BASE_WORKER_WATCH *watch,
    OPENNT_BASE_WORKER_INFO *item);
static void service_clear_management_labels(OPENNT_BASE_WORKER_WATCH *watch);
static void service_delete_win32record(OPENNT_BASE_WIN32RECORD *record)
{
    if (!record) return;
    RemoveEntryList(&record->link);
    if(record->receipt)CloseHandle(record->receipt);
    if(record->control)CloseHandle(record->control);
    if(record->control_worker)CloseHandle(record->control_worker);
    HeapFree(GetProcessHeap(),0,record);
}
static void service_release_console_identities(OPENNT_BASE_CONNECTION *connection)
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
static DWORD service_copy_execution_console_members(OPENNT_BASE_CONNECTION *destination,
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
static void service_clear_pending_win32record(OPENNT_BASE_CONNECTION *connection)
{
    if (connection->pending_win32record) {
        HeapFree(GetProcessHeap(),0,connection->pending_win32record);
        connection->pending_win32record=NULL;
    }
}
static void service_clear_win32records(OPENNT_BASE_CONNECTION *connection)
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
static void service_release_launcher_results(OPENNT_BASE_SERVICE *service,DWORD generation)
{
    LIST_ENTRY *link;
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
static DWORD service_win32record_depth(const OPENNT_BASE_CONNECTION *connection)
{
    const LIST_ENTRY *link;
    DWORD depth=0;
    /* Only broker-admitted Direct records exist in this management stack. */
    for(link=connection->win32records.Flink;link!=&connection->win32records;link=link->Flink)
        if(!CONTAINING_RECORD(link,OPENNT_BASE_WIN32RECORD,link)->completed)++depth;
    return depth;
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
static void service_delete_frontend(OPENNT_FRONTEND_ROUTE *route)
{
    RemoveEntryList(&route->link);
    if (route->pipe) CloseHandle(route->pipe);
    if (route->ready) CloseHandle(route->ready);
    CloseHandle(route->worker);
    HeapFree(GetProcessHeap(),0,route);
}
static void service_clear_frontend_channel(OPENNT_BASE_CONNECTION *connection)
{
    if (connection->frontend_channel) CloseHandle(connection->frontend_channel);
    if (connection->frontend_execution) CloseHandle(connection->frontend_execution);
    if (connection->channel_frontend) CloseHandle(connection->channel_frontend);
    connection->frontend_channel=NULL;connection->frontend_execution=NULL;
    connection->channel_frontend=NULL;connection->channel_worker_generation=0;
    connection->frontend_channel_root=0;
    service_clear_pending_win32record(connection);
}
static void service_clear_frontend(OPENNT_BASE_CONNECTION *connection)
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
            route->request==connection->process.SequenceNumber)) {
            if (route->delivered || route->native_worker) service_delete_frontend(route);
            else {
                /* Retain only the selected worker identity until its exit:
                 * a waiter must observe cancellation, not await a new root. */
                route->root=NULL;
                if (route->pipe) { CloseHandle(route->pipe);route->pipe=NULL; }
                if (route->ready) { CloseHandle(route->ready);route->ready=NULL; }
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
static void service_prune_cancelled_frontends(OPENNT_BASE_SERVICE *service)
{
    LIST_ENTRY *link=service->frontend_routes.Flink;
    while (link!=&service->frontend_routes) {
        OPENNT_FRONTEND_ROUTE *route=CONTAINING_RECORD(link,OPENNT_FRONTEND_ROUTE,link);
        link=link->Flink;
        if (!route->root && WaitForSingleObject(route->worker,0)==WAIT_OBJECT_0)
            service_delete_frontend(route);
    }
}
static void service_capture_initial_management_labels(OPENNT_BASE_WORKER_WATCH *watch);
static void service_capture_checked_management_label(OPENNT_BASE_SERVICE *service,
    HANDLE console,const BASE_CHECKVDM_MSG *command);
/* Called under the service lock before either original ExitVDM or process
 * rundown destroys records. Both paths must preserve the same parent result. */
static void service_preserve_parent_results(OPENNT_BASE_SERVICE *service,
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
static VOID CALLBACK service_worker_terminated(PVOID context,BOOLEAN fired)
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
static NTSTATUS service_wait_deliver(void *context,HANDLE event,uint32_t *receipt)
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
static void service_resources_init(OPENNT_BASE_SERVICE_RESOURCES *scope,
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
static void service_abandon_launch(OPENNT_BASE_CONNECTION *connection)
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
/* Original guarded USER hook is absent in standalone CLI composition. */
PFNNOTIFYPROCESSCREATE UserNotifyProcessCreate=NULL;
OPENNT_BASE_SERVICE *OpenNtBaseServiceStart(void)
{
    OPENNT_BASE_SERVICE *service=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*service));
    FILETIME now;
    if (!service) return NULL;
    InitializeConditionVariable(&service->frontend_changed);
    if (!InitializeCriticalSectionEx(&service->lock,0,0)) {
        HeapFree(GetProcessHeap(),0,service); return NULL;
    }
    InitializeListHead(&service->connections);
    InitializeListHead(&service->retired_connections);
    InitializeListHead(&service->worker_watches);
    InitializeListHead(&service->frontend_routes);
    InitializeListHead(&service->console_contexts);
    service->frontend_lifetime_changed=CreateEventW(NULL,FALSE,FALSE,NULL);
    if(!service->frontend_lifetime_changed) {
        DeleteCriticalSection(&service->lock);HeapFree(GetProcessHeap(),0,service);return NULL;
    }
    GetSystemTimeAsFileTime(&now);
    service->management_epoch=((uint64_t)now.dwHighDateTime<<32)|now.dwLowDateTime;
    service->management_epoch^=(uint64_t)GetCurrentProcessId()<<17;
    if (!service->management_epoch) service->management_epoch=1;
    if (!OpenNtBaseInitializeProcessRegistry(&service->registry)) {
        CloseHandle(service->frontend_lifetime_changed);
        DeleteCriticalSection(&service->lock); HeapFree(GetProcessHeap(),0,service); return NULL;
    }
    if (!OpenNtBaseReservationsInitialize(&service->reservations)) {
        OpenNtBaseDestroyProcessRegistry(&service->registry);
        CloseHandle(service->frontend_lifetime_changed);
        DeleteCriticalSection(&service->lock);HeapFree(GetProcessHeap(),0,service);return NULL;
    }
    /* srvvdm.c normally resolves this private winsrv entry point lazily.  The
     * standalone broker has no winsrv DLL, but the original CheckWOW body
     * already accepts precisely this predicate.  Supply the existing adapter
     * binding and keep its trusted local logon scope outside the wire format. */
    if (!OpenNtBaseInitializeInteractiveScope(&service->interactive)) {
        OpenNtBaseReservationsDestroy(service->reservations);
        OpenNtBaseDestroyProcessRegistry(&service->registry);
        CloseHandle(service->frontend_lifetime_changed);
        DeleteCriticalSection(&service->lock); HeapFree(GetProcessHeap(),0,service); return NULL;
    }
    UserTestTokenForInteractive=_UserTestTokenForInteractive;
    BaseSrvVDMInit();
    return service;
}
BOOL OpenNtBaseServiceConfigureEmptyNotify(OPENNT_BASE_SERVICE *service,
    OPENNT_BASE_EMPTY_NOTIFY notify,void *context)
{
    BOOL result=FALSE;
    if (!service || !notify) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    EnterCriticalSection(&service->lock);
    if (IsListEmpty(&service->connections) && IsListEmpty(&service->worker_watches)) {
        service->empty_notify=notify;
        service->empty_notify_context=context;
        result=TRUE;
    } else SetLastError(ERROR_BUSY);
    LeaveCriticalSection(&service->lock);
    return result;
}
BOOL OpenNtBaseServiceStop(OPENNT_BASE_SERVICE *service)
{
    LIST_ENTRY *entry;
    /* Transport has stopped and joined all calls/rundowns before stop. */
    if (!service || !IsListEmpty(&service->connections) || !IsListEmpty(&service->worker_watches) ||
        !OpenNtBaseReservationsDestroy(service->reservations) ||
        !OpenNtBaseDestroyProcessRegistry(&service->registry)) return FALSE;
    /* A cancelled route may precede worker Connect, hence have no worker
     * exit watch. Calls and rundowns are joined; release its retained handles
     * without terminating a process or changing original DOS task policy. */
    while (!IsListEmpty(&service->frontend_routes))
        service_delete_frontend(CONTAINING_RECORD(service->frontend_routes.Flink,
            OPENNT_FRONTEND_ROUTE,link));
    while (!IsListEmpty(&service->console_contexts))
        service_delete_console_context(CONTAINING_RECORD(service->console_contexts.Flink,
            OPENNT_BASE_CONSOLE_CONTEXT,link));
    while (!IsListEmpty(&service->retired_connections)) {
        OPENNT_BASE_CONNECTION *connection;
        entry=RemoveHeadList(&service->retired_connections);
        connection=CONTAINING_RECORD(entry,OPENNT_BASE_CONNECTION,retired_link);
        if (connection->frontend_capability) CloseHandle(connection->frontend_capability);
        if (connection->frontend_retire) CloseHandle(connection->frontend_retire);
        if (connection->frontend_restored) CloseHandle(connection->frontend_restored);
        if (connection->frontend_state_changed) CloseHandle(connection->frontend_state_changed);
        if (connection->worker_state_changed) CloseHandle(connection->worker_state_changed);
        service_release_console_identities(connection);
        HeapFree(GetProcessHeap(),0,connection);
    }
    DeleteCriticalSection(&service->lock);
    CloseHandle(service->frontend_lifetime_changed);
    HeapFree(GetProcessHeap(),0,service);
    return TRUE;
}
BOOL OpenNtBaseServiceIsEmpty(OPENNT_BASE_SERVICE *service)
{
    BOOL empty;
    if (!service) return FALSE;
    EnterCriticalSection(&service->lock);
    empty=IsListEmpty(&service->worker_watches) && OpenNtBaseProcessRegistryIsEmpty(&service->registry) &&
        OpenNtBaseReservationsIsEmpty(service->reservations);
    LeaveCriticalSection(&service->lock);
    return empty;
}
/* The task manager is deliberately a projection of the original VDM lists,
 * not a second task registry.  All original list mutations in this
 * composition occur under service->lock; the original locks below preserve
 * the source owners' own DOS/WOW traversal contract as well. */
static void service_copy_management_text(PCSTR text,ULONG length,
    WCHAR image[OPENNT_BASE_WORKER_IMAGE_CHARS])
{
    int copied;
    if (!text || !length) return;
    copied=MultiByteToWideChar(CP_ACP,0,(LPCCH)text,(int)length,
        image,OPENNT_BASE_WORKER_IMAGE_CHARS-1);
    if (copied>0) image[copied]=L'\0';
}
static void service_copy_management_image(PVDMINFO info,WCHAR image[OPENNT_BASE_WORKER_IMAGE_CHARS])
{
    if (info) service_copy_management_text(info->AppName,info->AppLen,image);
}
static BOOL service_same_management_wait(HANDLE left,HANDLE right)
{
    /* BaseSrvGetVDMExitCode compares this source carrier after stripping the
     * historical low-bit tag.  The display sidecar uses the same identity. */
    return (((ULONG_PTR)left & ~(ULONG_PTR)1)==((ULONG_PTR)right & ~(ULONG_PTR)1));
}
static void service_clear_management_labels(OPENNT_BASE_WORKER_WATCH *watch)
{
    while (watch && !IsListEmpty(&watch->management_labels)) {
        LIST_ENTRY *link=RemoveHeadList(&watch->management_labels);
        HeapFree(GetProcessHeap(),0,CONTAINING_RECORD(link,
            OPENNT_BASE_MANAGEMENT_LABEL,link));
    }
}
static OPENNT_BASE_MANAGEMENT_LABEL *service_find_management_label(
    OPENNT_BASE_WORKER_WATCH *watch,PDOSRECORD record,HANDLE parent_wait)
{
    LIST_ENTRY *link;
    if (!watch || !record) return NULL;
    for (link=watch->management_labels.Flink;
         link!=&watch->management_labels;link=link->Flink) {
        OPENNT_BASE_MANAGEMENT_LABEL *label=CONTAINING_RECORD(link,
            OPENNT_BASE_MANAGEMENT_LABEL,link);
        if (label->record==record &&
            service_same_management_wait(label->parent_wait,parent_wait)) return label;
    }
    return NULL;
}
static void service_prune_management_labels(OPENNT_BASE_WORKER_WATCH *watch,
    PCONSOLERECORD console)
{
    LIST_ENTRY *link,*next;
    if (!watch || !console) return;
    for (link=watch->management_labels.Flink;
         link!=&watch->management_labels;link=next) {
        OPENNT_BASE_MANAGEMENT_LABEL *label=CONTAINING_RECORD(link,
            OPENNT_BASE_MANAGEMENT_LABEL,link);
        PDOSRECORD dos;
        BOOL live=FALSE;
        next=link->Flink;
        for (dos=console->DOSRecord;dos;dos=dos->DOSRecordNext) {
            if (dos==label->record &&
                service_same_management_wait(dos->hWaitForParent,label->parent_wait)) {
                live=TRUE;
                break;
            }
        }
        if (!live) {
            RemoveEntryList(link);
            HeapFree(GetProcessHeap(),0,label);
        }
    }
}
static void service_capture_management_label(OPENNT_BASE_WORKER_WATCH *watch,
    PDOSRECORD record,ULONG task_id,const WCHAR *image)
{
    OPENNT_BASE_MANAGEMENT_LABEL *label;
    if (!watch || !record || !image || !image[0]) return;
    label=service_find_management_label(watch,record,record->hWaitForParent);
    if (!label) {
        label=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*label));
        if (!label) return; /* The display projection never alters delivery. */
        label->record=record;
        label->parent_wait=record->hWaitForParent;
        InsertTailList(&watch->management_labels,&label->link);
    }
    label->task=task_id;
    lstrcpynW(label->image,image,OPENNT_BASE_WORKER_IMAGE_CHARS);
}
static void service_capture_management_label_from_info(OPENNT_BASE_WORKER_WATCH *watch,
    PDOSRECORD record)
{
    WCHAR image[OPENNT_BASE_WORKER_IMAGE_CHARS]={0};
    if (!record || !record->lpVDMInfo) return;
    service_copy_management_image(record->lpVDMInfo,image);
    service_capture_management_label(watch,record,record->lpVDMInfo->iTask,image);
}
static void service_capture_initial_management_labels(OPENNT_BASE_WORKER_WATCH *watch)
{
    PCONSOLERECORD console;
    PDOSRECORD dos;
    if (!watch || watch->kind!=OPENNT_BASE_WORKER_DOS) return;
    (void)RtlEnterCriticalSection(&BaseSrvDOSCriticalSection);
    for (console=DOSHead;console;console=console->Next) {
        if (console->SequenceNumber!=watch->process.SequenceNumber) continue;
        for (dos=console->DOSRecord;dos;dos=dos->DOSRecordNext)
            service_capture_management_label_from_info(watch,dos);
        break;
    }
    RtlLeaveCriticalSection(&BaseSrvDOSCriticalSection);
}
static OPENNT_BASE_WORKER_WATCH *service_find_management_watch_for_console(
    OPENNT_BASE_SERVICE *service,HANDLE console)
{
    LIST_ENTRY *link;
    if (!service || !console) return NULL;
    for (link=service->worker_watches.Flink;
         link!=&service->worker_watches;link=link->Flink) {
        OPENNT_BASE_WORKER_WATCH *watch=CONTAINING_RECORD(link,
            OPENNT_BASE_WORKER_WATCH,link);
        if (watch->kind==OPENNT_BASE_WORKER_DOS && watch->console==console) return watch;
    }
    return NULL;
}
static void service_capture_checked_management_label(OPENNT_BASE_SERVICE *service,
    HANDLE console,const BASE_CHECKVDM_MSG *command)
{
    OPENNT_BASE_WORKER_WATCH *watch;
    PCONSOLERECORD source_console;
    PDOSRECORD dos;
    WCHAR image[OPENNT_BASE_WORKER_IMAGE_CHARS]={0};
    if (!service || !console || !command || !command->AppName || !command->AppLen) return;
    watch=service_find_management_watch_for_console(service,console);
    if (!watch) return;
    service_copy_management_text(command->AppName,command->AppLen,image);
    if (!image[0]) return;
    (void)RtlEnterCriticalSection(&BaseSrvDOSCriticalSection);
    for (source_console=DOSHead;source_console;source_console=source_console->Next) {
        if (source_console->hConsole!=console) continue;
        for (dos=source_console->DOSRecord;dos;dos=dos->DOSRecordNext) {
            if (service_same_management_wait(dos->hWaitForParent,
                    command->WaitObjectForParent)) {
                service_capture_management_label(watch,dos,command->iTask,image);
                break;
            }
        }
        break;
    }
    RtlLeaveCriticalSection(&BaseSrvDOSCriticalSection);
}
static void service_copy_management_record(OPENNT_BASE_WORKER_WATCH *watch,
    OPENNT_BASE_WORKER_INFO *item)
{
    PCONSOLERECORD console;
    PDOSRECORD dos,selected=NULL;
    PWOWRECORD wow,selected_wow=NULL;
    if(watch->kind==OPENNT_BASE_WORKER_NATIVE) {
        item->kind=2u; /* Win32 text worker */
        /* Execution request/member publication is a separate binding. A
         * claimed process alone is not proof of native command readiness. */
        item->state=0;
        return;
    }
    if (watch->wow) {
        (void)RtlEnterCriticalSection(&BaseSrvWOWCriticalSection);
        if (WOWHead && WOWHead->SequenceNumber==watch->process.SequenceNumber) {
            for (wow=WOWHead->WOWRecord;wow;wow=wow->WOWRecordNext) {
                ++item->stack_depth;
                if (!selected_wow || (!selected_wow->fDispatched && wow->fDispatched))
                    selected_wow=wow;
            }
            if (selected_wow) {
                item->kind=1u; /* WOW16 / Win16 worker */
                item->state=selected_wow->fDispatched ? VDM_BUSY : VDM_READY;
                item->task=selected_wow->iTask;
                service_copy_management_image(selected_wow->lpVDMInfo,item->image);
            }
        }
        RtlLeaveCriticalSection(&BaseSrvWOWCriticalSection);
        return;
    }
    (void)RtlEnterCriticalSection(&BaseSrvDOSCriticalSection);
    for (console=DOSHead;console;console=console->Next) {
        if (console->SequenceNumber!=watch->process.SequenceNumber) continue;
        service_prune_management_labels(watch,console);
        for (dos=console->DOSRecord;dos;dos=dos->DOSRecordNext) {
            /* This chain is the original BaseSrv execution stack.  A returned
             * record is retained only until the parent consumes its exit code;
             * it is not a running child.  READY is the resident PermCom and
             * likewise has visible depth zero. */
            if (dos->VDMState==VDM_BUSY || dos->VDMState==VDM_TO_TAKE_A_COMMAND) {
                ++item->stack_depth;
                selected=dos; /* Tail is the currently selected child. */
            }
        }
        break;
    }
    if (selected) {
        OPENNT_BASE_MANAGEMENT_LABEL *label;
        item->kind=0u; /* DOS worker */
        item->state=selected->VDMState;
        if (selected->lpVDMInfo) {
            item->task=selected->lpVDMInfo->iTask;
            service_copy_management_image(selected->lpVDMInfo,item->image);
        } else if ((label=service_find_management_label(watch,selected,
                selected->hWaitForParent))!=NULL) {
            item->task=label->task;
            lstrcpynW(item->image,label->image,OPENNT_BASE_WORKER_IMAGE_CHARS);
        }
    }
    RtlLeaveCriticalSection(&BaseSrvDOSCriticalSection);
}
static void service_sort_management_records(OPENNT_BASE_WORKER_INFO *entries,uint32_t count)
{
    uint32_t index,insert;
    /* Original lists are ownership chains, not a UI ordering contract. */
    for (index=1;index<count;++index) {
        OPENNT_BASE_WORKER_INFO value=entries[index];
        for (insert=index;insert &&
            (entries[insert-1].sequence>value.sequence ||
             (entries[insert-1].sequence==value.sequence && entries[insert-1].task>value.task));--insert)
            entries[insert]=entries[insert-1];
        entries[insert]=value;
    }
}
static void service_copy_win32record(OPENNT_BASE_CONNECTION *native,
    OPENNT_BASE_WORKER_INFO *item)
{
    OPENNT_BASE_WIN32RECORD *record=NULL;
    {
        LIST_ENTRY *link;
        for(link=native->win32records.Blink;link!=&native->win32records;link=link->Blink) {
            OPENNT_BASE_WIN32RECORD *candidate=CONTAINING_RECORD(link,OPENNT_BASE_WIN32RECORD,link);
            if(!candidate->completed){record=candidate;break;}
        }
    }
    /* Only admitted Direct requests populate TASK/STACK. */
    item->state=native->native_inflight ? VDM_BUSY : VDM_READY;
    item->stack_depth=service_win32record_depth(native);
    item->task=record ? record->request : 0;
    lstrcpynW(item->image,record ? record->image : L"<EMPTY>",
        OPENNT_BASE_WORKER_IMAGE_CHARS);
    if(WaitForSingleObject(native->native_stop,0)==WAIT_OBJECT_0)item->state|=0x80000000u;
}
DWORD OpenNtBaseServiceSnapshot(OPENNT_BASE_SERVICE *service,uint64_t *epoch,
    OPENNT_BASE_WORKER_INFO *entries,uint32_t capacity,uint32_t *count)
{
    LIST_ENTRY *link;
    uint32_t needed=0,index=0;
    if (!service || !epoch || !count) return ERROR_INVALID_PARAMETER;
    *count=0; *epoch=0;
    EnterCriticalSection(&service->lock);
    for (link=service->worker_watches.Flink;link!=&service->worker_watches;link=link->Flink) ++needed;
    *epoch=service->management_epoch;
    *count=needed;
    if (needed>capacity || (needed && !entries)) {
        LeaveCriticalSection(&service->lock);
        return ERROR_INSUFFICIENT_BUFFER;
    }
    for (link=service->worker_watches.Flink;link!=&service->worker_watches;link=link->Flink) {
        OPENNT_BASE_WORKER_WATCH *watch=CONTAINING_RECORD(link,OPENNT_BASE_WORKER_WATCH,link);
        OPENNT_BASE_WORKER_INFO *item=&entries[index++];
        ZeroMemory(item,sizeof(*item));
        item->sequence=watch->process.SequenceNumber;
        item->process_id=(DWORD)(ULONG_PTR)watch->process.ClientId.UniqueProcess;
        item->kind=watch->wow ? 1u : 0u;
        item->state=watch->termination_requested ? 0x80000000u : VDM_READY;
        item->started_filetime=((uint64_t)watch->started.dwHighDateTime<<32)|watch->started.dwLowDateTime;
        service_copy_management_record(watch,item);
        if(watch->kind==OPENNT_BASE_WORKER_NATIVE) {
            LIST_ENTRY *entry;
            for(entry=service->connections.Flink;entry!=&service->connections;entry=entry->Flink) {
                OPENNT_BASE_CONNECTION *native=CONTAINING_RECORD(entry,OPENNT_BASE_CONNECTION,service_link);
                if(native->process.SequenceNumber!=watch->process.SequenceNumber || !native->native_worker)continue;
                service_copy_win32record(native,item);
                break;
            }
        }
        if (watch->termination_requested) item->state|=0x80000000u;
        /* A resident worker without an original VDMINFO has no active
         * product task. Do not misrepresent its historical launch image as
         * current work. */
        if (!item->image[0])
            lstrcpynW(item->image,L"<EMPTY>",OPENNT_BASE_WORKER_IMAGE_CHARS);
    }
    *count=index;
    service_sort_management_records(entries,index);
    LeaveCriticalSection(&service->lock);
    return ERROR_SUCCESS;
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
DWORD OpenNtBaseServiceConnect(OPENNT_BASE_SERVICE *service,HANDLE process,
    OPENNT_BASE_CONNECTION **output,DWORD *generation)
{
    OPENNT_BASE_CONNECTION *connection;
    DWORD error=0;
    if (!service || !output || !generation) return ERROR_INVALID_PARAMETER;
    *output=NULL; *generation=0;
    connection=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*connection));
    if (!connection) return ERROR_NOT_ENOUGH_MEMORY;
    connection->service=service;
    InitializeListHead(&connection->win32records);
    EnterCriticalSection(&service->lock);
    if (!OpenNtBaseRegisterProcess(&service->registry,&connection->process,process)) error=GetLastError();
    else {
        uint64_t reservation=0;
        ULONG task=0;
        HANDLE console=NULL;
        HANDLE reserved_worker=NULL;
        BOOL shared_wow=FALSE,watch_added=FALSE;
        OPENNT_BASE_WORKER_KIND kind=OPENNT_BASE_WORKER_DOS;
        broker_vdm_receipts_initialize(&connection->streams,connection->process.SequenceNumber);
        error=OpenNtBaseReservationClaimWorkerKind(service->reservations,
            (DWORD)connection->process.ClientId.UniqueProcess,connection->process.SequenceNumber,
            &reservation,&task,&console,&kind,&reserved_worker);
        if (error==ERROR_NOT_FOUND) error=ERROR_SUCCESS;
        else if (!error) {
            OPENNT_BASE_WORKER_WATCH *watch;
            shared_wow=kind==OPENNT_BASE_WORKER_WOW;
            connection->native_worker=kind==OPENNT_BASE_WORKER_NATIVE;
            connection->reservation=reservation;connection->task=task;connection->reservation_kind=kind;
            connection->console=console;connection->wow=shared_wow;
            /* This is the post-create registration performed by original
             * BaseSrvCreateProcess: publish the authenticated worker as a
             * VDM and attach its CSR sequence to the source ConsoleRecord.
             * The standalone learns that fact only when the reservation-bound
             * worker connects; it does not invent another worker state. */
            connection->process.fVDM=!connection->native_worker;
            /* Before first PIF binding a new DOS ConsoleRecord is keyed by
             * DosSesId, not the reserved transport Console identity. */
            if(!connection->native_worker)BaseSrvUpdateVDMSequenceNumber(shared_wow ? OPENNT_BASE_CONSOLE_WOW : task ? NULL : console,
                connection->process.SequenceNumber,task);
            watch=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*watch));
            if (!watch || !reserved_worker) {
                error=watch ? ERROR_INVALID_HANDLE : ERROR_NOT_ENOUGH_MEMORY;
                if (watch) HeapFree(GetProcessHeap(),0,watch);
                if(!connection->native_worker)BaseSrvCleanupVDMResources(&connection->process);
            } else {
                watch->process.ProcessHandle=reserved_worker;
                reserved_worker=NULL;
                watch->service=service;
                watch->process.ClientId=connection->process.ClientId;
                watch->process.SequenceNumber=connection->process.SequenceNumber;
                watch->process.fVDM=connection->process.fVDM;
                watch->kind=kind;
                watch->console=console;
                watch->wow=shared_wow;
                watch->reservation=reservation;
                watch->shutdown=CreateEventW(NULL,TRUE,FALSE,NULL);
                if(!watch->shutdown)error=GetLastError();
                InitializeListHead(&watch->management_labels);
                /* The first DOS record precedes the worker's first RPC.  Copy
                 * only its label before original GetNextVDMCommand releases
                 * VDMINFO; the record chain still owns all task state. */
                service_capture_initial_management_labels(watch);
                {
                    FILETIME ignored;
                    if (!GetProcessTimes(watch->process.ProcessHandle,&watch->started,&ignored,
                            &ignored,&ignored)) ZeroMemory(&watch->started,sizeof(watch->started));
                }
                /* Both resident worker kinds retain the authenticated root's
                 * outer-Console identity. NTW32 may later refresh it at direct
                 * delivery; DOS keeps the initial root for re-entry. */
                if(!error && !shared_wow && console) {
                    LIST_ENTRY *root_link;
                    for(root_link=service->connections.Flink;
                        root_link!=&service->connections;root_link=root_link->Flink) {
                        OPENNT_BASE_CONNECTION *root=CONTAINING_RECORD(root_link,
                            OPENNT_BASE_CONNECTION,service_link);
                        if(root->frontend_capability && service_root_console_matches(root,console) &&
                            root->console_member_count && !root->frontend_closing &&
                            WaitForSingleObject(root->process.ProcessHandle,0)==WAIT_TIMEOUT) {
                            error=service_copy_execution_console_members(connection,root);
                            watch->frontend_associated=TRUE;
                            break;
                        }
                    }
                }
                if (error || !RegisterWaitForSingleObject(&watch->wait,watch->process.ProcessHandle,
                    service_worker_terminated,watch,INFINITE,WT_EXECUTEONLYONCE)) {
                    if(!error)error=GetLastError();
                    CloseHandle(watch->process.ProcessHandle);
                    if(watch->shutdown)CloseHandle(watch->shutdown);
                    HeapFree(GetProcessHeap(),0,watch);
                    if(!connection->native_worker)BaseSrvCleanupVDMResources(&connection->process);
                } else {
                    InsertTailList(&service->worker_watches,&watch->link);
                    watch_added=TRUE;
}
            }
        }
        if (reserved_worker) CloseHandle(reserved_worker);
        if (!error) {
            service_prune_cancelled_frontends(service);
            InsertTailList(&service->connections,&connection->service_link);
            if(watch_added) {
                LIST_ENTRY *root_link;
                for(root_link=service->connections.Flink;root_link!=&service->connections;
                    root_link=root_link->Flink) {
                    OPENNT_BASE_CONNECTION *root=CONTAINING_RECORD(root_link,
                        OPENNT_BASE_CONNECTION,service_link);
                    if(root->frontend_capability && root->console==console)
                        root->frontend_admission_deadline=0;
                }
                service_signal_frontend_states(service);
            }
            if(connection->native_worker)service_signal_worker_states(service);
            *output=connection; *generation=connection->process.SequenceNumber;
        }
        else (void)OpenNtBaseRemoveProcess(&service->registry,&connection->process);
    }
    LeaveCriticalSection(&service->lock);
    if (error) {
        service_release_console_identities(connection);
        HeapFree(GetProcessHeap(),0,connection);
    }
    return error;
}
DWORD OpenNtBaseServiceDisconnect(OPENNT_BASE_CONNECTION *connection)
{
    OPENNT_BASE_SERVICE *service;
    DWORD error=0;
    if (!connection) return ERROR_INVALID_PARAMETER;
    service=connection->service;
    EnterCriticalSection(&service->lock);
    connection->vdm_exit_cancelled=TRUE;
    service_abandon_launch(connection);
    /* A client RPC context may close while resident COMMAND remains alive.
     * Only the retained process-exit watch may invoke the original cleanup. */
    if (connection->retired) {
        RemoveEntryList(&connection->retired_link);
    }
    else if (!OpenNtBaseRemoveProcess(&service->registry,&connection->process)) error=GetLastError();
    else {
        RemoveEntryList(&connection->service_link);
        broker_vdm_receipts_drain(&connection->streams);
        service_release_launcher_results(service,connection->process.SequenceNumber);
    }
    if (!error) service_clear_frontend(connection);
    if (!error) {
        LIST_ENTRY *link;
        /* Result latches and receipt signalling share the service lock with
         * process-exit rundown and the launcher's result query. */
        service_clear_pending_win32record(connection);
        service_clear_win32records(connection);
        for(link=service->connections.Flink;link!=&service->connections;link=link->Flink) {
            OPENNT_BASE_CONNECTION *root=CONTAINING_RECORD(link,OPENNT_BASE_CONNECTION,service_link);
            if(root->frontend_join_caller==connection) {
                root->frontend_join_caller=NULL;
                root->frontend_join_decision=-1;
            }
        }
        WakeAllConditionVariable(&service->frontend_changed);
    }
    LeaveCriticalSection(&service->lock);
    if (!error) {
        if(connection->vdm_exit_watch)
            (void)UnregisterWaitEx(connection->vdm_exit_watch,INVALID_HANDLE_VALUE);
        if(connection->vdm_exit_process)CloseHandle(connection->vdm_exit_process);
        if(connection->frontend_exit_watch)
            (void)UnregisterWaitEx(connection->frontend_exit_watch,INVALID_HANDLE_VALUE);
        if(connection->frontend_exit_process)CloseHandle(connection->frontend_exit_process);
        if (connection->frontend_capability) CloseHandle(connection->frontend_capability);
        if (connection->frontend_retire) CloseHandle(connection->frontend_retire);
        if (connection->frontend_restored) CloseHandle(connection->frontend_restored);
        if (connection->frontend_state_changed) CloseHandle(connection->frontend_state_changed);
        if (connection->worker_state_changed) CloseHandle(connection->worker_state_changed);
        if (connection->native_stop) CloseHandle(connection->native_stop);
        if (connection->native_closed) CloseHandle(connection->native_closed);
        if (connection->wow_start_event) CloseHandle(connection->wow_start_event);
        service_release_console_identities(connection);
        HeapFree(GetProcessHeap(),0,connection);
    }
    return error;
}
BOOL OpenNtBaseServicePeer(OPENNT_BASE_CONNECTION *connection,DWORD pid,DWORD generation)
{
    HANDLE process=NULL;
    DWORD error=OpenNtBaseServiceRetainPeer(connection,pid,generation,&process);
    if (process) CloseHandle(process);
    return error==0;
}
DWORD OpenNtBaseServiceAttachStream(OPENNT_BASE_CONNECTION *connection,DWORD pid,DWORD generation,
    DWORD role,HANDLE stream,DWORD *receipt)
{
    uint32_t id=0;
    DWORD error;
    if (!receipt) return ERROR_INVALID_PARAMETER;
    *receipt=0;
    if (!connection) return ERROR_ACCESS_DENIED;
    if (role<BROKER_VDM_STDIN || role>BROKER_VDM_STDERR) return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&connection->service->lock);
    error=OpenNtBaseServicePeer(connection,pid,generation) ?
        broker_vdm_receipt_accept(&connection->streams,role,stream,&id) : ERROR_ACCESS_DENIED;
    LeaveCriticalSection(&connection->service->lock);
    if (!error) *receipt=id;
    return error;
}
DWORD OpenNtBaseServiceRevokeStream(OPENNT_BASE_CONNECTION *connection,DWORD pid,DWORD generation,DWORD receipt)
{
    DWORD error;
    if (!connection) return ERROR_ACCESS_DENIED;
    EnterCriticalSection(&connection->service->lock);
    error=OpenNtBaseServicePeer(connection,pid,generation) ?
        broker_vdm_receipt_revoke(&connection->streams,generation,receipt) : ERROR_ACCESS_DENIED;
    LeaveCriticalSection(&connection->service->lock);
    return error;
}
DWORD OpenNtBaseServiceRetainPeer(OPENNT_BASE_CONNECTION *connection,DWORD pid,
    DWORD generation,HANDLE *output)
{
    DWORD error=0;
    if (!output) return ERROR_INVALID_PARAMETER;
    *output=NULL;
    if (!connection || !pid || !generation) return ERROR_ACCESS_DENIED;
    EnterCriticalSection(&connection->service->lock);
    if ((DWORD)connection->process.ClientId.UniqueProcess!=pid ||
        connection->process.SequenceNumber!=generation) error=ERROR_ACCESS_DENIED;
    else if (!OpenNtBaseRetainRegisteredProcess(&connection->service->registry,pid,generation,output))
        error=GetLastError();
    LeaveCriticalSection(&connection->service->lock);
    return error;
}
typedef BOOL (WINAPI *SERVICE_COMPARE_HANDLES)(HANDLE,HANDLE);
typedef NTSTATUS (NTAPI *SERVICE_QUERY_OBJECT)(HANDLE,ULONG,PVOID,ULONG,PULONG);

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
                if(!SetEvent(root->frontend_capability)) {
                    root->frontend_join_caller=NULL;error=GetLastError();break;
                }
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
    HANDLE process,HANDLE capability,HANDLE retire,HANDLE restored)
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
    caller->frontend_starting=FALSE;
    LeaveCriticalSection(&caller->service->lock);
}

DWORD OpenNtBaseServiceStartFrontend(OPENNT_BASE_CONNECTION *caller,DWORD pid,
    DWORD generation,uint64_t window,BOOL borrowed,HANDLE *root,HANDLE *capability,HANDLE *restored)
{
    static LONG serial;
    WCHAR image[MAX_PATH],name[96],command[1024],*slash;
    HANDLE pipe=INVALID_HANDLE_VALUE,child=INVALID_HANDLE_VALUE,creator=NULL;
    HANDLE notification=NULL,retire=NULL,ack=NULL,event=NULL,timer=NULL,verified=NULL;
    HANDLE inherited[5],unused_retire=NULL;
    SECURITY_ATTRIBUTES security={sizeof(security),NULL,TRUE};
    STARTUPINFOEXW startup={0};PROCESS_INFORMATION process={0};
    frontend_bootstrap_reply reply={0};
    const char expected[APP_VERSION_BYTES]=APP_VERSION;
    LARGE_INTEGER due;SIZE_T bytes=0;
    DWORD error,create=0,length,root_generation=0;
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
    length=GetModuleFileNameW(NULL,image,ARRAYSIZE(image));
    if(!length || length>=ARRAYSIZE(image) || !(slash=wcsrchr(image,L'\\')))
        {error=ERROR_BAD_PATHNAME;goto done;}
    if(wcscpy_s(slash+1,ARRAYSIZE(image)-(size_t)(slash+1-image),L"ntkvm.exe"))
        {error=ERROR_FILENAME_EXCED_RANGE;goto done;}
    swprintf_s(name,ARRAYSIZE(name),L"\\\\.\\pipe\\ntvdm-frontend-start-%lu-%lu",
        GetCurrentProcessId(),(DWORD)InterlockedIncrement(&serial));
    pipe=CreateNamedPipeW(name,PIPE_ACCESS_DUPLEX|FILE_FLAG_OVERLAPPED|FILE_FLAG_FIRST_PIPE_INSTANCE,
        PIPE_TYPE_BYTE|PIPE_WAIT|PIPE_REJECT_REMOTE_CLIENTS,1,4096,4096,0,NULL);
    if(pipe==INVALID_HANDLE_VALUE){error=GetLastError();goto done;}
    child=CreateFileW(name,GENERIC_READ|GENERIC_WRITE,0,&security,OPEN_EXISTING,
        FILE_FLAG_OVERLAPPED|SECURITY_SQOS_PRESENT|SECURITY_IDENTIFICATION,NULL);
    if(child==INVALID_HANDLE_VALUE){error=GetLastError();goto done;}
    notification=CreateEventW(&security,TRUE,FALSE,NULL);
    retire=CreateEventW(&security,TRUE,FALSE,NULL);
    ack=CreateEventW(&security,TRUE,FALSE,NULL);
    event=CreateEventW(NULL,TRUE,FALSE,NULL);
    if(!notification || !retire || !ack || !event){error=GetLastError();goto done;}
    {
        OVERLAPPED io={0};DWORD ignored;io.hEvent=event;
        if(!ConnectNamedPipe(pipe,&io) && GetLastError()!=ERROR_PIPE_CONNECTED){
            error=GetLastError();CancelIoEx(pipe,&io);GetOverlappedResult(pipe,&io,&ignored,TRUE);goto done;
        }
    }
    InitializeProcThreadAttributeList(NULL,1,0,&bytes);
    startup.lpAttributeList=HeapAlloc(GetProcessHeap(),0,bytes);
    if(!startup.lpAttributeList){error=ERROR_NOT_ENOUGH_MEMORY;goto done;}
    if(!InitializeProcThreadAttributeList(startup.lpAttributeList,1,0,&bytes))
        {error=GetLastError();goto done;}
    attributes=TRUE;
    inherited[0]=child;inherited[1]=creator;inherited[2]=notification;
    inherited[3]=retire;inherited[4]=ack;
    if(!UpdateProcThreadAttribute(startup.lpAttributeList,0,PROC_THREAD_ATTRIBUTE_HANDLE_LIST,
        inherited,sizeof(inherited),NULL,NULL)){error=GetLastError();goto done;}
    startup.StartupInfo.cb=sizeof(startup);
    if(swprintf_s(command,ARRAYSIZE(command),L"\"%ls\" --session %Ix %Ix %Ix %Ix %Ix %u %I64x",
        image,(UINT_PTR)child,(UINT_PTR)creator,(UINT_PTR)notification,(UINT_PTR)retire,
        (UINT_PTR)ack,borrowed!=FALSE,window)<0){error=ERROR_FILENAME_EXCED_RANGE;goto done;}
    if(!CreateProcessW(image,command,NULL,NULL,TRUE,
        CREATE_SUSPENDED|EXTENDED_STARTUPINFO_PRESENT|DETACHED_PROCESS,NULL,NULL,
        &startup.StartupInfo,&process)){error=GetLastError();goto done;}
    error=broker_frontend_admit(caller,pid,generation,process.hProcess,notification,retire,ack);
    if(error)goto done;
    if(ResumeThread(process.hThread)==(DWORD)-1){error=GetLastError();goto done;}
    CloseHandle(process.hThread);process.hThread=NULL;
    CloseHandle(child);child=INVALID_HANDLE_VALUE;
    timer=CreateWaitableTimerW(NULL,TRUE,NULL);
    if(!timer){error=GetLastError();goto done;}
    due.QuadPart=-10LL*1000*10000;
    if(!SetWaitableTimer(timer,&due,0,NULL,NULL,FALSE)){error=GetLastError();goto done;}
    error=frontend_request_transfer(pipe,process.hProcess,timer,event,FALSE,&reply,sizeof(reply));
    if(error==ERROR_OPERATION_ABORTED && WaitForSingleObject(timer,0)==WAIT_OBJECT_0)
        error=ERROR_TIMEOUT;
    if(error==ERROR_BROKEN_PIPE) {
        HANDLE waits[2]={process.hProcess,timer};
        /* Pipe rundown can precede the process object's exit notification.
         * Attribute an early fault within the existing admission deadline;
         * never add an unbounded wait or replay this bootstrap. */
        (void)WaitForMultipleObjects(2,waits,FALSE,INFINITE);
    }
    if(error && WaitForSingleObject(process.hProcess,0)==WAIT_OBJECT_0) {
        DWORD status;
        if(GetExitCodeProcess(process.hProcess,&status) && status)error=status;
    }
    if(error)goto done;
    if(reply.version!=FRONTEND_BOOTSTRAP_VERSION || memcmp(reply.application,expected,sizeof(expected)))
        {error=ERROR_REVISION_MISMATCH;goto done;}
    if(reply.status){error=reply.status;goto done;}
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
    if(child!=INVALID_HANDLE_VALUE)CloseHandle(child);
    if(pipe!=INVALID_HANDLE_VALUE)CloseHandle(pipe);
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
        WakeAllConditionVariable(&root->service->frontend_changed);
        error=ERROR_SUCCESS;
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

DWORD OpenNtBaseServiceCompleteWorkerChannel(OPENNT_BASE_CONNECTION *connection,DWORD pid,DWORD generation,
    DWORD request,DWORD exit_code)
{
    DWORD error=ERROR_ACCESS_DENIED;
    if(!connection)return ERROR_INVALID_PARAMETER;
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
                    HANDLE owned_receipt=NULL;
                    if(!DuplicateHandle(GetCurrentProcess(),receipt,GetCurrentProcess(),&owned_receipt,
                        EVENT_MODIFY_STATE,FALSE,0))error=GetLastError();
                    else {
                        record->receipt=owned_receipt;owned_receipt=NULL;
                        record->process_id=target_pid;
                        service_query_native_image(target_pid,record->image);
                        service_signal_frontend_states(connection->service);
                        error=ERROR_SUCCESS;
                    }
                    if(owned_receipt)CloseHandle(owned_receipt);
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

/* Called under the service lock, with an original-command-selected worker.
 * The route belongs to the root, not to the command submitting descendant. */
static DWORD service_attach_frontend(OPENNT_BASE_CONNECTION *connection,
    HANDLE worker,HANDLE pipe,HANDLE ready)
{
    OPENNT_FRONTEND_ROUTE *route=NULL;
    EVENT_BASIC_INFORMATION event_info;
    LIST_ENTRY *link;
    DWORD error,flags;
    if(connection->frontend_closing) return ERROR_PIPE_NOT_CONNECTED;
    if (NtQueryEvent(ready,EventBasicInformation,&event_info,sizeof(event_info),NULL)<0 ||
        event_info.EventType!=NotificationEvent) { error=ERROR_INVALID_PARAMETER;goto done; }
    if (GetFileType(pipe)!=FILE_TYPE_PIPE ||
        !GetNamedPipeInfo(pipe,&flags,NULL,NULL,NULL) || (flags&PIPE_TYPE_MESSAGE)) {
        error=ERROR_INVALID_PARAMETER;goto done;
    }
    link=connection->service->frontend_routes.Flink;
    while (link!=&connection->service->frontend_routes) {
        OPENNT_FRONTEND_ROUTE *existing=CONTAINING_RECORD(link,OPENNT_FRONTEND_ROUTE,link);
        link=link->Flink;
        if (GetProcessId(existing->worker)==GetProcessId(worker)) {
            if (!existing->root) { error=ERROR_PIPE_NOT_CONNECTED;goto done; }
            if (existing->root==connection && !existing->pipe && !existing->delivered)
                continue; /* Authorized pending request; replace only after success. */
            if (WaitForSingleObject(existing->root->process.ProcessHandle,0)==WAIT_OBJECT_0) {
                service_delete_frontend(existing);continue;
            }
            /* A live root owns presentation across nested command lifetimes.
             * Do not permit a child launcher to replace it, even after Take. */
            error=ERROR_ALREADY_EXISTS;goto done;
        }
    }
    route=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*route));
    if (!route) { error=ERROR_NOT_ENOUGH_MEMORY;goto done; }
    if (!DuplicateHandle(GetCurrentProcess(),ready,GetCurrentProcess(),&route->ready,
            SYNCHRONIZE,FALSE,0)) { error=GetLastError();goto done; }
    if (!DuplicateHandle(GetCurrentProcess(),pipe,GetCurrentProcess(),&route->pipe,
            0,FALSE,DUPLICATE_SAME_ACCESS)) { error=GetLastError();goto done; }
    if (!DuplicateHandle(GetCurrentProcess(),worker,GetCurrentProcess(),&route->worker,
            PROCESS_QUERY_LIMITED_INFORMATION|SYNCHRONIZE,FALSE,0)) { error=GetLastError();goto done; }
    route->root=connection;
    link=connection->service->frontend_routes.Flink;
    while (link!=&connection->service->frontend_routes) {
        OPENNT_FRONTEND_ROUTE *pending=CONTAINING_RECORD(link,OPENNT_FRONTEND_ROUTE,link);
        link=link->Flink;
        if (pending->root==connection && !pending->pipe && !pending->delivered &&
            GetProcessId(pending->worker)==GetProcessId(worker)) service_delete_frontend(pending);
    }
    InsertTailList(&connection->service->frontend_routes,&route->link);
    route=NULL;
    WakeAllConditionVariable(&connection->service->frontend_changed);
    service_signal_frontend_states(connection->service);
    error=ERROR_SUCCESS;
done:
    if (route) {
        if (route->ready) CloseHandle(route->ready);
        if (route->pipe) CloseHandle(route->pipe);
        HeapFree(GetProcessHeap(),0,route);
    }
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
    for (link=connection->service->frontend_routes.Flink;
         link!=&connection->service->frontend_routes;link=link->Flink) {
        OPENNT_FRONTEND_ROUTE *route=CONTAINING_RECORD(link,OPENNT_FRONTEND_ROUTE,link);
        if (GetProcessId(route->worker)==GetProcessId(worker)) {
            if (!route->root) error=ERROR_PIPE_NOT_CONNECTED;
            else if (route->root->process.SequenceNumber==root_generation)
                error=route->pipe || route->delivered ? ERROR_ALREADY_EXISTS : ERROR_SUCCESS;
            else error=ERROR_ACCESS_DENIED;
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
        if (!SetEvent(root->frontend_capability)) {
            error=GetLastError();HeapFree(GetProcessHeap(),0,pending);break;
        }
        pending->root=root;pending->worker=worker;worker=NULL;
        pending->request=generation;
        pending->native_worker=connection->selected_native_generation!=0 ||
            connection->reservation_kind==OPENNT_BASE_WORKER_NATIVE;
        InsertTailList(&connection->service->frontend_routes,&pending->link);
        connection->frontend_request_root=root_generation;
        service_signal_frontend_states(connection->service);
        error=ERROR_SUCCESS;
        break;
    }
done:
    if (root_process) CloseHandle(root_process);
    if (worker) CloseHandle(worker);
    LeaveCriticalSection(&connection->service->lock);
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

/* The broker alone owns the worker census and bounded startup admission.
 * This is a Console-root lifetime check, never a DOS/native task scheduler. */
static BOOL service_root_has_worker(OPENNT_BASE_CONNECTION *root)
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
     * A newly associated worker cancels it. NTKVM owns no local deadline. */
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
    LeaveCriticalSection(&service->lock);
    return ERROR_SUCCESS;
}
DWORD OpenNtBaseServiceRetireExpiredFrontends(OPENNT_BASE_SERVICE *service)
{
    LIST_ENTRY *link;
    BOOL changed=FALSE;
    if(!service)return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&service->lock);
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
    return ERROR_SUCCESS;
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

/* Frontend wakeups are for presentation attachment, never target execution. */
static DWORD service_frontend_idle(OPENNT_BASE_CONNECTION *root)
{
    LIST_ENTRY *link;
    for (link=root->service->connections.Flink;link!=&root->service->connections;link=link->Flink) {
        OPENNT_BASE_CONNECTION *caller=CONTAINING_RECORD(link,OPENNT_BASE_CONNECTION,service_link);
        if (caller->frontend_request_root==root->process.SequenceNumber)
            return ERROR_NOT_FOUND;
    }
    return ResetEvent(root->frontend_capability) ? ERROR_NOT_FOUND : GetLastError();
}

DWORD OpenNtBaseServiceQueueNativeChannel(OPENNT_BASE_CONNECTION *connection,DWORD pid,
    DWORD generation,HANDLE capability,HANDLE channel,const WCHAR image[OPENNT_BASE_WORKER_IMAGE_CHARS],
    DWORD channel_owner)
{
    HANDLE root_process=NULL,execution=NULL,owned=NULL,worker=NULL,frontend=NULL;
    DWORD root_generation=0,worker_generation=0,flags,server_pid,client_pid,error;
    OPENNT_BASE_WIN32RECORD *record=NULL;
    LIST_ENTRY *link;
    if (!connection || !image) return ERROR_ACCESS_DENIED;
    EnterCriticalSection(&connection->service->lock);
    error=ERROR_ACCESS_DENIED;
    if (!OpenNtBaseServicePeer(connection,pid,generation) || connection->process.fVDM || connection->native_worker) goto done;
    /* Only the producer's connected byte-pipe pair. In production the broker
     * creates both ends; neither a public listener nor a client pipe crosses
     * SubmitNativeRequest. Tests exercise this internal validation directly. */
    if (GetFileType(channel)!=FILE_TYPE_PIPE ||
        !GetNamedPipeInfo(channel,&flags,NULL,NULL,NULL) ||
        !(flags&PIPE_SERVER_END) || (flags&PIPE_TYPE_MESSAGE) ||
        !GetNamedPipeServerProcessId(channel,&server_pid) || server_pid!=channel_owner ||
        !GetNamedPipeClientProcessId(channel,&client_pid) || client_pid!=channel_owner) {
        error=ERROR_INVALID_PARAMETER;goto done;
    }
    if (connection->frontend_channel) { error=ERROR_BUSY;goto done; }
    if(image[0]) {
        SIZE_T characters=0;
        while(characters<OPENNT_BASE_WORKER_IMAGE_CHARS && image[characters])++characters;
        if(characters==OPENNT_BASE_WORKER_IMAGE_CHARS) { error=ERROR_INVALID_DATA;goto done; }
        record=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*record));
        if(!record) { error=ERROR_NOT_ENOUGH_MEMORY;goto done; }
        record->launcher_generation=generation;
        lstrcpynW(record->image,image,OPENNT_BASE_WORKER_IMAGE_CHARS);
    }
    error=OpenNtBaseServiceRetainFrontendRoot(connection,pid,generation,capability,
        &root_process,&root_generation);
    if (error) goto done;
    error=OpenNtBaseServiceAcquireConsoleContext(connection,pid,generation,capability,&execution);
    if (error) goto done;
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
        if (!DuplicateHandle(GetCurrentProcess(),capability,GetCurrentProcess(),&frontend,
                SYNCHRONIZE,FALSE,0)) { error=GetLastError();goto done; }
    }
    if (!DuplicateHandle(GetCurrentProcess(),channel,GetCurrentProcess(),&owned,
        0,FALSE,DUPLICATE_SAME_ACCESS)) { error=GetLastError();goto done; }
    error=ERROR_ACCESS_DENIED;
    for (link=connection->service->connections.Flink;link!=&connection->service->connections;link=link->Flink) {
        OPENNT_BASE_CONNECTION *root=CONTAINING_RECORD(link,OPENNT_BASE_CONNECTION,service_link);
        if (root->process.SequenceNumber!=root_generation) continue;
        connection->frontend_channel=owned;owned=NULL;
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
    if (owned) CloseHandle(owned);
    if (record) HeapFree(GetProcessHeap(),0,record);
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

DWORD OpenNtBaseServiceSubmitNativeRequest(OPENNT_BASE_CONNECTION *caller,DWORD pid,
    DWORD generation,HANDLE frontend,DWORD bytes,BYTE *payload,
    HANDLE *target,HANDLE *receipt,DWORD *request)
{
    static LONG serial;
    WCHAR name[96],image[OPENNT_BASE_WORKER_IMAGE_CHARS]={0};
    HANDLE server=INVALID_HANDLE_VALUE,client=INVALID_HANDLE_VALUE,event=NULL,worker=NULL;
    HANDLE exported_target=NULL,exported_receipt=NULL,root=NULL;
    DWORD root_generation=0;
    native_request_header header={NATIVE_REQUEST_VERSION,bytes};
    native_request_reply reply={0};DWORD error;
    if(!target || !receipt || !request)return ERROR_INVALID_PARAMETER;
    *target=*receipt=NULL;*request=0;
    if(!caller || !OpenNtBaseServicePeer(caller,pid,generation))return ERROR_ACCESS_DENIED;
    /* Authenticate the root before interpreting launch data or selecting a
     * worker. The copied request never supplies a worker pipe or its owner. */
    error=OpenNtBaseServiceRetainFrontendRoot(caller,pid,generation,frontend,&root,&root_generation);
    if(error)return error;
    CloseHandle(root);
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
    event=CreateEventW(NULL,TRUE,FALSE,NULL);
    if(!event){error=GetLastError();goto done;}
    swprintf_s(name,ARRAYSIZE(name),L"\\\\.\\pipe\\ntsrv-native-%lu-%lu",
        GetCurrentProcessId(),(DWORD)InterlockedIncrement(&serial));
    server=CreateNamedPipeW(name,PIPE_ACCESS_DUPLEX|FILE_FLAG_OVERLAPPED|FILE_FLAG_FIRST_PIPE_INSTANCE,
        PIPE_TYPE_BYTE|PIPE_WAIT|PIPE_REJECT_REMOTE_CLIENTS,1,65536,65536,0,NULL);
    if(server==INVALID_HANDLE_VALUE){error=GetLastError();goto done;}
    client=CreateFileW(name,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,
        FILE_FLAG_OVERLAPPED|SECURITY_SQOS_PRESENT|SECURITY_IDENTIFICATION,NULL);
    if(client==INVALID_HANDLE_VALUE){error=GetLastError();goto done;}
    {
        OVERLAPPED io={0};DWORD ignored;io.hEvent=event;
        if(!ConnectNamedPipe(server,&io) && GetLastError()!=ERROR_PIPE_CONNECTED) {
            error=GetLastError();CancelIoEx(server,&io);GetOverlappedResult(server,&io,&ignored,TRUE);goto done;
        }
    }
    error=OpenNtBaseServiceQueueNativeChannel(caller,pid,generation,frontend,server,image,GetCurrentProcessId());
    if(error)goto done;
    CloseHandle(server);server=INVALID_HANDLE_VALUE;
    error=frontend_request_transfer(client,worker,caller->process.ProcessHandle,event,TRUE,&header,sizeof(header));
    if(!error && bytes)error=frontend_request_transfer(client,worker,caller->process.ProcessHandle,event,TRUE,payload,bytes);
    if(!error)error=frontend_request_transfer(client,worker,caller->process.ProcessHandle,event,FALSE,&reply,sizeof(reply));
    if(error)goto done;
    error=broker_native_reply_status(&reply,bytes!=0);
    if(error)goto done;
    if(!bytes){error=ERROR_SUCCESS;goto done;}
    /* Existing worker exports name the authenticated launcher's namespace.
     * Reclaim those exact exports into service-owned typed RPC attachments;
     * the launcher never sees or operates the worker control pipe. */
    if(!DuplicateHandle(caller->process.ProcessHandle,(HANDLE)(ULONG_PTR)reply.target,
        GetCurrentProcess(),&exported_target,0,FALSE,DUPLICATE_SAME_ACCESS|DUPLICATE_CLOSE_SOURCE) ||
        !DuplicateHandle(caller->process.ProcessHandle,(HANDLE)(ULONG_PTR)reply.receipt,
        GetCurrentProcess(),&exported_receipt,0,FALSE,DUPLICATE_SAME_ACCESS|DUPLICATE_CLOSE_SOURCE)) {
        error=GetLastError();goto done;
    }
    EnterCriticalSection(&caller->service->lock);
    {
        OPENNT_BASE_WIN32RECORD *record=service_native_result_record(caller,reply.request,NULL);
        SERVICE_COMPARE_HANDLES compare=(SERVICE_COMPARE_HANDLES)GetProcAddress(
            GetModuleHandleW(L"kernelbase.dll"),"CompareObjectHandles");
        if(!record || record->control || !compare ||
            record->process_id!=GetProcessId(exported_target) ||
            !compare(record->receipt,exported_receipt))error=ERROR_INVALID_DATA;
        else {
            record->control=client;client=INVALID_HANDLE_VALUE;
            record->control_worker=worker;worker=NULL;
            *target=exported_target;exported_target=NULL;
            *receipt=exported_receipt;exported_receipt=NULL;
            *request=reply.request;error=ERROR_SUCCESS;
        }
    }
    LeaveCriticalSection(&caller->service->lock);
done:
    if(exported_target)CloseHandle(exported_target);
    if(exported_receipt)CloseHandle(exported_receipt);
    if(server!=INVALID_HANDLE_VALUE)CloseHandle(server);
    if(client!=INVALID_HANDLE_VALUE)CloseHandle(client);
    if(event)CloseHandle(event);if(worker)CloseHandle(worker);
    return error;
}

DWORD OpenNtBaseServiceFinishNativeRequest(OPENNT_BASE_CONNECTION *caller,DWORD pid,
    DWORD generation,DWORD request,DWORD *exit_code)
{
    HANDLE pipe=NULL,worker=NULL,event=NULL;DWORD error,idle_worker=0;
    native_request_completion completion={0};
    if(!caller || !request || !exit_code)return ERROR_INVALID_PARAMETER;
    if(!OpenNtBaseServicePeer(caller,pid,generation))return ERROR_ACCESS_DENIED;
    EnterCriticalSection(&caller->service->lock);
    {
        OPENNT_BASE_CONNECTION *owner=NULL;
        OPENNT_BASE_WIN32RECORD *record=service_native_result_record(caller,request,&owner);
        if(!record)error=ERROR_NOT_FOUND;
        else if(!record->completed)error=ERROR_NOT_READY;
        else if(record->completion_error)error=ERROR_SUCCESS;
        else if(!record->control || !record->control_worker)error=ERROR_INVALID_STATE;
        else if(!DuplicateHandle(GetCurrentProcess(),record->control,GetCurrentProcess(),&pipe,
            0,FALSE,DUPLICATE_SAME_ACCESS) ||
            !DuplicateHandle(GetCurrentProcess(),record->control_worker,GetCurrentProcess(),&worker,
                SYNCHRONIZE,FALSE,0))error=GetLastError();
        else {error=ERROR_SUCCESS;idle_worker=owner->native_worker ? owner->process.SequenceNumber : 0;}
    }
    LeaveCriticalSection(&caller->service->lock);
    if(error)goto done;
    /* Consuming the broker result is the one-time gate, including worker death.
     * Never demand a reply from a failed worker to report its broker failure. */
    error=OpenNtBaseServiceNativeExitCode(caller,pid,generation,request,exit_code);
    if(error)goto done;
    event=CreateEventW(NULL,TRUE,FALSE,NULL);
    if(!event){error=GetLastError();goto done;}
    error=frontend_request_transfer(pipe,worker,caller->process.ProcessHandle,event,FALSE,&completion,sizeof(completion));
    if(!error)error=broker_native_completion_status(&completion);
    if(!error && idle_worker && (completion.flags & NATIVE_COMPLETION_CONSOLE_EMPTY)) {
        EnterCriticalSection(&caller->service->lock);
        service_retire_completed_root(caller,idle_worker);
        LeaveCriticalSection(&caller->service->lock);
    }
done:
    if(event)CloseHandle(event);if(pipe)CloseHandle(pipe);if(worker)CloseHandle(worker);
    return error;
}

DWORD OpenNtBaseServiceTakeWorkerChannel(OPENNT_BASE_CONNECTION *root,DWORD pid,
    DWORD generation,HANDLE *channel,HANDLE *caller_process,HANDLE *execution,HANDLE *frontend,DWORD *request)
{
    LIST_ENTRY *link;
    DWORD error=ERROR_ACCESS_DENIED;
    if (!channel || !caller_process || !execution || !frontend || !request) return ERROR_INVALID_PARAMETER;
    *channel=NULL;*caller_process=NULL;*execution=NULL;*frontend=NULL;*request=0;
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
        if(caller->pending_win32record && (root->native_inflight==MAXDWORD ||
            (root->native_inflight && root->native_activity_root!=caller->frontend_channel_root))) {
            error=ERROR_BUSY;goto done;
        }
        /* The visible Console identity belongs to the authenticated
         * frontend root, not to this short-lived direct launcher. */
        {
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
        }
        if(error)goto done;
        /* The pinned sender process permits only this direct channel's finite
         * stream/capability exchange, never an arbitrary broker duplication API. */
        if (!DuplicateHandle(GetCurrentProcess(),caller->process.ProcessHandle,GetCurrentProcess(),
                caller_process,SYNCHRONIZE|PROCESS_QUERY_LIMITED_INFORMATION|PROCESS_DUP_HANDLE,FALSE,0) ||
            !DuplicateHandle(GetCurrentProcess(),caller->frontend_channel,GetCurrentProcess(),
                channel,0,FALSE,DUPLICATE_SAME_ACCESS) ||
            !DuplicateHandle(GetCurrentProcess(),caller->frontend_execution,GetCurrentProcess(),
                execution,SYNCHRONIZE,FALSE,0) ||
            !DuplicateHandle(GetCurrentProcess(),caller->channel_frontend,GetCurrentProcess(),
                frontend,SYNCHRONIZE,FALSE,0)) { error=GetLastError();goto done; }
        if(caller->pending_win32record) {
            error=service_next_win32record(root,&caller->pending_win32record->request);
            if(error)goto done;
            InsertTailList(&root->win32records,&caller->pending_win32record->link);
            *request=caller->pending_win32record->request;
            caller->pending_win32record=NULL;
            ++root->native_inflight;
            root->native_activity_root=caller->frontend_channel_root;
        }
        service_clear_frontend_channel(caller);
        service_signal_frontend_states(root->service);
        error=ERROR_SUCCESS;goto done;
    }
    error=ERROR_NOT_FOUND;
done:
    if (error) {
        if (*channel) CloseHandle(*channel);
        if (*caller_process) CloseHandle(*caller_process);
        if (*execution) CloseHandle(*execution);
        if (*frontend) { CloseHandle(*frontend);*frontend=NULL; }
        *channel=NULL;*caller_process=NULL;*execution=NULL;
    }
    LeaveCriticalSection(&root->service->lock);
    return error;
}

DWORD OpenNtBaseServiceGetNextNativeCommand(OPENNT_BASE_CONNECTION *connection,DWORD pid,
    DWORD generation,HANDLE *channel,HANDLE *caller_process,HANDLE *execution,HANDLE *frontend,DWORD *request)
{
    DWORD error;
    if (!connection) return ERROR_ACCESS_DENIED;
    if(!channel || !caller_process || !execution || !frontend || !request)return ERROR_INVALID_PARAMETER;
    *channel=*caller_process=*execution=*frontend=NULL;*request=0;
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
        error=OpenNtBaseServiceTakeWorkerChannel(connection,pid,generation,
            channel,caller_process,execution,frontend,request);
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
    /* Independent native worker attachment is not a pending DOS record.
     * The originating launcher can already have returned its direct result. */
    for(link=root->service->frontend_routes.Flink;link!=&root->service->frontend_routes;link=link->Flink) {
        OPENNT_FRONTEND_ROUTE *route=CONTAINING_RECORD(link,OPENNT_FRONTEND_ROUTE,link);
        if(route->root!=root || !route->native_worker || route->pipe || route->delivered ||
            WaitForSingleObject(route->worker,0)!=WAIT_TIMEOUT)continue;
        error=DuplicateHandle(GetCurrentProcess(),route->worker,GetCurrentProcess(),worker,
            PROCESS_QUERY_LIMITED_INFORMATION|SYNCHRONIZE,FALSE,0) ? ERROR_SUCCESS : GetLastError();
        if(!error)*request=route->request;
        goto done;
    }
    for (link=root->service->connections.Flink;link!=&root->service->connections;link=link->Flink) {
        OPENNT_BASE_CONNECTION *caller=CONTAINING_RECORD(link,OPENNT_BASE_CONNECTION,service_link);
        if (caller->frontend_request_root!=generation) continue;
        error=OpenNtBaseServiceRetainCommandWorker(caller,
            (DWORD)(ULONG_PTR)caller->process.ClientId.UniqueProcess,caller->process.SequenceNumber,worker);
        if (!error) { *request=caller->process.SequenceNumber;goto done; }
        caller->frontend_request_root=0; /* Completed/departed original caller, not a new task. */
    }
    error=service_frontend_idle(root);
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
        if(route->root!=root || !route->native_worker || route->request!=request)continue;
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
    for (link=root->service->connections.Flink;link!=&root->service->connections;link=link->Flink) {
        OPENNT_BASE_CONNECTION *caller=CONTAINING_RECORD(link,OPENNT_BASE_CONNECTION,service_link);
        if (caller->process.SequenceNumber!=request || caller->frontend_request_root!=generation) continue;
        error=OpenNtBaseServiceRetainCommandWorker(caller,
            (DWORD)(ULONG_PTR)caller->process.ClientId.UniqueProcess,request,&worker);
        if (!error) error=service_attach_frontend(root,worker,pipe,ready);
        if (!error || error==ERROR_ALREADY_EXISTS) caller->frontend_request_root=0;
        break;
    }
done:
    if (worker) CloseHandle(worker);
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
    if (connection->wow) { error=ERROR_NOT_SUPPORTED; goto done; }
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

typedef struct service_vdm_admission {
    OPENNT_BASE_CONNECTION *connection;
    DWORD pid,generation,binary;
    HANDLE frontend,parent,parent_export;
    uint32_t receipt;
    BOOL registered;
} service_vdm_admission;

/* Service lock held. The owner-specific boundary proves completion before
 * entering this shared Console-return/retirement path: original nonzero
 * DosSesId worker exit, or native final I/O acknowledgement with an empty
 * backend Console. Native passes its idle worker generation; DOS passes zero.
 * PIF CloseOnExit is never reimplemented here. */
static void service_retire_completed_root(OPENNT_BASE_CONNECTION *parent,DWORD idle_worker)
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
    if(wcscpy_s(slash+1,ARRAYSIZE(image)-(size_t)(slash+1-image),L"ntw32.exe") ||
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

/* Reintroduce only the modern transport for the original ConsoleHandle
 * discriminator.  srvvdm.c still decides whether that ConsoleRecord is
 * READY/BUSY and performs all command-record mutation. */
static DWORD service_bind_existing_console(OPENNT_BASE_CONNECTION *connection)
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
    /* Only NTKVM roots report the visible Console membership. A launcher
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
    status=OpenNtBaseDispatchOperation((PCSR_API_MSG)&message,BROKER_VDM_GET_NEXT,
        sizeof(message.u.GetNextVDMCommand));
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
