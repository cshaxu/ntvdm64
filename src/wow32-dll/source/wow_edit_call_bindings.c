#include "wow_edit_call_bindings.h"
#include "wow_edit_native_bindings.h"
#include "wow_list_native_bindings.h"
#include "wow_user_task_lifecycle.h"
#include "wow_window_client_state_bindings.h"
#include "wow_user_client_view_layout.h"
#include "wow_control_extent_bindings.h"
#include "wow_user_borrow_scope.h"

BOOL WINAPI wow_edit_NtUserDefSetText(HWND window, const LARGE_STRING *text)
{
    DWORD error = GetLastError();
    wow_user_native_call call;
    PWND state = wow_window_client_validate_hwnd(window);
    BOOL result = FALSE;
    if (!state || !wow_user_borrow_client_state(window, state)) return FALSE;
    if (!wow_user_native_call_begin(&call, window)) return FALSE;
    __try {
        SetLastError(error);
        result = wow_edit_native_NtUserDefSetText(window, text);
    } __finally {
        error = GetLastError();
        /* The native store may already have succeeded. Never report an
         * ordinary failure then resume with stale retained window text. */
        if (!wow_user_native_call_end(&call))
            wow_user_native_call_failed("control-text-return", GetLastError());
        SetLastError(error);
    }
    return result;
}

VOID WINAPI wow_list_reset_double_click(void)
{
    /* Original _ResetDblClk is VOID and changes only this GUI queue's
     * double-click history. No application callout or copied WND update. */
    if (!wow_list_native_reset_double_click())
        wow_user_native_call_failed("list-reset-double-click", GetLastError());
}

DWORD WINAPI wow_edit_control_extent(WORD fnid)
{
    DWORD extent;
    /* Original EDIT passes the literal FNID_EDIT. Retain the registered
     * size rather than deriving it from the current window or PED layout. */
    if (!wow_control_registered_extent(fnid, &extent))
        wow_user_native_call_failed("edit-control-extent", ERROR_INVALID_STATE);
    return extent;
}

static VOID change_window_state(HWND window, UINT flags, BOOL set)
{
    /* dlgmgr.c temporarily changes this behavior-private bit around its
     * original DefWindowProcWorker color selection. It is not a modern
     * USER flag: retain it in the same original WND that clmsg.c reads.
     * No native mutation or observation refresh belongs to this operation. */
    if (flags == WFWIN40COMPAT) {
        PWND state = wow_window_client_validate_hwnd(window);
        if (!state)
            wow_user_native_call_failed("control-private-state", GetLastError());
        if (set) SetWF(state, WFWIN40COMPAT);
        else ClrWF(state, WFWIN40COMPAT);
        return;
    }
    /* Original client.c owns TestWF/no-op policy. The audited native leaves
     * mutate USER bits without SetWindowLong's application notifications.
     * Publish the result through the existing WND owner before EDIT resumes. */
    if (!wow_edit_native_ready())
        wow_user_native_call_failed("control-state-ready", GetLastError());
    if (set) wow_edit_native_NtUserSetWindowState(window, flags);
    else wow_edit_native_NtUserClearWindowState(window, flags);
    if (!wow_window_client_refresh(window))
        wow_user_native_call_failed("control-state-refresh", GetLastError());
}

VOID WINAPI wow_edit_set_window_state(HWND window, UINT flags)
{
    change_window_state(window, flags, TRUE);
}

VOID WINAPI wow_edit_clear_window_state(HWND window, UINT flags)
{
    change_window_state(window, flags, FALSE);
}

VOID WINAPI wow_edit_alter_window_style(HWND window, DWORD mask, DWORD flags)
{
    /* Original lb1.c owns multicolumn/compatibility policy. This service
     * mutates real style bits without the public setter's notifications. */
    if (!wow_edit_native_ready())
        wow_user_native_call_failed("control-style-ready", GetLastError());
    wow_edit_native_NtUserAlterWindowStyle(window, mask, flags);
    if (!wow_window_client_refresh(window))
        wow_user_native_call_failed("control-style-refresh", GetLastError());
}

const struct wow_task_client_info *WINAPI wow_edit_client_info(void)
{
    wow_user_runtime_thread *binding = wow_user_runtime_current();
    /* Original usercli.h GetAppVer reads current client state populated by
     * xxxInitTask. It is not GETEXPWINVER(module) or WND.dwExpWinVer. The
     * canonical EDIT entry retains this task across native/guest callouts. */
    if (!binding || !binding->thread || !binding->thread->ptdb ||
            !binding->thread->pClientInfo)
        wow_user_native_call_failed("edit-client-info", ERROR_INVALID_STATE);
    /* ADAPTER-WOW-061: original queue.c::xxxInitTask owns TIF_16BIT.
     * Expose its current identity in the original EDIT clipboard guard;
     * do not read modern TEB private flags or infer it from
     * host executable bitness. This view has no independent flag writers. */
    binding->thread->pClientInfo->CI_flags =
        (binding->thread->TIF_flags & WOW_TASK_TIF_16BIT) ? WOW_TASK_CI_16BIT : 0;
    return binding->thread->pClientInfo;
}

BOOL WINAPI wow_edit_FillWindow(HWND brush_window, HWND paint_window,
    HDC dc, HBRUSH brush)
{
    DWORD error = GetLastError();
    wow_user_native_call call;
    BOOL result = FALSE, restored = FALSE;
    /* Original client FillWindow rejects null DC before its server call.
     * The EDIT consumer already carries native GDI handles, not guest IDs. */
    if (!dc) return FALSE;
    /* Original xxxFillWindow falls back to paint_window when brush_window
     * is absent; the potential CTLCOLOR receiver owns the native callout. */
    if (!wow_user_native_call_begin(&call, brush_window ? brush_window : paint_window))
        return FALSE;
    __try {
        SetLastError(error);
        result = wow_edit_native_NtUserFillWindow(brush_window, paint_window, dc, brush);
    } __finally {
        error = GetLastError();
        restored = wow_user_native_call_end(&call);
        if (restored) SetLastError(error);
    }
    return restored ? result : FALSE;
}

HBRUSH WINAPI wow_edit_NtUserGetControlColor(HWND parent, HWND control,
    HDC dc, UINT message)
{
    DWORD error = GetLastError();
    wow_user_native_call call;
    HBRUSH result = NULL;
    BOOL restored = FALSE;
    /* The original service receives its CTLCOLOR recipient explicitly.
     * Do not substitute the child owner for cross-task handoff, or call
     * SendMessage ourselves and recreate native brush validation policy. */
    if (!wow_user_native_call_begin(&call, parent)) return NULL;
    __try {
        SetLastError(error);
        result = wow_edit_native_NtUserGetControlColor(parent, control, dc, message);
    } __finally {
        error = GetLastError();
        restored = wow_user_native_call_end(&call);
        if (restored) SetLastError(error);
    }
    return restored ? result : NULL;
}
