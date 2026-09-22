#include <windows.h>
#include <stdio.h>
#include <string.h>
#define WOW_BITMAP_FIXTURE_BINDINGS 1

/* Test-only original USER header conversion; no production replacement.
 * usercli.h declarations and BitmapWidth retained in their original shape. */
typedef BITMAPINFOHEADER UNALIGNED *UPBITMAPINFOHEADER;
typedef BITMAPCOREHEADER UNALIGNED *UPBITMAPCOREHEADER;
#define BitmapWidth(cx, bpp) (((((cx)*(bpp)) + 31) & ~31) >> 3)
static unsigned allocations, frees;
static BOOL fail_alloc;
static void *fixture_alloc(UINT flags, SIZE_T size)
{
    void *p;
    (void)flags;
    if (fail_alloc) return NULL;
    p = HeapAlloc(GetProcessHeap(), 0, size);
    if (p) ++allocations;
    return p;
}
#define UserLocalAlloc fixture_alloc
#define RIPMSG0(level, message) ((void)0)
static void release_header(void *p);
#define UserLocalFree release_header
static CRITICAL_SECTION gcsHdc;
static HDC ghdcBits2;
static struct { BYTE Planes, BitsPixel, BitCount; } oemInfo;
static struct { BOOL fPaletteDisplay; } server_info, *gpsi = &server_info;
static BOOL fail_screen_dc;
#define GETINITDC() (fail_screen_dc ? NULL : GetDC(NULL))
#define RELEASEINITDC(hdc) ReleaseDC(NULL, hdc)
#define SelectBitmap(hdc, bitmap) ((HBITMAP)SelectObject(hdc, bitmap))
#define RtlEnterCriticalSection EnterCriticalSection
#define RtlLeaveCriticalSection LeaveCriticalSection
#define CBM_CREATEDIB 0x02L
#define LR_CREATEREALDIB 0x0800
/* These branches are excluded by the actual WOWLoadBitmapA arguments.
 * A reached branch is a hard test failure, never a successful stub. */
static int unexpected_branch(void) { ExitProcess(90); return 0; }
#define SetBestStretchMode(hdc, planes, bpp) \
    SetStretchBltMode(hdc, (((planes) * (bpp)) == 1 ? BLACKONWHITE : COLORONCOLOR))
#define SYSRGB(index) GetSysColor(COLOR_##index)
#define RGBX(rgb) RGB(GetBValue(rgb), GetGValue(rgb), GetRValue(rgb))
#define UserAssert(condition) do { if (!(condition)) unexpected_branch(); } while (0)
#define ID(string) (((DWORD)(string) & 0xffff0000) == 0)
/* This fixture tests supplied bytes; native name loading has a separate test. */
#define LoadBmp(module, name, cx, cy, flags) ((HBITMAP)(ULONG_PTR)unexpected_branch())
#define MBToWCS(text, count, output, capacity, allocate) unexpected_branch()
#include "../../../src/opennt-host/windows/core/ntuser/client/cldib.c"
#ifdef WOW_BITMAP_UPSTREAM_NEGATIVE
#define ChangeDibColors(header, flags) unexpected_branch()
#include "original-bitmap-header.inc"
#else
#include "../../../src/opennt-host/windows/core/ntuser/client/clres.c"
#endif

#define CHECK(x) do { if (!(x)) { printf("FAIL line %d\n", __LINE__); return 1; } } while (0)
static void release_header(void *p)
{
    if (p) { HeapFree(GetProcessHeap(), 0, p); ++frees; }
}

int __cdecl main(void)
{
    BYTE source[128], saved[128];
    BITMAPINFOHEADER *info = (BITMAPINFOHEADER *)source, *out;
    BITMAPCOREHEADER *core = (BITMAPCOREHEADER *)source;
    DWORD *colors;
    LPSTR bits;
    BOOL mono;
    unsigned i;
    memset(source, 0, sizeof(source));
    info->biSize = sizeof(*info);
    info->biWidth = 2; info->biHeight = 1;
    info->biPlanes = 1; info->biBitCount = 1;
    colors = (DWORD *)(info + 1);
    colors[0] = 0; colors[1] = 0xffffff;
    source[48] = 0x80;
    for (i = 0; i < 3; ++i) {
        if (i == 1) { colors[0] = 0xffffff; colors[1] = 0; }
        if (i == 2) { colors[0] = 0xff0000; colors[1] = 0x00ff00; }
        memcpy(saved, source, sizeof(source));
        out = CopyDibHdr(info, &bits, &mono);
        CHECK(out && bits == (LPSTR)source + 48 && mono == (i != 2));
        CHECK(!memcmp(out, source, 52) && !memcmp(saved, source, sizeof(source)));
        release_header(out);
    }
    fail_alloc = TRUE;
    CHECK(CopyDibHdr(info, &bits, &mono) == NULL);
    fail_alloc = FALSE;
    memset(source, 0, sizeof(source));
    core->bcSize = sizeof(*core); core->bcWidth = 2; core->bcHeight = 1;
    core->bcPlanes = 1; core->bcBitCount = 1;
    source[15] = source[16] = source[17] = 255; source[18] = 0x80;
    memcpy(saved, source, sizeof(source));
    out = CopyDibHdr(info, &bits, &mono);
    CHECK(out && mono && bits == (LPSTR)source + 18);
    CHECK(out->biSize == 40 && out->biWidth == 2 && out->biHeight == 1);
    CHECK(out->biClrUsed == 2 && out->biSizeImage == 4);
    CHECK(((DWORD *)(out + 1))[1] == 0xffffff);
    CHECK(!memcmp(saved, source, sizeof(source)));
    release_header(out);
    memset(source, 0, sizeof(source));
    CHECK(CopyDibHdr(info, &bits, &mono) == NULL);
    CHECK(allocations == 4 && frees == allocations);
    /* Remaining original header branches: masks, explicit color count,
     * and the old icon/cursor resource layouts. */
    for (i = 0; i < 5; ++i) {
        memset(source, 0, sizeof(source));
        info->biSize = sizeof(*info); info->biWidth = 2; info->biHeight = 1;
        info->biPlanes = 1;
        info->biBitCount = i == 0 ? 24 : (i == 1 ? 16 : 8);
        if (i == 1) { info->biCompression = BI_BITFIELDS; colors[0] = 0xf800; colors[1] = 0x7e0; colors[2] = 0x1f; }
        if (i == 2) { info->biClrUsed = 2; colors[1] = 0xffffff; }
        if (i >= 3) {
            POLDCURSOR old = (POLDCURSOR)source;
            memset(source, 0, sizeof(source));
            old->bType = i == 3 ? BMR_ICON : BMR_CURSOR;
            old->cx = 2; old->cy = 1; old->abBitmap[0] = 0x80;
        }
        memcpy(saved, source, sizeof(source));
        out = CopyDibHdr(info, &bits, &mono);
        CHECK(out != NULL && !memcmp(saved, source, sizeof(source)));
        if (i == 0) CHECK(bits == (LPSTR)source + 40 && !mono);
        if (i == 1) CHECK(bits == (LPSTR)source + 52 && !mono);
        if (i == 2) CHECK(bits == (LPSTR)source + 48 && mono);
        if (i >= 3) {
            printf("old-format=%u bits=%p expected=%p mono=%d height=%ld\n", i,
                bits, ((POLDCURSOR)source)->abBitmap, mono, out->biHeight);
            CHECK(bits == (LPSTR)((POLDCURSOR)source)->abBitmap && mono && out->biHeight == 2);
        }
        release_header(out);
    }
    CHECK(allocations == 9 && frees == allocations);
    {
        HDC screen, read_dc;
        HBITMAP bitmap, old;
        BITMAP object;
        DWORD before, after;
        InitializeCriticalSection(&gcsHdc);
        screen = GetDC(NULL);
        CHECK(screen != NULL);
        oemInfo.Planes = (BYTE)GetDeviceCaps(screen, PLANES);
        oemInfo.BitsPixel = (BYTE)GetDeviceCaps(screen, BITSPIXEL);
        oemInfo.BitCount = oemInfo.Planes * oemInfo.BitsPixel;
        ghdcBits2 = CreateCompatibleDC(screen);
        read_dc = CreateCompatibleDC(screen);
        ReleaseDC(NULL, screen);
        CHECK(ghdcBits2 && read_dc);
        before = GetGuiResources(GetCurrentProcess(), GR_GDIOBJECTS);
        for (i = 0; i < 30; ++i) {
            memset(source, 0, sizeof(source));
            info->biSize = sizeof(*info);
            info->biWidth = 2; info->biHeight = 1;
            info->biPlanes = 1; info->biBitCount = 1;
            colors = (DWORD *)(info + 1);
            colors[0] = i % 3 == 2 ? 0xff0000 : 0;
            colors[1] = i % 3 == 2 ? 0x00ff00 : 0xffffff;
            source[48] = 0x80;
            if (i % 3 == 1) {
                memset(source, 0, sizeof(source));
                core->bcSize = sizeof(*core); core->bcWidth = 2; core->bcHeight = 1;
                core->bcPlanes = 1; core->bcBitCount = 1;
                source[15] = source[16] = source[17] = 255; source[18] = 0x80;
            }
            memcpy(saved, source, sizeof(source));
            bitmap = ConvertDIBBitmap(info, 0, 0, LR_DEFAULTSIZE, NULL, NULL);
            CHECK(bitmap != NULL && !memcmp(source, saved, sizeof(source)));
            CHECK(GetObject(bitmap, sizeof(object), &object) == sizeof(object));
            CHECK(object.bmWidth == 2 && object.bmHeight == 1);
            CHECK(i % 3 == 2 || object.bmBitsPixel == 1);
            old = SelectBitmap(read_dc, bitmap);
            CHECK(old != NULL);
            CHECK(GetPixel(read_dc, 0, 0) == (i % 3 == 2 ? RGB(0,255,0) : RGB(255,255,255)));
            CHECK(GetPixel(read_dc, 1, 0) == (i % 3 == 2 ? RGB(255,0,0) : RGB(0,0,0)));
            SelectBitmap(read_dc, old);
            CHECK(DeleteObject(bitmap));
        }
        fail_alloc = TRUE;
        CHECK(ConvertDIBBitmap(info, 0, 0, LR_DEFAULTSIZE, NULL, NULL) == NULL);
        fail_alloc = FALSE;
        fail_screen_dc = TRUE;
        CHECK(ConvertDIBBitmap(info, 0, 0, LR_DEFAULTSIZE, NULL, NULL) == NULL);
        fail_screen_dc = FALSE;
        after = GetGuiResources(GetCurrentProcess(), GR_GDIOBJECTS);
        CHECK(before == after && allocations == frees);
        CHECK(DeleteDC(read_dc) && DeleteDC(ghdcBits2));
        DeleteCriticalSection(&gcsHdc);
        printf("WOW_ORIGINAL_BITMAP_GDI_OK repetitions=30 gdi-before=%lu gdi-after=%lu\n", before, after);
    }
    puts("WOW_ORIGINAL_BITMAP_HEADER_OK info=6 core=1 old=2 allocation-failure=1 invalid=1");
    return 0;
}
