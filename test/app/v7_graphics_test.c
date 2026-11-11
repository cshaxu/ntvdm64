#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "conapi.h"
#include "ntvdm-exe/win32/console_client.h"
#include "ntvdm-exe/win32/console_graphics.h"
#undef CreateConsoleScreenBuffer
#undef CloseHandle

#define CHECK(x) do { if (!(x)) { printf("FAIL line %d error %lu\n",__LINE__,GetLastError()); return 1; } } while (0)

static ntvdm_console_graphics *graphics;
static unsigned signals;
static worker_base_publication_capture_fn capture_fn;
static void *capture_context;
ntvdm_console_graphics *ntvdm_console_graphics_context(void) { return graphics; }
BOOL ntvdm_console_publish_video(const console_video_description *d,const void *p,size_t n)
{ (void)d;(void)p;(void)n;return TRUE; }
DWORD worker_base_publication_set_capture(worker_base_publication *p,worker_base_publication_capture_fn f,void *c)
{ (void)p;capture_fn=f;capture_context=c;return 0; }
DWORD worker_base_publication_signal(worker_base_publication *p)
{ (void)p;++signals;return 0; }
HANDLE WINAPI MvdmCreateConsoleScreenBuffer(DWORD,DWORD,const SECURITY_ATTRIBUTES *,DWORD,void *);
BOOL WINAPI MvdmCloseConsoleHandle(HANDLE);

int main(void)
{
    struct { BITMAPINFOHEADER header; WORD indices[256]; } bitmap={0};
    CONSOLE_GRAPHICS_BUFFER_INFO info={0};
    SMALL_RECT full={0,0,7,5}, partial={0,0,0,0};
    BYTE mask[256], *pixels;
    HANDLE output;
    unsigned i, count;
    graphics=ntvdm_console_graphics_create();CHECK(graphics);
    CHECK(!ntvdm_console_graphics_attach_publisher(graphics,(worker_base_publication *)1));
    CHECK(capture_fn && capture_context==graphics);
    bitmap.header.biSize=sizeof(bitmap.header);bitmap.header.biWidth=8;
    bitmap.header.biHeight=-6;bitmap.header.biPlanes=1;bitmap.header.biBitCount=8;
    bitmap.header.biCompression=BI_RGB;bitmap.header.biSizeImage=48;
    for(i=0;i<256;++i)bitmap.indices[i]=(WORD)i;
    info.dwBitMapInfoLength=sizeof(bitmap);info.lpBitMapInfo=(BITMAPINFO *)&bitmap;
    info.dwUsage=DIB_PAL_COLORS;
    output=MvdmCreateConsoleScreenBuffer(GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,CONSOLE_GRAPHICS_BUFFER,&info);
    CHECK(output!=INVALID_HANDLE_VALUE && info.lpBitMap && info.hMutex);
    pixels=info.lpBitMap;memset(pixels,0x35,48);
    memset(mask,0xff,128);memset(mask+128,0,128);mask[0]=0x7f;
    ntvdm_console_graphics_v7_pointer(mask,2,1,TRUE);
    CHECK(pixels[10]==0x35); /* no pointer until the first complete body */
    CHECK(ntvdm_console_graphics_invalidate(output,&full)==1);
    ntvdm_console_graphics_v7_pointer(mask,2,1,TRUE);CHECK(pixels[10]==0);
    count=signals;ntvdm_console_graphics_begin_update();CHECK(pixels[10]==0x35);
    CHECK(ntvdm_console_graphics_invalidate(output,&partial)==1 && signals==count);
    ntvdm_console_graphics_v7_pointer(mask,2,1,TRUE);CHECK(pixels[10]==0 && signals==count);
    ntvdm_console_graphics_end_update();CHECK(signals==count+1);
    ntvdm_console_graphics_begin_update();ntvdm_console_graphics_begin_update();
    count=signals;CHECK(ntvdm_console_graphics_invalidate(output,&partial)==1);
    ntvdm_console_graphics_end_update();CHECK(signals==count);
    ntvdm_console_graphics_end_update();CHECK(signals==count+1);
    CHECK(MvdmCloseConsoleHandle(output));CloseHandle(info.hMutex);
    ntvdm_console_graphics_destroy(graphics);
    puts("PASS Video7 surface readiness, restoration and batched publication");
    return 0;
}
