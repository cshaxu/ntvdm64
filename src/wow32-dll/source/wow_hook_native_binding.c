#include "wow_hook_native_binding.h"

/* ADAPTER-WOW-090: original WOW's fixed hook stubs remain in this provider
 * until worker teardown. Native USER needs no DLL-registration dependency
 * for those callbacks on this process's threads. All other inputs retain
 * native validation/registration; never retry a failed call with NULL. */
HHOOK WINAPI wow_native_SetWindowsHookExA(int kind, HOOKPROC callback,
    HINSTANCE module, DWORD thread)
{
    HMODULE callback_module = NULL, binding_module = NULL;
    HANDLE target = NULL;
    HINSTANCE native_module = module;
    HHOOK result;
    DWORD saved = GetLastError(), error;
    if (module && callback && thread &&
            GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                (LPCSTR)callback, &callback_module) && callback_module == module &&
            GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                (LPCSTR)wow_native_SetWindowsHookExA, &binding_module) &&
            binding_module == module) {
        if (thread == GetCurrentThreadId()) native_module = NULL;
        else {
            target = OpenThread(THREAD_QUERY_LIMITED_INFORMATION | SYNCHRONIZE,
                FALSE, thread);
            if (target && GetProcessIdOfThread(target) == GetCurrentProcessId() &&
                    WaitForSingleObject(target, 0) == WAIT_TIMEOUT)
                native_module = NULL;
        }
    }
    SetLastError(saved);
    result = SetWindowsHookExA(kind, callback, native_module, thread);
    error = GetLastError();
    if (target) CloseHandle(target);
    SetLastError(error);
    return result;
}
