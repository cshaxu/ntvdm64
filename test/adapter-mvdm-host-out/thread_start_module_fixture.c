#include "compat/include/thread_start_compat.h"
struct session;
struct session *__cdecl session_thread_current(void);

/* The loader still enters original redirector initialization. The DLL uses
 * the outer fixture's exported compatibility entry and bound host context. */
static int attached;
BOOLEAN VrDllInitialize(PVOID module, ULONG reason, PCONTEXT context)
{
    (void)module;
    (void)context;
    if (reason == DLL_PROCESS_ATTACH) attached = 1;
    return TRUE;
}

static DWORD __cdecl check_owner(LPVOID expected)
{
    return session_thread_current() == expected ? 0 : 1;
}

int thread_module_run(void *expected)
{
    DWORD result = 1;
    HANDLE thread;
    if (!attached || session_thread_current() != expected) return 1;
    thread = opennt_create_cdecl_thread_named(NULL, 0, check_owner, expected,
        0, NULL, "module-owner");
    if (!thread) return 2;
    if (WaitForSingleObject(thread, 5000) != WAIT_OBJECT_0 ||
            !GetExitCodeThread(thread, &result)) result = 3;
    CloseHandle(thread);
    return (int)result;
}
