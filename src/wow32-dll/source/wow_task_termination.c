#include "precomp.h"
#include "wow_task_termination.h"
#include "ntsrv-exe/opennt/include/base_rpc_client.h"

typedef struct wow_task_termination_entry {
    struct wow_task_termination_entry *next;
    uint64_t ticket;
    USHORT htask;
} wow_task_termination_entry;

static INIT_ONCE wow_task_termination_once=INIT_ONCE_STATIC_INIT;
static CRITICAL_SECTION wow_task_termination_lock;
static wow_task_termination_entry *wow_task_termination_entries;
/* W32HungAppNotifyThread owns one source-global completion event and target
 * TDB slot.  USER invokes that registered handler serially; preserve that
 * condition at the project management ingress. */
static uint64_t wow_task_termination_active;

DWORD WINAPI W32HungAppNotifyThread(UINT htask);

typedef struct wow_task_termination_wait {
    uint64_t ticket;
    HANDLE ready;
    HANDLE handler;
} wow_task_termination_wait;

static DWORD WINAPI wow_task_termination_wait_for_handler(PVOID parameter)
{
    wow_task_termination_wait *wait=parameter;
    DWORD result=WaitForSingleObject(wait->ready,INFINITE);
    CloseHandle(wait->ready);
    if(result==WAIT_OBJECT_0) {
        if(wait->handler) {
            WaitForSingleObject(wait->handler,INFINITE);
            CloseHandle(wait->handler);
        }
    }
    EnterCriticalSection(&wow_task_termination_lock);
    if(wow_task_termination_active==wait->ticket)
        wow_task_termination_active=0;
    LeaveCriticalSection(&wow_task_termination_lock);
    HeapFree(GetProcessHeap(),0,wait);
    return 0;
}

static BOOL CALLBACK wow_task_termination_initialize(PINIT_ONCE once,
    PVOID parameter,PVOID *context)
{
    (void)once;(void)parameter;(void)context;
    InitializeCriticalSection(&wow_task_termination_lock);
    return TRUE;
}

static BOOL wow_task_termination_ready(void)
{
    return InitOnceExecuteOnce(&wow_task_termination_once,
        wow_task_termination_initialize,NULL,NULL);
}

DWORD WINAPI wow_task_termination_bind(ULONG task,USHORT htask)
{
    wow_task_termination_entry *entry;
    uint64_t ticket=0;
    DWORD error=GetLastError();
    if(!htask || !wow_task_termination_ready())return ERROR_INVALID_PARAMETER;
    error=OpenNtBaseClientBindWowTask(task,htask,&ticket);
    if(error!=ERROR_SUCCESS || !ticket) {
        SetLastError(error);return error ? error : ERROR_INVALID_DATA;
    }
    entry=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*entry));
    if(!entry) {
        (void)OpenNtBaseClientRetireWowTask(ticket);
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);return ERROR_NOT_ENOUGH_MEMORY;
    }
    entry->ticket=ticket;entry->htask=htask;
    EnterCriticalSection(&wow_task_termination_lock);
    entry->next=wow_task_termination_entries;
    wow_task_termination_entries=entry;
    LeaveCriticalSection(&wow_task_termination_lock);
    SetLastError(error);
    return ERROR_SUCCESS;
}

void WINAPI wow_task_termination_retire(USHORT htask)
{
    wow_task_termination_entry **link,*entry=NULL;
    DWORD error=GetLastError();
    if(!htask || !wow_task_termination_ready())return;
    EnterCriticalSection(&wow_task_termination_lock);
    for(link=&wow_task_termination_entries;*link;link=&(*link)->next) {
        if((*link)->htask==htask) {
            entry=*link;*link=entry->next;break;
        }
    }
    if(entry && wow_task_termination_active==entry->ticket)
        wow_task_termination_active=0;
    LeaveCriticalSection(&wow_task_termination_lock);
    if(entry) {
        (void)OpenNtBaseClientRetireWowTask(entry->ticket);
        HeapFree(GetProcessHeap(),0,entry);
    }
    SetLastError(error);
}

void WINAPI wow_task_termination_fail(USHORT htask)
{
    wow_task_termination_entry *entry;
    uint64_t ticket=0;
    DWORD error=GetLastError();
    if(!htask || !wow_task_termination_ready())return;
    EnterCriticalSection(&wow_task_termination_lock);
    for(entry=wow_task_termination_entries;entry;entry=entry->next)
        if(entry->htask==htask) {ticket=entry->ticket;break;}
    if(ticket && wow_task_termination_active==ticket)
        wow_task_termination_active=0;
    LeaveCriticalSection(&wow_task_termination_lock);
    /* This is only the original explicit user-cancel path.  It reopens the
     * already authenticated management row for a later user request; it
     * neither retries nor changes original hung-task policy. */
    if(ticket)(void)OpenNtBaseClientFailWowTaskClose(ticket);
    SetLastError(error);
}

DWORD WINAPI W32TerminateTaskTicket(uint64_t ticket)
{
    wow_task_termination_entry *entry;
    USHORT htask=0;
    BOOL found=FALSE;
    HANDLE thread,watcher;
    wow_task_termination_wait *wait;
    if(!ticket || !wow_task_termination_ready())return ERROR_INVALID_PARAMETER;
    EnterCriticalSection(&wow_task_termination_lock);
    for(entry=wow_task_termination_entries;entry;entry=entry->next) {
        if(entry->ticket==ticket) {htask=entry->htask;found=TRUE;break;}
    }
    if(found && wow_task_termination_active)htask=0;
    if(found && htask)wow_task_termination_active=ticket;
    LeaveCriticalSection(&wow_task_termination_lock);
    if(!found)return ERROR_NOT_FOUND;
    if(!htask)return ERROR_BUSY;
    wait=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*wait));
    if(!wait) {
        EnterCriticalSection(&wow_task_termination_lock);
        if(wow_task_termination_active==ticket)wow_task_termination_active=0;
        LeaveCriticalSection(&wow_task_termination_lock);
        return ERROR_NOT_ENOUGH_MEMORY;
    }
    wait->ticket=ticket;
    wait->ready=CreateEventW(NULL,TRUE,FALSE,NULL);
    if(!wait->ready) {
        DWORD error=GetLastError();
        HeapFree(GetProcessHeap(),0,wait);
        EnterCriticalSection(&wow_task_termination_lock);
        if(wow_task_termination_active==ticket)wow_task_termination_active=0;
        LeaveCriticalSection(&wow_task_termination_lock);
        return error;
    }
    watcher=CreateThread(NULL,0,wow_task_termination_wait_for_handler,wait,0,NULL);
    if(!watcher) {
        DWORD error=GetLastError();
        CloseHandle(wait->ready);
        HeapFree(GetProcessHeap(),0,wait);
        EnterCriticalSection(&wow_task_termination_lock);
        if(wow_task_termination_active==ticket)wow_task_termination_active=0;
        LeaveCriticalSection(&wow_task_termination_lock);
        return error;
    }
    CloseHandle(watcher);
    /* The original routine exits its calling thread.  Match the original
     * debugger route by giving it a fresh local thread, never a remote one.
     * The already-running waiter owns that handle and releases only the source
     * handler's serial gate; task retirement/failure retains ticket state. */
    thread=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)W32HungAppNotifyThread,
        (PVOID)(ULONG_PTR)htask,0,NULL);
    if(!thread) {
        DWORD error=GetLastError();
        EnterCriticalSection(&wow_task_termination_lock);
        if(wow_task_termination_active==ticket)wow_task_termination_active=0;
        LeaveCriticalSection(&wow_task_termination_lock);
        SetEvent(wait->ready);
        return error;
    }
    wait->handler=thread;
    SetEvent(wait->ready);
    return ERROR_SUCCESS;
}
