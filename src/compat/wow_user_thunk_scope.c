#include <windows.h>
#include <winternl.h>
#include "wow_user_borrow_scope.h"
#include "wow_user_thunk_scope.h"
#include "wow_task_order_bindings.h"
#include "wow_user_native_call.h"

#include <stdio.h>

static const wow_thunk_provider *provider;
BOOL WINAPI wow_thunk_bind(const wow_thunk_provider *value)
{
    const wow_thunk_provider *previous;
    if (!value || !value->exception || !value->capture || !value->restore || !value->frame) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    previous = InterlockedCompareExchangePointer((PVOID volatile *)&provider, (PVOID)value, NULL);
    if (previous && previous != value) { SetLastError(ERROR_ALREADY_INITIALIZED); return FALSE; }
    return TRUE;
}
static const wow_thunk_provider *current_provider(void)
{
    if (!provider) RaiseFailFastException(NULL, NULL, 0);
    return provider;
}
/* Existing named-image service ABI; not a private provider layout. */
ULONG __cdecl WOWSysErrorBox(LPSTR, LPSTR, USHORT, USHORT, USHORT);

/* ADAPTER-WOW-100: the native modal service may synchronously activate a
 * different WOW task even during first-pass exception handling. Existing
 * native-call scopes own readiness/handoff; original error policy stays. */
ULONG __cdecl wow_user_sys_error_box(LPSTR title, LPSTR message,
    USHORT first, USHORT second, USHORT third)
{
    wow_user_native_call call;
    ULONG response;
    DWORD error = GetLastError();
    if (!wow_user_native_call_begin(&call, NULL))
        wow_user_native_call_failed("hard-error-native-entry", GetLastError());
    __try {
        SetLastError(error);
        response = WOWSysErrorBox(title, message, first, second, third);
    } __finally {
        error = GetLastError();
        if (!wow_user_native_call_end(&call))
            wow_user_native_call_failed("hard-error-native-return", GetLastError());
        SetLastError(error);
    }
    return response;
}

INT WINAPI wow_user_dispatch_exception(DWORD code, PEXCEPTION_POINTERS info,
    struct _TD *task, DWORD entry, BOOL established)
{
    return current_provider()->exception(code, info, task, entry, established);
}

/* Both sides of a traced thunk must use the same bounded admission state.
 * The prior one-sided limit stopped entries after 128 but kept writing every
 * return, so a crash trace's tail could not be paired to a call.  This
 * host-only observer remains disabled unless its path variable is set. */
static LONG trace_count;
#define WOW_TRACE_STACK_MAXIMUM 64
static __declspec(thread) unsigned char trace_admitted[WOW_TRACE_STACK_MAXIMUM];
static __declspec(thread) unsigned char trace_depth;

static void trace_thunk(struct _VDMFRAME *frame, wow_thunk_procedure thunk)
{
    char path[MAX_PATH];
    char line[192];
    HANDLE file;
    DWORD bytes;
    DWORD written;
    HMODULE module;
    ULONG_PTR rva;

    unsigned char slot;
    wow_thunk_frame view;

    if (trace_depth == WOW_TRACE_STACK_MAXIMUM)
        return;
    slot = trace_depth++;
    trace_admitted[slot] = 0;
    if (!frame || InterlockedIncrement(&trace_count) > 2048 ||
            !GetEnvironmentVariableA("MVDM_WOW_DISPATCH_TRACE_PATH", path,
                sizeof(path)))
        return;
    trace_admitted[slot] = 1;
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ |
        FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE)
        return;
    rva = GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
              GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
              (LPCSTR)trace_thunk, &module) && (ULONG_PTR)thunk >= (ULONG_PTR)module ?
        (ULONG_PTR)thunk - (ULONG_PTR)module : 0;
    current_provider()->frame(frame, &view);
    bytes = (DWORD)sprintf_s(line, sizeof(line),
        "%lu W32Dispatch call=%04X thunk=%08lX rva=%08lX task=%04X return=%08lX ax=%04X args=%u tid=%lu depth=%u\r\n",
        (unsigned long)GetCurrentProcessId(), view.call,
        (unsigned long)(ULONG_PTR)thunk, (unsigned long)rva, view.task,
        (unsigned long)view.return_ip, view.ax, (unsigned)view.args,
        (unsigned long)GetCurrentThreadId(), (unsigned)trace_depth);
    if (bytes)
        (void)WriteFile(file, line, bytes, &written, NULL);
    CloseHandle(file);
}

static void trace_return(struct _VDMFRAME *frame, ULONG result)
{
    char path[MAX_PATH];
    char line[112];
    HANDLE file;
    DWORD bytes;
    DWORD written;

    unsigned char admitted;
    wow_thunk_frame view;

    if (trace_depth == 0)
        return;
    admitted = trace_admitted[--trace_depth];
    if (!frame || !admitted ||
            !GetEnvironmentVariableA("MVDM_WOW_DISPATCH_TRACE_PATH",
            path, sizeof(path)))
        return;
    file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ |
        FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE)
        return;
    current_provider()->frame(frame, &view);
    bytes = (DWORD)sprintf_s(line, sizeof(line),
        "%lu W32Return call=%04X result=%08lX task=%04X tid=%lu depth=%u\r\n",
        (unsigned long)GetCurrentProcessId(), view.call,
        (unsigned long)result, view.task,
        (unsigned long)GetCurrentThreadId(), (unsigned)trace_depth + 1);
    if (bytes)
        (void)WriteFile(file, line, bytes, &written, NULL);
    CloseHandle(file);
}

/* ADAPTER-WOW-032: retain private USER aliases only for the original thunk
 * invocation. Do not catch/translate exceptions or alter frame/result state;
 * original W32Dispatch remains the exception-policy owner. */
ULONG WINAPI wow_user_invoke_thunk(wow_thunk_procedure thunk, struct _VDMFRAME *frame)
{
    wow_user_borrow_scope scope;
    ULONG result;
    DWORD trace_error = GetLastError();
    wow_thunk_mode mode;
    current_provider()->capture(&mode);
    trace_thunk(frame, thunk);
    SetLastError(trace_error);
    if (!wow_user_borrow_enter(&scope))
        wow_user_native_call_failed("enter-thunk-borrow", GetLastError());
    __try {
        result = (*thunk)(frame);
    } __finally {
        /* ADAPTER-WOW-095: nested original thunks cannot overwrite outer mode. */
        current_provider()->restore(&mode);
        wow_user_borrow_leave_required(&scope);
    }
    trace_error = GetLastError();
    trace_return(frame, result);
    SetLastError(trace_error);
    return result;
}
