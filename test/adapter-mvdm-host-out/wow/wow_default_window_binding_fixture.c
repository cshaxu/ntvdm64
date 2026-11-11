#include "wow_default_window_bindings.h"
#include "wow_user_client_view_layout.h"
#include "wow_window_words_binding.h"
#include "wow_user_borrow_scope.h"
#include <stdio.h>

/* Links the production wrapper with boundary doubles. It proves lock and
 * exception scope only, not original text conversion or native dispatch. */
static WND state, replacement;
static int lookups, locked, calls, failures;
static BOOL mismatch, raise_in_reader;
static BOOL nest_next, fail_capture;
static DWORD facts_error;
static BOOL reject_borrow, reject_source, native_raises;
static int native_calls;
static wow_default_context *nesting_parent;
BOOL WINAPI wow_default_capture_context(wow_default_context *context, UINT message)
{
    if (fail_capture) { SetLastError(ERROR_NOT_READY); return FALSE; }
    ZeroMemory(context, sizeof(*context));
    context->server.uiShellMsg = message;
    return TRUE;
}
#define CHECK(x) do { if (!(x)) { ++failures; printf("FAIL %d: %s\n", __LINE__, #x); } } while (0)

wow_window_words_binding *WINAPI wow_user_borrow_client_state(HWND window, PWND expected)
{
    /* The caller's existing scope owns this reference. The default wrapper
     * must neither reacquire the HWND property nor release that scope borrow. */
    CHECK((window == state.head.h && expected == &state) ||
        (window == replacement.head.h && expected == &replacement));
    ++lookups;
    if (reject_borrow || (reject_source && expected == &replacement)) {
        SetLastError(ERROR_INVALID_STATE); return NULL;
    }
    SetLastError(111);
    return (wow_window_words_binding *)expected;
}
BOOL WINAPI wow_window_words_creation_facts(wow_window_words_binding *binding,
    wow_window_creation_facts *facts)
{
    CHECK(binding == (wow_window_words_binding *)&state);
    if (facts_error) { SetLastError(facts_error); return FALSE; }
    ZeroMemory(facts, sizeof(*facts));
    return TRUE;
}
LONG WINAPI wow_default_CsSendMessage(HWND window, UINT message, WPARAM wp,
    LPARAM lp, DWORD extra, DWORD function, BOOL ansi)
{
    CHECK(window == state.head.h && message == WM_CTLCOLORSTATIC && wp == 73);
    CHECK(lp == (LPARAM)replacement.head.h && extra == 0 &&
        function == FNID_DEFWINDOWPROC && ansi == 1);
    CHECK(GetLastError() == 456);
    ++native_calls;
    SetLastError(790);
    if (native_raises) RaiseException(0xe0447004, 0, 0, NULL);
    return 124;
}
PWND WINAPI wow_window_words_lock_client_name(wow_window_words_binding *binding)
{
    CHECK(binding == (wow_window_words_binding *)&state);
    ++locked;
    return mismatch ? &replacement : &state;
}
void WINAPI wow_window_words_unlock_client_name(wow_window_words_binding *binding)
{
    CHECK(binding == (wow_window_words_binding *)&state && locked == 1);
    --locked;
    SetLastError(222);
}
LONG WINAPI wow_original_default_window_proc(PWND window, UINT message,
    WPARAM wp, LPARAM lp, DWORD ansi)
{
    CHECK(window == &state && wp == 73 && lp == 91 && ansi == 1);
    CHECK(GetLastError() == 456);
    CHECK(wow_default_current && wow_default_current->server.uiShellMsg == message);
    CHECK(locked == (message == WM_GETTEXT || message == WM_GETTEXTLENGTH));
    ++calls;
    if (nest_next) {
        wow_default_context *parent = wow_default_current;
        nest_next = FALSE;
        nesting_parent = parent;
        SetLastError(456);
        CHECK(wow_default_window_proc(&state, WM_HELP, 73, 91, 1) == 123);
        CHECK(wow_default_current == parent && !locked);
        nesting_parent = NULL;
    } else if (nesting_parent) {
        CHECK(wow_default_current != nesting_parent);
    }
    SetLastError(789);
    if (raise_in_reader) RaiseException(0xe0447003, 0, 0, NULL);
    return 123;
}

int main(void)
{
    UINT messages[] = {WM_GETTEXT, WM_GETTEXTLENGTH, WM_HELP, WM_CTLCOLORSTATIC};
    UINT i;
    BOOL caught = FALSE;
    state.head.h = (HWND)(ULONG_PTR)0x1234;
    for (i = 0; i < ARRAYSIZE(messages); ++i) {
        SetLastError(456);
        CHECK(wow_default_window_proc(&state, messages[i], 73, 91, 1) == 123);
        CHECK(GetLastError() == 789 && !locked && !wow_default_current);
    }
    mismatch = TRUE;
    CHECK(!wow_default_window_proc(&state, WM_GETTEXT, 73, 91, 1));
    CHECK(GetLastError() == ERROR_INVALID_STATE && calls == 4 && !locked);
    mismatch = FALSE;
    raise_in_reader = TRUE;
    __try {
        SetLastError(456);
        (void)wow_default_window_proc(&state, WM_GETTEXT, 73, 91, 1);
    } __except (GetExceptionCode() == 0xe0447003 ?
        EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
        caught = TRUE;
    }
    CHECK(caught && !locked && calls == 5 && !wow_default_current);
    CHECK(!wow_default_window_proc(NULL, WM_GETTEXTLENGTH, 0, 0, 0));
    CHECK(GetLastError() == ERROR_INVALID_WINDOW_HANDLE && calls == 5);
    raise_in_reader = FALSE;
    nest_next = TRUE;
    SetLastError(456);
    CHECK(wow_default_window_proc(&state, WM_HELP, 73, 91, 1) == 123);
    CHECK(calls == 7 && !wow_default_current && GetLastError() == 789);
    fail_capture = TRUE;
    CHECK(!wow_default_window_proc(&state, WM_HELP, 73, 91, 1));
    CHECK(GetLastError() == ERROR_NOT_READY && calls == 7 && !wow_default_current);
    CHECK(lookups == 9); /* Every default entry, including failed capture. */
    {
        int before = calls;
        reject_borrow = TRUE;
        fail_capture = FALSE;
        CHECK(!wow_default_window_proc(&state, WM_HELP, 73, 91, 1));
        CHECK(GetLastError() == ERROR_INVALID_STATE && calls == before);
        CHECK(!locked && !wow_default_current);
        reject_borrow = FALSE;
    }
    {
        wow_default_context control_geometry = {0};
        /* Direct list geometry and a nested default worker use distinct
         * snapshots. The nested worker must restore the control snapshot
         * on both normal return and an exception. This does not substitute
         * for the full original ListBox entry/resize acceptance. */
        control_geometry.server.gclBorder = 19;
        wow_default_current = &control_geometry;
        fail_capture = FALSE;
        SetLastError(456);
        CHECK(wow_default_window_proc(&state, WM_HELP, 73, 91, 1) == 123);
        CHECK(wow_default_current == &control_geometry &&
            control_geometry.server.gclBorder == 19);
        raise_in_reader = TRUE;
        caught = FALSE;
        __try {
            SetLastError(456);
            (void)wow_default_window_proc(&state, WM_HELP, 73, 91, 1);
        } __except (GetExceptionCode() == 0xe0447003 ?
            EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
            caught = TRUE;
        }
        CHECK(caught && wow_default_current == &control_geometry &&
            control_geometry.server.gclBorder == 19 && !locked);
        wow_default_current = NULL;
    }
    /* Exact ECGetControlBrush boundary, not a general CTLCOLOR dispatch.
     * Known facts keep the original worker; only absent facts use native. */
    raise_in_reader = FALSE;
    replacement.head.h = (HWND)(ULONG_PTR)0x5678;
    SetLastError(456);
    CHECK(wow_edit_default_control_color(&state, WM_CTLCOLORSTATIC, 73, 91, 1) == 123);
    CHECK(GetLastError() == 789 && !native_calls);
    facts_error = ERROR_NOT_FOUND;
    SetLastError(456);
    CHECK(wow_edit_default_control_color(&state, WM_CTLCOLOREDIT, 73, 91, 1) == 123);
    CHECK(!native_calls);
    SetLastError(456);
    CHECK(wow_edit_default_control_color(&state, WM_CTLCOLORSTATIC, 73,
        (LPARAM)&replacement, 1) == 124);
    CHECK(native_calls == 1 && GetLastError() == 790);
    reject_borrow = TRUE;
    CHECK(!wow_edit_default_control_color(&state, WM_CTLCOLORSTATIC, 73,
        (LPARAM)&replacement, 1));
    CHECK(native_calls == 1 && GetLastError() == ERROR_INVALID_STATE);
    reject_borrow = FALSE;
    reject_source = TRUE;
    CHECK(!wow_edit_default_control_color(&state, WM_CTLCOLORSTATIC, 73,
        (LPARAM)&replacement, 1));
    CHECK(native_calls == 1 && GetLastError() == ERROR_INVALID_STATE);
    reject_source = FALSE;
    facts_error = ERROR_INVALID_DATA;
    CHECK(!wow_edit_default_control_color(&state, WM_CTLCOLORSTATIC, 73,
        (LPARAM)&replacement, 1));
    CHECK(native_calls == 1 && GetLastError() == ERROR_INVALID_DATA);
    facts_error = ERROR_NOT_FOUND;
    native_raises = TRUE;
    caught = FALSE;
    __try {
        SetLastError(456);
        (void)wow_edit_default_control_color(&state, WM_CTLCOLORSTATIC, 73,
            (LPARAM)&replacement, 1);
    } __except (GetExceptionCode() == 0xe0447004 ?
        EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
        caught = TRUE;
    }
    CHECK(caught && native_calls == 2 && !wow_default_current);
    printf("default_window_binding_failures=%d\n", failures);
    return failures != 0;
}
