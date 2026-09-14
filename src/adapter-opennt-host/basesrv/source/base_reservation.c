/* Finite worker-launch binding below original BaseSrv record ownership.
 * This has no command queue or selection logic: srvvdm.c owns those. */
#include <base_reservation.h>

typedef struct OPENNT_BASE_RESERVATION {
    LIST_ENTRY link;
    uint64_t id;
    DWORD launcher_pid,launcher_generation;
    DWORD worker_pid,worker_generation;
    ULONG task;
    HANDLE console,worker;
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

DWORD OpenNtBaseReservationCreate(OPENNT_BASE_RESERVATIONS *state,DWORD launcher_pid,
    DWORD launcher_generation,ULONG task,HANDLE console,uint64_t *reservation)
{
    OPENNT_BASE_RESERVATION *entry;
    if (!state || !launcher_pid || !launcher_generation || !console || !reservation)
        return ERROR_INVALID_PARAMETER;
    *reservation=0;
    entry=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*entry));
    if (!entry) return ERROR_NOT_ENOUGH_MEMORY;
    EnterCriticalSection(&state->lock);
    if (!state->next) { LeaveCriticalSection(&state->lock);HeapFree(GetProcessHeap(),0,entry);return ERROR_ARITHMETIC_OVERFLOW; }
    entry->id=state->next++;
    entry->launcher_pid=launcher_pid;entry->launcher_generation=launcher_generation;
    entry->task=task;entry->console=console;
    InsertTailList(&state->entries,&entry->link);
    *reservation=entry->id;
    LeaveCriticalSection(&state->lock);
    return ERROR_SUCCESS;
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
    DWORD worker_generation,uint64_t *reservation,ULONG *task,HANDLE *console)
{
    LIST_ENTRY *cursor;
    if (!state || !worker_pid || !worker_generation || !reservation || !task || !console)
        return ERROR_INVALID_PARAMETER;
    *reservation=0;*task=0;*console=NULL;
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
        *reservation=entry->id;*task=entry->task;*console=entry->console;
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
    if (entry->worker) CloseHandle(entry->worker);
    HeapFree(GetProcessHeap(),0,entry);
    return ERROR_SUCCESS;
}
