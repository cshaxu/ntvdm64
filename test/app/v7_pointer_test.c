#include <stdio.h>
#include <string.h>
#include "ntvdm-exe/softpc/v7_pointer.h"
#define CHECK(x) do { if (!(x)) { printf("FAIL line %d\n",__LINE__); return 1; } } while (0)

int main(void)
{
    BYTE pixels[10*6], before[sizeof(pixels)], mask[256];
    mvdm_v7_pointer pointer={0}; SMALL_RECT dirty;
    memset(pixels,0x35,sizeof(pixels)); memcpy(before,pixels,sizeof(pixels));
    memset(mask,0xff,128); memset(mask+128,0,128);
    mask[0]=0x7f; mask[128]=0x40; /* black, inverted, then transparent */
    CHECK(mvdm_v7_pointer_paint(&pointer,pixels,8,6,10,mask,sizeof(mask),2,1,&dirty));
    CHECK(dirty.Left==2 && dirty.Top==1 && dirty.Right==7 && dirty.Bottom==5);
    CHECK(pixels[12]==0 && pixels[13]==(BYTE)(0x35^0xff) && pixels[14]==0x35);
    CHECK(pixels[18]==0x35 && pixels[19]==0x35); /* row padding untouched */
    CHECK(mvdm_v7_pointer_paint(&pointer,pixels,8,6,10,mask,sizeof(mask),1,0,&dirty));
    CHECK(dirty.Left==1 && dirty.Top==0 && dirty.Right==7 && dirty.Bottom==5);
    CHECK(mvdm_v7_pointer_clear(&pointer,pixels,8,6,10,&dirty));
    CHECK(!memcmp(pixels,before,sizeof(pixels)));
    CHECK(!mvdm_v7_pointer_clear(&pointer,pixels,8,6,10,&dirty));
    CHECK(!mvdm_v7_pointer_paint(&pointer,pixels,8,6,10,mask,255,1,1,&dirty));
    CHECK(!memcmp(pixels,before,sizeof(pixels)));
    CHECK(!mvdm_v7_pointer_paint(&pointer,pixels,8,6,7,mask,sizeof(mask),1,1,&dirty));
    CHECK(mvdm_v7_pointer_paint(&pointer,pixels,8,6,10,mask,sizeof(mask),7,5,&dirty));
    CHECK(dirty.Left==7 && dirty.Right==7 && dirty.Top==5 && dirty.Bottom==5);
    CHECK(mvdm_v7_pointer_paint(&pointer,pixels,8,6,10,mask,sizeof(mask),8,6,&dirty));
    CHECK(!pointer.visible && !memcmp(pixels,before,sizeof(pixels)));
    pointer.visible=TRUE; pointer.left=0; pointer.top=0; pointer.right=32; pointer.bottom=0;
    CHECK(!mvdm_v7_pointer_clear(&pointer,pixels,40,1,40,&dirty));
    CHECK(!pointer.visible && !memcmp(pixels,before,sizeof(pixels)));
    puts("PASS V7 AND/XOR, clipping, movement, restore, damage and invalid inputs");
    return 0;
}
