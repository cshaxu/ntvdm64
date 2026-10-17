#include "wow_control_draw_state.h"
#include "wow_edit_entry_bindings.h"
#include "wow_edit_native_bindings.h"
#include <stdio.h>

/* Original drawing body with real GDI; explicit context and PolyPatBlt leaf.
 * No claim of native control entry or real guest acceptance. */
__declspec(thread) wow_edit_context *wow_edit_current;
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
    }
    if (select_borrowed) CHECK(SelectObject(dc, borrowed) != NULL);
    if (raise_callback) RaiseException(DRAW_EXCEPTION, 0, 0, NULL);
    CHECK(PatBlt(dc, 0, 0, width, height, BLACKNESS));
    return FALSE; /* Original non-MONO branch returns TRUE despite callback FALSE. */
}
int __cdecl main(void)
{
    wow_edit_context context = {0};
    HDC screen = GetDC(NULL), dc = CreateCompatibleDC(screen);
    HBITMAP bitmap = CreateCompatibleBitmap(screen, 80, 40);
    HGDIOBJ previous = SelectObject(dc, bitmap);
    POINT viewport;
    DWORD before, after;
    unsigned i;
    BOOL caught;
    BITMAP info;
    CHECK(screen && dc && bitmap && previous && previous != HGDI_ERROR);
    ReleaseDC(NULL, screen);
    context.screen.BitCount = 32;
    wow_edit_current = &context;
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
    printf("control_draw_state_failures=%u\n", failures);
    return failures ? 1 : 0;
}
