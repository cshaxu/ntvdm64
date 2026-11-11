#include "wow_bitmap_bindings.h"
#include <stdio.h>
#define CHECK(x) do { if (!(x)) { printf("FAIL line %d\n", __LINE__); return 1; } } while (0)
static DWORD WINAPI parallel_convert(void *start)
{
    unsigned i;
    struct { BITMAPINFOHEADER header; DWORD colors[2]; DWORD bits; } resource;
    if (WaitForSingleObject((HANDLE)start, 10000) != WAIT_OBJECT_0) return 2;
    ZeroMemory(&resource, sizeof(resource));
    resource.header.biSize = sizeof(resource.header);
    resource.header.biWidth = 2; resource.header.biHeight = 1;
    resource.header.biPlanes = 1; resource.header.biBitCount = 1;
    resource.colors[1] = 0xffffff; resource.bits = 0x80;
    for (i = 0; i < 50; ++i) {
        wow_bitmap_context context;
        HBITMAP bitmap;
        if (wow_bitmap_current || !wow_bitmap_bindings_enter(&context)) return 3;
        if (wow_bitmap_current != &context) return 4;
        bitmap = wow_bitmap_load(NULL, NULL, (LPBYTE)&resource, sizeof(resource));
        if (!bitmap || !DeleteObject(bitmap)) return 5;
        wow_bitmap_bindings_leave(&context);
        if (wow_bitmap_current) return 6;
    }
    GdiFlush();
    return 0;
}
int __cdecl main(void)
{
    struct { BITMAPINFOHEADER header; DWORD colors[2]; DWORD bits; } resource;
    wow_bitmap_context outer, inner;
    HBITMAP bitmap;
    BITMAP object;
    unsigned i;
    DWORD before, after;
    ZeroMemory(&resource, sizeof(resource));
    resource.header.biSize = sizeof(resource.header);
    resource.header.biWidth = 2; resource.header.biHeight = 1;
    resource.header.biPlanes = 1; resource.header.biBitCount = 1;
    resource.colors[1] = 0xffffff; resource.bits = 0x80;
    CHECK(!wow_bitmap_bindings_enter(NULL));
    before = GetGuiResources(GetCurrentProcess(), GR_GDIOBJECTS);
    for (i = 0; i < 30; ++i) {
        CHECK(wow_bitmap_bindings_enter(&outer));
        CHECK(wow_bitmap_current == &outer);
        CHECK(wow_bitmap_bindings_enter(&inner));
        CHECK(wow_bitmap_current == &inner && inner.previous == &outer);
        bitmap = wow_bitmap_load(NULL, NULL, (LPBYTE)&resource, sizeof(resource));
        CHECK(bitmap && GetObject(bitmap, sizeof(object), &object) == sizeof(object));
        CHECK(object.bmWidth == 2 && object.bmHeight == 1 && object.bmBitsPixel == 1);
        CHECK(DeleteObject(bitmap));
        wow_bitmap_bindings_leave(&inner);
        CHECK(wow_bitmap_current == &outer && inner.dc == NULL);
        wow_bitmap_bindings_leave(&outer);
        CHECK(wow_bitmap_current == NULL && outer.dc == NULL);
    }
    {
        HANDLE workers[4], start = CreateEvent(NULL, TRUE, FALSE, NULL);
        DWORD result;
        CHECK(start);
        for (i = 0; i < 4; ++i) {
            workers[i] = CreateThread(NULL, 0, parallel_convert, start, 0, NULL);
            CHECK(workers[i]);
        }
        CHECK(SetEvent(start));
        CHECK(WaitForMultipleObjects(4, workers, TRUE, 10000) == WAIT_OBJECT_0);
        for (i = 0; i < 4; ++i) {
            CHECK(GetExitCodeThread(workers[i], &result) && result == 0);
            CHECK(CloseHandle(workers[i]));
        }
        CHECK(CloseHandle(start));
    }
    GdiFlush();
    after = GetGuiResources(GetCurrentProcess(), GR_GDIOBJECTS);
    CHECK(after == before);
    CHECK(wow_bitmap_load(NULL, "MISSING-S40-BITMAP", NULL, 0) == NULL);
    CHECK(wow_bitmap_load(NULL, MAKEINTRESOURCEA(0xffff), NULL, 0) == NULL);
    for (i = 0; i < 2; ++i) {
        HDC dc;
        HBITMAP previous;
        bitmap = wow_bitmap_load(GetModuleHandle(NULL),
            i ? "S40BITMAP" : MAKEINTRESOURCEA(101), NULL, 0);
        CHECK(bitmap && GetObject(bitmap, sizeof(object), &object) == sizeof(object));
        CHECK(object.bmWidth == 2 && object.bmHeight == 1);
        dc = CreateCompatibleDC(NULL);
        CHECK(dc);
        previous = (HBITMAP)SelectObject(dc, bitmap);
        CHECK(previous);
        CHECK(GetPixel(dc, 0, 0) == RGB(255,255,255) && GetPixel(dc, 1, 0) == RGB(0,0,0));
        SelectObject(dc, previous);
        CHECK(DeleteDC(dc) && DeleteObject(bitmap));
    }
    wow_bitmap_bindings_shutdown();
    wow_bitmap_bindings_shutdown();
    printf("WOW_BITMAP_BINDING_OK nested=30 parallel=4x50 named=2 missing=2 gdi-before=%lu gdi-after=%lu\n", before, after);
    return 0;
}
