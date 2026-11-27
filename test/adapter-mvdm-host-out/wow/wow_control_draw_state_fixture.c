#include "wow_button_resource_provider.h"
#include "non-mvdm/windows/core/w32inc/w32gdip.h"
#include "wow_control_draw_state.h"
#include "wow_edit_entry_bindings.h"
#include "wow_edit_native_bindings.h"
#include <stdio.h>

/* Original drawing body with real GDI; explicit context and PolyPatBlt leaf.
 * No claim of native control entry or real guest acceptance. */
__declspec(thread) wow_edit_context *wow_edit_current;
/* Explicit fixture task input; the product obtains this from its bound task. */
#include "wow_task_init_bindings.h"
const wow_task_client_info *WINAPI wow_edit_client_info(void)
{
    static const wow_task_client_info info = { WOW_TASK_TIF_16BIT, 0x030a, 0, 0 };
    return &info;
}
DWORD WINAPI wow_edit_native_GdiGetCodePage(HDC dc)
{
    typedef DWORD (WINAPI *provider)(HDC);
    provider call = (provider)GetProcAddress(GetModuleHandleW(L"gdi32.dll"), "GdiGetCodePage");
    return call ? call(dc) : 0;
}
BOOL WINAPI wow_edit_native_GdiValidateHandle(HANDLE object)
{
    typedef BOOL (WINAPI *provider)(HANDLE);
    provider call = (provider)GetProcAddress(GetModuleHandleW(L"gdi32.dll"), "GdiValidateHandle");
    return call ? call(object) : FALSE;
}
static unsigned failures, unions;
static BOOL nested, raise_callback, select_borrowed;
static HBITMAP borrowed;
#define DRAW_EXCEPTION 0xe0477116u
#define CHECK(x) do { if (!(x)) { ++failures; printf("FAIL line=%d\n", __LINE__); } } while (0)

BOOL WINAPI wow_edit_native_PolyPatBlt(HDC dc, DWORD operation,
    PPOLYPATBLT rectangles, DWORD count, DWORD mode)
{
    typedef BOOL (WINAPI *native_fn)(HDC, DWORD, PPOLYPATBLT, DWORD, DWORD);
    native_fn function = (native_fn)GetProcAddress(GetModuleHandleW(L"gdi32.dll"), "PolyPatBlt");
    CHECK(function != NULL && count == 1 && rectangles != NULL);
    if (!function || !rectangles || count != 1) return FALSE;
    CHECK(rectangles[0].cx == 7 && rectangles[0].cy == 3);
    ++unions;
    return function(dc, operation, rectangles, count, mode);
}
/* Direct aliases select the restored local bodies, not USER32 import thunks. */
#pragma comment(linker, "/alternatename:_original_tab_extent@20=_GetTabbedTextExtentW@20")
#pragma comment(linker, "/alternatename:_original_tab_out@32=_TabbedTextOutW@32")
#pragma comment(linker, "/alternatename:_original_draw_text@20=_DrawTextW@20")
DWORD WINAPI original_tab_extent(HDC, LPCWSTR, int, int, const INT *);
LONG WINAPI original_tab_out(HDC, int, int, LPCWSTR, int, int, const INT *, int);
int WINAPI original_draw_text(HDC, LPCWSTR, int, LPRECT, UINT);
#pragma comment(linker, "/alternatename:_original_draw_state_w@40=_DrawStateW@40")
#pragma comment(linker, "/alternatename:_original_draw_state_a@40=_DrawStateA@40")
BOOL WINAPI original_draw_state_w(HDC,HBRUSH,DRAWSTATEPROC,LPARAM,WPARAM,int,int,int,int,UINT);
BOOL WINAPI original_draw_state_a(HDC,HBRUSH,DRAWSTATEPROC,LPARAM,WPARAM,int,int,int,int,UINT);
int WINAPI wow_edit_native_GdiGetCharDimensions(HDC dc, TEXTMETRICW *metrics, LPINT height)
{
    typedef int (WINAPI *dimensions_proc)(HDC, TEXTMETRICW *, LPINT);
    dimensions_proc function = (dimensions_proc)GetProcAddress(GetModuleHandleW(L"gdi32.dll"), "GdiGetCharDimensions");
    CHECK(function != NULL);
    return function ? function(dc, metrics, height) : 0;
}
DWORD WINAPI wow_original_GetTabbedTextExtentA(HDC, LPCSTR, int, int, const INT *);
static void check_tab_text(HDC dc, wow_edit_context *context)
{
    typedef DWORD (WINAPI *extent_proc)(HDC, LPCWSTR, int, int, const INT *);
    typedef int (WINAPI *draw_proc)(HDC, LPCWSTR, int, LPRECT, UINT);
    extent_proc native_extent = (extent_proc)GetProcAddress(GetModuleHandleW(L"user32.dll"), "GetTabbedTextExtentW");
    draw_proc native_draw = (draw_proc)GetProcAddress(GetModuleHandleW(L"user32.dll"), "DrawTextW");
    const WCHAR text[] = L"a\tb";
    INT tabs[] = {32, 80}, height;
    RECT local = {0,0,80,40}, system = local;
    DWORD extent;
    CHECK(native_extent && native_draw);
    if (!native_extent || !native_draw) return;
    context->cxSysFontChar = wow_edit_native_GdiGetCharDimensions(dc, &context->tmSysFont, &height);
    context->cySysFontChar = height;
    extent = native_extent(dc, text, 3, 2, tabs);
    CHECK(original_tab_extent(dc, text, 3, 2, tabs) == extent);
    CHECK((DWORD)original_tab_out(dc, 0, 0, text, 3, 2, tabs, 0) == extent);
    /* XP accepts an empty run and preserves its measured line height. */
    CHECK(original_tab_extent(dc, text, 0, 0, NULL) == MAKELONG(0, height));
    CHECK(original_tab_extent(dc, text, -1, 2, tabs) == extent);
    CHECK(wow_original_GetTabbedTextExtentA(dc, "a\tb", -1, 2, tabs) == extent);
    CHECK(wow_original_GetTabbedTextExtentA(dc, "a\tb", 3, 2, tabs) == extent);
    CHECK(original_draw_text(dc, text, -2, &local, DT_CALCRECT) == 0);
    CHECK(original_draw_text(dc, text, 3, &local, DT_CALCRECT | DT_EXPANDTABS) ==
          native_draw(dc, text, 3, &system, DT_CALCRECT | DT_EXPANDTABS));
    CHECK(EqualRect(&local, &system));
}
static BOOL CALLBACK paint(HDC dc, LPARAM data, WPARAM parameter, int width, int height)
{
    wow_control_draw_context *outer = wow_control_draw_current;
    (void)parameter;
    CHECK(outer != NULL);
    if (data) CHECK(outer != (wow_control_draw_context *)data);
    if (nested) {
        nested = FALSE;
        CHECK(wow_control_draw_state(dc, NULL, paint, (LPARAM)outer, 0, 0, 0, 2, 2, DST_COMPLEX));
        CHECK(wow_control_draw_current == outer);
        CHECK(original_draw_state_w(dc, NULL, NULL, (LPARAM)L"x", 1, 0, 0, 7, 3, DST_TEXT));
        CHECK(original_draw_state_a(dc, NULL, NULL, (LPARAM)"x", 1, 0, 0, 7, 3, DST_TEXT));
    }
    if (select_borrowed) CHECK(SelectObject(dc, borrowed) != NULL);
    if (raise_callback) RaiseException(DRAW_EXCEPTION, 0, 0, NULL);
    CHECK(PatBlt(dc, 0, 0, width, height, BLACKNESS));
    return FALSE; /* Original non-MONO branch returns TRUE despite callback FALSE. */
}
static void check_icon_size(void)
{
    BYTE mask[12] = {0}, colors[24] = {0};
    HICON icon = CreateIcon(GetModuleHandleW(NULL), 2, 3, 1, 32, mask, colors);
    int width = -1, height = -1;
    CHECK(icon != NULL);
    if (!icon) return;
    CHECK(wow_control_icon_size(icon, 0, &width, &height));
    CHECK(width == 2 && height == 6);
    width = height = -1;
    CHECK(!wow_control_icon_size(icon, 1, &width, &height));
    CHECK(GetLastError() == ERROR_INVALID_PARAMETER && width == -1 && height == -1);
    CHECK(!wow_control_icon_size(icon, 0, NULL, &height));
    CHECK(GetLastError() == ERROR_INVALID_PARAMETER);
    DestroyIcon(icon);
}

BOOL WINAPI ClientDrawState(HDC, HBRUSH, DRAWSTATEPROC, LPARAM, WPARAM, int, int, int, int, UINT);
int __cdecl main(void)
{
    if (!wow_button_resources_bind(wow_original_button_resources())) return 3;
    wow_edit_context context = {0};
    HDC screen = GetDC(NULL), dc = CreateCompatibleDC(screen);
    HBITMAP bitmap = CreateCompatibleBitmap(screen, 80, 40);
    HGDIOBJ previous = SelectObject(dc, bitmap);
    POINT viewport;
    DWORD before, after;
    unsigned i;
    BOOL caught;
    BITMAP info;
    CHECK(wow_control_draw_bind(ClientDrawState));
    CHECK(screen && dc && bitmap && previous && previous != HGDI_ERROR);
    ReleaseDC(NULL, screen);
    context.screen.BitCount = 32;
    wow_edit_current = &context;
    check_tab_text(dc, &context);
    borrowed = CreateBitmap(40, 40, 1, 1, NULL);
    CHECK(borrowed != NULL);
    CHECK(SetViewportOrgEx(dc, 3, 4, NULL));
    CHECK(wow_control_draw_state(dc, NULL, paint, 0, 0, 11, 12, 7, 3, DST_COMPLEX));
    CHECK(GetViewportOrgEx(dc, &viewport) && viewport.x == 3 && viewport.y == 4);
    CHECK(!wow_control_draw_current);
    nested = TRUE;
    CHECK(wow_control_draw_state(dc, NULL, paint, 0, 0, 1, 2, 7, 3, DST_COMPLEX));
    CHECK(!nested && !wow_control_draw_current);
    /* Original MONO path preserves the callback's FALSE result. */
    CHECK(!wow_control_draw_state(dc, NULL, paint, 0, 0, 1, 2, 7, 3, DST_COMPLEX | DSS_UNION));
    CHECK(unions == 1);
    for (i = 0; i < 2; ++i) {
        raise_callback = TRUE;
        select_borrowed = i != 0;
        caught = FALSE;
        __try { (void)wow_control_draw_state(dc, NULL, paint, 0, 0, 5, 6, 17, 11,
            DST_COMPLEX | (i ? DSS_DISABLED : 0)); }
        __except (GetExceptionCode() == DRAW_EXCEPTION ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) { caught = TRUE; }
        CHECK(caught && !wow_control_draw_current);
        CHECK(GetViewportOrgEx(dc, &viewport) && viewport.x == 3 && viewport.y == 4);
        CHECK(GetObjectW(borrowed, sizeof(info), &info) == sizeof(info));
    }
    raise_callback = select_borrowed = FALSE;
    /* Warm existing process-lifetime brushes before measuring scoped handles. */
    before = GetGuiResources(GetCurrentProcess(), GR_GDIOBJECTS);
    for (i = 0; i < 20; ++i)
        CHECK(!wow_control_draw_state(dc, NULL, paint, 0, 0, 1, 2, 31, 13,
            DST_COMPLEX | DSS_DISABLED));
    GdiFlush();
    after = GetGuiResources(GetCurrentProcess(), GR_GDIOBJECTS);
    CHECK(before == after && !wow_control_draw_current);
    CHECK(wow_control_draw_state(dc, NULL, NULL, (LPARAM)L"A&B", 3, 0, 0, 20, 20, DST_PREFIXTEXT));
    CHECK(!wow_control_draw_state(dc, NULL, NULL, 0, 0, 0, 0, 1, 1, 5));
    CHECK(GetLastError() == ERROR_INVALID_PARAMETER);
    wow_edit_current = NULL;
    CHECK(!wow_control_draw_state(dc, NULL, paint, 0, 0, 0, 0, 1, 1, DST_COMPLEX));
    CHECK(GetLastError() == ERROR_INVALID_STATE && !wow_control_draw_current);
    SelectObject(dc, previous);
    DeleteDC(dc);
    DeleteObject(bitmap);
    DeleteObject(borrowed);
    check_icon_size();
    printf("control_draw_state_failures=%u\n", failures);
    return failures ? 1 : 0;
}
