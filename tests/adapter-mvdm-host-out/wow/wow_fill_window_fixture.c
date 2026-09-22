#include <windows.h>
#include <stdio.h>
#include "wow_private_user_compat.h"

static unsigned color_calls;
static HBRUSH reply;
static POINT callback_origin;
static BOOL destroy_on_color;

static LRESULT CALLBACK fixture_window(HWND window, UINT message,
    WPARAM wparam, LPARAM lparam)
{
    if (message == WM_CTLCOLORSTATIC) {
        ++color_calls;
        GetBrushOrgEx((HDC)wparam, &callback_origin);
        if (destroy_on_color) DestroyWindow(window);
        return (LRESULT)reply;
    }
    return DefWindowProcA(window, message, wparam, lparam);
}

int __cdecl main(void)
{
    WNDCLASSA klass = {0};
    HWND parent = NULL, child = NULL;
    HDC dc = NULL;
    HBITMAP bitmap = NULL, previous = NULL;
    HBRUSH red = NULL;
    HRGN empty = NULL;
    POINT origin;
    RECT whole = {0, 0, 64, 64};
    int passed = 0;

    klass.hInstance = GetModuleHandleA(NULL);
    klass.lpfnWndProc = fixture_window;
    klass.lpszClassName = "S40FillParent";
    if (!RegisterClassA(&klass)) goto done;
    klass.style = CS_PARENTDC;
    klass.lpszClassName = "S40FillChild";
    if (!RegisterClassA(&klass)) goto done;
    parent = CreateWindowA("S40FillParent", "", WS_POPUP,
        100, 100, 100, 100, NULL, NULL, klass.hInstance, NULL);
    child = CreateWindowA("S40FillChild", "", WS_CHILD,
        10, 12, 20, 24, parent, NULL, klass.hInstance, NULL);
    dc = CreateCompatibleDC(NULL);
    bitmap = CreateBitmap(64, 64, 1, 32, NULL);
    red = CreateSolidBrush(RGB(255, 0, 0));
    empty = CreateRectRgn(0, 0, 0, 0);
    if (!parent || !child || !dc || !bitmap || !red || !empty) goto done;
#ifdef WOW_TEST_SHORT_HWND
    parent = (HWND)(LONG)(SHORT)LOWORD(parent);
    child = (HWND)(LONG)(SHORT)LOWORD(child);
#endif
    previous = (HBITMAP)SelectObject(dc, bitmap);
    FillRect(dc, &whole, (HBRUSH)GetStockObject(BLACK_BRUSH));
    SetBrushOrgEx(dc, 3, 7, NULL);
    reply = red;
    if (!wow_private_user_fill_window(parent, child, dc,
            (HBRUSH)CTLCOLOR_STATIC) || color_calls != 1 ||
        callback_origin.x != -10 || callback_origin.y != -12 ||
        GetPixel(dc, 19, 23) != RGB(255, 0, 0) ||
        GetPixel(dc, 20, 23) != RGB(0, 0, 0) ||
        GetPixel(dc, 19, 24) != RGB(0, 0, 0)) goto done;
    if (!GetBrushOrgEx(dc, &origin) || origin.x != 3 || origin.y != 7)
        goto done;
    reply = (HBRUSH)(ULONG_PTR)0x12345678;
    if (!wow_private_user_fill_window(NULL, child, dc,
            (HBRUSH)CTLCOLOR_STATIC) || color_calls != 2 ||
        callback_origin.x != 0 || callback_origin.y != 0) goto done;
    /* Original GreGetClipBox(TRUE) returns logical coordinates. Preserve
     * that ordering even with a non-unit viewport; do not add a guessed
     * device-to-logical conversion to the original client intersection. */
    FillRect(dc, &whole, (HBRUSH)GetStockObject(BLACK_BRUSH));
    SetMapMode(dc, MM_ANISOTROPIC);
    SetWindowExtEx(dc, 32, 32, NULL);
    SetViewportExtEx(dc, 64, 64, NULL);
    reply = red;
    if (!wow_private_user_fill_window(parent, child, dc,
            (HBRUSH)CTLCOLOR_STATIC) || color_calls != 3) goto done;
    SetMapMode(dc, MM_TEXT);
    if (GetPixel(dc, 39, 47) != RGB(255, 0, 0) ||
        GetPixel(dc, 40, 47) != RGB(0, 0, 0) ||
        GetPixel(dc, 39, 48) != RGB(0, 0, 0) ||
        !GetBrushOrgEx(dc, &origin) || origin.x != 3 || origin.y != 7)
        goto done;
    puts("WOW_FILL_WINDOW_NONUNIT_DC_OK");
    SelectClipRgn(dc, empty);
    if (!wow_private_user_fill_window(parent, child, dc,
            (HBRUSH)CTLCOLOR_STATIC) || color_calls != 3) goto done;
    if (wow_private_user_fill_window(parent, child, NULL, red)) goto done;
    SelectClipRgn(dc, NULL);
    destroy_on_color = TRUE;
    if (!wow_private_user_fill_window(NULL, child, dc,
            (HBRUSH)CTLCOLOR_STATIC) || color_calls != 4 || IsWindow(child) ||
        !GetBrushOrgEx(dc, &origin) || origin.x != 3 || origin.y != 7)
        goto done;
    child = NULL;
    puts("WOW_FILL_WINDOW_CALLBACK_DESTROY_OK");
    passed = 1;
done:
    if (!passed) printf("WOW_FILL_WINDOW_ERROR=%lu callbacks=%u\n", GetLastError(), color_calls);
    if (previous) SelectObject(dc, previous);
    if (bitmap) DeleteObject(bitmap);
    if (red) DeleteObject(red);
    if (empty) DeleteObject(empty);
    if (dc) DeleteDC(dc);
    if (child) DestroyWindow(child);
    if (parent) DestroyWindow(parent);
    UnregisterClassA("S40FillChild", klass.hInstance);
    UnregisterClassA("S40FillParent", klass.hInstance);
    puts(passed ? "WOW_FILL_WINDOW_BOUNDARY_OK" : "WOW_FILL_WINDOW_FAILED");
    return passed ? 0 : 1;
}
