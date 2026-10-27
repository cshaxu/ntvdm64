#include <windows.h>
#include <stdint.h>
#include <stdio.h>
uint8_t *CCPU_BR[8]; uint32_t CCPU_FLAGS[32]; uint8_t pf_table[256];
void AAD(uint32_t); void DAA(void);
static uint32_t native_flags,native_value;
static void host_aad(uint32_t value) {
 __asm { mov eax,value }
 __asm { _emit 0xd5 }
 __asm { _emit 0x0a }
 __asm { mov native_value,eax }
 __asm { pushfd }
 __asm { pop eax }
 __asm { mov native_flags,eax }
}
static void host_daa(uint32_t value,uint32_t initial) {
 __asm { pushfd }
 __asm { pop ecx }
 __asm { and ecx,0xffffffee }
 __asm { or ecx,initial }
 __asm { push ecx }
 __asm { popfd }
 __asm { mov eax,value }
 __asm { daa }
 __asm { mov native_value,eax }
 __asm { pushfd }
 __asm { pop eax }
 __asm { mov native_flags,eax }
}
static int eqflags(uint32_t mask) {
 unsigned j;for(j=0;j<32;j++)if((mask&(1u<<j)) && CCPU_FLAGS[j]!=((native_flags>>j)&1u))return 0;
 return 1;
}
int main(void) {
 uint32_t eax_reg,i,k,fail=0,bit,count;
 CCPU_BR[0]=(uint8_t*)&eax_reg; CCPU_BR[4]=(uint8_t*)&eax_reg+1;
 for(i=0;i<256;i++){count=0;for(bit=0;bit<8;bit++)count+=(i>>bit)&1;pf_table[i]=(count%2)==0;}
 for(i=0;i<65536;i++){
  eax_reg=0xa55a0000u|i; CCPU_FLAGS[0]=CCPU_FLAGS[4]=0; AAD(10);host_aad(0xa55a0000u|i);
  if(eax_reg!=native_value || !eqflags(0xc4))fail++;
 }
 for(i=0;i<256;i++)for(k=0;k<4;k++){
  uint32_t initial=(k&1)|((k&2)?0x10:0);
  eax_reg=0xa55a1100u|i;CCPU_FLAGS[0]=initial&1;CCPU_FLAGS[4]=(initial>>4)&1;
  DAA();host_daa(0xa55a1100u|i,initial);
  if(eax_reg!=native_value || !eqflags(0xd5))fail++;
 }
 fprintf(stderr,"AAD ax=65536 base=10; DAA al/af/cf=1024; failures=%u\n",fail);
 return fail?1:0;
}
