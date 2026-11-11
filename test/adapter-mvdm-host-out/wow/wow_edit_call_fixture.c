#include "wow_edit_call_bindings.h"
#include "wow_user_task_lifecycle.h"
#include "wow_user_client_view_layout.h"
#include "wow_user_borrow_scope.h"
#include <stdio.h>

/* Link the actual FillWindow adapter with these transport doubles. This
 * tests its call lifetime, not native drawing, WOW scheduling or the guest.
 * Execute only with the completed EDIT dependency group's background suite. */
static unsigned begins, leaves, native_calls, depth;
static BOOL reject_begin, reject_end, native_failure, nested, raise_native;
static HWND receiver_seen;
static int failures;
static wow_user_runtime_thread runtime;
static wow_task_order_thread task;
static wow_task_order_entry task_entry;
static wow_task_client_info client;
static WND state_window;
static DWORD native_style;
static DWORD native_exstyle;
static unsigned state_calls, state_refreshes;
static BOOL reject_ready, reject_refresh;
static BOOL reject_state;
static unsigned reset_calls;
static BOOL reject_reset;
static unsigned text_calls;
static BOOL reject_text_borrow;
static unsigned color_native_calls;
static BOOL color_nested, color_exception;
/* Link the unchanged OPENNT-HOST-088 client.c selection as well. */
void WINAPI SetWindowState(PWND, UINT);
void WINAPI ClearWindowState(PWND, UINT);
#define CHECK(x) do { if (!(x)) { \
    printf("FAIL line %d: %s\n", __LINE__, #x); ++failures; } } while (0)
#define BRUSH_WINDOW ((HWND)(ULONG_PTR)0x101)
#define PAINT_WINDOW ((HWND)(ULONG_PTR)0x202)
#define PAINT_DC ((HDC)(ULONG_PTR)0x303)
#define PAINT_BRUSH ((HBRUSH)(ULONG_PTR)6)
wow_window_words_binding *WINAPI wow_user_borrow_client_state(HWND window, PWND state)
{
    CHECK(window == state_window.head.h && state == &state_window);
    return reject_text_borrow ? NULL : (wow_window_words_binding *)&state_window;
}
BOOL WINAPI wow_edit_native_NtUserDefSetText(HWND window, const LARGE_STRING *text)
{
    CHECK(window == state_window.head.h && depth > 0 && !text);
    CHECK(GetLastError() == 456);
    ++text_calls;
    SetLastError(705);
    return !native_failure;
}
static void check_text_calls(void)
{
    unsigned before;
    state_window.head.h = BRUSH_WINDOW;
    SetLastError(456);
    CHECK(wow_edit_NtUserDefSetText(BRUSH_WINDOW, NULL));
    CHECK(text_calls == 1 && GetLastError() == 705 && !depth);
    native_failure = TRUE;
    SetLastError(456);
    CHECK(!wow_edit_NtUserDefSetText(BRUSH_WINDOW, NULL));
    CHECK(text_calls == 2 && GetLastError() == 705 && !depth);
    native_failure = FALSE;
    before = begins;
    reject_text_borrow = TRUE;
    CHECK(!wow_edit_NtUserDefSetText(BRUSH_WINDOW, NULL));
    CHECK(begins == before && text_calls == 2);
    reject_text_borrow = FALSE;
}
#define TEST_EXCEPTION ((DWORD)0xE0424473)

BOOL WINAPI wow_list_native_reset_double_click(void)
{
    ++reset_calls;
    CHECK(depth == 0);
    if (reject_reset) { SetLastError(ERROR_INVALID_STATE); return FALSE; }
    return TRUE;
}

BOOL WINAPI wow_edit_native_ready(void)
{
    if (reject_ready) { SetLastError(ERROR_INVALID_STATE); return FALSE; }
    return TRUE;
}

VOID WINAPI wow_edit_native_NtUserSetWindowState(HWND window, UINT flags)
{
    CHECK(window == state_window.head.h);
    ++state_calls;
    if (HIBYTE(flags) >= 12)
        native_style |= (DWORD)LOBYTE(flags) << (8 * (HIBYTE(flags) - 12));
    else {
        CHECK(flags == 0x0903);
        native_exstyle |= WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE;
    }
    SetLastError(706);
}

VOID WINAPI wow_edit_native_NtUserClearWindowState(HWND window, UINT flags)
{
    CHECK(window == state_window.head.h);
    ++state_calls;
    if (HIBYTE(flags) >= 12)
        native_style &= ~((DWORD)LOBYTE(flags) << (8 * (HIBYTE(flags) - 12)));
    else {
        CHECK(flags == 0x0903);
        native_exstyle &= ~(WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE);
    }
    SetLastError(706);
}

VOID WINAPI wow_edit_native_NtUserAlterWindowStyle(HWND window, DWORD mask,
    DWORD flags)
{
    CHECK(window == state_window.head.h);
    ++state_calls;
    native_style = (native_style & ~mask) | (flags & mask);
    SetLastError(706);
}

BOOL WINAPI wow_window_client_refresh(HWND window)
{
    CHECK(window == state_window.head.h && GetLastError() == 706);
    ++state_refreshes;
    if (reject_refresh) { SetLastError(ERROR_INVALID_STATE); return FALSE; }
    state_window.style = native_style;
    state_window.ExStyle = native_exstyle;
    return TRUE;
}

PWND WINAPI wow_window_client_validate_hwnd(HWND window)
{
    CHECK(window == state_window.head.h);
    if (reject_state) { SetLastError(ERROR_INVALID_STATE); return NULL; }
    return &state_window;
}

static void check_state_calls(void)
{
    static const UINT flags[] = {0x0e80, 0x0e10, 0x0d08, 0x0c20, 0x0d02, 0x0eb0};
    unsigned i, calls, refreshes;
    BOOL caught;
    state_window.head.h = BRUSH_WINDOW;
    /* Original dialog's temporary color-compatibility flag is owned by the
     * retained WND, not the modern USER state services. Unavailable native
     * exports or refresh must therefore be irrelevant to this operation. */
    calls = state_calls;
    refreshes = state_refreshes;
    reject_ready = reject_refresh = TRUE;
    SetLastError(456);
    SetWindowState(&state_window, WFWIN40COMPAT);
    CHECK(TestWF(&state_window, WFWIN40COMPAT) == LOBYTE(WFWIN40COMPAT));
    CHECK(GetLastError() == 456 && native_style == 0);
    SetWindowState(&state_window, WFWIN40COMPAT);
    ClearWindowState(&state_window, WFWIN40COMPAT);
    ClearWindowState(&state_window, WFWIN40COMPAT);
    CHECK(!TestWF(&state_window, WFWIN40COMPAT));
    CHECK(state_calls == calls && state_refreshes == refreshes);
    CHECK(GetLastError() == 456 && native_style == 0);
    reject_state = TRUE;
    caught = FALSE;
    __try { SetWindowState(&state_window, WFWIN40COMPAT); }
    __except (GetExceptionCode() == TEST_EXCEPTION ?
            EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) { caught = TRUE; }
    CHECK(caught && !TestWF(&state_window, WFWIN40COMPAT));
    CHECK(state_calls == calls && state_refreshes == refreshes);
    reject_state = reject_ready = reject_refresh = FALSE;
    for (i = 0; i < ARRAYSIZE(flags); ++i) {
        calls = state_calls;
        refreshes = state_refreshes;
        SetWindowState(&state_window, flags[i]);
        CHECK(TestWF(&state_window, flags[i]) == LOBYTE(flags[i]));
        CHECK(state_calls == calls + 1 && state_refreshes == refreshes + 1);
        CHECK(GetLastError() == 706);
        SetLastError(456);
        SetWindowState(&state_window, flags[i]);
        CHECK(state_calls == calls + 1 && state_refreshes == refreshes + 1);
        CHECK(GetLastError() == 456); /* Original guard: no native call. */
        ClearWindowState(&state_window, flags[i]);
        CHECK(!TestWF(&state_window, flags[i]));
        CHECK(state_calls == calls + 2 && state_refreshes == refreshes + 2);
        SetLastError(456);
        ClearWindowState(&state_window, flags[i]);
        CHECK(state_calls == calls + 2 && state_refreshes == refreshes + 2);
        CHECK(GetLastError() == 456);
    }
    native_exstyle = state_window.ExStyle = WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE |
        WS_EX_CONTROLPARENT;
    calls = state_calls;
    refreshes = state_refreshes;
    ClearWindowState(&state_window, WEFEDGEMASK);
    CHECK(native_exstyle == WS_EX_CONTROLPARENT && state_window.ExStyle == native_exstyle);
    CHECK(state_calls == calls + 1 && state_refreshes == refreshes + 1);
    CHECK(state_window.style == 0 && native_style == 0);
    calls = state_calls;
    reject_ready = TRUE;
    caught = FALSE;
    __try { SetWindowState(&state_window, flags[0]); }
    __except (GetExceptionCode() == TEST_EXCEPTION ?
            EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) { caught = TRUE; }
    CHECK(caught && state_calls == calls && native_style == 0);
    reject_ready = FALSE;
    reject_refresh = TRUE;
    caught = FALSE;
    __try { SetWindowState(&state_window, flags[0]); }
    __except (GetExceptionCode() == TEST_EXCEPTION ?
            EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) { caught = TRUE; }
    CHECK(caught && state_calls == calls + 1 && state_window.style == 0);
    CHECK(native_style == WS_BORDER); /* Never claim the failed refresh worked. */
    reject_refresh = FALSE;
    native_style = state_window.style = WS_BORDER | WS_VSCROLL;
    calls = state_calls;
    refreshes = state_refreshes;
    wow_edit_alter_window_style(BRUSH_WINDOW, WS_VSCROLL, 0);
    CHECK(native_style == WS_BORDER && state_window.style == native_style);
    CHECK(state_calls == calls + 1 && state_refreshes == refreshes + 1);
    CHECK(GetLastError() == 706);
    wow_edit_alter_window_style(BRUSH_WINDOW, WS_VSCROLL | WS_HSCROLL, WS_HSCROLL);
    CHECK(native_style == (WS_BORDER | WS_HSCROLL) && state_window.style == native_style);
    CHECK(state_calls == calls + 2 && state_refreshes == refreshes + 2);
    reject_ready = TRUE;
    caught = FALSE;
    __try { wow_edit_alter_window_style(BRUSH_WINDOW, WS_HSCROLL, 0); }
    __except (GetExceptionCode() == TEST_EXCEPTION ?
            EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) { caught = TRUE; }
    CHECK(caught && state_calls == calls + 2 && state_refreshes == refreshes + 2);
    reject_ready = FALSE;
    reject_refresh = TRUE;
    caught = FALSE;
    __try { wow_edit_alter_window_style(BRUSH_WINDOW, WS_HSCROLL, 0); }
    __except (GetExceptionCode() == TEST_EXCEPTION ?
            EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) { caught = TRUE; }
    CHECK(caught && native_style == WS_BORDER);
    CHECK(state_window.style == (WS_BORDER | WS_HSCROLL));
    CHECK(state_calls == calls + 3 && state_refreshes == refreshes + 3);
    reject_refresh = FALSE;
}

wow_user_runtime_thread *WINAPI wow_user_runtime_current(void) { return &runtime; }
__declspec(noreturn) void WINAPI wow_user_native_call_failed(const char *stage, DWORD error)
{
    (void)stage;
    CHECK(error == ERROR_INVALID_STATE);
    RaiseException(TEST_EXCEPTION, 0, 0, NULL);
    ExitProcess(99); /* A fixture must never continue a rejected invariant. */
}

BOOL WINAPI wow_user_native_call_begin(wow_user_native_call *call, HWND receiver)
{
    ++begins;
    receiver_seen = receiver;
    ZeroMemory(call, sizeof(*call));
    if (reject_begin) { SetLastError(701); return FALSE; }
    ++depth;
    SetLastError(702); /* Bookkeeping must not replace native entry error. */
    return TRUE;
}

BOOL WINAPI wow_user_native_call_end(wow_user_native_call *call)
{
    (void)call;
    CHECK(depth > 0);
    --depth;
    ++leaves;
    SetLastError(reject_end ? 703 : 704);
    return !reject_end;
}

BOOL WINAPI wow_edit_native_NtUserFillWindow(HWND brush, HWND paint,
    HDC dc, HBRUSH color)
{
    ++native_calls;
    CHECK(depth > 0 && GetLastError() == 456);
    CHECK(brush == BRUSH_WINDOW || brush == NULL);
    CHECK(paint == PAINT_WINDOW && dc == PAINT_DC && color == PAINT_BRUSH);
    if (nested && depth == 1) {
        CHECK(wow_edit_FillWindow(NULL, paint, dc, color));
        CHECK(depth == 1 && GetLastError() == 705);
    }
    SetLastError(705);
    if (raise_native) RaiseException(TEST_EXCEPTION, 0, 0, NULL);
    return !native_failure;
}

static BOOL invoke(HWND brush)
{
    SetLastError(456);
    return wow_edit_FillWindow(brush, PAINT_WINDOW, PAINT_DC, PAINT_BRUSH);
}

HBRUSH WINAPI wow_edit_native_NtUserGetControlColor(HWND parent, HWND control,
    HDC dc, UINT message)
{
    ++color_native_calls;
    CHECK(parent == BRUSH_WINDOW && control == PAINT_WINDOW && dc == PAINT_DC);
    CHECK(message == WM_CTLCOLORLISTBOX && depth > 0 && GetLastError() == 456);
    if (color_nested && depth == 1) {
        CHECK(wow_edit_NtUserGetControlColor(parent, control, dc, message) == PAINT_BRUSH);
        CHECK(depth == 1 && GetLastError() == 705);
    }
    SetLastError(705);
    if (color_exception) RaiseException(TEST_EXCEPTION, 0, 0, NULL);
    return native_failure ? NULL : PAINT_BRUSH;
}

static HBRUSH invoke_color(void)
{
    SetLastError(456);
    /* Link the selected original ntstubs control-color object, not a local
     * replacement: its return/error shell must reach the scoped binding. */
    return GetControlColor(BRUSH_WINDOW, PAINT_WINDOW,
        PAINT_DC, WM_CTLCOLORLISTBOX);
}

static void check_reset_calls(void)
{
    BOOL caught = FALSE;
    SetLastError(456);
    wow_list_reset_double_click();
    CHECK(reset_calls == 1 && GetLastError() == 456 && begins == 0 && leaves == 0);
    reject_reset = TRUE;
    __try { wow_list_reset_double_click(); }
    __except (GetExceptionCode() == TEST_EXCEPTION ?
            EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) { caught = TRUE; }
    CHECK(caught && reset_calls == 2 && begins == 0 && leaves == 0);
    reject_reset = FALSE;
}

static void check_color_calls(void)
{
    unsigned calls, exits;
    BOOL caught = FALSE;
    CHECK(invoke_color() == PAINT_BRUSH && receiver_seen == BRUSH_WINDOW);
    CHECK(GetLastError() == 705 && depth == 0);
    calls = color_native_calls;
    exits = leaves;
    reject_begin = TRUE;
    CHECK(!invoke_color() && GetLastError() == 701);
    CHECK(color_native_calls == calls && leaves == exits);
    reject_begin = FALSE;
    native_failure = TRUE;
    CHECK(!invoke_color() && GetLastError() == 705 && depth == 0);
    native_failure = FALSE;
    reject_end = TRUE;
    CHECK(!invoke_color() && GetLastError() == 703 && depth == 0);
    reject_end = FALSE;
    calls = color_native_calls;
    exits = leaves;
    color_nested = TRUE;
    CHECK(invoke_color() == PAINT_BRUSH && GetLastError() == 705);
    CHECK(color_native_calls == calls + 2 && leaves == exits + 2 && depth == 0);
    color_nested = FALSE;
    exits = leaves;
    color_exception = TRUE;
    __try { (void)invoke_color(); }
    __except (GetExceptionCode() == TEST_EXCEPTION ?
            EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) { caught = TRUE; }
    CHECK(caught && leaves == exits + 1 && depth == 0 && GetLastError() == 705);
    color_exception = FALSE;
}

int main(void)
{
    unsigned old_begins, old_leaves, old_calls;
    BOOL caught = FALSE;
    check_reset_calls();
    SetLastError(456);
    CHECK(wow_edit_control_extent(FNID_EDIT) == sizeof(WND) + 6);
    CHECK(GetLastError() == 456);
    CHECK(wow_edit_control_extent(FNID_EDIT | FNID_CLEANEDUP_BIT) == sizeof(WND) + 6);
    __try { (void)wow_edit_control_extent(0); }
    __except (GetExceptionCode() == TEST_EXCEPTION ?
            EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) { caught = TRUE; }
    CHECK(caught);
    caught = FALSE;
    check_state_calls();
    runtime.thread = &task;
    task.ptdb = &task_entry;
    task.pClientInfo = &client;
    task.dwExpWinVer = 0x030a; /* Not the field used by original GetAppVer. */
    client.dwExpWinVer = 0x12340400;
    SetLastError(456);
    CHECK(wow_edit_client_info() == &client && GetLastError() == 456);
    CHECK(wow_edit_client_info()->dwExpWinVer == 0x12340400);
    client.dwExpWinVer = 0x5678030a;
    CHECK(wow_edit_client_info()->dwExpWinVer == 0x5678030a);
    task.TIF_flags |= WOW_TASK_TIF_16BIT;
    SetLastError(456);
    CHECK(wow_edit_client_info()->CI_flags == WOW_TASK_CI_16BIT);
    CHECK(GetLastError() == 456);
    task.TIF_flags &= ~WOW_TASK_TIF_16BIT;
    CHECK(wow_edit_client_info()->CI_flags == 0);
    CHECK(wow_edit_client_info()->dwExpWinVer == 0x5678030a);
    CHECK(invoke(BRUSH_WINDOW));
    CHECK(receiver_seen == BRUSH_WINDOW && GetLastError() == 705);
    CHECK(invoke(NULL));
    CHECK(receiver_seen == PAINT_WINDOW && GetLastError() == 705);
    CHECK(begins == 2 && leaves == 2 && native_calls == 2 && depth == 0);

    old_begins = begins;
    SetLastError(456);
    CHECK(!wow_edit_FillWindow(BRUSH_WINDOW, PAINT_WINDOW, NULL, PAINT_BRUSH));
    CHECK(GetLastError() == 456 && begins == old_begins);
    reject_begin = TRUE;
    CHECK(!invoke(BRUSH_WINDOW));
    CHECK(GetLastError() == 701 && leaves == 2 && native_calls == 2 && depth == 0);
    reject_begin = FALSE;

    native_failure = TRUE;
    CHECK(!invoke(BRUSH_WINDOW));
    CHECK(GetLastError() == 705 && depth == 0 && leaves == 3);
    native_failure = FALSE;
    reject_end = TRUE;
    CHECK(!invoke(BRUSH_WINDOW));
    CHECK(GetLastError() == 703 && depth == 0);
    reject_end = FALSE;

    old_leaves = leaves;
    old_calls = native_calls;
    nested = TRUE;
    CHECK(invoke(BRUSH_WINDOW));
    CHECK(GetLastError() == 705 && leaves == old_leaves + 2 &&
        native_calls == old_calls + 2 && depth == 0);
    nested = FALSE;

    old_leaves = leaves;
    raise_native = TRUE;
    __try { (void)invoke(BRUSH_WINDOW); }
    __except (GetExceptionCode() == TEST_EXCEPTION ?
            EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
        caught = TRUE;
    }
    CHECK(caught && leaves == old_leaves + 1 && depth == 0 && GetLastError() == 705);
    caught = FALSE;
    task.pClientInfo = NULL;
    __try { (void)wow_edit_client_info(); }
    __except (GetExceptionCode() == TEST_EXCEPTION ?
            EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
        caught = TRUE;
    }
    CHECK(caught);
    check_color_calls();
    check_text_calls();
    /* Original usercli.h CBEDITEXTRA is six, not a single pointer word. */
    CHECK(wow_edit_control_extent(FNID_EDIT) == sizeof(WND) + 6);
    printf("edit_call_failures=%d\n", failures);
    return failures != 0;
}
