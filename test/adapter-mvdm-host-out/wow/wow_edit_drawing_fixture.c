#include "edit-drawing/precomp.h"
#include <stdio.h>

/* Supplemental original geometry/dispatch witness, not raster/guest proof.
 * Original draw.c uses /Gz, as does the original EDIT provider. */
HBRUSH ahbrSystem[COLOR_MAX], ghbrGray;
int drawing_fixture_CXBORDER = 2, drawing_fixture_CYBORDER = 3;
static POLYPATBLT captured[4];
static DWORD operation;
static unsigned calls, failures;
#define CHECK(x) do { if (!(x)) { ++failures; printf("FAIL line %d\n", __LINE__); } } while (0)

BOOL WINAPI drawing_fixture_poly(HDC dc, DWORD rop, PPOLYPATBLT data,
    DWORD count, DWORD mode)
{
    CHECK(dc == (HDC)(ULONG_PTR)7 && count == 4 && mode == PPB_BRUSH);
    if (count == 4) CopyMemory(captured, data, sizeof(captured));
    operation = rop; ++calls;
    return FALSE; /* Original DrawFrame still returns TRUE; preserve it. */
}

static void rectangle(unsigned index, int x, int y, int cx, int cy, HBRUSH brush)
{
    CHECK(captured[index].x == x && captured[index].y == y);
    CHECK(captured[index].cx == cx && captured[index].cy == cy);
    CHECK(captured[index].BrClr.hbr == brush);
}

#ifdef WOW_ORIGINAL_BUTTON_DRAWING
struct drawing_display drawing_display = {32};
COLORREF drawing_colors[COLOR_MAX];
static HBRUSH filled;
static UINT edge_kind, edge_flags;
static COLORREF background = 0x1234, foreground = 0x5678;
static unsigned color_changes;
BOOL WINAPI drawing_edge(HDC dc, LPRECT rect, UINT edge, UINT flags)
{
    (void)dc;
    edge_kind = edge; edge_flags = flags;
    InflateRect(rect, -2, -2);
    return TRUE;
}
int WINAPI drawing_fill(HDC dc, CONST RECT *rect, HBRUSH brush)
{
    (void)dc; (void)rect; filled = brush; return 1;
}
COLORREF WINAPI drawing_back(HDC dc, COLORREF value)
{
    COLORREF old = background; (void)dc;
    background = value; ++color_changes; return old;
}
COLORREF WINAPI drawing_fore(HDC dc, COLORREF value)
{
    COLORREF old = foreground; (void)dc;
    foreground = value; ++color_changes; return old;
}
static void push_button_contract(void)
{
    RECT rect = {0, 0, 100, 40};
    ahbrSystem[COLOR_3DFACE] = (HBRUSH)(ULONG_PTR)20;
    ahbrSystem[COLOR_3DHILIGHT] = (HBRUSH)(ULONG_PTR)21;
    drawing_colors[COLOR_3DHILIGHT] = RGB(230,230,230);
    DrawPushButton(NULL, &rect, 0, BF_SOFT);
    CHECK(edge_kind == EDGE_RAISED && (edge_flags & BF_SOFT));
    CHECK(filled == ahbrSystem[COLOR_3DFACE] && rect.left == 0);
    DrawPushButton(NULL, &rect, DFCS_CHECKED, BF_ADJUST);
    CHECK(edge_kind == EDGE_SUNKEN && rect.left == 2 && rect.right == 98);
    CHECK(filled == ahbrSystem[COLOR_3DHILIGHT] && !color_changes);
    drawing_display.BitCount = 4;
    DrawPushButton(NULL, &rect, DFCS_CHECKED, 0);
    CHECK(filled == ghbrGray && color_changes == 4);
    CHECK(background == 0x1234 && foreground == 0x5678);
    drawing_display.BitCount = 32;
    drawing_colors[COLOR_3DHILIGHT] = RGB(255,255,255);
    DrawPushButton(NULL, &rect, DFCS_CHECKED, 0);
    CHECK(filled == ghbrGray && color_changes == 8);
    CHECK(background == 0x1234 && foreground == 0x5678);
    DrawPushButton(NULL, &rect, DFCS_PUSHED, 0);
    CHECK(edge_kind == EDGE_SUNKEN && filled == ahbrSystem[COLOR_3DFACE]);
}
#endif
int __cdecl main(void)
{
    RECT input = {10, 20, 110, 90}, output = {0};
    HBRUSH frame = (HBRUSH)(ULONG_PTR)9;
    ahbrSystem[COLOR_WINDOWFRAME] = frame;
    ghbrGray = (HBRUSH)(ULONG_PTR)11;
    CHECK(CopyInflateRect(&output, &input, 2, 3));
    CHECK(output.left == 8 && output.top == 17 && output.right == 112 && output.bottom == 93);
    CHECK(CopyInflateRect(&output, &output, -2, -3));
    CHECK(EqualRect(&output, &input));
    CHECK(DrawFrame((HDC)(ULONG_PTR)7, &input, 2, DF_WINDOWFRAME));
    CHECK(calls == 1 && operation == PATCOPY);
    rectangle(0, 10, 20, 4, 64, frame);
    rectangle(1, 14, 20, 96, 6, frame);
    rectangle(2, 10, 84, 96, 6, frame);
    rectangle(3, 106, 26, 4, 64, frame);
    CHECK(DrawFrame((HDC)(ULONG_PTR)7, &input, 1, DF_GRAY | DF_PATINVERT));
    CHECK(calls == 2 && operation == PATINVERT);
    rectangle(0, 10, 20, 2, 67, ghbrGray);
    rectangle(1, 12, 20, 98, 3, ghbrGray);
    rectangle(2, 10, 87, 98, 3, ghbrGray);
    rectangle(3, 108, 23, 2, 67, ghbrGray);
    CHECK(EqualRect(&input, &output));
#ifdef WOW_ORIGINAL_BUTTON_DRAWING
    push_button_contract();
#endif
    printf("original_edit_drawing_failures=%u\n", failures);
    return failures != 0;
}
