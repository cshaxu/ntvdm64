#include "mnemonic-drawing/precomp.h"
#include <stdio.h>
#include <string.h>
#include <wchar.h>

/* Real prefix/output algorithm, instrumented resource/drawing leaves. */
static unsigned failures, allocations, frees, outputs, underlines;
static BOOL fail_allocate;
static COLORREF background = 0x123456;
static WCHAR output[1025];
static RECT underline;
#define CHECK(x) do { if (!(x)) { ++failures; printf("FAIL line=%d\n", __LINE__); } } while (0)
PVOID WINAPI mnemonic_allocate(SIZE_T bytes)
{
    BYTE *block;
    ++allocations;
    if (fail_allocate) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return NULL; }
    block = HeapAlloc(GetProcessHeap(), 0, sizeof(SIZE_T) + bytes + 4);
    if (!block) return NULL;
    *(SIZE_T *)block = bytes;
    memset(block + sizeof(SIZE_T) + bytes, 0x5a, 4);
    return block + sizeof(SIZE_T);
}
VOID WINAPI mnemonic_free(PVOID pointer)
{
    BYTE *block = (BYTE *)pointer - sizeof(SIZE_T);
    SIZE_T bytes = *(SIZE_T *)block;
    unsigned i;
    for (i = 0; i < 4; ++i) CHECK(((BYTE *)pointer)[bytes+i] == 0x5a);
    CHECK(HeapFree(GetProcessHeap(), 0, block));
    ++frees;
}
BOOL WINAPI mnemonic_text(HDC dc, int x, int y, LPCWSTR text, int count)
{
    WCHAR saved[1025];
    (void)dc; (void)x; (void)y;
    CHECK(count >= 0 && count <= 1024);
    if (count < 0 || count > 1024) return FALSE;
    ++outputs;
    CopyMemory(saved, text, count * sizeof(WCHAR));
    saved[count] = 0;
    CopyMemory(output, saved, (count+1) * sizeof(WCHAR));
    SetLastError(0x47115);
    return TRUE;
}
BOOL WINAPI mnemonic_metrics(HDC dc, LPTEXTMETRICW metrics)
{
    (void)dc;
    ZeroMemory(metrics, sizeof(*metrics));
    metrics->tmAscent = 8;
    metrics->tmOverhang = 2;
    return TRUE;
}
BOOL WINAPI mnemonic_extent(HDC dc, LPCWSTR text, int count, LPSIZE size)
{
    (void)dc; (void)text;
    size->cx = count * 4; size->cy = 10;
    return TRUE;
}
COLORREF WINAPI mnemonic_background(HDC dc, COLORREF value)
{
    COLORREF old = background;
    (void)dc;
    background = value;
    return old;
}
COLORREF WINAPI mnemonic_foreground(HDC dc) { (void)dc; return 0xabcdef; }
BOOL WINAPI mnemonic_underline(HDC dc, int x, int y, UINT options,
    CONST RECT *rect, LPCWSTR text, UINT count, CONST INT *spacing)
{
    (void)dc; (void)text; (void)spacing;
    CHECK(options == ETO_OPAQUE && !count && background == 0xabcdef);
    CHECK(x == rect->left && y == rect->top);
    underline = *rect;
    ++underlines;
    SetLastError(0x47115);
    return TRUE;
}
int __cdecl main(void)
{
    WCHAR long_text[1025];
    unsigned i, before;
    for (i = 0; i < 1024; ++i) long_text[i] = L'Q';
    long_text[1024] = 0;
    PSMTextOut(NULL, 10, 20, L"A&B", 3, 0);
    CHECK(!wcscmp(output, L"AB") && underlines == 1);
    CHECK(underline.left == 12 && underline.top == 29 && underline.right == 15 && underline.bottom == 30);
    CHECK(background == 0x123456);
    PSMTextOut(NULL, 0, 0, L"A&&B", 4, 0);
    CHECK(!wcscmp(output, L"A&B") && underlines == 1);
    PSMTextOut(NULL, 0, 0, long_text, 254, 0);
    CHECK(!allocations);
    PSMTextOut(NULL, 0, 0, long_text, 255, 0);
    CHECK(!allocations);
    PSMTextOut(NULL, 0, 0, long_text, 256, 0);
    CHECK(allocations == 1 && frees == 1);
    PSMTextOut(NULL, 0, 0, long_text, 1024, 0);
    CHECK(allocations == 2 && frees == 2);
    fail_allocate = TRUE; before = outputs;
    PSMTextOut(NULL, 0, 0, long_text, 256, 0);
    CHECK(outputs == before && GetLastError() == ERROR_NOT_ENOUGH_MEMORY);
    fail_allocate = FALSE;
    before = underlines;
    PSMTextOut(NULL, 0, 0, L"&A", 2, DT_HIDEPREFIX);
    CHECK(!wcscmp(output, L"A") && underlines == before);
    before = outputs;
    PSMTextOut(NULL, 0, 0, L"&A", 2, DT_PREFIXONLY);
    CHECK(outputs == before);
    printf("mnemonic_drawing_failures=%u\n", failures);
    return failures ? 1 : 0;
}
