#include <stdint.h>
#include <stdio.h>
#include <setjmp.h>
uint32_t CCPU_CR[8],CCPU_DR[8],CCPU_TR[8];static int fl,br,tl,seg,regn;static uint16_t val;static jmp_buf fault;
void load_data_seg(int r,uint16_t v){seg++;regn=r;val=v;}void load_stack_seg(uint16_t v){seg++;regn=2;val=v;}
void GP(uint16_t selector,uint16_t code){(void)selector;(void)code;longjmp(fault,1);}
void flush_tlb(void){fl++;}void test_tlb(void){tl++;}void setup_breakpoints(void){br++;}
extern void MOV(uint32_t*,uint32_t),MOV_SR(uint32_t,uint32_t),MOV_CR(uint32_t,uint32_t),MOV_DR(uint32_t,uint32_t),MOV_TR(uint32_t,uint32_t);
#define CHECK(x) do{if(!(x)){printf("FAIL %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(void){uint32_t v=0;MOV(&v,0xdeadbeef);CHECK(v==0xdeadbeef);
 MOV_SR(3,0xabc12345);CHECK(seg==1&&regn==3&&val==0x2345);MOV_SR(2,0x9876);CHECK(seg==2&&regn==2&&val==0x9876);MOV_SR(1,7);CHECK(seg==2);
 CCPU_CR[0]=0x10;MOV_CR(0,1);CHECK(CCPU_CR[0]==0x11);MOV_CR(0,0xffffffff);CHECK(CCPU_CR[0]==0x8000001f);CCPU_CR[0]=0x10;
 if(!setjmp(fault)){MOV_CR(0,0x80000000);CHECK(0);}CHECK(CCPU_CR[0]==0x10);
 MOV_CR(3,0x12345fff);CHECK(CCPU_CR[3]==0x12345000&&fl==1);MOV_CR(2,0x12345678);CHECK(CCPU_CR[2]==0x12345678);
 MOV_DR(0,0x1234);CHECK(CCPU_DR[0]==0x1234&&br==1);MOV_DR(6,0xffffffff);CHECK(CCPU_DR[6]==0xe00f&&br==1);MOV_DR(7,0xffffffff);CHECK(CCPU_DR[7]==0xffff03ff&&br==2);
 MOV_TR(6,0x42);CHECK(CCPU_TR[6]==0x42&&tl==1);MOV_TR(7,0x99);CHECK(CCPU_TR[7]==0x99&&tl==1);
 puts("PASS MOV object register branch, masks, callback counts, PG/PE reject");return 0;}
