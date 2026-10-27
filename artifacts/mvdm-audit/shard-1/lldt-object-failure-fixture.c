#include <stdint.h>
#include <stdio.h>
#include <setjmp.h>
typedef struct {uint16_t selector;int32_t ar_super;uint32_t base,limit;} SAR;
typedef struct {uint32_t base,limit;uint16_t AR;} DESC;
SAR CCPU_SAR[2];uint32_t __security_cookie=0x12345;static jmp_buf j;static int mode,reads,err;
void __attribute__((fastcall)) __security_check_cookie(uint32_t v){(void)v;}
int selector_is_null(uint16_t s){return (s&0xfffcu)==0;}
int selector_outside_GDT(uint16_t s,uint32_t *p){(void)s;*p=0x1000;return mode==1;}
void read_descriptor_linear(uint32_t p,DESC *d){(void)p;reads++;d->base=0x45670000;d->limit=0x1234;d->AR=mode==3?2:0x82;}
int descriptor_super_type(uint16_t ar){return mode==2 ? 3 : (ar&0x1f);}
void GP(uint16_t s,uint16_t code){(void)s;err=code;longjmp(j,1);}
void NP(uint16_t s,uint16_t code){(void)s;err=code;longjmp(j,2);}
extern void LLDT(uint32_t);
#define CHECK(x) do{if(!(x)){printf("FAIL %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(void){int jump;CCPU_SAR[0].selector=0x29;CCPU_SAR[0].base=0xbeef;CCPU_SAR[0].limit=0xcafe;mode=0;LLDT(0);CHECK(!CCPU_SAR[0].selector&&!CCPU_SAR[0].base&&!CCPU_SAR[0].limit);
for(mode=1;mode<=3;mode++){CCPU_SAR[0].selector=0x29;CCPU_SAR[0].base=0xbeef;CCPU_SAR[0].limit=0xcafe;reads=0;err=0;jump=setjmp(j);if(!jump)LLDT(0x20);CHECK(jump==(mode==3?2:1));CHECK(err==(mode==1?0x86:mode==2?0x87:0x88));CHECK(CCPU_SAR[0].selector==0x29&&CCPU_SAR[0].base==0xbeef&&CCPU_SAR[0].limit==0xcafe);CHECK(reads==(mode!=1));}
mode=0;reads=0;LLDT(0x20);CHECK(reads==1&&CCPU_SAR[0].selector==0x20&&CCPU_SAR[0].base==0x45670000&&CCPU_SAR[0].limit==0x1234);puts("PASS LLDT null, GDT range, wrong type, not-present, valid; faults preserve state");return 0;}
