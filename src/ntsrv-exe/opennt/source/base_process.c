/* NT4 process.c lookup holds the registry lock until CsrUnlockProcess.
 * Retain that critical-section lifetime over explicitly registered local
 * records. Full CSR root/reference/deletion machinery is not imported.
 * Authentication, task selection and worker role assignment are not here. */
#include <base_process.h>
static __declspec(thread) OPENNT_BASE_PROCESS_REGISTRY *registry;

BOOL OpenNtBaseInitializeProcessRegistry(OPENNT_BASE_PROCESS_REGISTRY *state)
{
    if (!state) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    if (!InitializeCriticalSectionEx(&state->Lock,0,0)) return FALSE;
    InitializeListHead(&state->Processes);
    state->NextSequence=0;
    state->Pins=0;
    return TRUE;
}

OPENNT_BASE_PROCESS_REGISTRY *OpenNtBaseBindProcessRegistry(OPENNT_BASE_PROCESS_REGISTRY *state)
{
    OPENNT_BASE_PROCESS_REGISTRY *previous=registry;
    registry=state;
    return previous;
}

BOOL OpenNtBaseRegisterProcess(OPENNT_BASE_PROCESS_REGISTRY *state, PCSR_PROCESS process, HANDLE handle)
{
    HANDLE owned=NULL;
    DWORD pid;
    LIST_ENTRY *entry;
    BOOL result=FALSE;
    if (!state || !process) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    if (!DuplicateHandle(GetCurrentProcess(),handle,GetCurrentProcess(),&owned,
        0,FALSE,DUPLICATE_SAME_ACCESS)) return FALSE;
    pid=GetProcessId(owned);
    if (!pid || WaitForSingleObject(owned,0)!=WAIT_TIMEOUT) {
        CloseHandle(owned); SetLastError(ERROR_INVALID_HANDLE); return FALSE;
    }
    EnterCriticalSection(&state->Lock);
    for (entry=state->Processes.Flink;entry!=&state->Processes;entry=entry->Flink) {
        PCSR_PROCESS current=CONTAINING_RECORD(entry,CSR_PROCESS,ListLink);
        if (current==process || current->ClientId.UniqueProcess==(HANDLE)pid) {
            SetLastError(ERROR_ALREADY_EXISTS); goto done;
        }
    }
    if (state->NextSequence==MAXULONG) { SetLastError(ERROR_ARITHMETIC_OVERFLOW); goto done; }
    process->ClientId.UniqueProcess=(HANDLE)pid;
    process->ProcessHandle=owned;
    process->SequenceNumber=++state->NextSequence;
    InsertTailList(&state->Processes,&process->ListLink);
    result=TRUE;
done:
    LeaveCriticalSection(&state->Lock);
    if (!result) { DWORD error=GetLastError(); CloseHandle(owned); SetLastError(error); }
    return result;
}

BOOL OpenNtBaseRemoveProcess(OPENNT_BASE_PROCESS_REGISTRY *state, PCSR_PROCESS process)
{
    LIST_ENTRY *entry;
    BOOL found=FALSE;
    if (!state || !process) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    EnterCriticalSection(&state->Lock);
    if (state->Pins) {
        LeaveCriticalSection(&state->Lock); SetLastError(ERROR_BUSY); return FALSE;
    }
    for (entry=state->Processes.Flink;entry!=&state->Processes;entry=entry->Flink) {
        if (CONTAINING_RECORD(entry,CSR_PROCESS,ListLink)==process) {
            RemoveEntryList(entry);
            CloseHandle(process->ProcessHandle);
            process->ProcessHandle=NULL;
            found=TRUE;
            break;
        }
    }
    LeaveCriticalSection(&state->Lock);
    if (!found) SetLastError(ERROR_NOT_FOUND);
    return found;
}

BOOL OpenNtBaseProcessRegistryIsEmpty(OPENNT_BASE_PROCESS_REGISTRY *state)
{
    BOOL empty;
    if (!state) return FALSE;
    EnterCriticalSection(&state->Lock);
    empty=state->Pins==0 && IsListEmpty(&state->Processes);
    LeaveCriticalSection(&state->Lock);
    return empty;
}

NTSTATUS NTAPI CsrLockProcessByClientId(HANDLE id, PCSR_PROCESS *out)
{
    LIST_ENTRY *entry;
    if (!out) return (NTSTATUS)0xc000000dL;
    *out=NULL;
    if (!registry) return (NTSTATUS)0xc0000001L;
    EnterCriticalSection(&registry->Lock);
    for (entry=registry->Processes.Flink;entry!=&registry->Processes;entry=entry->Flink) {
        PCSR_PROCESS process=CONTAINING_RECORD(entry,CSR_PROCESS,ListLink);
        if (process->ClientId.UniqueProcess==id) {
            if (registry->Pins==MAXULONG) {
                LeaveCriticalSection(&registry->Lock);
                return (NTSTATUS)0xc000009aL;
            }
            ++registry->Pins;
            *out=process;
            return 0; /* Lock pins record and owned process handle until unlock. */
        }
    }
    LeaveCriticalSection(&registry->Lock);
    return (NTSTATUS)0xc0000001L;
}

NTSTATUS NTAPI CsrUnlockProcess(PCSR_PROCESS process)
{
    (void)process;
    --registry->Pins;
    LeaveCriticalSection(&registry->Lock);
    return 0;
}

BOOL OpenNtBaseDestroyProcessRegistry(OPENNT_BASE_PROCESS_REGISTRY *state)
{
    /* Owner must stop dispatch and unbind all threads before destruction. */
    if (!state) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    if (!IsListEmpty(&state->Processes)) { SetLastError(ERROR_BUSY); return FALSE; }
    DeleteCriticalSection(&state->Lock);
    return TRUE;
}

BOOL OpenNtBaseRetainRegisteredProcess(OPENNT_BASE_PROCESS_REGISTRY *state,
    DWORD pid,ULONG generation,HANDLE *output)
{
    OPENNT_BASE_PROCESS_REGISTRY *previous;
    PCSR_PROCESS process=NULL;
    DWORD error=0;
    if (!output) {SetLastError(ERROR_INVALID_PARAMETER);return FALSE;}
    *output=NULL;
    if (!state || !pid || !generation) {SetLastError(ERROR_INVALID_PARAMETER);return FALSE;}
    previous=OpenNtBaseBindProcessRegistry(state);
    if (CsrLockProcessByClientId((HANDLE)pid,&process)<0) error=ERROR_NOT_FOUND;
    else {
        if (process->SequenceNumber!=generation) error=ERROR_ACCESS_DENIED;
        else if (WaitForSingleObject(process->ProcessHandle,0)!=WAIT_TIMEOUT) error=ERROR_PROCESS_ABORTED;
        else if (!DuplicateHandle(GetCurrentProcess(),process->ProcessHandle,
            GetCurrentProcess(),output,0,FALSE,DUPLICATE_SAME_ACCESS)) error=GetLastError();
        CsrUnlockProcess(process);
    }
    OpenNtBaseBindProcessRegistry(previous);
    if (error) SetLastError(error);
    return error==0;
}
