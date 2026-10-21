/* Process lifecycle is independent of the optional character frontend. */
#include "worker_lifecycle.h"
#include "worker-base/connection.h"
#include "worker-base/input_watch.h"
#include "common/protocol/frontend_lifecycle.h"

extern BOOL CntrlHandler(ULONG type);
extern VOID TerminateVDM(VOID);
extern UINT VdmExitCode;
static struct {
    session *owner;
    HANDLE stop,console_close,worker_close;
    worker_base_input_watch *watch;
    volatile LONG closing;
} lifecycle;

static DWORD WINAPI close_console(void *context)
{
    session *owner=context;
    if(!session_thread_bind(owner))return ERROR_INVALID_STATE;
    (void)CntrlHandler(CTRL_CLOSE_EVENT);
    (void)session_thread_unbind(owner);
    return ERROR_SUCCESS;
}
static DWORD WINAPI close_worker(void *context)
{
    session *owner=context;
    if(!session_thread_bind(owner))return ERROR_INVALID_STATE;
    /* Original TerminateVDM selects DOS or whole-WOW cleanup itself. */
    TerminateVDM();
    (void)session_thread_unbind(owner);
    return ERROR_PROCESS_ABORTED;
}
static void console_instruction(void *context)
{
    if(InterlockedCompareExchange(&lifecycle.closing,1,0))return;
    worker_base_shutdown_close(close_console,context,
        OPENNT_FRONTEND_LIFECYCLE_TIMEOUT_MS,CONTROL_C_EXIT,NULL);
}
static void worker_instruction(void *context)
{
    if(InterlockedCompareExchange(&lifecycle.closing,1,0))return;
    worker_base_shutdown_close(close_worker,context,INFINITE,ERROR_CANCELLED,NULL);
}
static void service_lost(void *context)
{
    if(InterlockedCompareExchange(&lifecycle.closing,1,0))return;
    /* Original DOS/WOW termination owns guest and host cleanup. RPC failure
     * is already latched, so ExitVDM cannot wait on the dead service. */
    VdmExitCode=ERROR_INVALID_HANDLE;
    worker_base_shutdown_close(close_worker,context,INFINITE,ERROR_INVALID_HANDLE,NULL);
}
void ntvdm_worker_lifecycle_end(void)
{
    worker_base_unwatch_service();
    if(lifecycle.stop)SetEvent(lifecycle.stop);
    worker_base_input_watch_destroy(lifecycle.watch);
    if(lifecycle.stop)CloseHandle(lifecycle.stop);
    if(lifecycle.console_close)CloseHandle(lifecycle.console_close);
    if(lifecycle.worker_close)CloseHandle(lifecycle.worker_close);
    ZeroMemory(&lifecycle,sizeof(lifecycle));
}
DWORD ntvdm_worker_lifecycle_begin(session *owner,BOOL character_io)
{
    DWORD error;
    if(!owner || lifecycle.owner)return ERROR_INVALID_STATE;
    lifecycle.owner=owner;
    lifecycle.stop=CreateEventW(NULL,TRUE,FALSE,NULL);
    if(!lifecycle.stop){error=GetLastError();goto fail;}
    error=worker_base_shutdown_event(&lifecycle.worker_close);
    if(error)goto fail;
    /* WOW has no Console-close membership. No frontend is created or queried. */
    if(character_io) {
        error=worker_base_console_close_event(&lifecycle.console_close);
        if(error)goto fail;
    }
    lifecycle.watch=worker_base_input_watch_create(lifecycle.console_close,
        lifecycle.worker_close,lifecycle.stop,NULL,console_instruction,
        worker_instruction,NULL,owner);
    if(!lifecycle.watch){error=GetLastError();goto fail;}
    error=worker_base_watch_service(service_lost,owner);
    if(error)goto fail;
    return ERROR_SUCCESS;
fail:
    ntvdm_worker_lifecycle_end();return error;
}
