/* Original BaseSrv initialization/dispatch plus finite registered caller
 * binding. No test provider, command state machine or CSR runtime. */
#include "basesrv.h"
#include <base_service.h>
#include <base_process.h>
#include <base_dispatch.h>
#include <base_reservation.h>
#include <base_command.h>
#include <base_wait.h>
#include "broker/vdm_receipt.h"
struct OPENNT_BASE_SERVICE {
    OPENNT_BASE_PROCESS_REGISTRY registry;
    OPENNT_BASE_RESERVATIONS *reservations;
    CRITICAL_SECTION lock;
    ULONG next_console;
};
struct OPENNT_BASE_CONNECTION {
    OPENNT_BASE_SERVICE *service;
    CSR_PROCESS process;
    broker_vdm_receipts streams;
    uint64_t reservation;
    ULONG task;
    HANDLE console;
};
typedef struct OPENNT_BASE_SERVICE_RESOURCES {
    OPENNT_BASE_CONNECTION *connection;
    DWORD role;
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
    if (!scope) return STATUS_INVALID_PARAMETER;
    /* BaseSrvUpdateDOSEntry duplicates the authenticated worker's own process
     * pseudo-handle into BaseSrv. This is the one process-object carrier; all
     * event delivery remains receipt-backed below. */
    if (source_process==scope->connection->process.ProcessHandle && source==NtCurrentProcess() &&
        target_process==NtCurrentProcess() && target && !access && !attributes &&
        options==DUPLICATE_SAME_ACCESS) {
        if (DuplicateHandle(source_process,source,target_process,target,0,FALSE,DUPLICATE_SAME_ACCESS))
            return STATUS_SUCCESS;
        return STATUS_ACCESS_DENIED;
    }
    return OpenNtBaseDuplicateWait(&scope->wait,source_process,source,target_process,
        target,access,attributes,options);
}
static NTSTATUS service_close_resource(void *context,HANDLE handle)
{
    OPENNT_BASE_SERVICE_RESOURCES *scope=context;
    if (!scope || !handle) return STATUS_INVALID_HANDLE;
    if (handle==scope->wait.local_event) return OpenNtBaseCloseWait(&scope->wait,handle);
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
    if (!OpenNtBaseInitializeProcessRegistry(&service->registry)) {
        DeleteCriticalSection(&service->lock); HeapFree(GetProcessHeap(),0,service); return NULL;
    }
    if (!OpenNtBaseReservationsInitialize(&service->reservations)) {
        OpenNtBaseDestroyProcessRegistry(&service->registry);
        DeleteCriticalSection(&service->lock);HeapFree(GetProcessHeap(),0,service);return NULL;
    }
    BaseSrvVDMInit();
    return service;
}
BOOL OpenNtBaseServiceStop(OPENNT_BASE_SERVICE *service)
{
    /* Transport has stopped and joined all calls/rundowns before stop. */
    if (!service || !OpenNtBaseReservationsDestroy(service->reservations) ||
        !OpenNtBaseDestroyProcessRegistry(&service->registry)) return FALSE;
    DeleteCriticalSection(&service->lock);
    HeapFree(GetProcessHeap(),0,service);
    return TRUE;
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
        if (!error) { *output=connection; *generation=connection->process.SequenceNumber; }
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
    if (!error) broker_vdm_receipts_drain(&connection->streams);
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
    if (!connection->console) return ERROR_INVALID_HANDLE;
    return OpenNtBaseReservationCreate(connection->service->reservations,pid,generation,
        task,connection->console,reservation);
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

DWORD OpenNtBaseServiceCheck(OPENNT_BASE_CONNECTION *connection,DWORD pid,DWORD generation,
    void *input,uint32_t bytes,void *output,uint32_t capacity,uint32_t *required)
{
    BASE_API_MSG message={0};
    STARTUPINFOA startup;
    CSR_THREAD thread={0};
    PCSR_THREAD previousThread;
    OPENNT_BASE_PROCESS_REGISTRY *previousRegistry;
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
    EnterCriticalSection(&connection->service->lock);
    if (message.u.CheckVDM.ConsoleHandle==OPENNT_BASE_CONSOLE_EXISTING) {
        if (!connection->console) {
            if (!++connection->service->next_console || connection->service->next_console==MAXDWORD) {
                LeaveCriticalSection(&connection->service->lock);return ERROR_ARITHMETIC_OVERFLOW;
            }
            connection->console=(HANDLE)(ULONG_PTR)connection->service->next_console;
        }
        message.u.CheckVDM.ConsoleHandle=connection->console;
    }
    thread.Process=&connection->process;
    thread.ClientId.UniqueProcess=connection->process.ClientId.UniqueProcess;
    previousThread=OpenNtBaseBindServerRequestThread(&thread);
    previousRegistry=OpenNtBaseBindProcessRegistry(&connection->service->registry);
    status=OpenNtBaseDispatchOperation((PCSR_API_MSG)&message,BROKER_VDM_CHECK,
        sizeof(message.u.CheckVDM));
    OpenNtBaseBindProcessRegistry(previousRegistry);
    OpenNtBaseBindServerRequestThread(previousThread);
    LeaveCriticalSection(&connection->service->lock);
    if (status && !message.ReturnValue) message.ReturnValue=status;
    if (!OpenNtBaseEncodeCheckReply(&message,request,generation,output,capacity,required))
        return ERROR_INVALID_PARAMETER;
    return ERROR_SUCCESS;
}

DWORD OpenNtBaseServiceUpdate(OPENNT_BASE_CONNECTION *connection,DWORD pid,DWORD generation,
    const void *input,uint32_t bytes,void *output,uint32_t capacity,uint32_t *required,HANDLE *parent_event)
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
    if (!parent_event) return ERROR_INVALID_PARAMETER;
    *parent_event=NULL;
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
    if (status && !message.ReturnValue) message.ReturnValue=status;
    if (!OpenNtBaseEncodeUpdateReply(&message,request,generation,output,capacity,required)) {
        error=ERROR_INVALID_DATA; goto done;
    }
    error=service_wait_resolve(connection,generation,message.u.UpdateVDMEntry.WaitObjectForParent,
        BROKER_VDM_PARENT_WAIT,parent_event);
    if (error) goto done;
    error=ERROR_SUCCESS;
done:
    LeaveCriticalSection(&connection->service->lock);
    return error;
}

DWORD OpenNtBaseServiceGet(OPENNT_BASE_CONNECTION *connection,DWORD pid,DWORD generation,
    const void *input,uint32_t bytes,void **output,uint32_t *output_bytes,HANDLE *wait_event)
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
    if (!output || !output_bytes || !wait_event) return ERROR_INVALID_PARAMETER;
    *output=NULL; *output_bytes=0; *wait_event=NULL;
    if (!connection || !OpenNtBaseServicePeer(connection,pid,generation))
        return ERROR_ACCESS_DENIED;
    error=OpenNtBasePrepareGetCommand(input,bytes,generation,&message,&state);
    if (error) return error;
    EnterCriticalSection(&connection->service->lock);
    /* BasepGetNextVDMCommand's ConsoleHandle is a local service identity,
     * not a copied client field. A registered worker receives this value from
     * its launcher reservation; a same-connection test obtains it from Check. */
    if (!connection->console) { error=ERROR_INVALID_HANDLE; goto done; }
    message.u.GetNextVDMCommand.ConsoleHandle=connection->console;
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
    *output=state.reply; *output_bytes=state.reply_bytes; state.reply=NULL;
    /* This is an original target event, held by the console record. It crosses
     * process boundaries only as a typed RPC event attachment, never in the
     * copied VDM command record. */
    error=service_wait_resolve(connection,generation,message.u.GetNextVDMCommand.WaitObjectForVDM,
        BROKER_VDM_WORKER_WAIT,wait_event);
    if (error) goto done;
    error=ERROR_SUCCESS;
done:
    LeaveCriticalSection(&connection->service->lock);
    OpenNtBaseReleaseGetCommand(&state);
    return error;
}

void OpenNtBaseServiceReleaseCommandReply(void *reply)
{
    if (reply) HeapFree(GetProcessHeap(),0,reply);
}
