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
#include "broker/vdm_receipt.h"
typedef NTSTATUS (*OPENNT_USER_TEST_TOKEN_FOR_INTERACTIVE)(HANDLE,PLUID);
extern OPENNT_USER_TEST_TOKEN_FOR_INTERACTIVE UserTestTokenForInteractive;

/* Default-off evidence for the shared-WOW acquisition seam.  It writes only
 * operation state and status, never command text, guest addresses or handles. */
static void service_trace_get(const char *phase,ULONG state,NTSTATUS status)
{
    CHAR path[MAX_PATH],line[160];
    DWORD length,written,saved=GetLastError();
    HANDLE file;
    int bytes;
    length=GetEnvironmentVariableA("MVDM_BASESRV_TRACE_PATH",path,sizeof(path));
    if (!length || length>=sizeof(path)) goto done;
    bytes=wsprintfA(line,"BASESRV-S5 phase=%s state=%08lX status=%08lX\r\n",
        phase,(unsigned long)state,(unsigned long)status);
    if (bytes<=0 || (size_t)bytes>=sizeof(line)) goto done;
    file=CreateFileA(path,FILE_APPEND_DATA,FILE_SHARE_READ,NULL,OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,NULL);
    if (file!=INVALID_HANDLE_VALUE) {
        (void)WriteFile(file,line,(DWORD)bytes,&written,NULL);
        CloseHandle(file);
    }
done:
    SetLastError(saved);
}
struct OPENNT_BASE_SERVICE {
    OPENNT_BASE_PROCESS_REGISTRY registry;
    OPENNT_BASE_RESERVATIONS *reservations;
    CRITICAL_SECTION lock;
    ULONG next_console;
    OPENNT_BASE_INTERACTIVE_SCOPE interactive;
    LIST_ENTRY connections;
    OPENNT_BASE_CONSOLE_QUERY console_query;
    void *console_query_context;
};
struct OPENNT_BASE_CONNECTION {
    OPENNT_BASE_SERVICE *service;
    CSR_PROCESS process;
    broker_vdm_receipts streams;
    uint64_t reservation;
    ULONG task;
    HANDLE console;
    BOOL wow;
    LIST_ENTRY service_link;
};
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
static NTSTATUS service_wait_deliver(void *context,HANDLE event,uint32_t *receipt)
{
    OPENNT_BASE_SERVICE_RESOURCES *scope=context;
    DWORD error;
    if (!scope || !event || !receipt) return STATUS_INVALID_PARAMETER;
    error=broker_vdm_receipt_accept(&scope->connection->streams,scope->role,event,receipt);
    return error ? STATUS_INVALID_HANDLE : STATUS_SUCCESS;
}
static NTSTATUS service_stream_deliver(void *context,HANDLE stream,uint32_t *receipt)
{
    OPENNT_BASE_SERVICE_RESOURCES *scope=context;
    DWORD error;
    if (!scope || !stream || !receipt) return STATUS_INVALID_PARAMETER;
    /* Update precedes worker Connect.  The reservation is the only finite
     * owner spanning that interval; srvvdm.c retains alias comparison. */
    error=OpenNtBaseReservationAcceptStream(scope->connection->service->reservations,
        scope->connection->reservation,stream,receipt);
    return error ? STATUS_INVALID_HANDLE : STATUS_SUCCESS;
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
    /* Keep the original Update-before-Connect ordering executable while S4
     * completes the receipt-only first-command path.  This branch is scoped
     * to the already reserved suspended worker and precedes its Connect. */
    if (scope->worker && source && target && !access && attributes==OBJ_INHERIT &&
        options==DUPLICATE_SAME_ACCESS) {
        HANDLE stream=NULL;
        if (!broker_vdm_receipt_resolve(&scope->connection->streams,
                scope->connection->process.SequenceNumber,(uint32_t)(ULONG_PTR)source,
                BROKER_VDM_STDIN,&stream) && DuplicateHandle(GetCurrentProcess(),stream,
                scope->worker,target,0,TRUE,DUPLICATE_SAME_ACCESS) &&
            OpenNtBaseReservationMarkWorkerLocalStream(
                scope->connection->service->reservations,
                scope->connection->reservation,*target)==ERROR_SUCCESS) return STATUS_SUCCESS;
        return STATUS_INVALID_HANDLE;
    }
    /* The retained srvvdm.c body asks to duplicate a source-shaped standard
     * handle into its VDM process.  In standalone the field is a receipt
     * issued to the launcher, so resolve only that authenticated source and
     * deliver an actual typed attachment to this worker connection. */
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
/* Original guarded USER hook is absent in standalone CLI composition. */
PFNNOTIFYPROCESSCREATE UserNotifyProcessCreate=NULL;
OPENNT_BASE_SERVICE *OpenNtBaseServiceStart(void)
{
    OPENNT_BASE_SERVICE *service=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*service));
    if (!service) return NULL;
    if (!InitializeCriticalSectionEx(&service->lock,0,0)) {
        HeapFree(GetProcessHeap(),0,service); return NULL;
    }
    InitializeListHead(&service->connections);
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
BOOL OpenNtBaseServiceStop(OPENNT_BASE_SERVICE *service)
{
    /* Transport has stopped and joined all calls/rundowns before stop. */
    if (!service || !IsListEmpty(&service->connections) ||
        !OpenNtBaseReservationsDestroy(service->reservations) ||
        !OpenNtBaseDestroyProcessRegistry(&service->registry)) return FALSE;
    DeleteCriticalSection(&service->lock);
    HeapFree(GetProcessHeap(),0,service);
    return TRUE;
}
BOOL OpenNtBaseServiceIsEmpty(OPENNT_BASE_SERVICE *service)
{
    BOOL empty;
    if (!service) return FALSE;
    EnterCriticalSection(&service->lock);
    empty=OpenNtBaseProcessRegistryIsEmpty(&service->registry) &&
        OpenNtBaseReservationsIsEmpty(service->reservations);
    LeaveCriticalSection(&service->lock);
    return empty;
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
        broker_vdm_receipts_initialize(&connection->streams,connection->process.SequenceNumber);
        error=OpenNtBaseReservationClaimWorker(service->reservations,
            (DWORD)connection->process.ClientId.UniqueProcess,connection->process.SequenceNumber,
            &reservation,&task,&console);
        if (error==ERROR_NOT_FOUND) error=ERROR_SUCCESS;
        else if (!error) {
            connection->reservation=reservation;connection->task=task;connection->console=console;
        }
        if (!error) {
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
    if (!OpenNtBaseRemoveProcess(&service->registry,&connection->process)) error=GetLastError();
    else {
        RemoveEntryList(&connection->service_link);
        broker_vdm_receipts_drain(&connection->streams);
    }
    LeaveCriticalSection(&service->lock);
    if (!error) HeapFree(GetProcessHeap(),0,connection);
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
    if (!connection) return ERROR_INVALID_PARAMETER;
    if (!OpenNtBaseServicePeer(connection,pid,generation)) return ERROR_ACCESS_DENIED;
    return OpenNtBaseReservationRelease(connection->service->reservations,reservation,pid,generation);
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
    members=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,count);
    if (!candidates || !members) { LeaveCriticalSection(&service->lock); error=ERROR_NOT_ENOUGH_MEMORY; goto done; }
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
        ++index;
    }
    LeaveCriticalSection(&service->lock);
    if (index!=count) { error=ERROR_RETRY; goto done; }
    error=query(context,caller,&candidates[0].process,count,NULL,5000,members);
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
    return error;
}

DWORD OpenNtBaseServiceCheck(OPENNT_BASE_CONNECTION *connection,DWORD pid,DWORD generation,
    void *input,uint32_t bytes,void *output,uint32_t capacity,uint32_t *required)
{
    BASE_API_MSG message={0};
    STARTUPINFOA startup;
    CSR_THREAD thread={0};
    PCSR_THREAD previousThread;
    OPENNT_BASE_PROCESS_REGISTRY *previousRegistry;
    const OPENNT_BASE_INTERACTIVE_SCOPE *previousInteractive;
    uint32_t request;
    uint32_t needed=0;
    NTSTATUS status;
    if (required) *required=0;
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
        message.u.CheckVDM.ConsoleHandle=connection->console;
    }
    EnterCriticalSection(&connection->service->lock);
    thread.Process=&connection->process;
    thread.ClientId.UniqueProcess=connection->process.ClientId.UniqueProcess;
    previousThread=OpenNtBaseBindServerRequestThread(&thread);
    previousRegistry=OpenNtBaseBindProcessRegistry(&connection->service->registry);
    previousInteractive=OpenNtBaseBindInteractiveScope(&connection->service->interactive);
    status=OpenNtBaseDispatchOperation((PCSR_API_MSG)&message,BROKER_VDM_CHECK,
        sizeof(message.u.CheckVDM));
    OpenNtBaseBindInteractiveScope(previousInteractive);
    OpenNtBaseBindProcessRegistry(previousRegistry);
    OpenNtBaseBindServerRequestThread(previousThread);
    if (!status && NT_SUCCESS((NTSTATUS)message.ReturnValue))
        connection->wow=message.u.CheckVDM.BinaryType==BINARY_TYPE_WIN16;
    LeaveCriticalSection(&connection->service->lock);
    if (status && !message.ReturnValue) message.ReturnValue=status;
    if (!OpenNtBaseEncodeCheckReply(&message,request,generation,output,capacity,required))
        return ERROR_INVALID_PARAMETER;
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
    *parent_receipt=(uint32_t)(ULONG_PTR)message.u.UpdateVDMEntry.WaitObjectForParent;
    error=service_wait_resolve(connection,generation,message.u.UpdateVDMEntry.WaitObjectForParent,
        BROKER_VDM_PARENT_WAIT,parent_event);
    if (error) goto done;
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
    if (!connection->console) { error=ERROR_INVALID_HANDLE; goto done; }
    message.u.GetVDMExitCode.ConsoleHandle=connection->console;
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
    service_trace_get("get-prepared",message.u.GetNextVDMCommand.VDMState,STATUS_SUCCESS);
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
    service_trace_get("get-dispatched",message.u.GetNextVDMCommand.VDMState,
        (NTSTATUS)message.ReturnValue);
    if (!OpenNtBaseFinishGetCommand(&message,&state)) { error=ERROR_INVALID_DATA; goto done; }
    if (message.u.GetNextVDMCommand.StdIn || message.u.GetNextVDMCommand.StdOut ||
        message.u.GetNextVDMCommand.StdErr) {
        HANDLE ids[3]={message.u.GetNextVDMCommand.StdIn,message.u.GetNextVDMCommand.StdOut,
            message.u.GetNextVDMCommand.StdErr};
        DWORD index;
        for (index=0;index<3;++index) if (ids[index] &&
            !OpenNtBaseReservationIsWorkerLocalStream(connection->service->reservations,
                connection->reservation,ids[index]) &&
            OpenNtBaseReservationResolveStream(connection->service->reservations,
                connection->reservation,(uint32_t)(ULONG_PTR)ids[index],&standard[index])) {
            service_trace_get("get-unresolved-stream",message.u.GetNextVDMCommand.VDMState,
                STATUS_INVALID_HANDLE);
            error=ERROR_INVALID_HANDLE;goto done;
        }
        if (!OpenNtBaseReservationIsWorkerLocalStream(connection->service->reservations,
                connection->reservation,ids[0]) &&
            !OpenNtBaseReservationIsWorkerLocalStream(connection->service->reservations,
                connection->reservation,ids[1]) &&
            !OpenNtBaseReservationIsWorkerLocalStream(connection->service->reservations,
                connection->reservation,ids[2])) *standard_count=3;
    }
    *output=state.reply; *output_bytes=state.reply_bytes; state.reply=NULL;
    /* This is an original target event, held by the console record. It crosses
     * process boundaries only as a typed RPC event attachment, never in the
     * copied VDM command record. */
    error=service_wait_resolve(connection,generation,message.u.GetNextVDMCommand.WaitObjectForVDM,
        BROKER_VDM_WORKER_WAIT,wait_event);
    if (error) service_trace_get("get-unresolved-wait",message.u.GetNextVDMCommand.VDMState,
        STATUS_INVALID_HANDLE);
    if (error) goto done;
    error=ERROR_SUCCESS;
done:
    LeaveCriticalSection(&connection->service->lock);
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
