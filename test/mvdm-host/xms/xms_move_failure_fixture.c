/* Isolated real xmsMoveBlock caller; memory and termination seams only.
 * This proves leaf control flow, not real CPU or product shutdown. */
#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <setjmp.h>
static unsigned char memory[131072];
static uint16_t ax;
static unsigned moves, terminated, checks, failures;
static int reject_descriptor;
static jmp_buf escape;
UINT VdmExitCode;
PVOID ExtMemSA;
void xmsMoveBlock(void);
USHORT getSS(void) { return 0; }
USHORT getBP(void) { return 0x100; }
void setAX(USHORT value) { ax=value; }
void fixture_unexpected(void) { abort(); }
void TerminateVDM(void) { ++terminated; longjmp(escape,1); }
int mvdm_softpc_guest_memory_range_valid(uint32_t a,uint32_t n,int write)
{ (void)write; return a<=sizeof(memory) && n<=sizeof(memory)-a; }
int mvdm_softpc_guest_memory_copy_from(uint32_t a,uint8_t *out,uint32_t n)
{
    if(reject_descriptor || !mvdm_softpc_guest_memory_range_valid(a,n,0)) return 0;
    memcpy(out,memory+a,n); return 1;
}
void c_sas_move_bytes_forward(uint32_t src,uint32_t dst,uint32_t n)
{ uint32_t i; ++moves; for(i=0;i<n;++i) memory[dst+i]=memory[src+i]; }
static void descriptor(uint32_t dst,uint32_t src,uint32_t words)
{ memcpy(memory+0xf4,&dst,4);memcpy(memory+0xf8,&src,4);memcpy(memory+0xfc,&words,4); }
#define CHECK(x) do { ++checks; if(!(x)) { ++failures; printf("FAIL %u %s\n",__LINE__,#x); } } while(0)
int main(void)
{
    unsigned i;
    int returned;
    for(i=0;i<64;++i) memory[0x1000+i]=(unsigned char)i;
    descriptor(0x2000,0x1000,32); xmsMoveBlock();
    CHECK(ax==1 && moves==1 && !memcmp(memory+0x1000,memory+0x2000,64));
    descriptor(0x1000,0x1008,16); xmsMoveBlock();
    CHECK(ax==1 && moves==2 && memory[0x1000]==8 && memory[0x101f]==39);
    /* Deliberately test the original forward-copy result, not memmove. */
    memory[0x3000]=0x61;memory[0x3001]=0x62;
    descriptor(0x3002,0x3000,4); xmsMoveBlock();
    CHECK(moves==3 && memory[0x3008]==0x61 && memory[0x3009]==0x62);
    descriptor(0xffffffffu,0xffffffffu,0);
    ax=0xdead; xmsMoveBlock();
    CHECK(ax==1 && moves==3 && terminated==0);
    descriptor(sizeof(memory)-1,0x1000,1);
    memory[sizeof(memory)-1]=0x7b; ax=0xdead;
    returned=0;
    if(setjmp(escape)==0) { xmsMoveBlock(); returned=1; }
    CHECK(moves==3 && memory[sizeof(memory)-1]==0x7b && ax==0xdead);
    CHECK(returned==0 && terminated==1 && VdmExitCode==ERROR_INVALID_ADDRESS);
    reject_descriptor=1;
    returned=0;
    if(setjmp(escape)==0) { xmsMoveBlock(); returned=1; }
    CHECK(moves==3);
    CHECK(returned==0 && terminated==2 && VdmExitCode==ERROR_INVALID_ADDRESS);
    printf("XMS leaf assertions=%u failures=%u terminated=%u\n",checks,failures,terminated);
    return failures?1:0;
}
