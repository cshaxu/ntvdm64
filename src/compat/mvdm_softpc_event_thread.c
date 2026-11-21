#include "include/mvdm_softpc_event_thread.h"
#include "include/wow_startup_report.h"

#include <windows.h>
#include <winternl.h>
/* Native NTDLL ABI; OpenNT public/sdk/inc/ntpsapi.h:792-797. */
NTSYSAPI NTSTATUS NTAPI NtAlertThread(HANDLE ThreadHandle);

mvdm_softpc_terminal_events mvdm_softpc_input_terminal_events;
static mvdm_softpc_control control;
static void *control_context;
static SRWLOCK console_lock = SRWLOCK_INIT;
static BOOL console_issued;
static DWORD console_status;

void mvdm_softpc_bind_control(mvdm_softpc_control callback, void *context)
{
    control = callback;
    control_context = callback ? context : NULL;
    console_issued = FALSE;
    console_status = ERROR_SUCCESS;
}
volatile LONG mvdm_softpc_exit_owner;
BOOL mvdm_softpc_exit_original_ready;

static DWORD exchange_worker_control(mvdm_softpc_exit_operation operation,
    ULONG request, USHORT task, DWORD *reply)
{
    if (!control) return ERROR_INVALID_STATE;
    if (operation == MVDM_EXIT_CONSOLE_GROUP) {
        AcquireSRWLockExclusive(&console_lock);
        if (!console_issued) {
            console_status = control(control_context, operation, request, task);
            console_issued = TRUE;
        }
        *reply = console_status;
        ReleaseSRWLockExclusive(&console_lock);
    } else {
        *reply = control(control_context, operation, request, task);
    }
    return ERROR_SUCCESS;
}

DWORD mvdm_softpc_exchange_exit(mvdm_softpc_exit_operation operation, DWORD *reply)
{
    return exchange_worker_control(operation, 0, 0, reply);
}

DWORD WINAPI NtvdmReportWowStartup(ULONG request, USHORT task, BOOL bind)
{
    DWORD reply;
    DWORD error = exchange_worker_control(bind ? MVDM_WOW_BIND : MVDM_WOW_STARTED,
        request, task, &reply);
    return error ? error : reply;
}

DWORD mvdm_softpc_request_console_exit(void)
{
    DWORD reply, error;
    if (!mvdm_softpc_input_terminal_events.console_close || !mvdm_softpc_exit_original_ready)
        return ERROR_INVALID_STATE;
    error = mvdm_softpc_exchange_exit(MVDM_EXIT_CONSOLE_GROUP, &reply);
    return error ? error : reply;
}

mvdm_softpc_exit_permission mvdm_softpc_wait_for_exit(void)
{
    const mvdm_softpc_terminal_events *events = &mvdm_softpc_input_terminal_events;
    HANDLE waits[3] = {events->server, events->worker_close, events->console_close};
    DWORD count = events->console_close ? 3 : 2;
    LONG thread = (LONG)GetCurrentThreadId(), previous;
    DWORD reply;

    if (!events->worker_close) return MVDM_EXIT_UNMANAGED;
    previous = InterlockedCompareExchange(&mvdm_softpc_exit_owner, thread, 0);
    if (previous == thread) return MVDM_EXIT_WORKER_GRANTED;
    if (!previous) {
        DWORD wake;
        (void)mvdm_softpc_exchange_exit(MVDM_EXIT_PARTICIPANT, &reply);
        wake = WaitForMultipleObjects(count, waits, FALSE, INFINITE);
        if (wake == WAIT_OBJECT_0) return MVDM_EXIT_SERVICE_LOST;
        if (wake == WAIT_OBJECT_0 + 1) return MVDM_EXIT_WORKER_GRANTED;
        if (wake == WAIT_OBJECT_0 + 2) return MVDM_EXIT_CONSOLE_GRANTED;
    }
    /* Other threads and failed waits do not have permission to destroy
     * original resources. The authorized cleanup owner ends the process. */
    WaitForSingleObject(GetCurrentProcess(), INFINITE);
    ExitProcess(ERROR_INVALID_STATE);
}

DWORD WINAPI NtvdmAuthorizeExit(DWORD exit_code)
{
    DWORD reply;
    mvdm_softpc_exit_permission permission = mvdm_softpc_wait_for_exit();
    /* This entry is the whole-WOW boundary and has no Console membership. */
    if (permission == MVDM_EXIT_CONSOLE_GRANTED) ExitProcess(ERROR_INVALID_STATE);
    if (permission != MVDM_EXIT_UNMANAGED) {
        DWORD error = mvdm_softpc_exchange_exit(permission == MVDM_EXIT_SERVICE_LOST ?
            MVDM_EXIT_STATUS_ON_LOSS : MVDM_EXIT_STATUS, &reply);
        if (error) ExitProcess(error);
        if (reply == ERROR_INVALID_HANDLE) return reply;
    }
    return exit_code;
}

BOOL ntvdm_worker_lifecycle_input_terminal(void)
{
    const mvdm_softpc_terminal_events *events = &mvdm_softpc_input_terminal_events;
    DWORD saved = GetLastError();
    BOOL terminal =
        ((events->server && WaitForSingleObject(events->server,0) == WAIT_OBJECT_0) ||
         (events->worker_close && WaitForSingleObject(events->worker_close,0) == WAIT_OBJECT_0) ||
         (events->console_close && WaitForSingleObject(events->console_close,0) == WAIT_OBJECT_0));
    SetLastError(saved);
    return terminal;
}

int mvdm_softpc_event_thread_alert_and_join(HANDLE event_thread, BOOL started)
{
    HANDLE retained_thread;
    NTSTATUS status;
    DWORD wait_result;

    if (event_thread == NULL || event_thread == INVALID_HANDLE_VALUE)
        return 0;

    /* Before the original first ResumeThread, the thread has no session
     * binding and cannot touch teardown resources. Keep it suspended until
     * this worker process exits; never start input while closing the host. */
    if (!started) return 1;

    /* The original worker closes ThreadInfo.EventMgr.Handle immediately
     * before returning.  Retain a duplicate before alerting it, so the
     * session can wait without racing that source-owned CloseHandle. */
    if (!DuplicateHandle(GetCurrentProcess(), event_thread,
            GetCurrentProcess(), &retained_thread, SYNCHRONIZE, FALSE, 0))
        return 0;

    status = NtAlertThread(event_thread);
    if (status < 0) {
        CloseHandle(retained_thread);
        return 0;
    }

    wait_result = WaitForSingleObject(retained_thread, INFINITE);
    CloseHandle(retained_thread);
    return wait_result == WAIT_OBJECT_0;
}
