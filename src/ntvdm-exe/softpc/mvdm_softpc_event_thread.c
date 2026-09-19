#include "mvdm_softpc_event_thread.h"

#include "nt_thread_alert_compat.h"

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
