#include <stdint.h>
#include <stdio.h>
uint32_t CCPU_CR[8],CCPU_SAR[32],CCPU_STAR[16];
extern void SLDT(uint32_t*),SMSW(uint32_t*),SGDT16(uint32_t*),SGDT32(uint32_t*);
int main(void){uint32_t state=0x2b495c31,x,y,pair[2],checks=0;
for(unsigned i=0;i<65536;i++){state=state*1664525u+1013904223u;CCPU_SAR[0]=state;CCPU_CR[0]=~state;x=y=0;SLDT(&x);SMSW(&y);if(x!=(state&0xffff)||y!=((~state)&0xffff))return 1;checks+=2;}
for(unsigned i=0;i<100000;i++){state=state*1664525u+1013904223u;CCPU_STAR[0]=state;CCPU_STAR[1]=i;SGDT16(pair);if(pair[0]!=(i&0xffff)||pair[1]!=state)return 2;SGDT32(pair);if(pair[0]!=(i&0xffff)||pair[1]!=state)return 3;checks+=2;}
printf("PASS SLDT SMSW SGDT16/32 object ownership values %u cases\n",checks);return 0;}
