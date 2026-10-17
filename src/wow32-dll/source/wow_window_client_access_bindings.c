#include "wow_window_client_state_bindings.h"
#include "wow_user_client_view_layout.h"
#include "wow_user_private_access.h"
#include "wow_user_borrow_scope.h"

/* ADAPTER-WOW-063: common private receive edge, not a control-specific
 * storage policy. Original kernel/getset.c owns validation and old values. */
DWORD WINAPI xxxSetWindowLong(PWND, int, DWORD, BOOL);

/* ADAPTER-WOW-063: only GWL_STYLE reaches this server-data continuation
 * through the selected client contract. Original _SetWindowLong retains
 * its ListBox guard. Modern USER owns real mutation and notifications,
 * including the explicitly approved pre-4.0 notification difference. */
DWORD WINAPI xxxSetWindowData(PWND state, int index, DWORD value, BOOL ansi)
{
    HWND window;
    DWORD error = GetLastError();
    (void)ansi; /* Style bits carry no A/W text or procedure identity. */
    if (!state) {
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return 0;
    }
    if (index != GWL_STYLE) {
        SetLastError(ERROR_INVALID_INDEX);
        return 0;
    }
    window = state->head.h;
    if (!wow_user_borrow_client_state(window, state)) return 0;
    SetLastError(error);
    /* Existing native boundary restores execution and publishes actual
     * return state. A callback may change style or destroy this window;
     * never overwrite that result with value or dereference state afterward. */
    return (DWORD)wow_native_SetWindowLongA(window, index, (LONG)value);
}

LONG WINAPI wow_window_client_send_message(PWND state, UINT message,
    WPARAM wp, LPARAM lp, BOOL ansi)
{
    HWND window;
    /* clmsg.c SendMessageWorker consumes PWND, but its message payload is
     * already in the public native A/W contract. Never pass this PWND to
     * USER or recreate the NT4 private Hook/FNID fast dispatch table.
     * The existing native boundary restores WOW execution and publishes
     * required return state, including after reentrant window destruction. */
    if (!state) {
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return 0;
    }
    window = state->head.h;
    /* Retain this exact allocation in the caller's existing scope before
     * native dispatch can destroy/rebind its HWND. Parent/control pointers
     * cached by original code are not themselves an extra storage borrow.
     * Never resolve a replacement allocation solely from the numeric HWND. */
    if (!wow_user_borrow_client_state(window, state)) return 0;
    return (LONG)(ansi ?
        wow_native_SendMessageA(window, message, wp, lp) :
        wow_native_SendMessageW(window, message, wp, lp));
}

LONG WINAPI wow_window_client_set_window_long(HWND window, int index, LONG value,
    BOOL ansi)
{
    PWND state = wow_window_client_validate_hwnd(window);
    if (!state) return 0;
    return (LONG)xxxSetWindowLong(state, index, (DWORD)value, ansi);
}

/* Public access and private storage share the retained original getset
 * owners; dialog-specific pointer publication remains in its own binding. */
DWORD WINAPI _GetWindowLong(PWND, int, BOOL);
LONG WINAPI _SetWindowLong(HWND, int, LONG, BOOL);

/* Failure to retain an owned original view is not permission to read/write
 * native extra bytes instead. This query acquires only the existing window
 * owner; it neither allocates a view nor invents another identity map. */
static BOOL private_access_failed(HWND window, LONG *result, DWORD entry_error)
{
    DWORD failure = GetLastError();
    wow_window_words_binding *binding = wow_window_words_acquire(window);
    BOOL owned = binding && wow_window_words_owns_client_state(binding,
        wow_window_words_client_state_value(binding));
    if (binding) wow_window_words_release(binding);
    if (owned) *result = 0;
    SetLastError(owned ? failure : entry_error);
    return owned;
}

BOOL WINAPI wow_window_client_try_get_long(HWND window, int index, BOOL ansi,
    LONG *result)
{
    DWORD error = GetLastError();
    PWND state;
    if (index < 0 || !result) return FALSE;
    state = wow_window_client_validate_hwnd(window);
    if (!state) return private_access_failed(window, result, error);
    /* Original getter owns control and dialog extra-word policy.
     * A zero result, including denied access, must not fall through to the
     * unrelated modern USER storage. Negative indices retain their
     * existing WND procedure/style/identity handling outside this boundary. */
    SetLastError(error);
    *result = (LONG)_GetWindowLong(state, index, ansi);
    return TRUE;
}

BOOL WINAPI wow_window_client_try_set_long(HWND window, int index, LONG value,
    BOOL ansi, LONG *previous)
{
    DWORD error = GetLastError();
    PWND state;
    if ((index < 0 && index != GWL_STYLE) || !previous) return FALSE;
    state = wow_window_client_validate_hwnd(window);
    if (!state) return private_access_failed(window, previous, error);
    /* The original client owns dialog procedure/CPD normalization and the
     * initialized ListBox style guard; its server
     * continuation owns result/user/extra-byte writes. Never store a second
     * copy in native USER or treat original zero/error as unhandled. */
    SetLastError(error);
    *previous = _SetWindowLong(window, index, value, ansi);
    return TRUE;
}
