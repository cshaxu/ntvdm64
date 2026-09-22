#include "wow_bitmap_bindings.h"
#include <stdio.h>
static unsigned fail_at, locks, screen_acquired, screen_released;
static unsigned conversions, conversion_errors;
HBITMAP WOWLoadBitmapA(HINSTANCE module, LPCSTR name, LPBYTE bytes, DWORD count)
{
    (void)module; (void)name; (void)count;
    ++conversions;
    if (bytes && !wow_bitmap_current) ++conversion_errors;
    return (HBITMAP)7;
}
static BOOL WINAPI test_initialize(LPCRITICAL_SECTION lock, DWORD spin, DWORD flags)
{
    ++locks;
    if (fail_at == 1) return FALSE;
    return InitializeCriticalSectionEx(lock, spin, flags);
}
static HDC WINAPI test_get_dc(HWND window)
{
    HDC dc;
    if (fail_at == 2) return NULL;
    dc = GetDC(window);
    if (dc) ++screen_acquired;
    return dc;
}
static int WINAPI test_release_dc(HWND window, HDC dc)
{
    int result = ReleaseDC(window, dc);
    if (result) ++screen_released;
    return result;
}
static HDC WINAPI test_compatible_dc(HDC dc)
{
    return fail_at == 3 ? NULL : CreateCompatibleDC(dc);
}
#define InitializeCriticalSectionEx test_initialize
#define GetDC test_get_dc
#define ReleaseDC test_release_dc
#define CreateCompatibleDC test_compatible_dc
#include "../../../src/ntvdm-exe/wow/wow_bitmap_bindings.c"
#define CHECK(x) do { if (!(x)) { printf("FAIL line %d\n", __LINE__); return 1; } } while (0)
int __cdecl main(void)
{
    wow_bitmap_context outer, failed;
    unsigned i;
    fail_at = 1;
    CHECK(!wow_bitmap_bindings_enter(&failed) && !wow_bitmap_current);
    fail_at = 0;
    CHECK(wow_bitmap_bindings_enter(&outer) && locks == 2);
    for (i = 2; i <= 3; ++i) {
        BYTE resource = 0;
        unsigned calls = conversions;
        fail_at = i;
        CHECK(!wow_bitmap_bindings_enter(&failed));
        CHECK(wow_bitmap_current == &outer && !failed.dc);
        CHECK(!wow_bitmap_load(NULL, NULL, &resource, 1));
        CHECK(conversions == calls && wow_bitmap_current == &outer);
        fail_at = 0;
        CHECK(wow_bitmap_load(NULL, NULL, &resource, 1) == (HBITMAP)7);
        CHECK(conversions == calls + 1 && wow_bitmap_current == &outer);
        CHECK(wow_bitmap_bindings_enter(&failed));
        wow_bitmap_bindings_leave(&failed);
        CHECK(wow_bitmap_current == &outer);
    }
    wow_bitmap_bindings_leave(&outer);
    CHECK(!wow_bitmap_current && screen_acquired == screen_released);
    CHECK(!conversion_errors && conversions == 2);
    wow_bitmap_bindings_shutdown();
    printf("WOW_BITMAP_BINDING_FAILURE_OK locks=%u screen=%u/%u\n",
        locks, screen_acquired, screen_released);
    return 0;
}
