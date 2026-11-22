#include <windows.h>
#include <winternl.h>
#include "wow_window_access_provider.h"
#include "wow_window_client_state_bindings.h"
#include "wow_user_private_access.h"
#include "wow_user_borrow_scope.h"

/* ADAPTER-WOW-063: common private receive edge, not a control-specific
 * storage policy. Original kernel/getset.c owns validation and old values. */
static const wow_window_access_provider *provider;

BOOL WINAPI wow_window_access_bind(const wow_window_access_provider *value)
{
    PVOID previous;
    if (!value || !value->set_long || !value->get_long ||
            !value->set_client_long || !value->load_menu || !value->window_handle ||
            !value->words || !value->restore_words ||
            !value->layout ||
            !value->capture_words ||
            !value->initialize_state ||
            !value->state_extent ||
            !value->promote_state ||
            !value->initialize_resource ||
            !value->refresh_state ||
            !value->publish_links ||
            !value->state_name ||
            !value->state_fnid ||
            !value->mark_destroyed ||
            !value->creation_class_name) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    previous = InterlockedCompareExchangePointer((PVOID volatile *)&provider,
        (PVOID)value, NULL);
    if (previous && previous != value) {
        SetLastError(ERROR_ALREADY_INITIALIZED); return FALSE;
    }
    return TRUE;
}

const wow_window_access_provider *WINAPI wow_window_access_current(void)
{
    return provider;
}

/* ADAPTER-WOW-063: only GWL_STYLE reaches this server-data continuation
 * through the selected client contract. Original _SetWindowLong retains
 * its ListBox guard. Modern USER owns real mutation and notifications,
 * including the explicitly approved pre-4.0 notification difference. */
DWORD WINAPI xxxSetWindowData(struct tagWND * state, int index, DWORD value, BOOL ansi)
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
    window = wow_window_access_current()->window_handle(state);
    if (!wow_user_borrow_client_state(window, state)) return 0;
    SetLastError(error);
    /* Existing native boundary restores execution and publishes actual
     * return state. A callback may change style or destroy this window;
     * never overwrite that result with value or dereference state afterward. */
    return (DWORD)wow_native_SetWindowLongA(window, index, (LONG)value);
}

LONG WINAPI wow_window_client_send_message(struct tagWND * state, UINT message,
    WPARAM wp, LPARAM lp, BOOL ansi)
{
    HWND window;
    /* clmsg.c SendMessageWorker consumes struct tagWND *, but its message payload is
     * already in the public native A/W contract. Never pass this struct tagWND * to
     * USER or recreate the NT4 private Hook/FNID fast dispatch table.
     * The existing native boundary restores WOW execution and publishes
     * required return state, including after reentrant window destruction. */
    if (!state) {
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return 0;
    }
    window = wow_window_access_current()->window_handle(state);
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
    struct tagWND * state = wow_window_client_validate_hwnd(window);
    if (!state) return 0;
    if (!provider) { SetLastError(ERROR_INVALID_STATE); return 0; }
    return (LONG)provider->set_long(state, index, (DWORD)value, ansi);
}

/* Public access and private storage share the retained original getset
 * owners; dialog-specific pointer publication remains in its own binding. */


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
    struct tagWND * state;
    if (index < 0 || !result) return FALSE;
    state = wow_window_client_validate_hwnd(window);
    if (!state) return private_access_failed(window, result, error);
    /* Original getter owns control and dialog extra-word policy.
     * A zero result, including denied access, must not fall through to the
     * unrelated modern USER storage. Negative indices retain their
     * existing WND procedure/style/identity handling outside this boundary. */
    if (!provider) { *result = 0; SetLastError(ERROR_INVALID_STATE); return TRUE; }
    SetLastError(error);
    *result = (LONG)provider->get_long(state, index, ansi);
    return TRUE;
}

BOOL WINAPI wow_window_client_try_set_long(HWND window, int index, LONG value,
    BOOL ansi, LONG *previous)
{
    DWORD error = GetLastError();
    struct tagWND * state;
    if ((index < 0 && index != GWL_STYLE) || !previous) return FALSE;
    state = wow_window_client_validate_hwnd(window);
    if (!state) return private_access_failed(window, previous, error);
    /* The original client owns dialog procedure/CPD normalization and the
     * initialized ListBox style guard; its server
     * continuation owns result/user/extra-byte writes. Never store a second
     * copy in native USER or treat original zero/error as unhandled. */
    if (!provider) { *previous = 0; SetLastError(ERROR_INVALID_STATE); return TRUE; }
    SetLastError(error);
    *previous = provider->set_client_long(window, index, value, ansi);
    return TRUE;
}
