#include "ntvdm-exe/win32/nt_thread_alert_compat.h"

#include <windows.h>

typedef struct alert_fixture_state {
    HANDLE ready;
} alert_fixture_state;

static DWORD WINAPI alertable_waiter(LPVOID parameter)
{
    alert_fixture_state *state = (alert_fixture_state *)parameter;
    typedef NTSTATUS (NTAPI *delay_fn)(BOOLEAN, PLARGE_INTEGER);
    delay_fn delay = (delay_fn)GetProcAddress(GetModuleHandleW(L"ntdll.dll"),
        "NtDelayExecution");
    LARGE_INTEGER timeout;
    timeout.QuadPart = -50000000;
    if (delay == NULL || !SetEvent(state->ready)) return 10u;
    return delay(TRUE, &timeout) == (NTSTATUS)0x101L ? 0u : 11u;
}

int main(void)
{
    alert_fixture_state state;
    DWORD exit_code = 0u;
    HANDLE worker;

    state.ready = CreateEventA(NULL, TRUE, FALSE, NULL);
    if (state.ready == NULL) return 1;
    if (NtAlertThread(NULL) >= 0) return 2;
    worker = CreateThread(NULL, 0u, alertable_waiter, &state, 0u, NULL);
    if (worker == NULL) return 3;
    if (WaitForSingleObject(state.ready, 5000u) != WAIT_OBJECT_0) return 4;
    if (NtAlertThread(worker) < 0) return 5;
    if (WaitForSingleObject(worker, 5000u) != WAIT_OBJECT_0) return 6;
    if (!GetExitCodeThread(worker, &exit_code) || exit_code != 0u) return 7;
    CloseHandle(worker);
    CloseHandle(state.ready);
    return 0;
}
