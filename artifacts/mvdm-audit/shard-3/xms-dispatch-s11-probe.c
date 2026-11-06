#include <stdio.h>
static unsigned selected, calls;
extern int XMSDispatch(unsigned long service);
void xmsA20(void) { selected=0; ++calls; }
void xmsMoveBlock(void) { selected=1; ++calls; }
void xmsAllocBlock(void) { selected=2; ++calls; }
void xmsFreeBlock(void) { selected=3; ++calls; }
void xmsSysPageSize(void) { selected=4; ++calls; }
void xmsQueryExtMem(void) { selected=5; ++calls; }
void xmsInitUMB(void) { selected=6; ++calls; }
void xmsRequestUMB(void) { selected=7; ++calls; }
void xmsReleaseUMB(void) { selected=8; ++calls; }
void xmsNotifyHookI15(void) { selected=9; ++calls; }
void xmsQueryFreeExtMem(void) { selected=10; ++calls; }
void xmsReallocBlock(void) { selected=11; ++calls; }
int main(void) {
    unsigned i;
    for(i=0;i<12;++i) {
        selected=99;calls=0;
        if(!XMSDispatch(i) || selected!=i || calls!=1) return 1;
    }
    puts("XMS_PRIVATE_DISPATCH_PASS services=12 actual_dispatch_object=1 sentinel_handlers=1");
    return 0;
}
