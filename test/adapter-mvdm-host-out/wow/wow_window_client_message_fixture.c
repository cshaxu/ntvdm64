#include "wow_window_client_state_bindings.h"
#include "wow_user_client_view_layout.h"
#include "wow_user_borrow_scope.h"
#include <stdio.h>

/* Production access binding with transport doubles: proves argument mapping,
 * not native dispatch, original editor behavior or guest acceptance. */
static unsigned failures, calls;
static BOOL expected_ansi;
static BOOL reject_borrow;
static unsigned borrows;
static WND state;
static BOOL valid_state, owns_state, has_binding;
static unsigned original_gets, original_sets, owner_releases;
DWORD WINAPI xxxSetWindowData(PWND, int, DWORD, BOOL);
static unsigned style_calls;
LONG WINAPI wow_native_SetWindowLongA(HWND window, int index, LONG value)
{
    if (window != state.head.h || index != GWL_STYLE || value != WS_VISIBLE ||
            GetLastError() != 0x3456) ++failures;
    ++style_calls;
    state.head.h = NULL; /* Native reentry retires the retained projection. */
    SetLastError(0x789a);
    return (LONG)0xfedcba98;
}
#define CHECK(c) do { if (!(c)) { ++failures; printf("FAIL line=%d\n", __LINE__); } } while (0)
wow_window_words_binding *WINAPI wow_user_borrow_client_state(HWND window, PWND view)
{
    CHECK(view == &state && window == state.head.h);
    CHECK(GetLastError() == 0x3456);
    ++borrows;
    if (reject_borrow) {
        SetLastError(ERROR_INVALID_STATE);
        return NULL;
    }
    return (wow_window_words_binding *)(ULONG_PTR)1; /* Opaque boundary double. */
}
static LRESULT transport(HWND window, UINT message, WPARAM wp, LPARAM lp,
    BOOL ansi)
{
    CHECK(window == state.head.h && ansi == expected_ansi);
    CHECK(message == WM_CTLCOLOREDIT && wp == 0x1234 && lp == 0x5678);
    CHECK(GetLastError() == 0x3456);
    ++calls;
    CHECK(borrows == calls);
    state.head.h = NULL; /* Simulated reentrant mutation of retained state. */
    SetLastError(0x789a);
    return (LRESULT)0xfedcba98;
}
LRESULT WINAPI wow_native_SendMessageA(HWND w, UINT m, WPARAM p, LPARAM l)
{ return transport(w, m, p, l, TRUE); }
LRESULT WINAPI wow_native_SendMessageW(HWND w, UINT m, WPARAM p, LPARAM l)
{ return transport(w, m, p, l, FALSE); }
PWND WINAPI wow_window_client_validate_hwnd(HWND window)
{ CHECK(window == state.head.h); if (valid_state) return &state;
  SetLastError(ERROR_NOT_ENOUGH_MEMORY); return NULL; }
wow_window_words_binding *WINAPI wow_window_words_acquire(HWND window)
{ CHECK(window == state.head.h); SetLastError(ERROR_NOT_FOUND);
  return has_binding ? (wow_window_words_binding *)(ULONG_PTR)1 : NULL; }
PWND WINAPI wow_window_words_client_state_value(wow_window_words_binding *binding)
{ CHECK(binding == (wow_window_words_binding *)(ULONG_PTR)1); return &state; }
BOOL WINAPI wow_window_words_owns_client_state(wow_window_words_binding *binding, PWND view)
{ CHECK(binding == (wow_window_words_binding *)(ULONG_PTR)1 && view == &state); return owns_state; }
void WINAPI wow_window_words_release(wow_window_words_binding *binding)
{ CHECK(binding == (wow_window_words_binding *)(ULONG_PTR)1); ++owner_releases; SetLastError(0x9999); }
DWORD WINAPI _GetWindowLong(PWND view, int index, BOOL ansi)
{ CHECK(view == &state && index == 12 && ansi); ++original_gets;
  SetLastError(ERROR_INVALID_INDEX); return 0; }
LONG WINAPI _SetWindowLong(HWND window, int index, LONG value, BOOL ansi)
{ CHECK(window == state.head.h && index == 12 && value == 17 && ansi); ++original_sets;
  SetLastError(ERROR_INVALID_INDEX); return 0; }
DWORD WINAPI xxxSetWindowLong(PWND window, int index, DWORD value, BOOL ansi)
{ (void)window; (void)index; (void)value; (void)ansi; ++failures; return 0; }
int __cdecl main(void)
{
    unsigned i;
    LONG output;
    for (i = 0; i < 2; ++i) {
        expected_ansi = i != 0;
        state.head.h = (HWND)(ULONG_PTR)0x13579;
        SetLastError(0x3456);
        CHECK((DWORD)wow_window_client_send_message(&state, WM_CTLCOLOREDIT,
            0x1234, 0x5678, expected_ansi) == 0xfedcba98);
        CHECK(GetLastError() == 0x789a && state.head.h == NULL);
    }
    CHECK(wow_window_client_send_message(NULL, 0, 0, 0, FALSE) == 0);
    CHECK(GetLastError() == ERROR_INVALID_WINDOW_HANDLE && calls == 2);
    reject_borrow = TRUE;
    state.head.h = (HWND)(ULONG_PTR)0x13579;
    SetLastError(0x3456);
    CHECK(wow_window_client_send_message(&state, WM_CTLCOLOREDIT,
        0x1234, 0x5678, FALSE) == 0);
    CHECK(GetLastError() == ERROR_INVALID_STATE && calls == 2 && borrows == 3);
    has_binding = owns_state = TRUE;
    output = 99; SetLastError(0x3456);
    CHECK(wow_window_client_try_get_long(state.head.h, 12, TRUE, &output));
    CHECK(!output && GetLastError() == ERROR_NOT_ENOUGH_MEMORY && owner_releases == 1);
    output = 99; SetLastError(0x3456);
    CHECK(wow_window_client_try_set_long(state.head.h, 12, 17, TRUE, &output));
    CHECK(!output && GetLastError() == ERROR_NOT_ENOUGH_MEMORY && owner_releases == 2);
    CHECK(!original_gets && !original_sets);
    for (i = 0; i < 2; ++i) {
        has_binding = i != 0; owns_state = FALSE; output = 99;
        SetLastError(0x3456);
        CHECK(!wow_window_client_try_get_long(state.head.h, 12, TRUE, &output));
        CHECK(output == 99 && GetLastError() == 0x3456);
        CHECK(!wow_window_client_try_set_long(state.head.h, 12, 17, TRUE, &output));
        CHECK(output == 99 && GetLastError() == 0x3456);
    }
    valid_state = TRUE; output = 99;
    CHECK(wow_window_client_try_get_long(state.head.h, 12, TRUE, &output));
    CHECK(!output && GetLastError() == ERROR_INVALID_INDEX && original_gets == 1);
    output = 99;
    CHECK(wow_window_client_try_set_long(state.head.h, 12, 17, TRUE, &output));
    CHECK(!output && GetLastError() == ERROR_INVALID_INDEX && original_sets == 1);
    reject_borrow = FALSE;
    for (i = 0; i < 2; ++i) {
        state.head.h = (HWND)(ULONG_PTR)0x13579;
        SetLastError(0x3456);
        CHECK(xxxSetWindowData(&state, GWL_STYLE, WS_VISIBLE, i != 0) == 0xfedcba98);
        CHECK(GetLastError() == 0x789a && state.head.h == NULL);
    }
    CHECK(xxxSetWindowData(NULL, GWL_STYLE, 0, TRUE) == 0);
    CHECK(GetLastError() == ERROR_INVALID_WINDOW_HANDLE);
    CHECK(xxxSetWindowData(&state, GWL_EXSTYLE, 0, TRUE) == 0);
    CHECK(GetLastError() == ERROR_INVALID_INDEX && style_calls == 2);
    state.head.h = (HWND)(ULONG_PTR)0x13579;
    reject_borrow = TRUE;
    SetLastError(0x3456);
    CHECK(xxxSetWindowData(&state, GWL_STYLE, WS_VISIBLE, TRUE) == 0);
    CHECK(GetLastError() == ERROR_INVALID_STATE && style_calls == 2);
    printf("WOW_CONTROL_MESSAGE_BINDING failures=%u\n", failures);
    return failures ? 3 : 0;
}
