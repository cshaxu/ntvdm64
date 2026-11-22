#include <windows.h>
#include <winternl.h>
#include "wow_edit_conversion_bindings.h"
#include "wow_edit_entry_bindings.h"
#include "wow_edit_native_bindings.h"
#include "wow_user_private_access.h"

static const wow_edit_conversion_provider *conversion_provider;
BOOL WINAPI wow_edit_conversion_bind(const wow_edit_conversion_provider *provider)
{
    const wow_edit_conversion_provider *previous;
    if (!provider || !provider->ansi || !provider->unicode) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    previous = InterlockedCompareExchangePointer((PVOID volatile *)&conversion_provider,
        (PVOID)provider, NULL);
    if (previous && previous != provider) {
        SetLastError(ERROR_ALREADY_INITIALIZED); return FALSE;
    }
    return TRUE;
}
C_ASSERT(sizeof(void *) == sizeof(DWORD));

LONG WINAPI wow_edit_CsSendMessage(HWND window, UINT message, WPARAM wp,
    LPARAM lp, DWORD procedure, DWORD function, BOOL ansi)
{
    DWORD entry_error = GetLastError();
    DWORD options, token;
    WNDPROC destination;
    /* Original EditWndProcWorker reaches only these opposite-encoding
     * native entries. Refuse other server operations and arbitrary targets;
     * do not turn this private boundary into a second SendMessage engine. */
    if (!conversion_provider) RaiseFailFastException(NULL, NULL, 0);
    if (function != conversion_provider->call_window_function ||
            procedure != (DWORD)(ULONG_PTR)(ansi ? conversion_provider->unicode : conversion_provider->ansi)) {
        SetLastError(ERROR_NOT_SUPPORTED);
        return 0;
    }
    destination = ansi ? wow_edit_window_procW : wow_edit_window_procA;
    options = ansi ? conversion_provider->ansi_to_unicode_options : conversion_provider->unicode_to_ansi_options;
    token = wow_edit_native_NtUserGetCPD(window, options,
        (DWORD)(ULONG_PTR)destination);
    if (!token) return 0;
    /* USER owns this opaque token in the window's class CPD list, not in
     * the individual window (original kernel/getset.c GetCPD). It can be
     * retained as an old procedure across window subclass changes. Never
     * free or invalidate it merely because this HWND is destroyed.
     * The existing bridge
     * releases/reenters the native-call boundary; the destination wrapper
     * establishes the original EDIT task/borrow scope before its body runs.
     * The converted arguments remain call-local, with no retained buffer. */
    SetLastError(entry_error);
    return ansi ? wow_user_call_window_procA((WNDPROC)(ULONG_PTR)token,
        window, message, wp, lp) :
        wow_user_call_window_procW((WNDPROC)(ULONG_PTR)token,
            window, message, wp, lp);
}
