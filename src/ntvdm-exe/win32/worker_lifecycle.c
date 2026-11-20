/* Process lifecycle is independent of the optional character frontend. */
#include "worker_lifecycle.h"
#include "compat/include/mvdm_softpc_event_thread.h"
#include "compat/include/mvdm_softpc_worker_exit.h"
#include "worker-base/connection.h"
#include "worker-base/input_watch.h"
#include "common/protocol/frontend_lifecycle.h"
#include "common/base_srv/base_rpc_client.h"
#include "ntvdm-exe/softpc/include/mvdm_standalone_worker.h"

extern BOOL CntrlHandler(ULONG type);
extern VOID TerminateVDM(VOID);
extern UINT VdmExitCode;
extern HANDLE hWOWDll;
static struct {
    session *owner;
    HANDLE stop,task_close,fault;
    worker_base_input_watch *watch;
    PTP_WAIT exit_wait;
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
    if(!InterlockedCompareExchange(&mvdm_softpc_exit_owner,0,0))
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
    InterlockedExchange(&mvdm_softpc_exit_owner,(LONG)GetCurrentThreadId());
    if(!session_thread_bind(owner))return ERROR_INVALID_STATE;
    (void)CntrlHandler(CTRL_CLOSE_EVENT);
    (void)session_thread_unbind(owner);
    return ERROR_SUCCESS;
}
static DWORD WINAPI close_worker(void *context)
{
    session *owner=context;
    InterlockedExchange(&mvdm_softpc_exit_owner,(LONG)GetCurrentThreadId());
    if(!session_thread_bind(owner))return ERROR_INVALID_STATE;
    /* Original TerminateVDM selects DOS or whole-WOW cleanup itself. */
    TerminateVDM();
    (void)session_thread_unbind(owner);
    return ERROR_PROCESS_ABORTED;
}
static void console_instruction(void *context)
{
    if(InterlockedCompareExchange(&mvdm_softpc_exit_owner,(LONG)GetCurrentThreadId(),0))return;
    worker_base_shutdown_close(close_console,context,
        OPENNT_LIFECYCLE_GRACE_MS,CONTROL_C_EXIT,NULL);
}
static void worker_instruction(void *context)
{
    if(InterlockedCompareExchange(&mvdm_softpc_exit_owner,(LONG)GetCurrentThreadId(),0))return;
    worker_base_shutdown_close(close_worker,context,INFINITE,ERROR_CANCELLED,NULL);
}
static DWORD task_instruction(void *context)
{
    typedef DWORD (WINAPI *terminate_ticket_fn)(uint64_t);
    terminate_ticket_fn terminate_ticket;
    uint64_t identity=0;DWORD error;
    (void)context;
    error=worker_base_take_wow_task_close(&identity);
    if(error==ERROR_NOT_FOUND)return ERROR_SUCCESS; /* stale source record retired normally */
    if(error)return error;
    if(!identity || !hWOWDll)error=ERROR_INVALID_STATE;
    else {
        terminate_ticket=(terminate_ticket_fn)GetProcAddress(hWOWDll,"W32TerminateTaskTicket");
        error=terminate_ticket ? terminate_ticket(identity) : ERROR_PROC_NOT_FOUND;
    }
    if(!error)return ERROR_SUCCESS;
    /* Injection was not accepted locally. Release this request for a later
     * authenticated management action; do not spin or kill the whole worker. */
    return worker_base_fail_wow_task_close(identity);
}
static void service_lost(void *context)
{
    if(InterlockedCompareExchange(&mvdm_softpc_exit_owner,(LONG)GetCurrentThreadId(),0))return;
    /* Original DOS/WOW termination owns guest and host cleanup. RPC failure
     * is already latched, so ExitVDM cannot wait on the dead service. */
    VdmExitCode=ERROR_INVALID_HANDLE;
    worker_base_shutdown_close(close_worker,context,INFINITE,ERROR_INVALID_HANDLE,NULL);
}
static void CALLBACK worker_exit_request(PTP_CALLBACK_INSTANCE instance,
    void *context, PTP_WAIT wait, TP_WAIT_RESULT result)
{
    mvdm_softpc_exit_handoff *exit = context;
    DWORD status = ERROR_SUCCESS;
    mvdm_softpc_exit_operation operation = exit->operation;
    (void)result;
    switch (operation) {
    case MVDM_EXIT_CONSOLE_GROUP:
        status = worker_base_request_console_exit();
        break;
    case MVDM_EXIT_PARTICIPANT:
        status = worker_base_request_exit();
        break;
    case MVDM_EXIT_STATUS_ON_LOSS:
        VdmExitCode = ERROR_INVALID_HANDLE;
        /* fall through */
    case MVDM_EXIT_STATUS:
        status = OpenNtBaseClientFailure(0);
        break;
    case MVDM_EXIT_CLOSE_FRONTENDS:
        status = mvdm_standalone_worker_close_frontends();
        break;
    case MVDM_EXIT_CONSOLE_CONTROL:
        /* Release the handoff before the Console handler claims cleanup. */
        (void)CallbackMayRunLong(instance);
        break;
    }
    exit->status = status;
    SetThreadpoolWait(wait, exit->request, NULL);
    SetEvent(exit->completion);
    if (operation == MVDM_EXIT_CONSOLE_CONTROL)
        worker_base_shutdown_close(close_console,lifecycle.owner,
            OPENNT_LIFECYCLE_GRACE_MS,CONTROL_C_EXIT,NULL);
}

void ntvdm_worker_lifecycle_end(void)
{
    mvdm_softpc_exit_handoff *exit = &mvdm_softpc_exit_request;
    if (lifecycle.exit_wait) {
        SetThreadpoolWait(lifecycle.exit_wait, NULL, NULL);
        WaitForThreadpoolWaitCallbacks(lifecycle.exit_wait, TRUE);
        CloseThreadpoolWait(lifecycle.exit_wait);
    }
    if (exit->request) CloseHandle(exit->request);
    if (exit->completion) CloseHandle(exit->completion);
    ZeroMemory(exit, sizeof(*exit));
    worker_base_unwatch_service();
    if(lifecycle.stop)SetEvent(lifecycle.stop);
    worker_base_input_watch_destroy(lifecycle.watch);
    if(lifecycle.stop)CloseHandle(lifecycle.stop);
    if(lifecycle.fault)CloseHandle(lifecycle.fault);
    /* The connection owns all three lifecycle handles. */
    ZeroMemory(&mvdm_softpc_input_terminal_events,sizeof(mvdm_softpc_input_terminal_events));
    mvdm_softpc_exit_rundown = NULL;
    mvdm_softpc_exit_original_ready = FALSE;
    mvdm_softpc_exit_owner = 0;
    ZeroMemory(&lifecycle,sizeof(lifecycle));
}
DWORD ntvdm_worker_lifecycle_exit_result(DWORD exit_code)
{
    ntvdm_worker_lifecycle_authorize_exit();
    return OpenNtBaseClientFailure(0)==ERROR_INVALID_HANDLE ? ERROR_INVALID_HANDLE : exit_code;
}
DWORD ntvdm_worker_lifecycle_begin(session *owner,BOOL character_io)
{
    DWORD error;
    mvdm_softpc_terminal_events events;
    mvdm_softpc_exit_handoff *exit = &mvdm_softpc_exit_request;
    if(!owner || lifecycle.owner || lifecycle.exit_wait)return ERROR_INVALID_STATE;
    exit->request = CreateEventW(NULL, FALSE, FALSE, NULL);
    if (!exit->request) return GetLastError();
    exit->completion = CreateEventW(NULL, FALSE, FALSE, NULL);
    if (!exit->completion) return GetLastError();
    lifecycle.exit_wait = CreateThreadpoolWait(worker_exit_request, exit, NULL);
    if (!lifecycle.exit_wait) return GetLastError();
    SetThreadpoolWait(lifecycle.exit_wait, exit->request, NULL);
    error=worker_base_controls(&events.server,&events.worker_close,&events.console_close,&lifecycle.task_close);
    if(error)return error;
    if(!character_io)events.console_close=NULL;
    lifecycle.owner=owner;
    mvdm_softpc_input_terminal_events=events;
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
    if(mvdm_softpc_exit_original_ready || !lifecycle.stop)return ERROR_INVALID_STATE;
    mvdm_softpc_exit_original_ready=TRUE;
    lifecycle.watch=worker_base_input_watch_create(mvdm_softpc_input_terminal_events.console_close,
        mvdm_softpc_input_terminal_events.worker_close,lifecycle.task_close,lifecycle.stop,report_io_fault,console_instruction,
        worker_instruction,task_instruction,NULL,lifecycle.owner);
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
        VdmExitCode=ntvdm_worker_lifecycle_exit_result(error);
        TerminateVDM();
    }
}
