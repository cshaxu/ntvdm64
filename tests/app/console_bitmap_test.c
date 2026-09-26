#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "ntvdm-exe/win32/console_bitmap.h"
#define REQUIRE(x) do { if (!(x)) { printf("FAIL line=%d error=%lu\n",__LINE__,GetLastError()); return 1; } } while (0)

typedef struct lock_case { HANDLE mutex, ready, release; BOOL abandon; } lock_case;
static DWORD WINAPI hold_mutex(void *argument)
{
    lock_case *test = argument;
    if (WaitForSingleObject(test->mutex, 5000) != WAIT_OBJECT_0) return 1;
    SetEvent(test->ready);
    if (test->abandon) return 0;
    WaitForSingleObject(test->release, 5000);
    return ReleaseMutex(test->mutex) ? 0 : 1;
}

int main(void)
{
    struct { BITMAPINFOHEADER h; WORD indices[256]; } info;
    struct { WORD version, count; PALETTEENTRY entries[256]; } logical;
    ntvdm_console_bitmap *buffer = NULL;
    ntvdm_bitmap_description description;
    BYTE *pixels, copied[64];
    HANDLE mutex, thread;
    HPALETTE palette;
    lock_case held;
    DWORD code;
    unsigned int depth, i;
    for (depth = 1; depth <= 8; depth += 7) {
        ZeroMemory(&info, sizeof(info));
        info.h.biSize = sizeof(info.h); info.h.biWidth = 9;
        info.h.biHeight = -2; info.h.biPlanes = 1;
        info.h.biBitCount = (WORD)depth; info.h.biCompression = BI_RGB;
        /* Deliberately bogus SizeImage cannot overrun the actual padded rows. */
        info.h.biSizeImage = 1;
        for (i = 0; i < 256; ++i) info.indices[i] = (WORD)(255-i);
        REQUIRE(ntvdm_console_bitmap_create((BITMAPINFO *)&info, sizeof(info),
            DIB_PAL_COLORS, &buffer, (void **)&pixels, &mutex));
        memset(copied, 0x5a, sizeof(copied));
        REQUIRE(!ntvdm_console_bitmap_copy(buffer,copied,sizeof(copied),&description,0));
        REQUIRE(GetLastError() == ERROR_NOT_READY && copied[0] == 0x5a);
        ZeroMemory(&logical, sizeof(logical));
        logical.version = 0x300; logical.count = 256;
        for (i = 0; i < 256; ++i) {logical.entries[i].peRed=(BYTE)i; logical.entries[i].peBlue=(BYTE)(255-i);}
        palette = CreatePalette((LOGPALETTE *)&logical); REQUIRE(palette);
        REQUIRE(ntvdm_console_bitmap_palette(buffer,palette));
        REQUIRE(WaitForSingleObject(mutex,0) == WAIT_OBJECT_0);
        memset(pixels,0xa5,depth == 1 ? 8 : 24);
        REQUIRE(ReleaseMutex(mutex));
        REQUIRE(ntvdm_console_bitmap_copy(buffer,copied,sizeof(copied),&description,0));
        REQUIRE(description.width == 9 && description.height == 2 && description.depth == depth);
        REQUIRE(description.stride == (depth == 1 ? 4u : 12u));
        REQUIRE(description.bytes == description.stride * 2 && copied[description.bytes] == 0x5a);
        REQUIRE(description.palette[0].rgbRed == 255 && description.palette[1].rgbBlue == 1);
        for (i=0;i<description.bytes;++i) REQUIRE(copied[i] == 0xa5);
        REQUIRE(!ntvdm_console_bitmap_copy(buffer,copied,description.bytes-1,&description,0));
        REQUIRE(GetLastError() == ERROR_INSUFFICIENT_BUFFER);
        REQUIRE(!ntvdm_console_bitmap_palette(buffer,(HPALETTE)(ULONG_PTR)1));
        REQUIRE(ntvdm_console_bitmap_copy(buffer,copied,sizeof(copied),&description,0));
        REQUIRE(description.palette[0].rgbRed == 255);
        DeleteObject(palette);
        held.mutex=mutex; held.ready=CreateEventW(NULL,TRUE,FALSE,NULL);
        held.release=CreateEventW(NULL,TRUE,FALSE,NULL); held.abandon=FALSE;
        REQUIRE(held.ready && held.release);
        thread=CreateThread(NULL,0,hold_mutex,&held,0,NULL); REQUIRE(thread);
        REQUIRE(WaitForSingleObject(held.ready,5000) == WAIT_OBJECT_0);
        REQUIRE(!ntvdm_console_bitmap_copy(buffer,copied,sizeof(copied),&description,0));
        REQUIRE(GetLastError() == ERROR_BUSY);
        SetEvent(held.release); REQUIRE(WaitForSingleObject(thread,5000) == WAIT_OBJECT_0);
        REQUIRE(GetExitCodeThread(thread,&code) && code == 0);
        CloseHandle(thread); ResetEvent(held.ready); held.abandon=TRUE;
        thread=CreateThread(NULL,0,hold_mutex,&held,0,NULL); REQUIRE(thread);
        REQUIRE(WaitForSingleObject(thread,5000) == WAIT_OBJECT_0);
        for(i=0;i<2;++i) {
            REQUIRE(!ntvdm_console_bitmap_copy(buffer,copied,sizeof(copied),&description,0));
            REQUIRE(GetLastError() == ERROR_ABANDONED_WAIT_0);
        }
        CloseHandle(thread); CloseHandle(held.ready); CloseHandle(held.release);
        ntvdm_console_bitmap_destroy(buffer);
        REQUIRE(WaitForSingleObject(mutex,0) == WAIT_OBJECT_0);
        REQUIRE(ReleaseMutex(mutex)); CloseHandle(mutex);
    }
    info.h.biWidth=LONG_MAX;
    REQUIRE(!ntvdm_console_bitmap_create((BITMAPINFO *)&info,sizeof(info),DIB_PAL_COLORS,&buffer,(void **)&pixels,&mutex));
    REQUIRE(!buffer && !pixels && !mutex && GetLastError()==ERROR_INVALID_PARAMETER);
    info.h.biWidth=9; info.h.biHeight=LONG_MIN;
    REQUIRE(!ntvdm_console_bitmap_create((BITMAPINFO *)&info,sizeof(info),DIB_PAL_COLORS,&buffer,(void **)&pixels,&mutex));
    info.h.biHeight=-2;
    REQUIRE(!ntvdm_console_bitmap_create((BITMAPINFO *)&info,sizeof(info.h),DIB_PAL_COLORS,&buffer,(void **)&pixels,&mutex));
    info.h.biBitCount=16;
    REQUIRE(!ntvdm_console_bitmap_create((BITMAPINFO *)&info,sizeof(info),DIB_PAL_COLORS,&buffer,(void **)&pixels,&mutex));
    REQUIRE(GetLastError()==ERROR_NOT_SUPPORTED);
    {
        struct { BITMAPINFOHEADER h; RGBQUAD colors[2]; } rgb;
        ZeroMemory(&rgb,sizeof(rgb)); rgb.h=info.h;
        rgb.h.biBitCount=1; rgb.h.biHeight=2; rgb.h.biClrUsed=2;
        rgb.colors[1].rgbGreen=123; rgb.colors[1].rgbReserved=255;
        REQUIRE(ntvdm_console_bitmap_create((BITMAPINFO *)&rgb,sizeof(rgb),DIB_RGB_COLORS,&buffer,(void **)&pixels,&mutex));
        pixels[0]=0x80; pixels[4]=0x40;
        REQUIRE(ntvdm_console_bitmap_copy(buffer,copied,sizeof(copied),&description,0));
        REQUIRE(copied[0]==0x80 && copied[4]==0x40); /* server-normalized top-down, no flip */
        REQUIRE(description.palette[1].rgbGreen==123 && !description.palette[1].rgbReserved);
        ntvdm_console_bitmap_destroy(buffer); CloseHandle(mutex);
    }
    puts("PASS local packed bitmap/palette, bounds, busy/abandoned and independent mutex lifetime; no IPC or rendering acceptance");
    return 0;
}
