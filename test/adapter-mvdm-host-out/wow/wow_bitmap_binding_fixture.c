#include "wow_bitmap_bindings.h"
HBITMAP WINAPI WOWLoadBitmapA(HINSTANCE, LPCSTR, LPBYTE, DWORD);
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
BOOL ScaleDIB(LPBITMAPINFOHEADER, LPVOID, LPBITMAPINFOHEADER, LPVOID);
static int xp_scaling(void)
{
    struct { BITMAPINFOHEADER h; DWORD palette[256]; } src = {0}, dst = {0};
    BYTE pixels[16], output[8] = {0};
    void *unreadable;
    unsigned i;
    src.h.biSize = dst.h.biSize = sizeof(BITMAPINFOHEADER);
    src.h.biWidth = src.h.biHeight = 4;
    dst.h.biWidth = dst.h.biHeight = 2;
    src.h.biPlanes = dst.h.biPlanes = 1;
    src.h.biBitCount = 4;
    dst.h.biBitCount = 8;
    src.palette[1] = RGB(26, 26, 26);
    memset(pixels, 0x11, sizeof(pixels));
    CHECK(ScaleDIB(&src.h, pixels, &dst.h, output));
    for (i = 0; i < 2; ++i)
        CHECK(output[i * 4] == 43 && output[i * 4 + 1] == 43);
    unreadable = VirtualAlloc(NULL, 4096, MEM_RESERVE | MEM_COMMIT, PAGE_NOACCESS);
    CHECK(unreadable);
    SetLastError(0x1234);
    CHECK(!ScaleDIB(&src.h, unreadable, &dst.h, output));
    CHECK(GetLastError() == 0x1234);
    CHECK(VirtualFree(unreadable, 0, MEM_RELEASE));
    puts("XP_DIB_SCALING_OK rounding=43 invalid-pixels=false last-error=preserved");
    return 0;
}
/* Exercise XP color-table/monochrome changes through the original bodies. */
DWORD HowManyColors(UPBITMAPINFOHEADER, BOOL, LPBYTE *);
LPBITMAPINFOHEADER CopyDibHdr(UPBITMAPINFOHEADER, LPSTR *, LPBOOL);
VOID ChangeDibColors(LPBITMAPINFOHEADER, UINT);
PVOID DIBFromBitmap(HBITMAP, HDC);
#include "non-mvdm/windows/core/w32inc/newres.h"
UINT GetResourceBpp(LPRESDIR, BOOL);
int RtlGetIdFromDirectory(PBYTE, BOOL, int, int, DWORD, PDWORD);
static int xp_resources(void)
{
    union { BITMAPINFOHEADER align; BYTE bytes[128]; } source = {0};
    BITMAPINFOHEADER *header = &source.align, *copy;
    LPBYTE palette;
    LPSTR bits;
    BOOL mono;
    wow_bitmap_context context;
    struct { NEWHEADER header; RESDIR item[2]; } directory = {0};
    DWORD size;
    header->biSize = sizeof(*header);
    header->biWidth = header->biHeight = 1;
    header->biPlanes = 1; header->biBitCount = 32;
    CHECK(HowManyColors(header, FALSE, &palette) == 0);
    CHECK(palette == source.bytes + sizeof(*header));
    copy = CopyDibHdr(header, &bits, &mono);
    CHECK(copy && !mono && bits == (LPSTR)(source.bytes + sizeof(*header)));
    ChangeDibColors(copy, LR_MONOCHROME);
    CHECK(copy->biBitCount == 1 && copy->biPlanes == 1 && copy->biCompression == BI_RGB);
    CHECK(((DWORD *)(copy + 1))[0] == 0 && ((DWORD *)(copy + 1))[1] == 0xffffff);
    CHECK(LocalFree(copy) == NULL);
    header->biCompression = BI_BITFIELDS;
    CHECK(HowManyColors(header, FALSE, &palette) == 3);
    header->biClrUsed = 7;
    CHECK(HowManyColors(header, FALSE, &palette) == 7);
    directory.header.ResType = FT_ICON; directory.header.ResCount = 2;
    directory.item[0].Icon.Width = directory.item[0].Icon.Height = 32;
    directory.item[0].BitCount = 4; directory.item[0].idIcon = 10;
    directory.item[1] = directory.item[0];
    directory.item[1].BitCount = 32; directory.item[1].idIcon = 20;
    directory.item[1].BytesInRes = 123;
    CHECK(GetResourceBpp(&directory.item[1], TRUE) == 32);
    CHECK(wow_bitmap_bindings_enter(&context));
    context.server.BitCount = 32;
    CHECK(RtlGetIdFromDirectory((PBYTE)&directory, TRUE, 32, 32, 0, &size) == 20 && size == 123);
    CHECK(DIBFromBitmap((HBITMAP)(ULONG_PTR)1, context.dc) == NULL);
    wow_bitmap_bindings_leave(&context);
    puts("XP_RESOURCE_COLOR_ICON_OK rgb32=0 bitfields=3 mono=2 icon=20 invalid-bitmap=null");
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
    CHECK(!wow_bitmap_bind_provider(NULL));
    CHECK(wow_bitmap_bind_provider(WOWLoadBitmapA));
    CHECK(wow_bitmap_bind_provider(WOWLoadBitmapA));
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
    CHECK(xp_scaling() == 0);
    CHECK(xp_resources() == 0);
    wow_bitmap_bindings_shutdown();
    wow_bitmap_bindings_shutdown();
    printf("WOW_BITMAP_BINDING_OK nested=30 parallel=4x50 named=2 missing=2 gdi-before=%lu gdi-after=%lu\n", before, after);
    return 0;
}
