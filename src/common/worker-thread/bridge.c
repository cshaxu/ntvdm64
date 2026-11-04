#include "compat/include/thread_start_compat.h"
#include "common/worker-thread/session.h"
#include "lib-win32/thread/thread.h"
#undef CreateThread
#undef ExitThread

VOID WINAPI opennt_prepare_thread_exit(VOID)
{
    struct session *owner = session_thread_current();
    if (owner != NULL && !session_thread_finish(owner)) {
        EXCEPTION_RECORD record = {0};
        record.ExceptionCode = 0xc0000420;
        record.ExceptionFlags = EXCEPTION_NONCONTINUABLE;
        record.NumberParameters = 1;
        record.ExceptionInformation[0] = GetLastError();
        RaiseFailFastException(&record, NULL, 0);
    }
}
VOID WINAPI opennt_exit_thread(DWORD exit_code)
{
    opennt_prepare_thread_exit();
    ExitThread(exit_code);
}
static int enter_owner(void *owner, const char *name)
{
    return session_thread_bind_owned_source((struct session *)owner,
        SESSION_THREAD_BINDING_ORIGINAL_WORKER, name);
}
static void finish_owner(void) { opennt_prepare_thread_exit(); }
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
    hooks.owner = session_thread_current();
    hooks.source_name = source_name;
    hooks.enter = enter_owner;
    hooks.finish = finish_owner;
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

HANDLE opennt_create_cdecl_thread(LPSECURITY_ATTRIBUTES attributes,
    SIZE_T stack_bytes, OPENNT_CDECL_THREAD_START_ROUTINE start_routine, LPVOID parameter,
    DWORD flags, LPDWORD thread_id)
{
    return opennt_create_cdecl_thread_named(attributes, stack_bytes,
        start_routine, parameter, flags, thread_id, NULL);
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

HANDLE opennt_create_void_cdecl_thread(LPSECURITY_ATTRIBUTES attributes,
    SIZE_T stack_bytes, OPENNT_VOID_CDECL_THREAD_START_ROUTINE start_routine, LPVOID parameter,
    DWORD flags, LPDWORD thread_id)
{
    return opennt_create_void_cdecl_thread_named(attributes, stack_bytes,
        start_routine, parameter, flags, thread_id, NULL);
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
