#include <windows.h>
#include "wow_window_access_provider.h"
#include <winternl.h>
#include "wow_default_window_bindings.h"
#include "wow_window_words_binding.h"
#include "wow_user_borrow_scope.h"

/* ADAPTER-WOW-063 / OPENNT-HOST-089: original client algorithm, with a
 * host stored-name lifetime boundary. No text conversion is duplicated. */
static wow_default_window_provider default_provider;
BOOL WINAPI wow_default_window_bind(wow_default_window_provider provider)
{
    wow_default_window_provider previous;
    if (!provider) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    previous = (wow_default_window_provider)InterlockedCompareExchangePointer(
        (PVOID volatile *)&default_provider, (PVOID)provider, NULL);
    if (previous && previous != provider) {
        SetLastError(ERROR_ALREADY_INITIALIZED);
        return FALSE;
    }
    return TRUE;
}
__declspec(thread) wow_default_context *wow_default_current;

LONG WINAPI wow_edit_default_control_color(struct tagWND * state, UINT message,
    WPARAM wp, LPARAM lp, DWORD ansi)
{
    wow_window_words_binding *binding;
    wow_window_creation_facts facts;
    struct tagWND * source;
    DWORD error = GetLastError();
    HWND recipient, control;
    if (message != WM_CTLCOLORSTATIC)
        return wow_default_window_proc(state, message, wp, lp, ansi);
    if (!state) {
        SetLastError(ERROR_INVALID_WINDOW_HANDLE); return 0;
    }
    binding = wow_user_borrow_client_state(wow_window_access_current()->window_handle(state), state);
    if (!binding) return 0;
    if (wow_window_words_creation_facts(binding, &facts)) {
        SetLastError(error);
        return wow_default_window_proc(state, message, wp, lp, ansi);
    }
    if (GetLastError() != ERROR_NOT_FOUND) return 0;
    /* Original ECGetControlBrush passes struct tagWND * here, not the public HWND.
     * Retain both exact allocations and capture handles before reentry. */
    source = (struct tagWND *)lp;
    if (!source) {
        SetLastError(ERROR_INVALID_WINDOW_HANDLE); return 0;
    }
    if (!wow_user_borrow_client_state(wow_window_access_current()->window_handle(source), source)) return 0;
    recipient = wow_window_access_current()->window_handle(state);
    control = wow_window_access_current()->window_handle(source);
    SetLastError(error);
    return wow_default_CsSendMessage(recipient, message, wp, (LPARAM)control,
        0, wow_window_access_current()->default_function, ansi);
}

static LONG invoke_default(wow_window_words_binding *binding, struct tagWND * state, UINT message,
    WPARAM wp, LPARAM lp, DWORD ansi)
{
    struct tagWND * locked;
    LONG result = 0;
    DWORD error = GetLastError();
    /* Native USER owns UI-state propagation and input-modality policy.
     * Do not maintain a second copy in the retained original WND view. */
    if (message == WM_UPDATEUISTATE || message == WM_CHANGEUISTATE ||
            message == WM_QUERYUISTATE)
        return wow_default_CsSendMessage(wow_window_access_current()->window_handle(state),
            message, wp, lp, 0, wow_window_access_current()->default_function, ansi);
    if (message != WM_GETTEXT && message != WM_GETTEXTLENGTH)
        return default_provider(state, message, wp, lp, ansi);
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
        result = default_provider(state, message, wp, lp, ansi);
    } __finally {
        error = GetLastError();
        wow_window_words_unlock_client_name(binding);
        SetLastError(error);
    }
    return result;
}

LONG WINAPI wow_default_window_proc(struct tagWND * state, UINT message,
    WPARAM wp, LPARAM lp, DWORD ansi)
{
    DWORD error = GetLastError();
    wow_default_context context, *previous = wow_default_current;
    wow_window_words_binding *binding;
    LONG result;
    if (!state) {
        SetLastError(ERROR_INVALID_WINDOW_HANDLE); return 0;
    }
    if (!default_provider) RaiseFailFastException(NULL, NULL, 0);
    /* Every admitted caller enters through the control/dialog graph owner.
     * Retain its exact allocation for all messages, not only name reads.
     * Do not refresh by HWND here: after nested destruction that handle may
     * denote a replacement, while original code still owns this struct tagWND *. */
    binding = wow_user_borrow_client_state(wow_window_access_current()->window_handle(state), state);
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
