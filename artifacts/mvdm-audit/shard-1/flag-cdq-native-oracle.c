#include <stdint.h>
#include <stdio.h>
uint8_t *CCPU_BR[8];uint32_t CCPU_GR[8],CCPU_FLAGS[32];extern void STC(void);extern void LAHF(void);extern void CDQ(void);
static const unsigned bits[]={0,2,4,6,7,11};static uint32_t unpack(unsigned v){uint32_t f=0x202;for(int i=0;i<6;i++)if(v&(1u<<i))f|=1u<<bits[i];return f;}
static void seed(uint32_t f){for(int i=0;i<6;i++)CCPU_FLAGS[bits[i]]=(f>>bits[i])&1;}
static int check(uint32_t f){for(int i=0;i<6;i++)if(CCPU_FLAGS[bits[i]]!=((f>>bits[i])&1))return 0;return 1;}
int main(void){uint16_t ax;uint32_t flags,native,reg,dx;unsigned i,j;uint64_t count=0;CCPU_BR[4]=(uint8_t*)&ax+1;
for(i=0;i<64;i++){uint32_t input=unpack(i);seed(input);STC();__asm__ volatile("pushl %1; popfl; stc; pushfl; popl %0":"=r"(flags):"r"(input):"cc");if(!check(flags)){printf("FAIL STC %u\n",i);return 1;}count++;
for(j=0;j<256;j++){input=unpack(i);seed(input);ax=(uint16_t)(0xa500|j);LAHF();reg=0xa500|j;__asm__ volatile("pushl %2; popfl; lahf; pushfl; popl %1":"+a"(reg),"=r"(flags):"r"(input):"cc");if(ax!=(uint16_t)reg||!check(flags)){printf("FAIL LAHF flags=%u al=%u ours=%04x native=%04x\n",i,j,ax,(uint16_t)reg);return 1;}count++;}}
uint32_t state=0x6e4f9a13;for(i=0;i<100000;i++){state=state*1664525u+1013904223u;uint32_t edge[]={0,0x7fffffffu,0x80000000u,0xffffffffu};reg=i<4?edge[i]:state;dx=0xabcdef01;uint32_t input=unpack(i&63);seed(input);CCPU_GR[0]=reg;CCPU_GR[2]=dx;CDQ();native=reg;__asm__ volatile("pushl %3; popfl; cltd; pushfl; popl %2":"+a"(native),"+d"(dx),"=r"(flags):"r"(input):"cc");if(CCPU_GR[0]!=native||CCPU_GR[2]!=dx||!check(flags)){printf("FAIL CDQ eax=%08x ours=%08x native=%08x\n",reg,CCPU_GR[2],dx);return 1;}count++;}printf("PASS LAHF/STC/CDQ %llu cases; selected flags and register outputs match native 32-bit CPU\n",(unsigned long long)count);return 0;}
