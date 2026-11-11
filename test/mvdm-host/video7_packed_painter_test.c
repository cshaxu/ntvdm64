/* Calls the actual nt_vga.c painter, with only the native invalidation sink
 * replaced. Source/destination guards cover full frames and clipped damage. */
#include <windows.h>
#include <insignia.h>
#include <host_def.h>
#include <xt.h>
#include <gvi.h>
#include <gmi.h>
#include <gfx_upd.h>
#include <egagraph.h>
#include <egacpu.h>
#include <egaports.h>
#include <conapi.h>
#include <nt_graph.h>
#include <stdio.h>
#include <string.h>
#undef CloseHandle

SCREEN_DESCRIPTION sc;
DISPLAY_GLOBS PCDisplay;
struct EGA_GLOBALS EGA_GRAPH;
int host_screen_scale=2;
byte *EGA_planes;
static unsigned invalidations;
static SMALL_RECT damage;
BOOL WINAPI InvalidateConsoleDIBits(HANDLE output, PSMALL_RECT rect)
{ (void)output; ++invalidations; damage=*rect; return TRUE; }
#define CHECK(x) do { if(!(x)){fprintf(stderr,"FAIL line %d\n",__LINE__);return 1;} } while(0)

int main(void)
{
    enum { width=640,height=480,pitch=1024,guard=64 };
    BYTE *src,*dst;BITMAPINFO info={0};unsigned x,y,calls;
    src=HeapAlloc(GetProcessHeap(),0,4*EGA_PLANE_SIZE);
    dst=HeapAlloc(GetProcessHeap(),0,width*height+2*guard);
    CHECK(src && dst);
    EGA_planes=src;PCDisplay.offset_per_line=pitch;
    sc.ScreenBufHandle=(HANDLE)1;sc.PC_W_Width=width;sc.PC_W_Height=height;
    sc.ConsoleBufInfo.hMutex=CreateMutex(NULL,FALSE,NULL);
    CHECK(sc.ConsoleBufInfo.hMutex);
    info.bmiHeader.biWidth=width;info.bmiHeader.biHeight=-height;
    info.bmiHeader.biBitCount=8;
    sc.ConsoleBufInfo.lpBitMapInfo=&info;sc.ConsoleBufInfo.lpBitMap=dst+guard;
    for(y=0;y<height;++y)for(x=0;x<pitch;++x)
        src[y*pitch+x]=(BYTE)(1+(x+3*y)%253);
    memset(dst,0xa5,width*height+2*guard);
    nt_v7vga_hi_graph_std(0,0,0,width,height);
    CHECK(invalidations==1 && damage.Right==639 && damage.Bottom==479);
    for(y=0;y<height;++y)CHECK(!memcmp(dst+guard+y*width,src+y*pitch,width));
    for(x=0;x<guard;++x)CHECK(dst[x]==0xa5 && dst[guard+width*height+x]==0xa5);
    memset(dst+guard,0,width*height);
    nt_v7vga_hi_graph_std(100,638,478,20,20);
    CHECK(damage.Right==639 && damage.Bottom==479);
    CHECK(dst[guard+478*width+638]==src[100]);
    CHECK(dst[guard+479*width+639]==src[100+pitch+1]);
    CHECK(dst[guard+478*width+637]==0 && dst[guard+477*width+639]==0);
    nt_v7vga_hi_graph_std(4*EGA_PLANE_SIZE-2,0,0,20,20);
    CHECK(damage.Right==1 && damage.Bottom==0);
    calls=invalidations;
    nt_v7vga_hi_graph_std(0,0,0,0,1);
    nt_v7vga_hi_graph_std(0,0,0,1,0);
    nt_v7vga_hi_graph_std(-1,0,0,1,1);
    nt_v7vga_hi_graph_std(0,-1,0,1,1);
    nt_v7vga_hi_graph_std(0,640,0,1,1);
    PCDisplay.offset_per_line=0;nt_v7vga_hi_graph_std(0,0,0,1,1);
    CHECK(invalidations==calls);
    for(x=0;x<guard;++x)CHECK(dst[x]==0xa5 && dst[guard+width*height+x]==0xa5);
    CloseHandle(sc.ConsoleBufInfo.hMutex);
    HeapFree(GetProcessHeap(),0,src);HeapFree(GetProcessHeap(),0,dst);
    puts("PASS actual packed painter: 640x480, pitched rows, clipped damage, source end, invalid input, guards");
    return 0;
}
