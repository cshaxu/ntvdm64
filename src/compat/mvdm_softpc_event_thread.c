#include "include/mvdm_softpc_event_thread.h"

#include <windows.h>
#include <winternl.h>
/* Native NTDLL ABI; OpenNT public/sdk/inc/ntpsapi.h:792-797. */
NTSYSAPI NTSTATUS NTAPI NtAlertThread(HANDLE ThreadHandle);

mvdm_softpc_terminal_events mvdm_softpc_input_terminal_events;
mvdm_softpc_console_exit mvdm_softpc_console_exit_request;

DWORD mvdm_softpc_request_console_exit(void)
{
    const mvdm_softpc_console_exit *exit = &mvdm_softpc_console_exit_request;
    if (!exit->request || !exit->completion) return ERROR_INVALID_STATE;
    if (!SetEvent(exit->request) ||
        WaitForSingleObject(exit->completion, INFINITE) != WAIT_OBJECT_0)
        return GetLastError();
    return exit->status;
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
