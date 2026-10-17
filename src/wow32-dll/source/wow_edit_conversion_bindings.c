#include "wow_edit_conversion_bindings.h"
#include "wow_edit_entry_bindings.h"
#include "wow_edit_native_bindings.h"
#include "wow_user_private_access.h"
#include "opennt-host/windows/core/ntuser/inc/user.h"

LONG WINAPI EditWndProcA(HWND, UINT, WPARAM, LONG);
LONG WINAPI EditWndProcW(HWND, UINT, WPARAM, LONG);
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
    if (function != FNID_CALLWINDOWPROC ||
            procedure != (DWORD)(ULONG_PTR)(ansi ? EditWndProcW : EditWndProcA)) {
        SetLastError(ERROR_NOT_SUPPORTED);
        return 0;
    }
    destination = ansi ? wow_edit_window_procW : wow_edit_window_procA;
    options = CPD_WND | (ansi ? CPD_ANSI_TO_UNICODE : CPD_UNICODE_TO_ANSI);
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
