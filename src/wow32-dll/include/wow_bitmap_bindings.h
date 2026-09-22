#ifndef WOW_BITMAP_BINDINGS_H
#define WOW_BITMAP_BINDINGS_H
#include <windows.h>
#include <assert.h>

/* Finite USER client environment for the original bitmap owners.
 * No resource parser, palette algorithm or guest pointer policy lives here. */
typedef BITMAPINFOHEADER UNALIGNED *UPBITMAPINFOHEADER;
typedef BITMAPCOREHEADER UNALIGNED *UPBITMAPCOREHEADER;
typedef struct wow_bitmap_context {
    HDC dc;
    struct { BYTE Planes, BitsPixel, BitCount; } screen;
    struct { BOOL fPaletteDisplay; } server;
    struct wow_bitmap_context *previous;
} wow_bitmap_context;
extern __declspec(thread) wow_bitmap_context *wow_bitmap_current;
extern CRITICAL_SECTION wow_bitmap_lock;
BOOL WINAPI wow_bitmap_bindings_enter(wow_bitmap_context *context);
void WINAPI wow_bitmap_bindings_leave(wow_bitmap_context *context);
/* Terminal DLL teardown only, after all conversion calls have returned. */
void WINAPI wow_bitmap_bindings_shutdown(void);

#define gcsHdc wow_bitmap_lock
#define ghdcBits2 (wow_bitmap_current->dc)
#define oemInfo (wow_bitmap_current->screen)
#define gpsi (&wow_bitmap_current->server)
#define GETINITDC() GetDC(NULL)
#define RELEASEINITDC(hdc) ReleaseDC(NULL, hdc)
#define UserLocalAlloc(flags, bytes) LocalAlloc(flags, bytes)
#define UserLocalFree(p) LocalFree((HLOCAL)(p))
#define RtlEnterCriticalSection EnterCriticalSection
#define RtlLeaveCriticalSection LeaveCriticalSection
#define SelectBitmap(hdc, bitmap) ((HBITMAP)SelectObject(hdc, bitmap))
#define UserAssert(condition) assert(condition)
#define RIPMSG0(level, message) ((void)0)
#define BitmapWidth(cx, bpp) (((((cx)*(bpp)) + 31) & ~31) >> 3)
#define SetBestStretchMode(hdc, planes, bpp) \
    SetStretchBltMode(hdc, (((planes) * (bpp)) == 1 ? BLACKONWHITE : COLORONCOLOR))
#define RGBX(rgb) RGB(GetBValue(rgb), GetGValue(rgb), GetRValue(rgb))
#define SYSRGB(index) GetSysColor(COLOR_##index)
#define CBM_CREATEDIB 0x02L
#define LR_CREATEREALDIB 0x0800
#define ID(string) (((DWORD)(string) & 0xffff0000) == 0)
/* The only selected LoadBmp caller supplies zero dimensions/flags. */
#define LoadBmp(module, name, cx, cy, flags) LoadBitmapW(module, name)
int MBToWCSEx(WORD, LPCSTR, int, LPWSTR *, int, BOOL);
#define MBToWCS(text, count, output, capacity, allocate) \
    MBToWCSEx(0, text, count, output, capacity, allocate)

int SmartStretchDIBits(HDC, int, int, int, int, int, int, int, int,
    LPVOID, LPBITMAPINFO, UINT, DWORD);
HBITMAP ConvertDIBBitmap(UPBITMAPINFOHEADER, DWORD, DWORD, UINT,
    LPBITMAPINFOHEADER *, LPSTR *);
HBITMAP WOWLoadBitmapA(HINSTANCE, LPCSTR, LPBYTE, DWORD);
/* Same output-slot ABI; owns only the native conversion environment. */
HBITMAP WINAPI wow_bitmap_load(HINSTANCE, LPCSTR, LPBYTE, DWORD);
#endif
