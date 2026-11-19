#include "compat/include/thread_start_compat.h"
#include "compat/include/thread_lifecycle.h"
#undef CreateThread
#undef ExitThread

static const opennt_thread_lifecycle *lifecycle;

BOOL opennt_thread_install_lifecycle(const opennt_thread_lifecycle *value)
{
    const opennt_thread_lifecycle *previous;
    if (!value || !value->capture || !value->finish) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    previous = InterlockedCompareExchangePointer((PVOID volatile *)&lifecycle,
        (PVOID)value, NULL);
    if (previous && previous != value) {
        SetLastError(ERROR_ALREADY_INITIALIZED);
        return FALSE;
    }
    return TRUE;
}

VOID WINAPI opennt_prepare_thread_exit(VOID)
{
    if (!lifecycle) RaiseFailFastException(NULL, NULL, 0);
    lifecycle->finish();
}

VOID WINAPI opennt_exit_thread(DWORD exit_code)
{
    opennt_prepare_thread_exit();
    ExitThread(exit_code);
}

static HANDLE create_bound(LPSECURITY_ATTRIBUTES attributes, SIZE_T stack_bytes,
    win32_thread_entry entry, LPVOID parameter, DWORD flags, LPDWORD thread_id,
    const char *source_name)
{
    win32_thread_hooks hooks;
    unsigned count = (entry.cdecl_entry != NULL) + (entry.winapi_entry != NULL) +
        (entry.void_entry != NULL) + (entry.void_parameter_entry != NULL);
    if (count != 1u || (entry.void_entry && parameter)) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return NULL;
    }
    if (!lifecycle) {
        SetLastError(ERROR_INVALID_STATE);
        return NULL;
    }
    lifecycle->capture(&hooks);
    hooks.source_name = source_name;
    return win32_thread_create(attributes, stack_bytes, entry, parameter, flags,
        thread_id, &hooks);
}

HANDLE opennt_create_cdecl_thread_named(LPSECURITY_ATTRIBUTES attributes,
    SIZE_T stack_bytes, OPENNT_CDECL_THREAD_START_ROUTINE start_routine, LPVOID parameter,
    DWORD flags, LPDWORD thread_id, const char *source_name)
{
    win32_thread_entry entry = {0};
    entry.cdecl_entry = start_routine;
    return create_bound(attributes, stack_bytes, entry, parameter, flags,
        thread_id, source_name);
}

HANDLE opennt_create_winapi_thread_named(LPSECURITY_ATTRIBUTES attributes,
    SIZE_T stack_bytes, LPTHREAD_START_ROUTINE start_routine, LPVOID parameter,
    DWORD flags, LPDWORD thread_id, const char *source_name)
{
    win32_thread_entry entry = {0};
    entry.winapi_entry = start_routine;
    return create_bound(attributes, stack_bytes, entry, parameter, flags,
        thread_id, source_name);
}

HANDLE opennt_create_void_cdecl_thread_named(LPSECURITY_ATTRIBUTES attributes,
    SIZE_T stack_bytes, OPENNT_VOID_CDECL_THREAD_START_ROUTINE start_routine, LPVOID parameter,
    DWORD flags, LPDWORD thread_id, const char *source_name)
{
    win32_thread_entry entry = {0};
    entry.void_entry = start_routine;
    return create_bound(attributes, stack_bytes, entry, parameter, flags,
        thread_id, source_name);
}

HANDLE opennt_create_void_cdecl_parameter_thread_named(LPSECURITY_ATTRIBUTES attributes,
    SIZE_T stack_bytes, OPENNT_VOID_CDECL_PARAMETER_THREAD_START_ROUTINE start_routine, LPVOID parameter,
    DWORD flags, LPDWORD thread_id, const char *source_name)
{
    win32_thread_entry entry = {0};
    entry.void_parameter_entry = start_routine;
    return create_bound(attributes, stack_bytes, entry, parameter, flags,
        thread_id, source_name);
}

HANDLE opennt_create_void_cdecl_parameter_thread(LPSECURITY_ATTRIBUTES attributes,
    SIZE_T stack_bytes, OPENNT_VOID_CDECL_PARAMETER_THREAD_START_ROUTINE start_routine, LPVOID parameter,
    DWORD flags, LPDWORD thread_id)
{
    return opennt_create_void_cdecl_parameter_thread_named(attributes, stack_bytes,
        start_routine, parameter, flags, thread_id, NULL);
}
