#include "non-mvdm/public/sdk/inc/nt.h"
#include "include/wow_user_registration_state.h"
#include "include/wow_class_words_binding.h"
#include "include/wow_window_words_binding.h"
#include "include/wow_user_object_bindings.h"
#include "include/wow_user_borrow_scope.h"
#include "include/wow_menu_native_bindings.h"

static PFNWOWHANDLERSIN wow_input_handlers;
static wow_user_task_lifecycle wow_lifecycle;

const struct tagAPFNWOWHANDLERSIN *WINAPI wow_user_registered_callbacks(void)
{
    return &wow_input_handlers;
}


BOOL WINAPI wow_user_get_messageA(LPMSG message, HWND window, UINT first, UINT last)
{
    return wow_user_task_lifecycle_message(&wow_lifecycle, message, window,
        first, last, PM_REMOVE, TRUE);
}

BOOL WINAPI wow_user_peek_messageA(LPMSG message, HWND window, UINT first,
    UINT last, UINT flags)
{
    return wow_user_task_lifecycle_message(&wow_lifecycle, message, window,
        first, last, flags, FALSE);
}

BOOL WINAPI wow_user_wait_message(void)
{
    return wow_user_task_lifecycle_wait_message(&wow_lifecycle);
}

BOOL WINAPI wow_user_get_messageW(LPMSG message, HWND window, UINT first, UINT last)
{
    return wow_user_task_lifecycle_messageW(&wow_lifecycle, message, window,
        first, last, PM_REMOVE, TRUE);
}

BOOL WINAPI wow_user_peek_messageW(LPMSG message, HWND window, UINT first,
    UINT last, UINT flags)
{
    return wow_user_task_lifecycle_messageW(&wow_lifecycle, message, window,
        first, last, flags, FALSE);
}

static LRESULT call_previous_window_proc(WNDPROC procedure, HWND window,
    UINT message, WPARAM wp, LPARAM lp, BOOL unicode)
{
    wow_user_borrow_scope scope;
    wow_task_callback_scope task_scope;
    LRESULT result;
    DWORD resolved;
    /* Original clmsg.c::CallWindowProcAorW resolves CPD first, then
     * usercli.h::CALLPROC_WOWCHECK forwards tagged WOW targets with NULL WW.
     * Modern USER owns native/CPD translation but cannot interpret this
     * worker's WOW target. Keep the original registered callback as owner. */
    if (!wow_native_class_resolve_procedure((DWORD)procedure, &resolved)) return 0;
    if (resolved != (DWORD)procedure ||
            !((DWORD)(ULONG_PTR)procedure & WNDPROC_WOW)) {
        wow_user_native_call call;
        BOOL restored;
        DWORD error = GetLastError();
        wow_window_note_native_procedure(window, procedure, unicode);
        if (!wow_user_native_call_begin(&call, window)) return 0;
        SetLastError(error);
        __try {
            result = unicode ? CallWindowProcW(procedure, window, message, wp, lp) :
                CallWindowProcA(procedure, window, message, wp, lp);
            wow_user_native_call_result(&call, message, wp, lp, result);
        } __finally {
            error = GetLastError();
            restored = wow_user_native_call_end(&call);
            if (restored) SetLastError(error);
        }
        return restored ? result : 0;
    }
    if (!wow_input_handlers.pfnWowWndProcEx) {
        SetLastError(ERROR_INVALID_STATE);
        return 0;
    }
    if (!wow_task_callback_enter(&task_scope)) return 0;
    if (!wow_user_borrow_enter(&scope)) {
        (void)wow_task_callback_leave(&task_scope);
        return 0;
    }
    __try {
        /* CallWindowProc's tagged-WOW branch deliberately passes NULL WW;
         * it does not traverse wow_window_dispatch_bound. Preserve that
         * original subclass contract while publishing a menu carried by
         * the message before the original callback converts its handle. */
        result = 0;
        if (wow_user_window_refresh_message(window, message, wp, lp) &&
                wow_menu_native_refresh_message(message, wp, lp))
            result = wow_input_handlers.pfnWowWndProcEx(window, message, wp, lp,
                (DWORD)(ULONG_PTR)procedure, NULL);
    } __finally {
        wow_user_borrow_leave_required(&scope);
        (void)wow_task_callback_leave(&task_scope);
    }
    return result;
}

LRESULT WINAPI wow_user_call_window_procA(WNDPROC procedure, HWND window,
    UINT message, WPARAM wp, LPARAM lp)
{
    return call_previous_window_proc(procedure, window, message, wp, lp, FALSE);
}

LRESULT WINAPI wow_user_call_window_procW(WNDPROC procedure, HWND window,
    UINT message, WPARAM wp, LPARAM lp)
{
    return call_previous_window_proc(procedure, window, message, wp, lp, TRUE);
}

wow_user_task_lifecycle *WINAPI wow_user_registered_lifecycle(void)
{
    return &wow_lifecycle;
}

void WINAPI wow_user_set_registered_callbacks(APFNWOWHANDLERSIN input)
{
    wow_input_handlers = *input;
}
