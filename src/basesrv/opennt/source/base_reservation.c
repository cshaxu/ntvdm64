/* Finite worker-launch binding below original BaseSrv record ownership.
 * This has no command queue or selection logic: srvvdm.c owns those. */
#include <base_reservation.h>
#include "basesrv/transport/vdm_receipt.h"

typedef struct OPENNT_BASE_RESERVATION {
    LIST_ENTRY link;
    uint64_t id;
    DWORD launcher_pid,launcher_generation;
    DWORD worker_pid,worker_generation;
    ULONG task;
    HANDLE console,worker;
    BOOL shared_wow;
    BOOL abandoned;
    broker_vdm_receipts streams;
    HANDLE worker_streams[3];
    DWORD worker_stream_count;
} OPENNT_BASE_RESERVATION;

struct OPENNT_BASE_RESERVATIONS {
    CRITICAL_SECTION lock;
    LIST_ENTRY entries;
    uint64_t next;
};

static OPENNT_BASE_RESERVATION *find(OPENNT_BASE_RESERVATIONS *state,uint64_t id)
{
    LIST_ENTRY *cursor;
    for (cursor=state->entries.Flink;cursor!=&state->entries;cursor=cursor->Flink) {
        OPENNT_BASE_RESERVATION *entry=CONTAINING_RECORD(cursor,OPENNT_BASE_RESERVATION,link);
        if (entry->id==id) return entry;
    }
    return NULL;
}

BOOL OpenNtBaseReservationsInitialize(OPENNT_BASE_RESERVATIONS **output)
{
    OPENNT_BASE_RESERVATIONS *state;
    if (!output) return FALSE;
    *output=NULL;
    state=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*state));
    if (!state) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return FALSE; }
    if (!InitializeCriticalSectionEx(&state->lock,0,0)) {
        HeapFree(GetProcessHeap(),0,state); return FALSE;
    }
    InitializeListHead(&state->entries);state->next=1;*output=state;
    return TRUE;
}

BOOL OpenNtBaseReservationsDestroy(OPENNT_BASE_RESERVATIONS *state)
{
    if (!state) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    EnterCriticalSection(&state->lock);
    if (!IsListEmpty(&state->entries)) {
        LeaveCriticalSection(&state->lock);SetLastError(ERROR_BUSY);return FALSE;
    }
    LeaveCriticalSection(&state->lock);
    DeleteCriticalSection(&state->lock);HeapFree(GetProcessHeap(),0,state);
    return TRUE;
}

BOOL OpenNtBaseReservationsIsEmpty(OPENNT_BASE_RESERVATIONS *state)
{
    BOOL empty;
    if (!state) return FALSE;
    EnterCriticalSection(&state->lock);
    empty=IsListEmpty(&state->entries);
    LeaveCriticalSection(&state->lock);
    return empty;
}

DWORD OpenNtBaseReservationCreate(OPENNT_BASE_RESERVATIONS *state,DWORD launcher_pid,
    DWORD launcher_generation,ULONG task,HANDLE console,BOOL shared_wow,uint64_t *reservation)
{
    OPENNT_BASE_RESERVATION *entry;
    /* A shared WOW request has no DOS ConsoleRecord in original srvvdm.c.
     * Its launch reservation still binds one worker/task, so NULL is a valid
     * service-local Console identity only after BaseService verifies WIN16. */
    if (!state || !launcher_pid || !launcher_generation || (!console && !shared_wow) || !reservation)
        return ERROR_INVALID_PARAMETER;
    *reservation=0;
    entry=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*entry));
    if (!entry) return ERROR_NOT_ENOUGH_MEMORY;
    EnterCriticalSection(&state->lock);
    if (!state->next) { LeaveCriticalSection(&state->lock);HeapFree(GetProcessHeap(),0,entry);return ERROR_ARITHMETIC_OVERFLOW; }
    entry->id=state->next++;
    entry->launcher_pid=launcher_pid;entry->launcher_generation=launcher_generation;
    entry->task=task;entry->console=console;entry->shared_wow=shared_wow;
    if (broker_vdm_receipts_initialize(&entry->streams,launcher_generation)) {
        LeaveCriticalSection(&state->lock);HeapFree(GetProcessHeap(),0,entry);
        return ERROR_INVALID_DATA;
    }
    InsertTailList(&state->entries,&entry->link);
    *reservation=entry->id;
    LeaveCriticalSection(&state->lock);
    return ERROR_SUCCESS;
}

DWORD OpenNtBaseReservationAcceptStream(OPENNT_BASE_RESERVATIONS *state,uint64_t reservation,
    HANDLE stream,uint32_t *receipt)
{
    OPENNT_BASE_RESERVATION *entry;
    DWORD error;
    if (!state || !reservation || !stream || !receipt) return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&state->lock);
    entry=find(state,reservation);
    if (!entry) error=ERROR_NOT_FOUND;
    else error=broker_vdm_receipt_accept(&entry->streams,BROKER_VDM_STDIN,stream,receipt);
    LeaveCriticalSection(&state->lock);
    return error;
}

DWORD OpenNtBaseReservationResolveStream(OPENNT_BASE_RESERVATIONS *state,uint64_t reservation,
    uint32_t receipt,HANDLE *stream)
{
    OPENNT_BASE_RESERVATION *entry;
    DWORD error;
    if (!state || !reservation || !receipt || !stream) return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&state->lock);
    entry=find(state,reservation);
    if (!entry) error=ERROR_NOT_FOUND;
    else error=broker_vdm_receipt_resolve(&entry->streams,entry->launcher_generation,receipt,
        BROKER_VDM_STDIN,stream);
    LeaveCriticalSection(&state->lock);
    return error;
}

DWORD OpenNtBaseReservationRevokeStream(OPENNT_BASE_RESERVATIONS *state,uint64_t reservation,
    uint32_t receipt)
{
    OPENNT_BASE_RESERVATION *entry;
    DWORD error;
    if (!state || !reservation || !receipt) return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&state->lock);
    entry=find(state,reservation);
    if (!entry) error=ERROR_NOT_FOUND;
    else error=broker_vdm_receipt_revoke(&entry->streams,entry->launcher_generation,receipt);
    LeaveCriticalSection(&state->lock);
    return error;
}

DWORD OpenNtBaseReservationMarkWorkerLocalStream(OPENNT_BASE_RESERVATIONS *state,
    uint64_t reservation,HANDLE stream)
{
    OPENNT_BASE_RESERVATION *entry;
    DWORD index,error=ERROR_SUCCESS;
    if (!state || !reservation || !stream) return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&state->lock);
    entry=find(state,reservation);
    if (!entry) error=ERROR_NOT_FOUND;
    else {
        for (index=0;index<entry->worker_stream_count;++index)
            if (entry->worker_streams[index]==stream) break;
        if (index==entry->worker_stream_count) {
            if (index==ARRAYSIZE(entry->worker_streams)) error=ERROR_TOO_MANY_OPEN_FILES;
            else entry->worker_streams[entry->worker_stream_count++]=stream;
        }
    }
    LeaveCriticalSection(&state->lock);
    return error;
}

BOOL OpenNtBaseReservationIsWorkerLocalStream(OPENNT_BASE_RESERVATIONS *state,
    uint64_t reservation,HANDLE stream)
{
    OPENNT_BASE_RESERVATION *entry;
    DWORD index;
    BOOL found=FALSE;
    if (!state || !reservation || !stream) return FALSE;
    EnterCriticalSection(&state->lock);
    entry=find(state,reservation);
    if (entry) for (index=0;index<entry->worker_stream_count;++index)
        if (entry->worker_streams[index]==stream) { found=TRUE; break; }
    LeaveCriticalSection(&state->lock);
    return found;
}

DWORD OpenNtBaseReservationPrepareWorker(OPENNT_BASE_RESERVATIONS *state,uint64_t reservation,
    DWORD launcher_pid,DWORD launcher_generation,HANDLE worker)
{
    OPENNT_BASE_RESERVATION *entry;
    HANDLE retained=NULL;
    DWORD worker_pid;
    if (!state || !reservation || !launcher_pid || !launcher_generation || !worker)
        return ERROR_INVALID_PARAMETER;
    worker_pid=GetProcessId(worker);
    if (!worker_pid || WaitForSingleObject(worker,0)!=WAIT_TIMEOUT) return ERROR_PROCESS_ABORTED;
    if (!DuplicateHandle(GetCurrentProcess(),worker,GetCurrentProcess(),&retained,0,FALSE,DUPLICATE_SAME_ACCESS))
        return GetLastError();
    EnterCriticalSection(&state->lock);
    entry=find(state,reservation);
    if (!entry) { LeaveCriticalSection(&state->lock);CloseHandle(retained);return ERROR_NOT_FOUND; }
    if (entry->launcher_pid!=launcher_pid || entry->launcher_generation!=launcher_generation) {
        LeaveCriticalSection(&state->lock);CloseHandle(retained);return ERROR_ACCESS_DENIED;
    }
    if (entry->worker) { LeaveCriticalSection(&state->lock);CloseHandle(retained);return ERROR_ALREADY_EXISTS; }
    entry->worker=retained;entry->worker_pid=worker_pid;
    LeaveCriticalSection(&state->lock);
    return ERROR_SUCCESS;
}

DWORD OpenNtBaseReservationClaimWorker(OPENNT_BASE_RESERVATIONS *state,DWORD worker_pid,
    DWORD worker_generation,uint64_t *reservation,ULONG *task,HANDLE *console,BOOL *shared_wow)
{
    LIST_ENTRY *cursor;
    if (!state || !worker_pid || !worker_generation || !reservation || !task || !console || !shared_wow)
        return ERROR_INVALID_PARAMETER;
    *reservation=0;*task=0;*console=NULL;*shared_wow=FALSE;
    EnterCriticalSection(&state->lock);
    for (cursor=state->entries.Flink;cursor!=&state->entries;cursor=cursor->Flink) {
        OPENNT_BASE_RESERVATION *entry=CONTAINING_RECORD(cursor,OPENNT_BASE_RESERVATION,link);
        if (entry->worker_pid!=worker_pid) continue;
        if (!entry->worker || WaitForSingleObject(entry->worker,0)!=WAIT_TIMEOUT) {
            LeaveCriticalSection(&state->lock);return ERROR_PROCESS_ABORTED;
        }
        if (entry->worker_generation && entry->worker_generation!=worker_generation) {
            LeaveCriticalSection(&state->lock);return ERROR_ALREADY_EXISTS;
        }
        entry->worker_generation=worker_generation;
        *reservation=entry->id;*task=entry->task;*console=entry->console;*shared_wow=entry->shared_wow;
        LeaveCriticalSection(&state->lock);return ERROR_SUCCESS;
    }
    LeaveCriticalSection(&state->lock);return ERROR_NOT_FOUND;
}

DWORD OpenNtBaseReservationRetainWorker(OPENNT_BASE_RESERVATIONS *state,uint64_t reservation,
    DWORD launcher_pid,DWORD launcher_generation,HANDLE *worker)
{
    OPENNT_BASE_RESERVATION *entry;
    HANDLE retained=NULL;
    if (!state || !reservation || !launcher_pid || !launcher_generation || !worker)
        return ERROR_INVALID_PARAMETER;
    *worker=NULL;
    EnterCriticalSection(&state->lock);
    entry=find(state,reservation);
    if (!entry) { LeaveCriticalSection(&state->lock);return ERROR_NOT_FOUND; }
    if (entry->launcher_pid!=launcher_pid || entry->launcher_generation!=launcher_generation) {
        LeaveCriticalSection(&state->lock);return ERROR_ACCESS_DENIED;
    }
    if (!entry->worker || WaitForSingleObject(entry->worker,0)!=WAIT_TIMEOUT) {
        LeaveCriticalSection(&state->lock);return ERROR_PROCESS_ABORTED;
    }
    if (!DuplicateHandle(GetCurrentProcess(),entry->worker,GetCurrentProcess(),&retained,
            0,FALSE,DUPLICATE_SAME_ACCESS)) {
        DWORD error=GetLastError();
        LeaveCriticalSection(&state->lock);return error;
    }
    LeaveCriticalSection(&state->lock);
    *worker=retained;
    return ERROR_SUCCESS;
}

BOOL OpenNtBaseReservationAbandon(OPENNT_BASE_RESERVATIONS *state,uint64_t reservation)
{
    OPENNT_BASE_RESERVATION *entry;
    BOOL claimed=FALSE;
    EnterCriticalSection(&state->lock);
    entry=find(state,reservation);
    if (entry) {
        claimed=entry->worker_generation!=0;
        entry->abandoned=TRUE;
        if (!claimed && entry->worker)
            TerminateProcess(entry->worker,ERROR_PROCESS_ABORTED);
    }
    LeaveCriticalSection(&state->lock);
    return claimed;
}

void OpenNtBaseReservationCollectAbandoned(OPENNT_BASE_RESERVATIONS *state,uint64_t reservation)
{
    OPENNT_BASE_RESERVATION *entry;
    EnterCriticalSection(&state->lock);
    entry=find(state,reservation);
    if (entry && entry->abandoned && entry->worker &&
        WaitForSingleObject(entry->worker,0)==WAIT_OBJECT_0)
        OpenNtBaseReservationRelease(state,reservation,entry->launcher_pid,entry->launcher_generation);
    LeaveCriticalSection(&state->lock);
}

DWORD OpenNtBaseReservationRelease(OPENNT_BASE_RESERVATIONS *state,uint64_t reservation,
    DWORD launcher_pid,DWORD launcher_generation)
{
    OPENNT_BASE_RESERVATION *entry;
    if (!state || !reservation || !launcher_pid || !launcher_generation) return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&state->lock);
    entry=find(state,reservation);
    if (!entry) { LeaveCriticalSection(&state->lock);return ERROR_NOT_FOUND; }
    if (entry->launcher_pid!=launcher_pid || entry->launcher_generation!=launcher_generation) {
        LeaveCriticalSection(&state->lock);return ERROR_ACCESS_DENIED;
    }
    RemoveEntryList(&entry->link);LeaveCriticalSection(&state->lock);
    broker_vdm_receipts_drain(&entry->streams);
    if (entry->worker) CloseHandle(entry->worker);
    HeapFree(GetProcessHeap(),0,entry);
    return ERROR_SUCCESS;
}
