/* Original CGA/EGA/LUT/munge sources, controlled memory DIB and only
 * the native invalidation sink replaced. No windows or guest are created. */
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
#include <nt_cga.h>
#include <nt_ega.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#undef CloseHandle
SCREEN_DESCRIPTION sc;
DISPLAY_GLOBS PCDisplay;
struct EGA_GLOBALS EGA_GRAPH;
int host_screen_scale=2;
/* Other exported CGA modes refer to host-owned text state and graph services.
 * They are outside this fixture. Fail immediately if a tested path enters one;
 * these boundaries must never stand in for a successful original operation. */
PBYTE textBuffer;
int now_height,now_width;
VOID closeGraphicsBuffer(void){fputs("UNEXPECTED text teardown\n",stderr);exit(90);}
void high_stretch3(unsigned char *buffer,int length)
{(void)buffer;(void)length;fputs("UNEXPECTED mono stretch3\n",stderr);exit(91);}
void high_stretch4(unsigned char *buffer,int length)
{(void)buffer;(void)length;fputs("UNEXPECTED mono stretch4\n",stderr);exit(92);}
enum{PITCH=256,ROWS=64,GUARD=64,BYTES=PITCH*ROWS+2*GUARD};
static unsigned char source[1024],saved_source[1024],frame[BYTES],expected[BYTES];
static unsigned checks,failures,callbacks,cases;
static SMALL_RECT observed;
static int wrong_pixel;
#define CHECK(x) do{++checks;if(!(x)){if(failures<8)fprintf(stderr,"FAIL %d: %s\n",__LINE__,#x);++failures;}}while(0)
BOOL WINAPI InvalidateConsoleDIBits(HANDLE output,PSMALL_RECT rect)
{
    CHECK(output==(HANDLE)1);++callbacks;observed=*rect;
    CHECK(memcmp(frame,expected,sizeof(frame))==0); /* complete pixels before publish */
    return TRUE;
}
typedef void (*PAINTER)(int,int,int,int,int);
static PAINTER ega_med[]={nt_ega_med_graph_std,nt_ega_med_graph_big,nt_ega_med_graph_huge};
static PAINTER ega_hi[]={nt_ega_hi_graph_std,nt_ega_hi_graph_big,nt_ega_hi_graph_huge};
static PAINTER cga_med[]={nt_cga_colour_med_graph_std,nt_cga_colour_med_graph_big,nt_cga_colour_med_graph_huge};
static void clear_case(void)
{memset(frame,0xa5,sizeof(frame));memcpy(expected,frame,sizeof(frame));memcpy(saved_source,source,sizeof(source));}
static void finish_case(unsigned before,int left,int top,int width,int height)
{
    CHECK(callbacks==before+1);
    CHECK(observed.Left==left && observed.Top==top && observed.Right==left+width-1 && observed.Bottom==top+height-1);
    CHECK(memcmp(frame,expected,sizeof(frame))==0);CHECK(memcmp(source,saved_source,sizeof(source))==0);
    SetLastError(0);CHECK(!ReleaseMutex(sc.ConsoleBufInfo.hMutex) && GetLastError()==ERROR_NOT_OWNER);
    ++cases;
}
static void ega_case(unsigned sample,unsigned mask,unsigned scale,int medium)
{
    int width=1+(int)(sample%3),height=1+(int)(sample%2),offset=1+(int)(sample%4);
    int x=8,y=(int)(sample%4),pixels=(int)(8+4*scale),left=x*(int)(2+scale)/2;
    int top=medium?y*(int)(2+scale):(scale==1?(3*y+1)/2:y*(int)(2+scale)/2);
    int row,g,p,ox,repeat,dy=top;unsigned before=callbacks;
    host_screen_scale=2+(int)scale;PCDisplay.offset_per_line=11;EGA_GRAPH.plane_mask=(int)mask;
    memset(source,0x6d,sizeof(source));
    for(row=0;row<height;++row)for(g=0;g<width;++g)for(p=0;p<4;++p)
        source[4*(offset+row*PCDisplay.offset_per_line+g)+p]=(BYTE)(sample+(unsigned)(73*p+19*row+31*g));
    clear_case();
    for(row=0;row<height;++row){
        repeat=medium?(int)(2+scale):(scale==1?((3*(y+row+1)+1)/2-(3*(y+row)+1)/2):1+(int)scale/2);
        for(ox=0;ox<width*pixels;++ox){
            int src_pixel=((2*ox+1)*8)/(2*pixels),group=src_pixel/8,bit=7-src_pixel%8;
            unsigned value=0;int r;
            for(p=0;p<4;++p)if(mask&(1u<<p))value|=((source[4*(offset+row*PCDisplay.offset_per_line+group)+p]>>bit)&1u)<<p;
            for(r=0;r<repeat;++r)expected[GUARD+(dy+r)*PITCH+left+ox]=(BYTE)value;
        }
        dy+=repeat;
    }
    if(wrong_pixel)expected[GUARD+top*PITCH+left]^=1;
    (medium?ega_med[scale]:ega_hi[scale])(offset,x,y,width,height);
    finish_case(before,left,top,width*pixels,dy-top);
}
static void cga_case(unsigned sample,unsigned scale,unsigned parity)
{
    int offset=4+(int)parity,x=8,y=2,len=1+(int)(sample%3),pixels=(int)(8+4*scale);
    int left=x*(int)(2+scale)/2,top=y*(int)(2+scale)/2,ox,r;unsigned before=callbacks;
    host_screen_scale=2+(int)scale;
    for(ox=0;ox<(int)sizeof(source);++ox)source[ox]=(BYTE)(sample+(unsigned)(ox*37));
    clear_case();
    for(ox=0;ox<len*pixels;++ox){
        int src_pixel=((2*ox+1)*4)/(2*pixels),group=src_pixel/4,bit=6-2*(src_pixel%4);
        int address=offset+4*(group/2)+(group%2?(parity?3:1):0);
        BYTE value=(source[address]>>bit)&3;
        for(r=0;r<(int)(2+scale);++r)expected[GUARD+(top+r)*PITCH+left+ox]=value;
    }
    cga_med[scale](offset,x,y,len,1);
    finish_case(before,left,top,len*pixels,(int)(2+scale));
}
int main(int argc,char **argv)
{
    BITMAPINFO info;unsigned sample,mask,scale,parity,before;int medium;
    (void)argv;wrong_pixel=argc>1;
    memset(&info,0,sizeof(info));info.bmiHeader.biWidth=PITCH;info.bmiHeader.biHeight=-ROWS;info.bmiHeader.biBitCount=8;
    sc.ScreenBufHandle=(HANDLE)1;sc.ConsoleBufInfo.lpBitMapInfo=&info;sc.ConsoleBufInfo.lpBitMap=frame+GUARD;
    sc.ConsoleBufInfo.hMutex=CreateMutex(NULL,FALSE,NULL);if(!sc.ConsoleBufInfo.hMutex)return 2;
    EGA_GRAPH.regen_ptr[0]=source;PCDisplay.screen_ptr=source;
    nt_init_ega_med_graph();nt_init_ega_hi_graph();nt_init_cga_colour_med_graph();
    CHECK(sc.BitsPerPixel==8);
    if(wrong_pixel){ega_case(0,15,0,1);}
    else{
        for(sample=0;sample<256;++sample)for(mask=0;mask<16;++mask)
            for(scale=0;scale<3;++scale)for(medium=0;medium<2;++medium)ega_case(sample,mask,scale,medium);
        for(sample=0;sample<256;++sample)for(scale=0;scale<3;++scale)for(parity=0;parity<2;++parity)cga_case(sample,scale,parity);
        /* Reinitialization must leave the same conversion data. */
        nt_init_ega_med_graph();nt_init_ega_hi_graph();nt_init_cga_colour_med_graph();ega_case(91,7,1,1);cga_case(91,1,1);
        before=callbacks;sc.ScreenBufHandle=NULL;
        for(scale=0;scale<3;++scale){ega_med[scale](0,0,0,1,1);ega_hi[scale](0,0,0,1,1);cga_med[scale](0,0,0,1,1);}
        CHECK(callbacks==before);
    }
    CHECK(CloseHandle(sc.ConsoleBufInfo.hMutex));
    printf("PLANAR_OBJECT_CONTRACT checks=%u failures=%u callbacks=%u cases=%u wrong_oracle=%d\n",checks,failures,callbacks,cases,wrong_pixel);
    return failures?1:0;
}
