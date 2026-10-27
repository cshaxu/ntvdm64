#include <stdint.h>
#include <stdio.h>
uint32_t CCPU_FLAGS[32];
extern void ARPL(uint32_t *,uint32_t);
int main(void){unsigned d,s;uint32_t out,cpu;unsigned char z;unsigned long checks=0;
 for(d=0;d<=65535;d++)for(s=0;s<4;s++){
  out=0xbeef0000u|d;cpu=out;CCPU_FLAGS[6]=99; ARPL(&out,s);
  __asm__ volatile("arpl %w2,%w0; setz %1" : "+r"(cpu),"=qm"(z) : "r"(s) : "cc");
  if(out!=cpu||CCPU_FLAGS[6]!=z){printf("FAIL d=%x s=%x out=%x cpu=%x z=%u/%u\n",d,s,out,cpu,(unsigned)CCPU_FLAGS[6],(unsigned)z);return 1;}checks++;
 }
 printf("ARPL defined destination and ZF match native x86, %lu cases\n",checks);return 0;}
