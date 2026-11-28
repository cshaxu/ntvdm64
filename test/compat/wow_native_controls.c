#include "compat/include/wow_native_controls.h"
#include "non-mvdm/windows/core/w32inc/w32gdip.h"
#include <stdio.h>

static unsigned failures;
#define CHECK(c) do { if (!(c)) { ++failures; printf("FAIL line=%d error=%lu\n", __LINE__, GetLastError()); } } while (0)

int main(void)
{
    typedef UINT (WINAPI *font_assoc_fn)(void);
    font_assoc_fn font_assoc = (font_assoc_fn)GetProcAddress(GetModuleHandleW(L"gdi32.dll"), "QueryFontAssocStatus");
    HDC dc = CreateCompatibleDC(NULL);
    HBITMAP bitmap = CreateBitmap(8, 8, 1, 1, NULL);
    HGDIOBJ previous;
    TEXTMETRICW metrics;
    CHWIDTHINFO widths;
    POLYPATBLT rectangle = {0};
    int height = 0, width;
    HWND window;
    WCHAR result[32];
    const WCHAR title[] = L"native controls";
    wow_native_text_descriptor text = {sizeof(title) - sizeof(WCHAR),
        sizeof(title), (ULONG)(ULONG_PTR)title, 0};
    CHECK(dc != NULL && bitmap != NULL);
    SetLastError(0x1234);
    CHECK(wow_edit_native_ready() && GetLastError() == 0x1234);
    CHECK(wow_edit_native_ready() && GetLastError() == 0x1234);
    CHECK(font_assoc != NULL);
    if (font_assoc) CHECK(wow_edit_native_QueryFontAssocStatus() == font_assoc());
    if (dc && bitmap) {
        previous = SelectObject(dc, bitmap);
        width = wow_edit_native_GdiGetCharDimensions(dc, &metrics, &height);
        CHECK(width > 0 && height == metrics.tmHeight);
        CHECK(wow_edit_native_GdiGetCharDimensions(dc, NULL, &height) == width);
        CHECK(wow_edit_native_GetCharWidthInfo(dc, &widths));
        ZeroMemory(result, sizeof(result));
        CHECK(wow_edit_native_GetTextFaceAliasW(dc, ARRAYSIZE(result), result) > 0);
        CHECK(result[0] != L'\0');
        rectangle.cx = rectangle.cy = 8;
        rectangle.BrClr.hbr = GetStockObject(WHITE_BRUSH);
        CHECK(wow_edit_native_PolyPatBlt(dc, PATCOPY, &rectangle, 1, PPB_BRUSH));
        CHECK(GetPixel(dc, 0, 0) == RGB(255,255,255));
        SelectObject(dc, previous);
    }
    if (bitmap) DeleteObject(bitmap);
    if (dc) DeleteDC(dc);
    window = CreateWindowExW(0, L"STATIC", L"before", WS_POPUP,
        0, 0, 8, 8, NULL, NULL, GetModuleHandleW(NULL), NULL);
    CHECK(window != NULL);
    if (window) {
        CHECK(wow_native_set_window_text(window, &text));
        CHECK(GetWindowTextW(window, result, 32) == 15 && !lstrcmpW(result, title));
        CHECK(wow_native_set_window_text(window, NULL));
        CHECK(GetWindowTextLengthW(window) == 0);
        CHECK(DestroyWindow(window));
    }
    printf("native-controls failures=%u\n", failures);
    return failures ? 1 : 0;
}
