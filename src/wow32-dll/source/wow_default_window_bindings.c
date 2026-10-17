#include "wow_default_window_bindings.h"
#include "wow_user_client_view_layout.h"
#include "wow_window_words_binding.h"
#include "wow_user_borrow_scope.h"

/* ADAPTER-WOW-063 / OPENNT-HOST-089: original client algorithm, with a
 * host stored-name lifetime boundary. No text conversion is duplicated. */
LONG WINAPI wow_original_default_window_proc(PWND, UINT, WPARAM, LPARAM, DWORD);
__declspec(thread) wow_default_context *wow_default_current;

LONG WINAPI wow_edit_default_control_color(PWND state, UINT message,
    WPARAM wp, LPARAM lp, DWORD ansi)
{
    wow_window_words_binding *binding;
    wow_window_creation_facts facts;
    PWND source;
    DWORD error = GetLastError();
    HWND recipient, control;
    if (message != WM_CTLCOLORSTATIC)
        return wow_default_window_proc(state, message, wp, lp, ansi);
    if (!state) {
        SetLastError(ERROR_INVALID_WINDOW_HANDLE); return 0;
    }
    binding = wow_user_borrow_client_state(state->head.h, state);
    if (!binding) return 0;
    if (wow_window_words_creation_facts(binding, &facts)) {
        SetLastError(error);
        return wow_default_window_proc(state, message, wp, lp, ansi);
    }
    if (GetLastError() != ERROR_NOT_FOUND) return 0;
    /* Original ECGetControlBrush passes PWND here, not the public HWND.
     * Retain both exact allocations and capture handles before reentry. */
    source = (PWND)lp;
    if (!source) {
        SetLastError(ERROR_INVALID_WINDOW_HANDLE); return 0;
    }
    if (!wow_user_borrow_client_state(source->head.h, source)) return 0;
    recipient = state->head.h;
    control = source->head.h;
    SetLastError(error);
    return wow_default_CsSendMessage(recipient, message, wp, (LPARAM)control,
        0, FNID_DEFWINDOWPROC, ansi);
}

static LONG invoke_default(wow_window_words_binding *binding, PWND state, UINT message,
    WPARAM wp, LPARAM lp, DWORD ansi)
{
    PWND locked;
    LONG result = 0;
    DWORD error = GetLastError();
    if (message != WM_GETTEXT && message != WM_GETTEXTLENGTH)
        return wow_original_default_window_proc(state, message, wp, lp, ansi);
    /* The original caller's ThreadLock equivalent already retains this
     * allocation. After nested destruction the HWND property can be gone
     * (or replaced); use that exact borrow, not a new native identity. */
    locked = wow_window_words_lock_client_name(binding);
    if (locked != state) {
        if (locked) wow_window_words_unlock_client_name(binding);
        SetLastError(ERROR_INVALID_STATE); return 0;
    }
    __try {
        SetLastError(error);
        /* Original server.c classification keeps both messages in the
         * client-only switch. These branches convert/copy, never callback.
         * An invalid destination may raise: finally must still unlock. */
        result = wow_original_default_window_proc(state, message, wp, lp, ansi);
    } __finally {
        error = GetLastError();
        wow_window_words_unlock_client_name(binding);
        SetLastError(error);
    }
    return result;
}

LONG WINAPI wow_default_window_proc(PWND state, UINT message,
    WPARAM wp, LPARAM lp, DWORD ansi)
{
    DWORD error = GetLastError();
    wow_default_context context, *previous = wow_default_current;
    wow_window_words_binding *binding;
    LONG result;
    if (!state) {
        SetLastError(ERROR_INVALID_WINDOW_HANDLE); return 0;
    }
    /* Every admitted caller enters through the control/dialog graph owner.
     * Retain its exact allocation for all messages, not only name reads.
     * Do not refresh by HWND here: after nested destruction that handle may
     * denote a replacement, while original code still owns this PWND. */
    binding = wow_user_borrow_client_state(state->head.h, state);
    if (!binding) return 0;
    if (!wow_default_capture_context(&context, message)) return 0;
    wow_default_current = &context;
    __try {
        SetLastError(error);
        result = invoke_default(binding, state, message, wp, lp, ansi);
    } __finally {
        /* Reentrant callbacks get their own stack snapshot; neither normal
         * return nor an exception may leave TLS pointing at a dead frame. */
        wow_default_current = previous;
    }
    return result;
}
