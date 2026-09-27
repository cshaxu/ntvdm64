/* Compile extracted, unchanged nt_mouse.c function bodies. This isolates the
 * original coordinate contract; it is not a guest/IRQ or production-path test. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef int16_t IS16;
typedef uint8_t half_word;
#define FALSE 0
static int bFunctionZeroReset, bFunctionFour;
static IS16 newF4x, newF4y, VirtualX, VirtualY;
static struct { int bF7,bF8; IS16 xmin,xmax,ymin,ymax; } confine;
static half_word bios_rows;
static void sas_load(unsigned address,half_word *value)
{
    if(address!=0x484)abort();
    *value=bios_rows;
}
#include "original_mouse_algorithms.h"

static unsigned checks;
static void expect(const char *name,IS16 x,IS16 y,int wanted_x,int wanted_y)
{
    if(x!=wanted_x || y!=wanted_y) {
        fprintf(stderr,"FAIL %s: %d,%d expected %d,%d\n",name,x,y,wanted_x,wanted_y);
        exit(1);
    }
    ++checks;
    printf("PASS %s: %d,%d\n",name,x,y);
}
int main(void)
{
    IS16 x,y;unsigned i;
    const half_word rows[]={24,42,49};
    const int heights[]={199,343,399};
    VirtualX=640;VirtualY=200;bios_rows=24;
    bFunctionZeroReset=1;
    EmulateCoordinates(3,0,0,&x,&y);
    expect("reset-center",x,y,319,99);
    EmulateCoordinates(3,9,-4,&x,&y);
    expect("relative-motion",x,y,328,95);
    newF4x=80;newF4y=40;bFunctionFour=1;
    EmulateCoordinates(3,0,0,&x,&y);
    expect("guest-position",x,y,80,40);
    EmulateCoordinates(3,3,2,&x,&y);
    expect("motion-after-guest-position",x,y,83,42);
    EmulateCoordinates(3,0,0,&x,&y);
    expect("button-only-no-motion",x,y,83,42);
    EmulateCoordinates(3,-1000,-1000,&x,&y);
    expect("negative-limit",x,y,0,0);
    for(i=0;i<3;++i) {
        bios_rows=rows[i];x=1000;y=1000;
        LimitCoordinates(3,&x,&y);
        expect("text-row-limit",x,y,639,heights[i]);
    }
    x=1000;y=1000;LimitCoordinates(0x13,&x,&y);
    expect("mode13-virtual-width-not-raster-width",x,y,639,199);
    x=1000;y=1000;LimitCoordinates(0x12,&x,&y);
    expect("mode12-limit",x,y,639,479);
    confine.bF7=confine.bF8=1;
    confine.xmin=20;confine.xmax=120;confine.ymin=10;confine.ymax=90;
    x=-1;y=1000;LimitCoordinates(3,&x,&y);
    expect("guest-text-range",x,y,20,90);
    x=1000;y=-1;LimitCoordinates(0x13,&x,&y);
    expect("guest-graphics-range",x,y,120,10);
    confine.bF7=confine.bF8=0;
    bFunctionFour=1;newF4x=100;newF4y=50;
    EmulateCoordinates(3,7,9,&x,&y);
    expect("pending-position-consumes-this-sample",x,y,100,50);
    /* A relative transport must synchronize the original pending position
     * with zero motion before submitting its first real displacement. */
    bFunctionFour=1;newF4x=100;newF4y=50;
    EmulateCoordinates(3,0,0,&x,&y);
    EmulateCoordinates(3,7,9,&x,&y);
    expect("sync-then-motion-preserves-first-delta",x,y,107,59);
    bios_rows=24;bFunctionZeroReset=1;bFunctionFour=0;
    EmulateCoordinates(3,0,0,&x,&y);
    EmulateCoordinates(3,7,9,&x,&y);
    expect("reset-sync-then-motion",x,y,326,108);
    /* Characterize the historical signed-short arithmetic, not a new
     * product feature. SHRT-sized pieces alone do not make a relative host
     * vector safe: adding one to a nonzero coordinate may wrap before clamp. */
    bFunctionFour=1;newF4x=100;newF4y=50;
    EmulateCoordinates(3,0,0,&x,&y);
    EmulateCoordinates(3,INT16_MAX,0,&x,&y);
    expect("signed-short-wrap-before-clamp",x,y,0,50);
    printf("PASS original-coordinate-contract checks=%u; no guest/IRQ claim\n",checks);
    return 0;
}
