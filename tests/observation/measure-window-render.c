#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "lib/kvm-window/render.h"

#define SAMPLES 240u
#define CHECK(x) do { if(!(x)){fprintf(stderr,"FAIL line %u error %lu\n",__LINE__,GetLastError());return 1;} } while(0)

static int compare_u64(const void *left,const void *right)
{
    const unsigned __int64 a=*(const unsigned __int64 *)left;
    const unsigned __int64 b=*(const unsigned __int64 *)right;
    return a<b ? -1 : a>b;
}

int main(void)
{
    const lib_u32 width=640u,height=480u;
    kvm_window_frame *frame=calloc(1,sizeof(*frame));
    lib_u32 *surface=calloc((size_t)width*height,sizeof(*surface));
    unsigned __int64 samples[SAMPLES],total=0;
    LARGE_INTEGER frequency,start,end;
    lib_bool valid=LIB_FALSE;
    kvm_window_rect changed;
    unsigned i;
    CHECK(frame && surface && QueryPerformanceFrequency(&frequency));
    frame->valid=LIB_TRUE;frame->graphics=LIB_TRUE;
    frame->image.width=width;frame->image.height=height;frame->image.stride=width;
    frame->image.palette[0]=0;frame->image.palette[1]=0x00ffffffu;
    CHECK(kvm_window_render_frame(frame,surface,width,height,&valid,&changed));
    for(i=0;i<SAMPLES;++i) {
        /* One changing pixel models a software-pointer move.  Rendering still
         * traverses the complete graphics surface to establish its damage. */
        frame->image.pixels[(i*7919u)%((size_t)width*height)]^=1u;
        CHECK(QueryPerformanceCounter(&start));
        CHECK(kvm_window_render_frame(frame,surface,width,height,&valid,&changed));
        CHECK(QueryPerformanceCounter(&end));
        samples[i]=(unsigned __int64)(end.QuadPart-start.QuadPart)*1000000u/frequency.QuadPart;
        total+=samples[i];
    }
    qsort(samples,SAMPLES,sizeof(samples[0]),compare_u64);
    printf("window-render graphics=%ux%u samples=%u per-frame-us p50=%llu p95=%llu max=%llu mean=%llu\n",
        width,height,SAMPLES,samples[SAMPLES/2],samples[(SAMPLES*95u)/100u],samples[SAMPLES-1],total/SAMPLES);
    free(surface);free(frame);return 0;
}
