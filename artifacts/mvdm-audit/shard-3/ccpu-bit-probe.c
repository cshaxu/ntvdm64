#include <stdint.h>
#include <stdio.h>
uint32_t CCPU_FLAGS[32];
void BT(uint32_t,uint32_t,uint32_t);void BTR(uint32_t*,uint32_t,uint32_t);void BSR(uint32_t*,uint32_t,uint32_t);
static uint32_t val,flags;
static void native_bt(uint32_t v,uint32_t n){
 __asm { mov eax,v }
 __asm { mov ecx,n }
 __asm { bt eax,ecx }
 __asm { pushfd }
 __asm { pop eax }
 __asm { mov flags,eax }
}
static void native_btr(uint32_t v,uint32_t n){
 __asm { mov eax,v }
 __asm { mov ecx,n }
 __asm { btr eax,ecx }
 __asm { mov val,eax }
 __asm { pushfd }
 __asm { pop eax }
 __asm { mov flags,eax }
}
static void native_bsr(uint32_t v){
 __asm { mov eax,v }
 __asm { mov edx,0x5a5a5a5a }
 __asm { bsr edx,eax }
 __asm { mov val,edx }
 __asm { pushfd }
 __asm { pop eax }
 __asm { mov flags,eax }
}
int main(void){uint32_t v,n,t,i,fail=0,cases=0;
 for(v=0;v<65536;v++){
  t=0x5a5a5a5a;BSR(&t,v,16);native_bsr(v);if(CCPU_FLAGS[6]!=((flags>>6)&1u)||(v && t!=val)||(!v && t!=0x5a5a5a5a))fail++;cases++;
  for(n=0;n<32;n++){BT(v,n,16);native_bt(v,n%16);if(CCPU_FLAGS[0]!=(flags&1))fail++;cases++;t=v;BTR(&t,n,16);native_btr(v,n%16);if(t!=val||CCPU_FLAGS[0]!=(flags&1))fail++;cases++;}
 }
 for(i=0;i<10000;i++){v=i*2654435761u;n=i*7919u;t=0x5a5a5a5a;BSR(&t,v,32);native_bsr(v);if(CCPU_FLAGS[6]!=((flags>>6)&1u)||(v&&t!=val))fail++;cases++;BT(v,n,32);native_bt(v,n);if(CCPU_FLAGS[0]!=(flags&1))fail++;cases++;t=v;BTR(&t,n,32);native_btr(v,n);if(t!=val||CCPU_FLAGS[0]!=(flags&1))fail++;cases++;}
 printf("BSR BT BTR cases=%u failures=%u\n",cases,fail);return fail?1:0;}
