/* Controlled current DPMI buffer/state bodies; not guest acceptance. */

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define ASSERT(x) ((void)0)
#define SMALL_XLAT_BUFFER_SIZE 128
#define LARGE_XLAT_BUFFER_SIZE 8192
#define MAX_V86_ADDRESS 0x100000
typedef USHORT *PWORD16;
typedef struct { UCHAR Misc[28]; } VSAVEDSTATE;
static PUCHAR IntelBase, SmallXlatBuffer, LargeXlatBuffer, mapped;
static ULONG LargeBufferInUseCount, selectorLimit;
static BOOL SmallBufferInUse;
static USHORT regs[5];
static ULONG esp;
#define getSS() regs[0]
#define getDS() regs[1]
#define getES() regs[2]
#define getFS() regs[3]
#define getGS() regs[4]
#define getESP() esp
#define setSS(v) (regs[0]=(v))
#define setDS(v) (regs[1]=(v))
#define setES(v) (regs[2]=(v))
#define setFS(v) (regs[3]=(v))
#define setGS(v) (regs[4]=(v))
#define setESP(v) (esp=(v))
#define GET_SELECTOR_LIMIT(seg, limit) ((limit)=selectorLimit)
static PUCHAR Sim32GetVDMPointer(ULONG p, ULONG n, BOOL pm)
{ (void)p; (void)n; (void)pm; return mapped; }
PUCHAR DpmiAllocateBuffer(USHORT);
VOID DpmiFreeBuffer(PUCHAR, USHORT);

#include "dpmi-buffer-before.inc"
#include "dpmi-buffer-after.inc"
#include "dpmi-state.inc"

static unsigned checks;
#define CHECK(x) do { ++checks; if(!(x)) { printf("FAIL line=%d\n",__LINE__); return 1; } } while(0)
int main(void) {
    PUCHAR a,b,c,high;
    USHORT length;
    unsigned i,j;
    union { ULONG align; VSAVEDSTATE value; } saved[16];
    CHECK(sizeof(void*)==4);
    CHECK(sizeof(SAVEDCONTEXT)<sizeof(VSAVEDSTATE));
    IntelBase=calloc(1,0x200000);
    CHECK(IntelBase!=NULL);
    SmallXlatBuffer=IntelBase+0x1000; LargeXlatBuffer=IntelBase+0x2000;
    high=IntelBase+0x110000;
    for(i=0;i<8192;i++) high[i]=(UCHAR)(i*17);
    a=DpmiMapAndCopyBuffer(IntelBase+0x9000,128);
    CHECK(a==IntelBase+0x9000 && !SmallBufferInUse && !LargeBufferInUseCount);
    DpmiUnmapAndCopyBuffer(a,a,128);
    a=DpmiMapAndCopyBuffer(high,128);
    CHECK(a==SmallXlatBuffer && SmallBufferInUse && !memcmp(a,high,128));
    b=DpmiAllocateBuffer(128); CHECK(b==LargeXlatBuffer && LargeBufferInUseCount==128);
    c=DpmiAllocateBuffer(4096); CHECK(c==LargeXlatBuffer+128 && LargeBufferInUseCount==4224);
    DpmiFreeBuffer(c,4096); DpmiFreeBuffer(b,128);
    CHECK(LargeBufferInUseCount==0);
    a[7]=0x81; DpmiUnmapAndCopyBuffer(high,a,128);
    CHECK(high[7]==0x81 && !SmallBufferInUse);
    a=DpmiMapAndCopyBuffer(high,4096);
    CHECK(a==LargeXlatBuffer && !memcmp(a,high,4096));
    DpmiUnmapBuffer(a,4096); CHECK(!LargeBufferInUseCount);
    a=DpmiAllocateBuffer(8192); CHECK(a==LargeXlatBuffer && LargeBufferInUseCount==8192);
    b=DpmiAllocateBuffer(128); CHECK(b==SmallXlatBuffer);
    c=DpmiAllocateBuffer(1); CHECK(c==(PUCHAR)0xf00df00d);
    DpmiFreeAllBuffers(); CHECK(!SmallBufferInUse && !LargeBufferInUseCount);
    CHECK(DpmiCalcFcbLength((PUCHAR)"x")==0x25);
    CHECK(DpmiCalcFcbLength((PUCHAR)"\xff")==0x2c);
    mapped=high; selectorLimit=255; memset(high,0,256); memcpy(high,"ABC",3);
    a=DpmiMapString(8,0,&length);
    CHECK(length==8 && !memcmp(a,high,length));
    DpmiUnmapString(a,length); CHECK(!SmallBufferInUse);
    a=DpmiMapAndCopyBuffer(IntelBase+MAX_V86_ADDRESS-128,128);
    CHECK(a==SmallXlatBuffer); DpmiUnmapBuffer(a,128);
    for(i=0;i<16;i++) {
        for(j=0;j<5;j++) regs[j]=(USHORT)(i*8+j);
        esp=0x12340000+i;
        DpmiSaveSegmentsAndStack(&saved[i].value);
    }
    for(i=16;i--;) {
        memset(regs,0xff,sizeof regs); esp=0;
        CHECK(DpmiRestoreSegmentsAndStack()==&saved[i].value);
        CHECK(esp==0x12340000+i);
        for(j=0;j<5;j++) CHECK(regs[j]==i*8+j);
    }
    CHECK(StateStack==NULL);
    free(IntelBase);
    printf("PASS %u controlled original buffer/state checks; exhaustion sentinel retained\n", checks);
    return 0;
}
