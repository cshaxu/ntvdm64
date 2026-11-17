#include "text-measure/precomp.h"
#include <stdio.h>
#include <string.h>

/* Actual original measurement/prefix bodies; only allocation and GDI leaves
 * are instrumented. No claim of native painting or guest acceptance. */
static unsigned failures, allocations, frees, measurements;
static BOOL fail_allocate;
static BYTE *owned;
static SIZE_T owned_size;
#define CHECK(x) do { if (!(x)) { ++failures; printf("FAIL line=%d\n", __LINE__); } } while (0)
#define LEAF_ERROR 0x47112u

PVOID WINAPI measure_allocate(SIZE_T bytes)
{
    CHECK(!owned);
    ++allocations;
    if (fail_allocate) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return NULL; }
    owned = HeapAlloc(GetProcessHeap(), 0, bytes + 4);
    if (owned) {
        owned_size = bytes;
        memset(owned + bytes, 0x5a, 4);
    }
    return owned;
}
VOID WINAPI measure_free(PVOID pointer)
{
    unsigned i;
    CHECK(pointer == owned && owned != NULL);
    if (!owned) return;
    for (i = 0; i < 4; ++i) CHECK(owned[owned_size + i] == 0x5a);
    CHECK(HeapFree(GetProcessHeap(), 0, owned));
    owned = NULL;
    ++frees;
    SetLastError(0xbad); /* Original helper does not promise LastError preservation. */
}
BOOL WINAPI measure_extent(HDC dc, LPCWSTR text, int count, PSIZE size)
{
    (void)dc;
    CHECK(text != NULL && count >= 0);
    ++measurements;
    SetLastError(LEAF_ERROR);
    size->cx = count;
    size->cy = 1;
    return FALSE; /* Original PSMGetTextExtent deliberately returns TRUE. */
}
int __cdecl main(void)
{
    WCHAR text[301];
    SIZE size;
    unsigned i;
    for (i = 0; i < 300; ++i) text[i] = L'A';
    text[300] = 0;
    CHECK(PSMGetTextExtent(NULL, text, 254, &size));
    CHECK(!allocations && !frees && measurements == 1 && size.cx == 254);
    CHECK(PSMGetTextExtent(NULL, text, 255, &size));
    CHECK(allocations == 1 && frees == 1 && !owned && size.cx == 255);
    fail_allocate = TRUE;
    size.cx = size.cy = 99;
    CHECK(!PSMGetTextExtent(NULL, text, 300, &size));
    CHECK(GetLastError() == ERROR_NOT_ENOUGH_MEMORY && size.cx == 99 && size.cy == 99);
    CHECK(allocations == 2 && frees == 1 && measurements == 2 && !owned);
    fail_allocate = FALSE;
    text[0] = L'&';
    CHECK(PSMGetTextExtent(NULL, text, 300, &size));
    CHECK(size.cx == 299 && allocations == 3 && frees == 2 && !owned);
    printf("text_measure_failure_failures=%u\n", failures);
    return failures ? 1 : 0;
}
