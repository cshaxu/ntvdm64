#include "precomp.h"
#include "wow32-dll/include/wow_task_termination.h"
#include "common/base_srv/base_rpc_client.h"

static INIT_ONCE wow_task_termination_once=INIT_ONCE_STATIC_INIT;
static CRITICAL_SECTION wow_task_termination_lock;
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
    DWORD handler_exit;
    BOOL cancelled=FALSE;
    CloseHandle(wait->ready);
    if(result==WAIT_OBJECT_0 && wait->handler) {
        cancelled=WaitForSingleObject(wait->handler,INFINITE)==WAIT_OBJECT_0 &&
            GetExitCodeThread(wait->handler,&handler_exit) && handler_exit==ERROR_CANCELLED;
        CloseHandle(wait->handler);
    }
    EnterCriticalSection(&wow_task_termination_lock);
    if(wow_task_termination_active==wait->ticket)
        wow_task_termination_active=0;
    LeaveCriticalSection(&wow_task_termination_lock);
    /* Reopen the authenticated ticket this starter used for the handler. */
    if(cancelled)
        (void)OpenNtBaseClientFailWowTaskClose(wait->ticket);
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

DWORD WINAPI W32TerminateTaskTicket(uint64_t ticket)
{
    USHORT htask=0;
    DWORD error;
    HANDLE thread,watcher;
    wow_task_termination_wait *wait;
    if(!ticket || !wow_task_termination_ready())return ERROR_INVALID_PARAMETER;
    error=OpenNtBaseClientFindWowTask(ticket,&htask);
    if(error)return error;
    EnterCriticalSection(&wow_task_termination_lock);
    if(wow_task_termination_active) {
        LeaveCriticalSection(&wow_task_termination_lock);
        return ERROR_BUSY;
    }
    wow_task_termination_active=ticket;
    LeaveCriticalSection(&wow_task_termination_lock);
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
        error=GetLastError();
        HeapFree(GetProcessHeap(),0,wait);
        EnterCriticalSection(&wow_task_termination_lock);
        if(wow_task_termination_active==ticket)wow_task_termination_active=0;
        LeaveCriticalSection(&wow_task_termination_lock);
        return error;
    }
    watcher=CreateThread(NULL,0,wow_task_termination_wait_for_handler,wait,0,NULL);
    if(!watcher) {
        error=GetLastError();
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
        error=GetLastError();
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
