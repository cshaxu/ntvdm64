/* NT4 process.c lookup holds the registry lock until CsrUnlockProcess.
 * Retain that critical-section lifetime over explicitly registered local
 * records. Full CSR root/reference/deletion machinery is not imported.
 * Authentication, task selection and worker role assignment are not here. */
#include "compat/include/base_process.h"
#include "compat/include/base_request.h"

static __declspec(thread) OPENNT_BASE_PROCESS_REGISTRY *registry;

BOOL OpenNtBaseInitializeProcessRegistry(OPENNT_BASE_PROCESS_REGISTRY *state,
    const OPENNT_BASE_PROCESS_PROVIDER *provider)
{
    if (!state || !provider || !provider->record || !provider->view) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    if (!InitializeCriticalSectionEx(&state->Lock,0,0)) return FALSE;
    state->Provider=provider;
    state->Processes.Flink=state->Processes.Blink=&state->Processes;
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

BOOL OpenNtBaseRegisterProcess(OPENNT_BASE_PROCESS_REGISTRY *state, struct _CSR_PROCESS * process, HANDLE handle)
{
    HANDLE owned=NULL;
    DWORD pid;
    LIST_ENTRY *entry;
    BOOL result=FALSE;
    OPENNT_BASE_PROCESS_VIEW view;
    if (!state || !process) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    if (!DuplicateHandle(GetCurrentProcess(),handle,GetCurrentProcess(),&owned,
        0,FALSE,DUPLICATE_SAME_ACCESS)) return FALSE;
    pid=GetProcessId(owned);
    if (!pid || WaitForSingleObject(owned,0)!=WAIT_TIMEOUT) {
        CloseHandle(owned); SetLastError(ERROR_INVALID_HANDLE); return FALSE;
    }
    EnterCriticalSection(&state->Lock);
    for (entry=state->Processes.Flink;entry!=&state->Processes;entry=entry->Flink) {
        struct _CSR_PROCESS *current=state->Provider->record(entry);
        state->Provider->view(current,&view);
        if (current==process || *view.pid==(HANDLE)(ULONG_PTR)pid) {
            SetLastError(ERROR_ALREADY_EXISTS); goto done;
        }
    }
    if (state->NextSequence==MAXDWORD) { SetLastError(ERROR_ARITHMETIC_OVERFLOW); goto done; }
    state->Provider->view(process,&view);
    *view.pid=(HANDLE)(ULONG_PTR)pid;
    *view.handle=owned;
    *view.sequence=++state->NextSequence;
    view.link->Blink=state->Processes.Blink; view.link->Flink=&state->Processes;
    state->Processes.Blink->Flink=view.link; state->Processes.Blink=view.link;
    result=TRUE;
done:
    LeaveCriticalSection(&state->Lock);
    if (!result) { DWORD error=GetLastError(); CloseHandle(owned); SetLastError(error); }
    return result;
}

BOOL OpenNtBaseRemoveProcess(OPENNT_BASE_PROCESS_REGISTRY *state, struct _CSR_PROCESS * process)
{
    LIST_ENTRY *entry;
    BOOL found=FALSE;
    OPENNT_BASE_PROCESS_VIEW view;
    if (!state || !process) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    EnterCriticalSection(&state->Lock);
    if (state->Pins) {
        LeaveCriticalSection(&state->Lock); SetLastError(ERROR_BUSY); return FALSE;
    }
    for (entry=state->Processes.Flink;entry!=&state->Processes;entry=entry->Flink) {
        if (state->Provider->record(entry)==process) {
            state->Provider->view(process,&view);
            entry->Blink->Flink=entry->Flink; entry->Flink->Blink=entry->Blink;
            CloseHandle(*view.handle);
            *view.handle=NULL;
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
    empty=state->Pins==0 && (state->Processes.Flink==&state->Processes);
    LeaveCriticalSection(&state->Lock);
    return empty;
}

LONG NTAPI CsrLockProcessByClientId(HANDLE id, struct _CSR_PROCESS * *out)
{
    LIST_ENTRY *entry;
    if (!out) return (LONG)0xc000000dL;
    *out=NULL;
    if (!registry) return (LONG)0xc0000001L;
    EnterCriticalSection(&registry->Lock);
    for (entry=registry->Processes.Flink;entry!=&registry->Processes;entry=entry->Flink) {
        struct _CSR_PROCESS *process=registry->Provider->record(entry);
        OPENNT_BASE_PROCESS_VIEW view;
        registry->Provider->view(process,&view);
        if (*view.pid==id) {
            if (registry->Pins==MAXDWORD) {
                LeaveCriticalSection(&registry->Lock);
                return (LONG)0xc000009aL;
            }
            ++registry->Pins;
            *out=process;
            return 0; /* Lock pins record and owned process handle until unlock. */
        }
    }
    LeaveCriticalSection(&registry->Lock);
    return (LONG)0xc0000001L;
}

LONG NTAPI CsrUnlockProcess(struct _CSR_PROCESS * process)
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
    if (!(state->Processes.Flink==&state->Processes)) { SetLastError(ERROR_BUSY); return FALSE; }
    DeleteCriticalSection(&state->Lock);
    return TRUE;
}

BOOL OpenNtBaseRetainRegisteredProcess(OPENNT_BASE_PROCESS_REGISTRY *state,
    DWORD pid,ULONG generation,HANDLE *output)
{
    OPENNT_BASE_PROCESS_REGISTRY *previous;
    struct _CSR_PROCESS * process=NULL;
    DWORD error=0;
    OPENNT_BASE_PROCESS_VIEW view;
    if (!output) {SetLastError(ERROR_INVALID_PARAMETER);return FALSE;}
    *output=NULL;
    if (!state || !pid || !generation) {SetLastError(ERROR_INVALID_PARAMETER);return FALSE;}
    previous=OpenNtBaseBindProcessRegistry(state);
    if (CsrLockProcessByClientId((HANDLE)(ULONG_PTR)pid,&process)<0) error=ERROR_NOT_FOUND;
    else {
        state->Provider->view(process,&view);
        if (*view.sequence!=generation) error=ERROR_ACCESS_DENIED;
        else if (WaitForSingleObject(*view.handle,0)!=WAIT_TIMEOUT) error=ERROR_PROCESS_ABORTED;
        else if (!DuplicateHandle(GetCurrentProcess(),*view.handle,
            GetCurrentProcess(),output,0,FALSE,DUPLICATE_SAME_ACCESS)) error=GetLastError();
        CsrUnlockProcess(process);
    }
    OpenNtBaseBindProcessRegistry(previous);
    if (error) SetLastError(error);
    return error==0;
}

/* XP csr/server/process.c: CsrImpersonateClient / CsrRevertToSelf.
 * Standalone requests authenticate a process, not an LPC client thread.
 * Use its pinned process token; never accept a token/identity from the wire.
 * Original CSR request ImpersonateCount remains the sole nesting owner. */
BOOLEAN NTAPI CsrImpersonateClient(struct _CSR_THREAD *thread)
{
    struct _CSR_THREAD *caller=OpenNtBaseServerRequestThread();
    HANDLE process,token;
    ULONG *count,*unused;
    BOOL ok;
    DWORD error;
    if(!thread)thread=caller;
    if(!registry || !registry->Provider->thread_view || !caller || !thread ||
       !registry->Provider->thread_view(caller,&process,&count) || !count || *count==MAXDWORD ||
       !registry->Provider->thread_view(thread,&process,&unused) || !process) {
        SetLastError(ERROR_INVALID_STATE);return FALSE;
    }
    if(!OpenProcessToken(process,TOKEN_QUERY|TOKEN_DUPLICATE,&token))return FALSE;
    ok=ImpersonateLoggedOnUser(token);error=GetLastError();
    CloseHandle(token);
    if(ok)++*count;
    else SetLastError(error);
    return (BOOLEAN)ok;
}

BOOLEAN NTAPI CsrRevertToSelf(void)
{
    struct _CSR_THREAD *caller=OpenNtBaseServerRequestThread();
    HANDLE process;
    ULONG *count;
    if(caller) {
        if(!registry || !registry->Provider->thread_view ||
           !registry->Provider->thread_view(caller,&process,&count) || !count || !*count)
            return FALSE;
        if(--*count)return TRUE;
    }
    return (BOOLEAN)RevertToSelf();
}

/* XP base/subsys/csr/server/apireqst.c:924, CsrValidateMessageBuffer.
 * Retain capture size/offset validation. Standalone's decoded, synchronous
 * server-local message is bound explicitly instead of forging its caller PID. */
BOOLEAN NTAPI CsrValidateMessageBuffer(const struct _CSR_API_MSG *message,
    const void *const *buffer, ULONG count, ULONG size)
{
    OPENNT_BASE_MESSAGE_VIEW view={0};
    ULONG_PTR length,end,offset;
    ULONG i;
    if(!message || !buffer || !size || count>MAXDWORD/size)return FALSE;
    length=count*size;
    if(!*buffer && !length)return TRUE;
    if(!registry || !registry->Provider->message_view)return FALSE;
    registry->Provider->message_view(message,&view);
    if(view.capture) {
        end=(ULONG_PTR)view.capture+view.length;
        if(length<=end-(ULONG_PTR)*buffer) {
            offset=(ULONG_PTR)buffer-(ULONG_PTR)message;
            for(i=0;i<view.count;++i)
                if(view.offsets[i]==offset)return TRUE;
        }
        return FALSE;
    }
    return view.caller==(HANDLE)(ULONG_PTR)GetCurrentProcessId() ||
        message==OpenNtBaseServerRequestMessage();
}
