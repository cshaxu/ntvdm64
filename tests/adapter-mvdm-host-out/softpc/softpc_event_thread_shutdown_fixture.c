#include <windows.h>

#include "ntvdm-exe/softpc/include/mvdm_softpc_event_thread.h"

static DWORD WINAPI alertable_event_worker(LPVOID unused)
{
    LARGE_INTEGER timeout;
    typedef LONG (NTAPI *delay_fn)(BOOLEAN, LARGE_INTEGER *);
    delay_fn delay = (delay_fn)GetProcAddress(GetModuleHandleA("ntdll.dll"),
        "NtDelayExecution");
    if (!delay) return 2;
    timeout.QuadPart = -50000000;
    SetEvent((HANDLE)unused);
    return delay(TRUE, &timeout) == 0x101 ? 0u : 1u;
}

int main(void)
{
    HANDLE worker;
    HANDLE ready;
    DWORD exit_code;

    ready = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (ready == NULL) return 4;
    worker = CreateThread(NULL, 0u, alertable_event_worker, ready, 0u, NULL);
    if (worker == NULL) return 1;
    if (WaitForSingleObject(ready, 1000) != WAIT_OBJECT_0) return 5;
    if (!mvdm_softpc_event_thread_alert_and_join(worker, TRUE)) {
        CloseHandle(worker);
        return 2;
    }
    if (!GetExitCodeThread(worker, &exit_code) || exit_code != 0u) {
        CloseHandle(worker);
        return 3;
    }
    CloseHandle(worker);
    CloseHandle(ready);
    return 0;
}
