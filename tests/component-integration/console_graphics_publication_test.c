#include "ntvdm-exe/win32/console_graphics.h"
#include "worker-base/publication.h"
#include "common/protocol/console_video.h"
#include "conapi.h"
#include <stdio.h>
#include <string.h>

#undef CreateConsoleScreenBuffer
#undef SetConsoleActiveScreenBuffer
#undef CloseHandle

#define CHECK(x) do { if(!(x)) { fprintf(stderr,"FAIL line %d error %lu\n",__LINE__,GetLastError());return 1; } } while(0)

static ntvdm_console_graphics *current;
static HANDLE delivered;
static volatile LONG sends;
static BYTE final_pixel;

ntvdm_console_graphics *ntvdm_console_graphics_context(void) { return current; }
BOOL ntvdm_console_publish_video(const console_video_description *description,
    const void *pixels,size_t capacity)
{
    (void)description;(void)pixels;(void)capacity;
    return TRUE;
}

static DWORD send_frame(void *context,const void *payload,SIZE_T bytes)
{
    const console_video_description *description=payload;
    const BYTE *pixels=(const BYTE *)payload+sizeof(*description);
    (void)context;
    if(bytes<sizeof(*description) || description->bytes!=16 ||
        bytes!=sizeof(*description)+description->bytes)return ERROR_INVALID_DATA;
    final_pixel=pixels[15];InterlockedIncrement(&sends);SetEvent(delivered);
    return ERROR_SUCCESS;
}

int main(void)
{
    struct { BITMAPINFOHEADER header;RGBQUAD colours[256]; } bitmap={0};
    CONSOLE_GRAPHICS_BUFFER_INFO info={0};
    SMALL_RECT dirty={0,0,7,1};
    worker_base_publication *publisher;
    ntvdm_console_graphics *graphics;
    HANDLE surface,shutdown,mutex;
    DWORD index;

    bitmap.header.biSize=sizeof(bitmap.header);bitmap.header.biWidth=8;bitmap.header.biHeight=-2;
    bitmap.header.biPlanes=1;bitmap.header.biBitCount=8;bitmap.header.biSizeImage=16;
    info.lpBitMapInfo=(BITMAPINFO *)&bitmap;info.dwBitMapInfoLength=sizeof(bitmap);
    info.dwUsage=DIB_RGB_COLORS;
    shutdown=CreateEventW(NULL,TRUE,FALSE,NULL);delivered=CreateEventW(NULL,TRUE,FALSE,NULL);
    CHECK(shutdown && delivered);
    graphics=ntvdm_console_graphics_create();CHECK(graphics);current=graphics;
    publisher=worker_base_publication_create(send_frame,NULL,shutdown);CHECK(publisher);
    CHECK(!ntvdm_console_graphics_attach_publisher(graphics,publisher));
    CHECK(!worker_base_publication_active(publisher,TRUE));
    surface=MvdmCreateConsoleScreenBuffer(GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,CONSOLE_GRAPHICS_BUFFER,&info);
    CHECK(surface!=INVALID_HANDLE_VALUE && info.hMutex && info.lpBitMap);
    mutex=info.hMutex;
    /* Window-mode software VGA owns a DIB before the historical physical
     * Console selects it.  A completed host flush must still request one
     * copied publication; ``active`` is not a presentation-route predicate. */
    CHECK(ntvdm_console_graphics_flush()==1);
    CHECK(WaitForSingleObject(delivered,5000)==WAIT_OBJECT_0 && sends==1);
    ResetEvent(delivered);
    CHECK(MvdmSetConsoleActiveScreenBuffer(surface));
    CHECK(WaitForSingleObject(delivered,5000)==WAIT_OBJECT_0 && sends==2);
    ResetEvent(delivered);
    CHECK(WaitForSingleObject(mutex,5000)==WAIT_OBJECT_0);
    memset(info.lpBitMap,0x5a,16);CHECK(ReleaseMutex(mutex));
    for(index=0;index<200;++index)CHECK(ntvdm_console_graphics_invalidate(surface,&dirty)==1);
    CHECK(WaitForSingleObject(delivered,5000)==WAIT_OBJECT_0);
    CHECK(sends==3 && final_pixel==0x5a);
    ResetEvent(delivered);
    CHECK(WaitForSingleObject(delivered,80)==WAIT_TIMEOUT && sends==3);
    CHECK(WaitForSingleObject(mutex,5000)==WAIT_OBJECT_0);
    memset(info.lpBitMap,0x6b,16);CHECK(ReleaseMutex(mutex));
    CHECK(ntvdm_console_graphics_invalidate(surface,&dirty)==1);
    CHECK(!worker_base_publication_active(publisher,FALSE));
    CHECK(sends==4 && final_pixel==0x6b);
    CHECK(MvdmCloseConsoleHandle(surface));
    CloseHandle(mutex);
    worker_base_publication_destroy(publisher);ntvdm_console_graphics_destroy(graphics);current=NULL;
    CloseHandle(delivered);CloseHandle(shutdown);
    /* PAL-colour graphics become active before the palette arrives. The
     * first capture is deliberately deferred, and the palette installation
     * must re-signal the same publisher instead of poisoning it with
     * ERROR_NOT_READY. */
    {
        LOGPALETTE *logical;
        struct { BITMAPINFOHEADER header;WORD colours[256]; } pal_bitmap={0};
        PALETTEENTRY entries[256]={0};
        HANDLE pal_surface,pal_mutex;
        HPALETTE palette;
        shutdown=CreateEventW(NULL,TRUE,FALSE,NULL);delivered=CreateEventW(NULL,TRUE,FALSE,NULL);
        CHECK(shutdown && delivered);
        graphics=ntvdm_console_graphics_create();CHECK(graphics);current=graphics;
        publisher=worker_base_publication_create(send_frame,NULL,shutdown);CHECK(publisher);
        CHECK(!ntvdm_console_graphics_attach_publisher(graphics,publisher));
        CHECK(!worker_base_publication_active(publisher,TRUE));
        pal_bitmap.header.biSize=sizeof(pal_bitmap.header);pal_bitmap.header.biWidth=8;
        pal_bitmap.header.biHeight=-2;pal_bitmap.header.biPlanes=1;pal_bitmap.header.biBitCount=8;
        for(index=0;index<256;++index)pal_bitmap.colours[index]=(WORD)index;
        info.lpBitMapInfo=(BITMAPINFO *)&pal_bitmap;info.dwBitMapInfoLength=sizeof(pal_bitmap);
        info.dwUsage=DIB_PAL_COLORS;
        pal_surface=MvdmCreateConsoleScreenBuffer(GENERIC_READ|GENERIC_WRITE,
            FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,CONSOLE_GRAPHICS_BUFFER,&info);
        CHECK(pal_surface!=INVALID_HANDLE_VALUE && info.hMutex && info.lpBitMap);
        pal_mutex=info.hMutex;
        CHECK(MvdmSetConsoleActiveScreenBuffer(pal_surface));
        CHECK(WaitForSingleObject(delivered,80)==WAIT_TIMEOUT);
        logical=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*logical)+255*sizeof(PALETTEENTRY));
        CHECK(logical);
        logical->palVersion=0x300;logical->palNumEntries=256;
        memcpy(logical->palPalEntry,entries,sizeof(entries));
        palette=CreatePalette(logical);HeapFree(GetProcessHeap(),0,logical);CHECK(palette);
        CHECK(ntvdm_console_graphics_palette(pal_surface,palette,SYSPAL_STATIC)==1);
        CHECK(WaitForSingleObject(delivered,5000)==WAIT_OBJECT_0);
        CHECK(MvdmCloseConsoleHandle(pal_surface));CloseHandle(pal_mutex);
        worker_base_publication_destroy(publisher);ntvdm_console_graphics_destroy(graphics);current=NULL;
        CloseHandle(delivered);CloseHandle(shutdown);
    }
    puts("PASS graphics source: host flush publishes a valid unselected software-VGA DIB; 200 invalidations coalesce before one deferred full-frame capture/send; palette-late DIB defers without poisoning publication");
    return 0;
}
