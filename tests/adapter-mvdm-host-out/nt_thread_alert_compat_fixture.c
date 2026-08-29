#include "nt_thread_alert_compat.h"

#include <windows.h>

typedef struct alert_fixture_state {
    HANDLE ready;
} alert_fixture_state;

static DWORD WINAPI alertable_waiter(LPVOID parameter)
{
    alert_fixture_state *state = (alert_fixture_state *)parameter;
    if (!SetEvent(state->ready)) return 10u;
    return SleepEx(5000u, TRUE) == WAIT_IO_COMPLETION ? 0u : 11u;
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
