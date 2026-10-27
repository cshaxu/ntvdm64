#include <stdint.h>
#include <stdio.h>
uint16_t *CCPU_WR[8]; uint8_t *CCPU_BR[8];
void CBW(void);void CWD(void);
static unsigned host_cbw(unsigned value) { unsigned result;
 __asm { mov eax,value }
 __asm { cbw }
 __asm { mov result,eax }
 return result;
}
static unsigned host_cwd(unsigned value) { unsigned result;
 __asm { mov eax,value }
 __asm { cwd }
 __asm { mov result,edx }
 return result&0xffff;
}
int main(void) {
 uint32_t eax_reg,edx_reg; unsigned i, failures=0;
 CCPU_WR[0]=(uint16_t*)&eax_reg; CCPU_WR[2]=(uint16_t*)&edx_reg;
 CCPU_BR[0]=(uint8_t*)&eax_reg; CCPU_BR[4]=((uint8_t*)&eax_reg)+1;
 for(i=0;i<65536;i++) {
  eax_reg=0xa55a0000u|i; edx_reg=0x1357bdf0u; CWD();
  if(eax_reg!=(0xa55a0000u|i) || edx_reg!=(0x13570000u|host_cwd(i))) failures++;
  eax_reg=0xa55a0000u|i; edx_reg=0x1357bdf0u; CBW();
  if(eax_reg!=host_cbw(0xa55a0000u|i) || edx_reg!=0x1357bdf0u) failures++;
 }
 printf("CBW CWD native-reference inputs=65536 each failures=%u\n",failures); return failures?1:0;
}
