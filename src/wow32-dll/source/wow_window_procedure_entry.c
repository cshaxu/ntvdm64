#include "wow_window_procedure_entry.h"
#include "wow_window_words_binding.h"

#if !defined(_M_IX86)
#error The recovered WOW native procedure entry uses the Win32 x86 ABI.
#endif

/* ADAPTER-WOW-072. Original USER can dispatch a WNDPROC_WOW word directly
 * through pfnWowWndProcEx. Modern native USER cannot call that guest word.
 * Supply only the missing native ABI entry; original DispatchClientMessage,
 * W32Win16WndProcEx and CallBack16 continue to perform actual WOW work.
 *
 * This cache is not another window-procedure registry. A descriptor never
 * changes target and is not keyed by HWND/class. Native USER owns the WND
 * slot; original CLS owns WOW class behavior. An old entry must still call
 * its saved original target rather than either owner's current selection.
 * Original wsubcls.c likewise retains saved thunk identities for the worker
 * lifetime. The existing provider load lifetime must cover native callbacks.
 */
typedef struct wow_window_procedure_entry {
    struct wow_window_procedure_entry *next;
    wow_window_dispatch_target target;
    BYTE *code;
} wow_window_procedure_entry;

static SRWLOCK entry_lock = SRWLOCK_INIT;
static wow_window_procedure_entry *entries;

static LRESULT WINAPI enter_guest(wow_window_procedure_entry *entry,
    HWND window, UINT message, WPARAM wp, LPARAM lp)
{
    LONG result = message == WM_CREATE ? -1 : 0;
    (void)wow_window_dispatch_immutable(window,message,wp,lp,&entry->target,&result);
    return result;
}

static wow_window_procedure_entry *find_target(const wow_window_dispatch_target *target)
{
    wow_window_procedure_entry *entry;
    for (entry=entries; entry; entry=entry->next)
        if (entry->target.procedure == target->procedure &&
                entry->target.callback == target->callback)
            return entry;
    return NULL;
}

WNDPROC WINAPI wow_window_entry_for_guest(const wow_window_dispatch_target *target)
{
    wow_window_procedure_entry *entry, *existing;
    DWORD saved=GetLastError(), error, old_protection;
    ULONG immediate, relative;
    SYSTEM_INFO system;
    if (!target || !target->wow_encoded ||
            !(target->procedure & WOW_WINDOW_PROC_TAG) || !target->callback) {
        SetLastError(ERROR_INVALID_PARAMETER); return NULL;
    }
    AcquireSRWLockShared(&entry_lock);
    existing=find_target(target);
    ReleaseSRWLockShared(&entry_lock);
    if (existing) { SetLastError(saved); return (WNDPROC)existing->code; }

    entry=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*entry));
    if (!entry) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return NULL; }
    entry->target=*target;
    GetSystemInfo(&system);
    entry->code=VirtualAlloc(NULL,system.dwPageSize,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);
    if (!entry->code) goto failed;
    /* stdcall caller: [return, HWND, message, WPARAM, LPARAM]. Insert the
     * immutable descriptor after the return address, then tail-jump to the
     * five-argument stdcall entrance. Its ret20 removes our added argument
     * as well as the caller's original four. The original return address
     * remains unchanged, including on hosts with shadow-stack checking.
     * EAX is volatile in the selected x86 callback ABI. */
    entry->code[0]=0x58; /* pop eax */
    entry->code[1]=0x68; /* push descriptor */
    immediate=(ULONG)(ULONG_PTR)entry;
    CopyMemory(entry->code+2,&immediate,sizeof(immediate));
    entry->code[6]=0x50; /* push eax */
    entry->code[7]=0xe9; /* jmp enter_guest */
    relative=(ULONG)(ULONG_PTR)enter_guest-(ULONG)(ULONG_PTR)(entry->code+12);
    CopyMemory(entry->code+8,&relative,sizeof(relative));
    if (!VirtualProtect(entry->code,system.dwPageSize,PAGE_EXECUTE_READ,&old_protection) ||
            !FlushInstructionCache(GetCurrentProcess(),entry->code,12)) goto failed;

    /* Publish only complete executable entries. Concurrent duplicate creation
     * may discard this unpublished allocation; published entries never move
     * or acquire new targets while a native caller can retain their address. */
    AcquireSRWLockExclusive(&entry_lock);
    existing=find_target(target);
    if (!existing) {
        entry->next=entries;
        entries=entry;
    }
    ReleaseSRWLockExclusive(&entry_lock);
    if (existing) {
        VirtualFree(entry->code,0,MEM_RELEASE);
        HeapFree(GetProcessHeap(),0,entry);
        entry=existing;
    }
    SetLastError(saved);
    return (WNDPROC)entry->code;
failed:
    error=GetLastError();
    if (entry->code) VirtualFree(entry->code,0,MEM_RELEASE);
    HeapFree(GetProcessHeap(),0,entry);
    SetLastError(error);
    return NULL;
}

BOOL WINAPI wow_window_entry_guest_target(WNDPROC procedure,
    wow_window_dispatch_target *target)
{
    wow_window_procedure_entry *entry;
    DWORD saved=GetLastError();
    BOOL found=FALSE;
    if (!target) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    AcquireSRWLockShared(&entry_lock);
    for (entry=entries; entry; entry=entry->next) {
        if ((WNDPROC)entry->code == procedure) {
            *target=entry->target;
            found=TRUE;
            break;
        }
    }
    ReleaseSRWLockShared(&entry_lock);
    SetLastError(saved);
    return found;
}
