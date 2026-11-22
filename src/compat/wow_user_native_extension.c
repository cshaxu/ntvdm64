#include <windows.h>
#include <winternl.h>
/* ADAPTER-WOW-079: original bodies retain all callback/thunk algorithms.
 * This file pairs only the existing native-stack execution boundaries. */
#include "wow_user_native_extension.h"
#include "wow_native_extension_provider.h"
#include "compat/include/wow_user_runtime.h"
#include "wow_user_native_call.h"

static const wow_native_extension_provider *provider;
BOOL WINAPI wow_native_extension_bind(const wow_native_extension_provider *value)
{
    const wow_native_extension_provider *previous;
    if (!value || !value->kernel_resource || !value->callback ||
            !value->call_proc || !value->call_mm || !value->yield || !value->directed_yield) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    previous = InterlockedCompareExchangePointer((PVOID volatile *)&provider,
        (PVOID)value, NULL);
    if (previous && previous != value) {
        SetLastError(ERROR_ALREADY_INITIALIZED); return FALSE;
    }
    return TRUE;
}
static const wow_native_extension_provider *current_provider(void)
{
    if (!provider) RaiseFailFastException(NULL, NULL, 0);
    return provider;
}

BOOL WINAPI CallBack16(INT id, union _PARM16 *parameters, DWORD procedure, DWORD *output)
{
    wow_user_runtime_thread *binding = wow_user_runtime_current();
    wow_task_callback_scope scope;
    BOOL result;
    DWORD error;
    BOOL cleanup = binding && binding->thread &&
        (binding->thread->TIF_flags & WOW_TASK_TIF_INCLEANUP);
    if (!(cleanup && current_provider()->kernel_resource(id) ?
            wow_task_resource_cleanup_enter(&scope) : wow_task_callback_enter(&scope)))
        return FALSE;
    __try {
        result = current_provider()->callback(id, parameters, procedure, output);
    }
    __finally {
        error = GetLastError();
        (void)wow_task_callback_leave(&scope);
        SetLastError(error);
    }
    return result;
}

/* Inline entry never gratuitously releases inherited guest execution before
 * the original raw arguments are copied. Actual callback/yield entry may
 * acquire execution; this is exactly when the original allows memory moves. */
#define EXTENSION_CALL(type, convention, name, declaration, original, arguments, failed) \
type convention name declaration \
{ \
    wow_user_native_call call; \
    type result; \
    DWORD error = GetLastError(); \
    if (!wow_user_native_call_begin_inline(&call)) return failed; \
    __try { SetLastError(error); result = original arguments; } \
    __finally { \
        error = GetLastError(); \
        if (!wow_user_native_call_end(&call)) \
            wow_user_native_call_failed("native-extension-return", GetLastError()); \
        SetLastError(error); \
    } \
    return result; \
}
EXTENSION_CALL(DWORD, WINAPI, WK32ICallProc32, (struct _VDMFRAME *frame),
    current_provider()->call_proc, (frame), 0)
EXTENSION_CALL(ULONG, WINAPI, WMM32CallProc32, (struct _VDMFRAME *frame),
    current_provider()->call_mm, (frame), 0)
EXTENSION_CALL(HMODULE, WINAPI, wow_extension_load_library_a,
    (LPCSTR name, HANDLE file, DWORD flags), LoadLibraryExA, (name, file, flags), NULL)
EXTENSION_CALL(BOOL, WINAPI, wow_extension_free_library, (HMODULE module),
    FreeLibrary, (module), FALSE)
EXTENSION_CALL(FARPROC, WINAPI, wow_extension_get_proc_address,
    (HMODULE module, LPCSTR name), GetProcAddress, (module, name), NULL)
#undef EXTENSION_CALL

VOID WINAPI WOWYield16(VOID)
{
    wow_task_callback_scope scope;
    DWORD error;
    if (!wow_task_callback_enter(&scope)) return;
    __try { current_provider()->yield(); }
    __finally {
        error = GetLastError();
        (void)wow_task_callback_leave(&scope);
        SetLastError(error);
    }
}

VOID WINAPI WOWDirectedYield16(WORD task)
{
    wow_task_callback_scope scope;
    DWORD error;
    if (!wow_task_callback_enter(&scope)) return;
    __try { current_provider()->directed_yield(task); }
    __finally {
        error = GetLastError();
        (void)wow_task_callback_leave(&scope);
        SetLastError(error);
    }
}
