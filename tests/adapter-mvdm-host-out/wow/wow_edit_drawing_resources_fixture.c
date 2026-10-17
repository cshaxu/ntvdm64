#include "wow_edit_drawing_bindings.h"
#include <stdio.h>

/* Actual resource adapter with injected GDI failures. Original geometry is
 * covered separately by wow_edit_drawing_fixture; neither is guest proof. */
static unsigned failures, bitmap_calls, brush_calls, deletes;
static int fail_color = -1;
static BOOL fail_bitmap, fail_brush;
#define CHECK(c) do { if (!(c)) { ++failures; printf("FAIL line=%d\n", __LINE__); } } while (0)
static HBRUSH WINAPI color(int index)
{ return index == fail_color ? NULL : (HBRUSH)(ULONG_PTR)(100 + index); }
static HBITMAP WINAPI bitmap(int width, int height, UINT planes, UINT bits,
    const void *data)
{
    const WORD *pattern = data;
    unsigned i;
    ++bitmap_calls;
    CHECK(width == 8 && height == 8 && planes == 1 && bits == 1);
    for (i = 0; i != 8; ++i) CHECK(pattern[i] == (i & 1 ? 0xaa : 0x55));
    return fail_bitmap ? NULL : (HBITMAP)(ULONG_PTR)1;
}
static HBRUSH WINAPI brush(HBITMAP value)
{ ++brush_calls; CHECK(value == (HBITMAP)(ULONG_PTR)1); return fail_brush ? NULL : (HBRUSH)(ULONG_PTR)2; }
static BOOL WINAPI discard(HGDIOBJ value)
{ ++deletes; CHECK(value == (HGDIOBJ)(ULONG_PTR)1); return TRUE; }

#define GetSysColorBrush color
#define CreateBitmap bitmap
#define CreatePatternBrush brush
#define DeleteObject discard
#include "wow32-dll/source/wow_edit_drawing_bindings.c"

static void unpublished(void)
{
    unsigned i;
    CHECK(!wow_edit_gray_brush);
    for (i = 0; i != WOW_EDIT_COLOR_COUNT; ++i) CHECK(!wow_edit_system_brushes[i]);
}
int main(void)
{
    unsigned i;
    fail_color = 3;
    CHECK(!wow_edit_drawing_ready() && GetLastError() == ERROR_INVALID_HANDLE);
    CHECK(!bitmap_calls && !brush_calls && !deletes); unpublished();
    fail_color = -1; fail_bitmap = TRUE;
    CHECK(!wow_edit_drawing_ready() && GetLastError() == ERROR_NOT_ENOUGH_MEMORY);
    CHECK(bitmap_calls == 1 && !brush_calls && !deletes); unpublished();
    fail_bitmap = FALSE; fail_brush = TRUE;
    CHECK(!wow_edit_drawing_ready() && GetLastError() == ERROR_NOT_ENOUGH_MEMORY);
    CHECK(bitmap_calls == 2 && brush_calls == 1 && deletes == 1); unpublished();
    fail_brush = FALSE; SetLastError(0x4473);
    CHECK(wow_edit_drawing_ready() && GetLastError() == 0x4473);
    CHECK(wow_edit_gray_brush == (HBRUSH)(ULONG_PTR)2);
    for (i = 0; i != WOW_EDIT_COLOR_COUNT; ++i)
        CHECK(wow_edit_system_brushes[i] == (HBRUSH)(ULONG_PTR)(100 + i));
    CHECK(bitmap_calls == 3 && brush_calls == 2 && deletes == 2);
    CHECK(wow_edit_drawing_ready() && GetLastError() == 0x4473);
    CHECK(bitmap_calls == 3 && brush_calls == 2 && deletes == 2);
    printf("edit-drawing-resources failures=%u\n", failures);
    return failures ? 1 : 0;
}
