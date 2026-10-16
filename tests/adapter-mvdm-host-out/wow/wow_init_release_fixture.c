/* Observe the actual carrier's HeapFree calls without a product test seam. */
#include "wow_user_task_lifecycle.h"
int wow_init_hook_fail;
unsigned wow_init_hooks_live, wow_init_hook_failures;
static HHOOK WINAPI fixture_set_hook(int id, HOOKPROC proc, HINSTANCE module, DWORD tid)
{
    HHOOK hook;
    if (id == wow_init_hook_fail) {
        ++wow_init_hook_failures;
        SetLastError(ERROR_ACCESS_DENIED);
        return NULL;
    }
    hook = SetWindowsHookExA(id, proc, module, tid);
    if (hook) ++wow_init_hooks_live;
    return hook;
}
static BOOL WINAPI fixture_unhook(HHOOK hook)
{
    BOOL result = UnhookWindowsHookEx(hook);
    if (result) --wow_init_hooks_live;
    return result;
}
BOOL WINAPI wow_init_fixture_free(HANDLE, DWORD, PVOID);
#define SetWindowsHookExA fixture_set_hook
#define UnhookWindowsHookEx fixture_unhook
#define HeapFree wow_init_fixture_free
#include "../../../src/wow32-dll/source/wow_user_task_lifecycle.c"
