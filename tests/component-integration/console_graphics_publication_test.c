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
    CHECK(MvdmSetConsoleActiveScreenBuffer(surface));
    CHECK(WaitForSingleObject(delivered,5000)==WAIT_OBJECT_0 && sends==1);
    ResetEvent(delivered);
    CHECK(WaitForSingleObject(mutex,5000)==WAIT_OBJECT_0);
    memset(info.lpBitMap,0x5a,16);CHECK(ReleaseMutex(mutex));
    for(index=0;index<200;++index)CHECK(ntvdm_console_graphics_invalidate(surface,&dirty)==1);
    CHECK(WaitForSingleObject(delivered,5000)==WAIT_OBJECT_0);
    CHECK(sends==2 && final_pixel==0x5a);
    ResetEvent(delivered);
    CHECK(WaitForSingleObject(delivered,80)==WAIT_TIMEOUT && sends==2);
    CHECK(WaitForSingleObject(mutex,5000)==WAIT_OBJECT_0);
    memset(info.lpBitMap,0x6b,16);CHECK(ReleaseMutex(mutex));
    CHECK(ntvdm_console_graphics_invalidate(surface,&dirty)==1);
    CHECK(!worker_base_publication_active(publisher,FALSE));
    CHECK(sends==3 && final_pixel==0x6b);
    CHECK(MvdmCloseConsoleHandle(surface));
    CloseHandle(mutex);
    worker_base_publication_destroy(publisher);ntvdm_console_graphics_destroy(graphics);current=NULL;
    CloseHandle(delivered);CloseHandle(shutdown);
    puts("PASS graphics source: 200 DIB invalidations coalesce before one deferred full-frame capture/send");
    return 0;
}
