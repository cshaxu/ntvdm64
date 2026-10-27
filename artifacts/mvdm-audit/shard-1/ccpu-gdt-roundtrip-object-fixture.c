#include <stdint.h>
#include <stdio.h>
uint32_t CCPU_STAR[16];
extern void LGDT16(uint32_t*),LGDT32(uint32_t*),SGDT16(uint32_t*),SGDT32(uint32_t*);
int main(void){uint32_t s=0x57b39343,in[2],out[2];unsigned checks=0;
for(unsigned i=0;i<100000;i++){s=s*1664525u+1013904223u;in[0]=s;s=s*1664525u+1013904223u;in[1]=s;
LGDT16(in);SGDT16(out);if(out[0]!=(in[0]&65535u)||out[1]!=(in[1]&0xffffffu))return 1;checks++;
LGDT32(in);SGDT32(out);if(out[0]!=(in[0]&65535u)||out[1]!=in[1])return 2;checks++;}
printf("PASS LGDT16/32 -> SGDT16/32 selected objects %u pairs\n",checks);return 0;}
