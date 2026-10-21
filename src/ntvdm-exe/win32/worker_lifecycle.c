/* Process lifecycle is independent of the optional character frontend. */
#include "worker_lifecycle.h"
#include "worker-base/connection.h"
#include "worker-base/input_watch.h"
#include "common/protocol/frontend_lifecycle.h"
#include "ntsrv-exe/opennt/include/base_rpc_client.h"

extern BOOL CntrlHandler(ULONG type);
extern VOID TerminateVDM(VOID);
extern UINT VdmExitCode;
static struct {
    session *owner;
    HANDLE stop,console_close,worker_close,fault,server;
    worker_base_input_watch *watch;
    volatile LONG closing;
    BOOL original_ready;
} lifecycle;

DWORD ntvdm_worker_lifecycle_fault_event(HANDLE *event)
{
    if(!event)return ERROR_INVALID_PARAMETER;
    *event=NULL;
    if(!lifecycle.fault)return ERROR_INVALID_STATE;
    return DuplicateHandle(GetCurrentProcess(),lifecycle.fault,GetCurrentProcess(),
        event,EVENT_MODIFY_STATE,FALSE,0) ? ERROR_SUCCESS : GetLastError();
}
static DWORD report_io_fault(void *context)
{
    (void)context;
    if(!InterlockedCompareExchange(&lifecycle.closing,0,0))
        (void)worker_base_request_exit();
    /* Do not acknowledge/rearm this local fault. The watcher still receives
     * service close instructions; submission never grants local teardown. */
    return ERROR_SUCCESS;
}

static DWORD WINAPI close_console(void *context)
{
    session *owner=context;
    /* The grace helper may run on a new thread. Transfer the already claimed
     * cleanup right to that thread before it enters any original cleanup. */
    InterlockedExchange(&lifecycle.closing,(LONG)GetCurrentThreadId());
    if(!session_thread_bind(owner))return ERROR_INVALID_STATE;
    (void)CntrlHandler(CTRL_CLOSE_EVENT);
    (void)session_thread_unbind(owner);
    return ERROR_SUCCESS;
}
static DWORD WINAPI close_worker(void *context)
{
    session *owner=context;
    InterlockedExchange(&lifecycle.closing,(LONG)GetCurrentThreadId());
    if(!session_thread_bind(owner))return ERROR_INVALID_STATE;
    /* Original TerminateVDM selects DOS or whole-WOW cleanup itself. */
    TerminateVDM();
    (void)session_thread_unbind(owner);
    return ERROR_PROCESS_ABORTED;
}
static void console_instruction(void *context)
{
    if(InterlockedCompareExchange(&lifecycle.closing,(LONG)GetCurrentThreadId(),0))return;
    worker_base_shutdown_close(close_console,context,
        OPENNT_LIFECYCLE_GRACE_MS,CONTROL_C_EXIT,NULL);
}
static void worker_instruction(void *context)
{
    if(InterlockedCompareExchange(&lifecycle.closing,(LONG)GetCurrentThreadId(),0))return;
    worker_base_shutdown_close(close_worker,context,INFINITE,ERROR_CANCELLED,NULL);
}
static void service_lost(void *context)
{
    if(InterlockedCompareExchange(&lifecycle.closing,(LONG)GetCurrentThreadId(),0))return;
    /* Original DOS/WOW termination owns guest and host cleanup. RPC failure
     * is already latched, so ExitVDM cannot wait on the dead service. */
    VdmExitCode=ERROR_INVALID_HANDLE;
    worker_base_shutdown_close(close_worker,context,INFINITE,ERROR_INVALID_HANDLE,NULL);
}
void ntvdm_worker_lifecycle_authorize_exit(void)
{
    HANDLE waits[3];DWORD count=2,wait;
    LONG thread=(LONG)GetCurrentThreadId(),previous;
    /* In-process source fixtures do not establish a standalone worker. */
    if(!lifecycle.owner)return;
    previous=InterlockedCompareExchange(&lifecycle.closing,thread,0);
    if(previous==thread)return;
    if(previous) {
        /* Another owner is already performing terminal cleanup. Do not enter
         * terminate's non-atomic repeated-call shortcut and kill it midway. */
        WaitForSingleObject(GetCurrentProcess(),INFINITE);
        return;
    }
    (void)worker_base_request_exit();
    waits[0]=lifecycle.server;waits[1]=lifecycle.worker_close;
    if(lifecycle.console_close)waits[count++]=lifecycle.console_close;
    wait=WaitForMultipleObjects(count,waits,FALSE,INFINITE);
    if(wait==WAIT_OBJECT_0) {
        VdmExitCode=ERROR_INVALID_HANDLE;
        return;
    }
    if(wait==WAIT_OBJECT_0+1)return;
    if(count==3 && wait==WAIT_OBJECT_0+2 && !lifecycle.original_ready)return;
    if(count==3 && wait==WAIT_OBJECT_0+2)
        worker_base_shutdown_close(close_console,lifecycle.owner,
            OPENNT_LIFECYCLE_GRACE_MS,CONTROL_C_EXIT,NULL);
    /* Failed waits/requests are not permission. Do not invent a timeout,
     * reconnect or fall through into original resource destruction. */
    WaitForSingleObject(GetCurrentProcess(),INFINITE);
}
void ntvdm_worker_lifecycle_end(void)
{
    worker_base_unwatch_service();
    if(lifecycle.stop)SetEvent(lifecycle.stop);
    worker_base_input_watch_destroy(lifecycle.watch);
    if(lifecycle.stop)CloseHandle(lifecycle.stop);
    if(lifecycle.fault)CloseHandle(lifecycle.fault);
    /* The connection owns all three lifecycle handles. */
    ZeroMemory(&lifecycle,sizeof(lifecycle));
}
DWORD WINAPI NtvdmAuthorizeExit(DWORD exit_code)
{
    ntvdm_worker_lifecycle_authorize_exit();
    return OpenNtBaseClientFailure(0)==ERROR_INVALID_HANDLE ? ERROR_INVALID_HANDLE : exit_code;
}
DWORD ntvdm_worker_lifecycle_begin(session *owner,BOOL character_io)
{
    DWORD error;
    if(!owner || lifecycle.owner)return ERROR_INVALID_STATE;
    error=worker_base_controls(&lifecycle.server,&lifecycle.worker_close,&lifecycle.console_close);
    if(error)return error;
    lifecycle.owner=owner;
    if(!character_io)lifecycle.console_close=NULL;
    lifecycle.stop=CreateEventW(NULL,TRUE,FALSE,NULL);
    if(!lifecycle.stop)return GetLastError();
    /* WOW has no Console-close membership. No frontend is created or queried. */
    if(character_io) {
        lifecycle.fault=CreateEventW(NULL,TRUE,FALSE,NULL);
        if(!lifecycle.fault)return GetLastError();
    }
    return ERROR_SUCCESS;
}
DWORD ntvdm_worker_lifecycle_activate(void)
{
    DWORD error;
    if(!lifecycle.owner)return ERROR_SUCCESS; /* Non-standalone fixtures. */
    if(lifecycle.original_ready || !lifecycle.stop)return ERROR_INVALID_STATE;
    lifecycle.original_ready=TRUE;
    lifecycle.watch=worker_base_input_watch_create(lifecycle.console_close,
        lifecycle.worker_close,lifecycle.stop,report_io_fault,console_instruction,
        worker_instruction,NULL,lifecycle.owner);
    if(!lifecycle.watch)return GetLastError();
    if(lifecycle.fault) {
        error=worker_base_input_watch_bind(lifecycle.watch,lifecycle.fault);
        if(error)return error;
    }
    return worker_base_watch_service(service_lost,lifecycle.owner);
}
void NtvdmActivateLifecycle(void)
{
    DWORD error=ntvdm_worker_lifecycle_activate();
    if(error) {
        VdmExitCode=NtvdmAuthorizeExit(error);
        TerminateVDM();
    }
}
