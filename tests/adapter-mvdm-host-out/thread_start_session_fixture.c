#include <windows.h>

#include "adapter-mvdm-host-out/win32/include/thread_start_compat.h"
#include "session/session.h"

static DWORD __cdecl bound_worker(LPVOID parameter)
{
    session *expected = (session *)parameter;
    return session_thread_current() == expected ? 0u : 1u;
}

int main(void)
{
    session instance;
    HANDLE thread;
    DWORD exit_code = 0u;

    session_initialize(&instance, 1u);
    if (!session_activate(&instance) || !session_thread_bind(&instance)) return 1;
    thread = opennt_create_cdecl_thread(NULL, 0u, bound_worker, &instance,
        0u, NULL);
    if (thread == NULL) return 2;
    if (WaitForSingleObject(thread, 5000u) != WAIT_OBJECT_0 ||
        !GetExitCodeThread(thread, &exit_code) || exit_code != 0u) return 3;
    CloseHandle(thread);
    if (!session_thread_unbind(&instance) || !session_dispose(&instance)) return 4;
    return 0;
}
