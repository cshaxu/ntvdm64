#include "wow_edit_lookaside_bindings.h"
#include <stdio.h>

/* Supplemental host allocator contract only, not guest EDIT acceptance.
 * Link the original rtlinit.c WOW_ORIGINAL_EDIT_LOOKASIDE selection. */
static unsigned errors;
/* This fixture does not link the original editor. Its disposable pool is
 * not a second production definition of editec.c's EditLookaside. */
static LOOKASIDE fixture_pool;
#define CHECK(x) do { if (!(x)) { ++errors; printf("FAIL line=%d error=%lu\n", __LINE__, GetLastError()); } } while (0)
static BOOL in_pool(PVOID p)
{
    return (ULONG_PTR)p >= (ULONG_PTR)fixture_pool.LookasideBase &&
        (ULONG_PTR)p < (ULONG_PTR)fixture_pool.LookasideBounds;
}
static BOOL is_zero(const BYTE *p, SIZE_T bytes)
{
    SIZE_T i;
    for (i = 0; i != bytes; ++i) if (p[i]) return FALSE;
    return TRUE;
}
int __cdecl main(void)
{
    PVOID entries[5], base, reused;
    unsigned i, j;
    SetLastError(0x1234);
    CHECK(wow_edit_lookaside_ready());
    CHECK(GetLastError() == 0x1234);
    CHECK(wow_edit_lookaside_ready());
    CHECK(InitLookaside(&fixture_pool, 64, 4) == STATUS_SUCCESS);
    base = fixture_pool.LookasideBase;
    if (!base) return 3;
    for (i = 0; i != 5; ++i) {
        entries[i] = AllocLookasideEntry(&fixture_pool);
        CHECK(entries[i] != NULL);
        if (!entries[i]) return 3;
        CHECK(is_zero(entries[i], 64));
        CHECK(in_pool(entries[i]) == (i < 4));
        for (j = 0; j != i; ++j) CHECK(entries[i] != entries[j]);
        FillMemory(entries[i], 64, 0x55);
    }
    /* Reinitializing the same type does not discard live entries/free list. */
    CHECK(InitLookaside(&fixture_pool, 64, 4) == STATUS_SUCCESS);
    CHECK(fixture_pool.LookasideBase == base);
    CHECK(fixture_pool.LookasideZone.FreeList.Next == NULL);
    FreeLookasideEntry(&fixture_pool, entries[1]);
    reused = AllocLookasideEntry(&fixture_pool);
    CHECK(reused == entries[1]);
    CHECK(is_zero(reused, 64));
    for (i = 0; i != 5; ++i) FreeLookasideEntry(&fixture_pool, entries[i]);
    printf("WOW_EDIT_LOOKASIDE errors=%u pool=4 overflow=1 reuse=1\n", errors);
    return errors ? 3 : 0;
}
