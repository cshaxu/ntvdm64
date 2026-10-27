#include <stdint.h>
#include <stdio.h>
uint32_t CCPU_FLAGS[32]; uint8_t pf_table[256];
void ADC(uint32_t*,uint32_t,uint32_t); void INC(uint32_t*,uint32_t); void AND(uint32_t*,uint32_t,uint32_t);
static uint32_t val,flags;
static void native_adc(uint32_t a,uint32_t b,uint32_t c) {
 __asm { mov eax,a }
 __asm { mov edx,b }
 __asm { bt c,0 }
 __asm { adc al,dl }
 __asm { mov val,eax }
 __asm { pushfd }
 __asm { pop eax }
 __asm { mov flags,eax }
}
static void native_and(uint32_t a,uint32_t b) {
 __asm { mov eax,a }
 __asm { mov edx,b }
 __asm { and al,dl }
 __asm { mov val,eax }
 __asm { pushfd }
 __asm { pop eax }
 __asm { mov flags,eax }
}
static void native_inc(uint32_t a,uint32_t c) {
 __asm { mov eax,a }
 __asm { bt c,0 }
 __asm { inc al }
 __asm { mov val,eax }
 __asm { pushfd }
 __asm { pop eax }
 __asm { mov flags,eax }
}
static int eq(uint32_t mask){unsigned i;for(i=0;i<32;i++) if ((mask&(1u<<i)) && CCPU_FLAGS[i]!=((flags>>i)&1u)) return 0; return 1;}
int main(void){uint32_t a,b,c,v,f=0,n=0,i,j,k;
 for(i=0;i<256;i++){j=0;for(k=0;k<8;k++)j+=(i>>k)&1;pf_table[i]=(j%2)==0;}
 for(a=0;a<256;a++)for(b=0;b<256;b++){
  for(c=0;c<2;c++){v=a;CCPU_FLAGS[0]=c;ADC(&v,b,8);native_adc(a,b,c);if(v!=(val&255)||!eq(0x8d5))f++;n++;}
  v=a;AND(&v,b,8);native_and(a,b);if(v!=(val&255)||!eq(0x8c5))f++;n++;
 }
 for(a=0;a<256;a++)for(c=0;c<2;c++){v=a;CCPU_FLAGS[0]=c;INC(&v,8);native_inc(a,c);if(v!=(val&255)||!eq(0x8d5))f++;n++;}
 printf("ADC AND INC 8-bit cases=%u failures=%u\n",n,f);return f?1:0;
}
