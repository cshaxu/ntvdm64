#include <stdint.h>
#include <stdio.h>
uint16_t *CCPU_WR[8]; uint8_t *CCPU_BR[8];uint32_t CCPU_FLAGS[32];
extern void AAA(void);extern void AAS(void);
static uint16_t ax;
static int check(int sub,uint16_t value,int af,int cf){uint32_t eax=value,flags,initial=0x202u|(af?0x10u:0)|(cf?1u:0);ax=value;CCPU_FLAGS[4]=af;CCPU_FLAGS[0]=cf;if(sub){AAS();__asm__ volatile("pushl %[v]; popfl; aas; pushfl; popl %[f]" : "+a"(eax),[f]"=r"(flags):[v]"r"(initial):"cc");}else{AAA();__asm__ volatile("pushl %[v]; popfl; aaa; pushfl; popl %[f]" : "+a"(eax),[f]"=r"(flags):[v]"r"(initial):"cc");}if(ax!=(uint16_t)eax||CCPU_FLAGS[4]!=((flags>>4)&1)||CCPU_FLAGS[0]!=(flags&1)){printf("FAIL op=%s ax=%04x af=%d got=%04x:%u:%u cpu=%04x:%u:%u\n",sub?"AAS":"AAA",value,af,ax,CCPU_FLAGS[4],CCPU_FLAGS[0],(uint16_t)eax,(flags>>4)&1,flags&1);return 1;}return 0;}
int main(void){unsigned v,af,cf;CCPU_WR[0]=&ax;CCPU_BR[0]=(uint8_t*)&ax;CCPU_BR[4]=(uint8_t*)&ax+1;for(v=0;v<65536;v++)for(af=0;af<2;af++)for(cf=0;cf<2;cf++)if(check(0,(uint16_t)v,af,cf)||check(1,(uint16_t)v,af,cf))return 1;puts("PASS AAA/AAS: 524288 AX+AF+CF inputs against native 32-bit CPU; defined AX CF AF only");return 0;}
