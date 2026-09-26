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
#include "basesrv-exe/transport/vdm_receipt.h"
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
    OPENNT_BASE_INTERACTIVE_SCOPE interactive;
    LIST_ENTRY connections;
    LIST_ENTRY retired_connections;
    LIST_ENTRY worker_watches;
    LIST_ENTRY frontend_routes;
    OPENNT_BASE_CONSOLE_QUERY console_query;
    void *console_query_context;
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
    HANDLE console;
    BOOL wow;
    BOOL retired;
    BOOL pending_creation;
    BOOL registered_worker;
    /* The original BaseSrvDupStandardHandles copies these caller streams
     * into the suspended worker during UpdateDOSEntry.  The standalone
     * broker temporarily owns receipt-backed duplicates until that point;
     * it must release them afterwards so a pipe reader can observe EOF. */
    uint32_t pending_standard_streams[3];
    HANDLE parent_wait; /* Borrowed from this connection's receipt table. */
    uint32_t parent_receipt,completed_receipt;
    DWORD completed_exit_code;
    BOOL worker_failed;
    HANDLE frontend_capability; /* Root lease, independent of command lifetime. */
    DWORD frontend_request_root; /* Original pending command asks this root for I/O. */
    LIST_ENTRY service_link;
    LIST_ENTRY retired_link;
};
typedef struct OPENNT_FRONTEND_ROUTE {
    LIST_ENTRY link;
    OPENNT_BASE_CONNECTION *root; /* Removed before this connection is freed. */
    HANDLE worker;
    HANDLE pipe;
    HANDLE ready;
    BOOL delivered;
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
    FILETIME started;
    /* The original GetNextVDMCommand body frees VDMINFO immediately after
     * copying it to the worker.  These are labels keyed to surviving original
     * DOSRECORD identities.  They never model execution lifetime: srvvdm.c's
     * DOSRecord chain remains the sole source of task depth and state. */
    LIST_ENTRY management_labels;
    BOOL termination_requested;
} OPENNT_BASE_WORKER_WATCH;
typedef struct OPENNT_BASE_MANAGEMENT_LABEL {
    LIST_ENTRY link;
    PDOSRECORD record;
    HANDLE parent_wait;
    ULONG task;
    WCHAR image[OPENNT_BASE_WORKER_IMAGE_CHARS];
} OPENNT_BASE_MANAGEMENT_LABEL;
typedef struct OPENNT_BASE_CONSOLE_CANDIDATE {
    HANDLE process;
    ULONG generation;
    HANDLE console;
} OPENNT_BASE_CONSOLE_CANDIDATE;
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
static void service_delete_frontend(OPENNT_FRONTEND_ROUTE *route)
{
    RemoveEntryList(&route->link);
    if (route->pipe) CloseHandle(route->pipe);
    if (route->ready) CloseHandle(route->ready);
    CloseHandle(route->worker);
    HeapFree(GetProcessHeap(),0,route);
}
static void service_clear_frontend(OPENNT_BASE_CONNECTION *connection)
{
    LIST_ENTRY *link=connection->service->frontend_routes.Flink;
    while (link!=&connection->service->frontend_routes) {
        OPENNT_FRONTEND_ROUTE *route=CONTAINING_RECORD(link,OPENNT_FRONTEND_ROUTE,link);
        link=link->Flink;
        if (route->root==connection || (!route->pipe && !route->delivered &&
            route->request==connection->process.SequenceNumber)) {
            if (route->delivered) service_delete_frontend(route);
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
            parent->console==console && parent->parent_wait) {
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
    BaseSrvCleanupVDMResources(&watch->process);
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
        broker_vdm_receipts_drain(&connection->streams);
        connection->retired=TRUE;
        InsertTailList(&watch->service->retired_connections,&connection->retired_link);
    }
    RemoveEntryList(&watch->link);
    WakeAllConditionVariable(&watch->service->frontend_changed);
    notify=watch->service->empty_notify;
    notify_context=watch->service->empty_notify_context;
    LeaveCriticalSection(&watch->service->lock);
service_clear_management_labels(watch);
    CloseHandle(watch->process.ProcessHandle);
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
    if (connection->process.fVDM) return;
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
    GetSystemTimeAsFileTime(&now);
    service->management_epoch=((uint64_t)now.dwHighDateTime<<32)|now.dwLowDateTime;
    service->management_epoch^=(uint64_t)GetCurrentProcessId()<<17;
    if (!service->management_epoch) service->management_epoch=1;
    if (!OpenNtBaseInitializeProcessRegistry(&service->registry)) {
        DeleteCriticalSection(&service->lock); HeapFree(GetProcessHeap(),0,service); return NULL;
    }
    if (!OpenNtBaseReservationsInitialize(&service->reservations)) {
        OpenNtBaseDestroyProcessRegistry(&service->registry);
        DeleteCriticalSection(&service->lock);HeapFree(GetProcessHeap(),0,service);return NULL;
    }
    /* srvvdm.c normally resolves this private winsrv entry point lazily.  The
     * standalone broker has no winsrv DLL, but the original CheckWOW body
     * already accepts precisely this predicate.  Supply the existing adapter
     * binding and keep its trusted local logon scope outside the wire format. */
    if (!OpenNtBaseInitializeInteractiveScope(&service->interactive)) {
        OpenNtBaseReservationsDestroy(service->reservations);
        OpenNtBaseDestroyProcessRegistry(&service->registry);
        DeleteCriticalSection(&service->lock); HeapFree(GetProcessHeap(),0,service); return NULL;
    }
    UserTestTokenForInteractive=_UserTestTokenForInteractive;
    BaseSrvVDMInit();
    return service;
}
BOOL OpenNtBaseServiceConfigureConsoleQuery(OPENNT_BASE_SERVICE *service,
    OPENNT_BASE_CONSOLE_QUERY query,void *context)
{
    BOOL result=FALSE;
    if (!service || !query) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    EnterCriticalSection(&service->lock);
    if (IsListEmpty(&service->connections)) {
        service->console_query=query;
        service->console_query_context=context;
        result=TRUE;
    } else SetLastError(ERROR_BUSY);
    LeaveCriticalSection(&service->lock);
    return result;
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
    while (!IsListEmpty(&service->retired_connections)) {
        entry=RemoveHeadList(&service->retired_connections);
        HeapFree(GetProcessHeap(),0,CONTAINING_RECORD(entry,OPENNT_BASE_CONNECTION,retired_link));
    }
    DeleteCriticalSection(&service->lock);
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
    if (!watch || watch->wow) return;
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
        if (!watch->wow && watch->console==console) return watch;
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
    if (watch->wow) {
        (void)RtlEnterCriticalSection(&BaseSrvWOWCriticalSection);
        if (WOWHead && WOWHead->SequenceNumber==watch->process.SequenceNumber) {
            for (wow=WOWHead->WOWRecord;wow;wow=wow->WOWRecordNext) {
                ++item->reserved;
                if (!selected_wow || (!selected_wow->fDispatched && wow->fDispatched))
                    selected_wow=wow;
            }
            if (selected_wow) {
                item->kind=3u; /* WOW16 / Win16 worker */
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
        item->kind=1u; /* DOS worker */
        item->state=selected->VDMState;
        item->reserved=item->stack_depth;
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
        item->kind=watch->wow ? 3u : 1u;
        item->state=watch->termination_requested ? 0x80000000u : VDM_READY;
        item->started_filetime=((uint64_t)watch->started.dwHighDateTime<<32)|watch->started.dwLowDateTime;
        service_copy_management_record(watch,item);
        if (watch->termination_requested) item->state|=0x80000000u;
        /* A resident worker without an original VDMINFO has no active
         * product task. Do not misrepresent its historical launch image as
         * current work. */
        if (!item->image[0])
            lstrcpynW(item->image,L"<EMPTY>",OPENNT_BASE_WORKER_IMAGE_CHARS);
    }
    service_sort_management_records(entries,index);
    LeaveCriticalSection(&service->lock);
    return ERROR_SUCCESS;
}
DWORD OpenNtBaseServiceTerminateWorker(OPENNT_BASE_SERVICE *service,uint64_t epoch,uint32_t sequence)
{
    LIST_ENTRY *link;
    HANDLE process=NULL;
    DWORD error=ERROR_NOT_FOUND;
    if (!service || !sequence) return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&service->lock);
    if (epoch!=service->management_epoch) error=ERROR_REVISION_MISMATCH;
    else for (link=service->worker_watches.Flink;link!=&service->worker_watches;link=link->Flink) {
        OPENNT_BASE_WORKER_WATCH *watch=CONTAINING_RECORD(link,OPENNT_BASE_WORKER_WATCH,link);
        if (watch->process.SequenceNumber!=sequence) continue;
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
    if (error) return error;
    if (!TerminateProcess(process,ERROR_CANCELLED)) error=GetLastError();
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
    EnterCriticalSection(&service->lock);
    if (!OpenNtBaseRegisterProcess(&service->registry,&connection->process,process)) error=GetLastError();
    else {
        uint64_t reservation=0;
        ULONG task=0;
        HANDLE console=NULL;
        HANDLE reserved_worker=NULL;
        BOOL shared_wow=FALSE;
        broker_vdm_receipts_initialize(&connection->streams,connection->process.SequenceNumber);
        error=OpenNtBaseReservationClaimWorker(service->reservations,
            (DWORD)connection->process.ClientId.UniqueProcess,connection->process.SequenceNumber,
            &reservation,&task,&console,&shared_wow,&reserved_worker);
        if (error==ERROR_NOT_FOUND) error=ERROR_SUCCESS;
        else if (!error) {
            OPENNT_BASE_WORKER_WATCH *watch;
            connection->reservation=reservation;connection->task=task;
            connection->console=console;connection->wow=shared_wow;
            /* This is the post-create registration performed by original
             * BaseSrvCreateProcess: publish the authenticated worker as a
             * VDM and attach its CSR sequence to the source ConsoleRecord.
             * The standalone learns that fact only when the reservation-bound
             * worker connects; it does not invent another worker state. */
            connection->process.fVDM=TRUE;
            BaseSrvUpdateVDMSequenceNumber(shared_wow ? OPENNT_BASE_CONSOLE_WOW : console,
                connection->process.SequenceNumber,task);
            watch=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*watch));
            if (!watch || !reserved_worker) {
                error=watch ? ERROR_INVALID_HANDLE : ERROR_NOT_ENOUGH_MEMORY;
                if (watch) HeapFree(GetProcessHeap(),0,watch);
                BaseSrvCleanupVDMResources(&connection->process);
            } else {
                watch->process.ProcessHandle=reserved_worker;
                reserved_worker=NULL;
                watch->service=service;
                watch->process.ClientId=connection->process.ClientId;
                watch->process.SequenceNumber=connection->process.SequenceNumber;
                watch->process.fVDM=TRUE;
                watch->console=console;
                watch->wow=shared_wow;
                watch->reservation=reservation;
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
                if (!RegisterWaitForSingleObject(&watch->wait,watch->process.ProcessHandle,
                    service_worker_terminated,watch,INFINITE,WT_EXECUTEONLYONCE)) {
                    error=GetLastError();
                    CloseHandle(watch->process.ProcessHandle);
                    HeapFree(GetProcessHeap(),0,watch);
                    BaseSrvCleanupVDMResources(&connection->process);
                } else {
                    InsertTailList(&service->worker_watches,&watch->link);
}
            }
        }
        if (reserved_worker) CloseHandle(reserved_worker);
        if (!error) {
            service_prune_cancelled_frontends(service);
            InsertTailList(&service->connections,&connection->service_link);
            *output=connection; *generation=connection->process.SequenceNumber;
        }
        else (void)OpenNtBaseRemoveProcess(&service->registry,&connection->process);
    }
    LeaveCriticalSection(&service->lock);
    if (error) HeapFree(GetProcessHeap(),0,connection);
    return error;
}
DWORD OpenNtBaseServiceDisconnect(OPENNT_BASE_CONNECTION *connection)
{
    OPENNT_BASE_SERVICE *service;
    DWORD error=0;
    if (!connection) return ERROR_INVALID_PARAMETER;
    service=connection->service;
    EnterCriticalSection(&service->lock);
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
    }
    if (!error) service_clear_frontend(connection);
    LeaveCriticalSection(&service->lock);
    if (!error) {
        if (connection->frontend_capability) CloseHandle(connection->frontend_capability);
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
    /* A discoverable named event would turn the lease into a public name.
     * Only the root-created unnamed event can become this capability. */
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
done:
    LeaveCriticalSection(&connection->service->lock);
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
        if (WaitForSingleObject(root->process.ProcessHandle,0)!=WAIT_TIMEOUT) {
            error=ERROR_PIPE_NOT_CONNECTED;goto done;
        }
        if (!DuplicateHandle(GetCurrentProcess(),root->process.ProcessHandle,
            GetCurrentProcess(),process,PROCESS_QUERY_LIMITED_INFORMATION|SYNCHRONIZE,FALSE,0)) {
            error=GetLastError();goto done;
        }
        *root_generation=root->process.SequenceNumber;
        error=ERROR_SUCCESS;
        break;
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
    /* Membership alone is not a grant. Only Check/Update's still-pending
     * original parent completion associates this launcher with execution. */
    if (connection->wow || connection->process.fVDM || connection->worker_failed || !connection->parent_wait ||
        WaitForSingleObject(connection->parent_wait,0)!=WAIT_TIMEOUT) goto done;
    if (connection->reservation) {
        error=OpenNtBaseReservationRetainWorker(service->reservations,
            connection->reservation,pid,generation,&retained);
        if (error) goto done;
        selected=retained;
    } else {
        for (link=service->worker_watches.Flink;link!=&service->worker_watches;link=link->Flink) {
            OPENNT_BASE_WORKER_WATCH *watch=CONTAINING_RECORD(link,OPENNT_BASE_WORKER_WATCH,link);
            if (!watch->wow && watch->console==connection->console) {
                if (selected) { error=ERROR_INVALID_DATA; goto done; }
                selected=watch->process.ProcessHandle;
            }
        }
    }
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
    if (NtQueryEvent(ready,EventBasicInformation,&event_info,sizeof(event_info),NULL)<0 ||
        event_info.EventType!=NotificationEvent) { error=ERROR_INVALID_PARAMETER;goto done; }
    if (GetFileType(pipe)!=FILE_TYPE_PIPE ||
        !GetNamedPipeInfo(pipe,&flags,NULL,NULL,NULL) || (flags&PIPE_TYPE_MESSAGE)) {
        error=ERROR_INVALID_PARAMETER; goto done;
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
            error=ERROR_ALREADY_EXISTS; goto done;
        }
    }
    route=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*route));
    if (!route) { error=ERROR_NOT_ENOUGH_MEMORY;goto done; }
    if (!DuplicateHandle(GetCurrentProcess(),ready,GetCurrentProcess(),&route->ready,
            SYNCHRONIZE,FALSE,0)) { error=GetLastError();goto done; }
    if (!DuplicateHandle(GetCurrentProcess(),pipe,GetCurrentProcess(),&route->pipe,
            0,FALSE,DUPLICATE_SAME_ACCESS)) { error=GetLastError(); goto done; }
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
        InsertTailList(&connection->service->frontend_routes,&pending->link);
        connection->frontend_request_root=root_generation;
        error=ERROR_SUCCESS;
        break;
    }
done:
    if (root_process) CloseHandle(root_process);
    if (worker) CloseHandle(worker);
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
    for (link=root->service->connections.Flink;link!=&root->service->connections;link=link->Flink) {
        OPENNT_BASE_CONNECTION *caller=CONTAINING_RECORD(link,OPENNT_BASE_CONNECTION,service_link);
        if (caller->frontend_request_root!=generation) continue;
        error=OpenNtBaseServiceRetainCommandWorker(caller,
            (DWORD)(ULONG_PTR)caller->process.ClientId.UniqueProcess,caller->process.SequenceNumber,worker);
        if (!error) { *request=caller->process.SequenceNumber;goto done; }
        caller->frontend_request_root=0; /* Completed/departed original caller, not a new task. */
    }
    error=ResetEvent(root->frontend_capability) ? ERROR_NOT_FOUND : GetLastError();
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
    if (!OpenNtBaseServicePeer(root,pid,generation) || !root->frontend_capability) goto done;
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
    if (!OpenNtBaseServicePeer(connection,pid,generation) || !connection->process.fVDM || connection->wow) {
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
    if (!connection->process.fVDM) {
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
        if (error!=ERROR_NOT_READY && error!=ERROR_ALREADY_EXISTS) break;
        /* Only capability arrival is awaited. Original Get already selected
         * the command; no guest command is queued, replayed or selected here. */
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

DWORD OpenNtBaseServicePrepareWorker(OPENNT_BASE_CONNECTION *connection,DWORD pid,DWORD generation,
    uint64_t reservation,HANDLE worker)
{
    if (!connection) return ERROR_INVALID_PARAMETER;
    if (!OpenNtBaseServicePeer(connection,pid,generation)) return ERROR_ACCESS_DENIED;
    return OpenNtBaseReservationPrepareWorker(connection->service->reservations,reservation,
        pid,generation,worker);
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
        connection->parent_wait=NULL; connection->worker_failed=FALSE;
        connection->parent_receipt=connection->completed_receipt=0;
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
    OPENNT_BASE_CONSOLE_CANDIDATE *candidates=NULL;
    HANDLE *processes=NULL;
    OPENNT_BASE_CONSOLE_QUERY query;
    void *context;
    HANDLE caller=NULL,selected=NULL;
    BYTE *members=NULL;
    DWORD count=0,index=0,error=0;
    LIST_ENTRY *entry;
    if (!connection) return ERROR_INVALID_PARAMETER;
    service=connection->service;
    EnterCriticalSection(&service->lock);
    if (connection->console) { LeaveCriticalSection(&service->lock); return ERROR_SUCCESS; }
    for (entry=service->connections.Flink;entry!=&service->connections;entry=entry->Flink) {
        OPENNT_BASE_CONNECTION *other=CONTAINING_RECORD(entry,OPENNT_BASE_CONNECTION,service_link);
        if (other!=connection && other->console) ++count;
    }
    query=service->console_query;context=service->console_query_context;
    if (!count) {
        if (!++service->next_console || service->next_console==MAXDWORD) error=ERROR_ARITHMETIC_OVERFLOW;
        else connection->console=(HANDLE)(ULONG_PTR)service->next_console;
        LeaveCriticalSection(&service->lock);
        return error;
    }
    if (!query) { LeaveCriticalSection(&service->lock); return ERROR_NOT_SUPPORTED; }
    candidates=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,count*sizeof(*candidates));
    processes=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,count*sizeof(*processes));
    members=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,count);
    if (!candidates || !processes || !members) { LeaveCriticalSection(&service->lock); error=ERROR_NOT_ENOUGH_MEMORY; goto done; }
    if (!OpenNtBaseRetainRegisteredProcess(&service->registry,
            (DWORD)connection->process.ClientId.UniqueProcess,connection->process.SequenceNumber,&caller)) {
        LeaveCriticalSection(&service->lock); error=GetLastError(); goto done;
    }
    for (entry=service->connections.Flink;entry!=&service->connections;entry=entry->Flink) {
        OPENNT_BASE_CONNECTION *other=CONTAINING_RECORD(entry,OPENNT_BASE_CONNECTION,service_link);
        if (other==connection || !other->console) continue;
        candidates[index].generation=other->process.SequenceNumber;
        candidates[index].console=other->console;
        if (!OpenNtBaseRetainRegisteredProcess(&service->registry,
                (DWORD)other->process.ClientId.UniqueProcess,other->process.SequenceNumber,
                &candidates[index].process)) {
            LeaveCriticalSection(&service->lock); error=GetLastError(); goto done;
        }
        processes[index]=candidates[index].process;
        ++index;
    }
    LeaveCriticalSection(&service->lock);
    if (index!=count) { error=ERROR_RETRY; goto done; }
    /* The query consumes a packed HANDLE vector, not the first field of an
     * array whose stride also includes generation and Console identity.
     * candidates retains ownership and the post-query identity checks. */
    error=query(context,caller,processes,count,NULL,5000,members);
    if (error) goto done;
    for (index=0;index<count;++index) if (members[index]) {
        HANDLE verified=NULL;
        if (!OpenNtBaseRetainRegisteredProcess(&service->registry,GetProcessId(candidates[index].process),
                candidates[index].generation,&verified)) { error=ERROR_RETRY; goto done; }
        CloseHandle(verified);
        if (selected && selected!=candidates[index].console) { error=ERROR_RETRY; goto done; }
        selected=candidates[index].console;
    }
    EnterCriticalSection(&service->lock);
    if (!connection->console) {
        if (selected) connection->console=selected;
        else if (!++service->next_console || service->next_console==MAXDWORD) error=ERROR_ARITHMETIC_OVERFLOW;
        else connection->console=(HANDLE)(ULONG_PTR)service->next_console;
    }
    LeaveCriticalSection(&service->lock);
done:
    if (caller) CloseHandle(caller);
    if (candidates) {
        for (index=0;index<count;++index) if (candidates[index].process) CloseHandle(candidates[index].process);
        HeapFree(GetProcessHeap(),0,candidates);
    }
    if (members) HeapFree(GetProcessHeap(),0,members);
    if (processes) HeapFree(GetProcessHeap(),0,processes);
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
    BOOL separate_dos=FALSE;
    BOOL console_record_exists=FALSE;
    if (required) *required=0;
    if (!parent_event || !parent_receipt) return ERROR_INVALID_PARAMETER;
    *parent_event=NULL;*parent_receipt=0;
    if (!connection || !required || !OpenNtBaseServicePeer(connection,pid,generation) ||
        !OpenNtBaseDecodeCheckCommand(input,bytes,generation,&message,&startup,&request))
        return ERROR_INVALID_PARAMETER;
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
    if (!status && NT_SUCCESS((NTSTATUS)message.ReturnValue))
        connection->wow=message.u.CheckVDM.BinaryType==BINARY_TYPE_WIN16;
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
    }
    /* Once source has published the no-console record, the launcher and its
     * reservation must use the new identity.  The worker's first PIF request
     * then binds this record, not the resident COMMAND ConsoleRecord. */
    if (!status && NT_SUCCESS((NTSTATUS)message.ReturnValue) && separate_dos &&
        message.u.CheckVDM.VDMState==VDM_NOT_PRESENT)
        connection->console=separate_console;
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
    if (parent_receipt==connection->parent_receipt && *exit_code!=STILL_ACTIVE)
        connection->parent_receipt=0;
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
