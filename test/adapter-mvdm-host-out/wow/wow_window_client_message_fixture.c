#include "wow_window_client_state_bindings.h"
#include "wow_user_client_view_layout.h"
#include "wow_user_borrow_scope.h"
#include "wow_window_access_provider.h"
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
static HMENU WINAPI unexpected_menu(HINSTANCE module, LPCWSTR name)
{ (void)module; (void)name; ++failures; return NULL; }
static HWND WINAPI fixture_window_handle(struct tagWND *window)
{ return window->head.h; }
static DWORD *WINAPI unexpected_words(struct _WW *words)
{ (void)words; ++failures; return NULL; }
static VOID WINAPI unexpected_restore_words(struct _WW *words,
    const struct mvdm_softpc_wow_window_words_view *view)
{ (void)words; (void)view; ++failures; }
static const wow_window_layout *WINAPI unexpected_layout(void)
{ ++failures; return NULL;}

static VOID WINAPI unexpected_capture_words(const struct _WW * a0, struct mvdm_softpc_wow_window_words_view * a1)
{(void)a0; (void)a1; ++failures;}

static VOID WINAPI unexpected_initialize_state(struct tagWND * a0, const wow_window_state_initialization * a1)
{(void)a0; (void)a1; ++failures;}

static SIZE_T WINAPI unexpected_state_extent(const struct tagWND * a0)
{(void)a0; ++failures; return 0;}

static WCHAR *WINAPI unexpected_promote_state(struct tagWND * a0, struct tagWND * a1, struct tagCLS * a2)
{(void)a0; (void)a1; (void)a2; ++failures; return NULL;}

static VOID WINAPI unexpected_initialize_resource(struct tagWND * a0, struct tagCLS * a1, const struct mvdm_softpc_wow_window_observation * a2, HWND a3)
{(void)a0; (void)a1; (void)a2; (void)a3; ++failures;}

static WCHAR *WINAPI unexpected_refresh_state(struct tagWND * a0, struct _WW * a1, const WINDOWINFO * a2, DWORD a3, DWORD a4, WCHAR * a5, ULONG a6, ULONG a7)
{(void)a0; (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; (void)a6; (void)a7; ++failures; return NULL;}

static VOID WINAPI unexpected_publish_links(struct tagWND * a0, struct tagCLS * a1, ATOM a2, struct tagWND * a3, struct tagWND * a4, struct tagWND * a5, struct tagWND * a6)
{(void)a0; (void)a1; (void)a2; (void)a3; (void)a4; (void)a5; (void)a6; ++failures;}

static WCHAR *WINAPI unexpected_state_name(struct tagWND * a0)
{(void)a0; ++failures; return NULL;}

static WORD WINAPI unexpected_state_fnid(struct tagWND * a0)
{(void)a0; ++failures; return 0;}

static WORD WINAPI unexpected_mark_destroyed(struct tagWND * a0)
{(void)a0; ++failures; return 0;}

static BOOL WINAPI unexpected_creation_class_name(const struct _LARGE_STRING * a0, struct _UNICODE_STRING * a1)
{(void)a0; (void)a1; ++failures; return 0;}

static const wow_window_access_provider access_provider = {
    xxxSetWindowLong, _GetWindowLong, _SetWindowLong, unexpected_menu,
    fixture_window_handle, FNID_DEFWINDOWPROC, unexpected_words, unexpected_restore_words,
    unexpected_layout, unexpected_capture_words, unexpected_initialize_state, unexpected_state_extent, unexpected_promote_state, unexpected_initialize_resource, unexpected_refresh_state, unexpected_publish_links, unexpected_state_name, unexpected_state_fnid, unexpected_mark_destroyed, unexpected_creation_class_name
};
int __cdecl main(void)
{
    unsigned i;
    LONG output;
    CHECK(!wow_window_access_bind(NULL));
    CHECK(GetLastError() == ERROR_INVALID_PARAMETER);
    CHECK(wow_window_access_bind(&access_provider));
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
