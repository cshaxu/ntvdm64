#include "wow_user_client_view_layout.h"
#define WOW_ORIGINAL_CONTROL_EXTENT_LAYOUT
#include "opennt-host/windows/core/ntuser/inc/user.h"
#undef WOW_ORIGINAL_CONTROL_EXTENT_LAYOUT
#include <stdio.h>

HDC WINAPI xxxBNGetDC(PBUTN, HBRUSH *);
VOID WINAPI BNReleaseDC(PBUTN, HDC, HWND);
static WND state;
static BUTN button;
static unsigned failures, acquisitions, releases, colors;
static BOOL destroy_in_color;
static HWND expected_window;
#define CHECK(x) do { if (!(x)) { ++failures; printf("FAIL line=%d\n", __LINE__); } } while (0)
HDC WINAPI fixture_button_get_dc(HWND window)
{
    CHECK(window == expected_window);
    ++acquisitions;
    return GetDC(window);
}
int WINAPI fixture_button_release_dc(HWND window, HDC dc)
{
    CHECK(window == expected_window && dc != NULL);
    ++releases;
    return ReleaseDC(window, dc);
}
HBRUSH WINAPI GetControlBrush(HWND window, HDC dc, UINT message)
{
    CHECK(window == expected_window && dc != NULL && message == WM_CTLCOLORBTN);
    ++colors;
    if (destroy_in_color) button.spwnd = NULL;
    return GetSysColorBrush(COLOR_BTNFACE);
}
/* Exact original predicate's NULL outcome; this fixture isolates the DC
 * callout, not graph traversal or native window destruction enrollment. */
BOOL WINAPI IsVisible(PWND window)
{
    CHECK(window == &state);
    return TRUE;
}
int __cdecl main(void)
{
    HDC dc;
    HBRUSH brush = (HBRUSH)(ULONG_PTR)1;
    expected_window = CreateWindowExW(0, L"STATIC", L"button-dc", WS_POPUP,
        0, 0, 32, 32, NULL, NULL, GetModuleHandleW(NULL), NULL);
    CHECK(expected_window != NULL);
    if (!expected_window) return 1;
    state.head.h = expected_window;
    state.style = WS_VISIBLE | BS_PUSHBUTTON;
    state.rcClient.right = state.rcClient.bottom = 32;
    button.spwnd = &state;
    dc = xxxBNGetDC(&button, &brush);
    CHECK(dc != NULL && brush == GetSysColorBrush(COLOR_BTNFACE));
    /* Logical teardown during the drawing caller must not lose DC owner. */
    button.spwnd = NULL;
    BNReleaseDC(&button, dc, expected_window);
    CHECK(acquisitions == 1 && releases == 1 && colors == 1);
    CHECK(xxxBNGetDC(&button, NULL) == NULL && acquisitions == 1);
    button.spwnd = &state;
    destroy_in_color = TRUE;
    brush = (HBRUSH)(ULONG_PTR)1;
    CHECK(xxxBNGetDC(&button, &brush) == NULL);
    CHECK(brush == (HBRUSH)(ULONG_PTR)1);
    CHECK(acquisitions == 2 && releases == 2 && colors == 2);
    CHECK(button.spwnd == NULL);
    CHECK(DestroyWindow(expected_window));
    printf("button_dc_failures=%u\n", failures);
    return failures ? 1 : 0;
}
