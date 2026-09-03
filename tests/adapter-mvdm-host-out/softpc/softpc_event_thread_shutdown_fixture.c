#include <windows.h>

#include "adapter-mvdm-host-out/softpc/include/mvdm_softpc_event_thread.h"

static DWORD WINAPI alertable_event_worker(LPVOID unused)
{
    (void)unused;
    return SleepEx(INFINITE, TRUE) == WAIT_IO_COMPLETION ? 0u : 1u;
}

int main(void)
{
    HANDLE worker;
    DWORD exit_code;

    worker = CreateThread(NULL, 0u, alertable_event_worker, NULL, 0u, NULL);
    if (worker == NULL) return 1;
    if (!mvdm_softpc_event_thread_alert_and_join(worker)) {
        CloseHandle(worker);
        return 2;
    }
    if (!GetExitCodeThread(worker, &exit_code) || exit_code != 0u) {
        CloseHandle(worker);
        return 3;
    }
    CloseHandle(worker);
    return 0;
}
