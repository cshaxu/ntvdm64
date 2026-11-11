#include "mnemonic-drawing/precomp.h"
#include <stdio.h>
#include <string.h>
#include <wchar.h>

/* Real prefix/output algorithm, instrumented resource/drawing leaves. */
static unsigned failures, allocations, frees, outputs, underlines;
static BOOL fail_allocate, reenter, raise_output, raise_underline;
static COLORREF background = 0x123456;
static WCHAR output[1025];
static RECT underline;
#define MNEMONIC_EXCEPTION 0xe0477115u
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
    SetLastError(0xbad); /* Output error must survive cleanup. */
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
    if (reenter) {
        reenter = FALSE;
        PSMTextOut(NULL, 0, 0, L"nested", 6);
        CHECK(!memcmp(saved, text, count * sizeof(WCHAR)));
    }
    CopyMemory(output, saved, (count+1) * sizeof(WCHAR));
    SetLastError(0x47115);
    if (raise_output) RaiseException(MNEMONIC_EXCEPTION, 0, 0, NULL);
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
    if (raise_underline) RaiseException(MNEMONIC_EXCEPTION, 0, 0, NULL);
    return TRUE;
}
int __cdecl main(void)
{
    WCHAR long_text[1025];
    unsigned i, before;
    BOOL caught;
    for (i = 0; i < 1024; ++i) long_text[i] = L'Q';
    long_text[1024] = 0;
    PSMTextOut(NULL, 10, 20, L"A&B", 3);
    CHECK(!wcscmp(output, L"AB") && underlines == 1);
    CHECK(underline.left == 12 && underline.top == 29 && underline.right == 15 && underline.bottom == 30);
    CHECK(background == 0x123456);
    PSMTextOut(NULL, 0, 0, L"A&&B", 4);
    CHECK(!wcscmp(output, L"A&B") && underlines == 1);
    reenter = TRUE;
    PSMTextOut(NULL, 0, 0, L"outer", 5);
    CHECK(!wcscmp(output, L"outer"));
    PSMTextOut(NULL, 0, 0, long_text, 254);
    CHECK(!allocations);
    PSMTextOut(NULL, 0, 0, long_text, 255);
    CHECK(allocations == 1 && frees == 1 && GetLastError() == 0x47115);
    PSMTextOut(NULL, 0, 0, long_text, 1024);
    CHECK(allocations == 2 && frees == 2);
    fail_allocate = TRUE; before = outputs;
    PSMTextOut(NULL, 0, 0, long_text, 255);
    CHECK(outputs == before && GetLastError() == ERROR_NOT_ENOUGH_MEMORY);
    fail_allocate = FALSE;
    for (i = 0; i < 2; ++i) {
        caught = FALSE;
        raise_output = i == 0; raise_underline = i == 1;
        long_text[0] = L'&';
        before = frees;
        __try { PSMTextOut(NULL, 0, 0, long_text, 255); }
        __except (GetExceptionCode() == MNEMONIC_EXCEPTION ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) { caught = TRUE; }
        CHECK(caught && frees == before + 1 && background == 0x123456);
        CHECK(GetLastError() == 0x47115);
    }
    raise_output = raise_underline = FALSE;
    before = outputs;
    PSMTextOut(NULL, 0, 0, NULL, 1);
    CHECK(outputs == before && GetLastError() == ERROR_INVALID_PARAMETER);
    PSMTextOut(NULL, 0, 0, long_text, -1);
    CHECK(outputs == before && GetLastError() == ERROR_INVALID_PARAMETER);
    printf("mnemonic_drawing_failures=%u\n", failures);
    return failures ? 1 : 0;
}
