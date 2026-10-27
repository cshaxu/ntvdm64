#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
uint32_t CCPU_FLAGS[32];uint8_t pf_table[256];
void do_multiple_shiftrot_of(int x){(void)x;}
extern void SAR(uint32_t*,uint32_t,uint8_t);extern void SHR(uint32_t*,uint32_t,uint8_t);extern void ROR(uint32_t*,uint32_t,uint8_t);
static int probe(unsigned op,unsigned width,uint32_t input,unsigned count){
 uint32_t mask=width==8?255u:width==16?65535u:0xffffffffu, cpu=input&mask,out=cpu,flags;unsigned bits[]={0,2,6,7,11};
 for(unsigned i=0;i<32;i++)CCPU_FLAGS[i]=7;CCPU_FLAGS[0]=1;
 if(op==0)SAR(&out,count,width);else if(op==1)SHR(&out,count,width);else ROR(&out,count,width);
 if(width==8){if(op==0)__asm__ volatile("stc; sarb %%cl,%b0; pushfl; popl %1":"+a"(cpu),"=r"(flags):"c"(count):"cc");else if(op==1)__asm__ volatile("stc; shrb %%cl,%b0; pushfl; popl %1":"+a"(cpu),"=r"(flags):"c"(count):"cc");else __asm__ volatile("stc; rorb %%cl,%b0; pushfl; popl %1":"+a"(cpu),"=r"(flags):"c"(count):"cc");}
 else if(width==16){if(op==0)__asm__ volatile("stc; sarw %%cl,%w0; pushfl; popl %1":"+a"(cpu),"=r"(flags):"c"(count):"cc");else if(op==1)__asm__ volatile("stc; shrw %%cl,%w0; pushfl; popl %1":"+a"(cpu),"=r"(flags):"c"(count):"cc");else __asm__ volatile("stc; rorw %%cl,%w0; pushfl; popl %1":"+a"(cpu),"=r"(flags):"c"(count):"cc");}
 else{if(op==0)__asm__ volatile("stc; sarl %%cl,%0; pushfl; popl %1":"+a"(cpu),"=r"(flags):"c"(count):"cc");else if(op==1)__asm__ volatile("stc; shrl %%cl,%0; pushfl; popl %1":"+a"(cpu),"=r"(flags):"c"(count):"cc");else __asm__ volatile("stc; rorl %%cl,%0; pushfl; popl %1":"+a"(cpu),"=r"(flags):"c"(count):"cc");}
 if((out&mask)!=(cpu&mask))return 1;
 if((count&31)==0){if(CCPU_FLAGS[0]!=1||CCPU_FLAGS[2]!=7||CCPU_FLAGS[6]!=7||CCPU_FLAGS[7]!=7||CCPU_FLAGS[11]!=7)return 2;return 0;}
 for(unsigned i=0;i<5;i++){unsigned n=bits[i];if((count&31)>1 && n==11)continue;if(op==2 && n!=0 && n!=11){if(CCPU_FLAGS[n]!=7)return 3;continue;}if(CCPU_FLAGS[n]!=((flags>>n)&1))return 4;}
 return 0;
}
int main(void){unsigned long checks=0;uint32_t state=0x146bb791;for(unsigned i=0;i<256;i++)pf_table[i]=!__builtin_parity(i);for(unsigned op=0;op<3;op++)for(unsigned w=8;w<=32;w*=2)for(unsigned count=2;count<w && count<32;count++){unsigned n=w==8?256:4096;for(unsigned i=0;i<n;i++){state=state*1664525u+1013904223u;uint32_t x=w==8?i:state;int rc=probe(op,w,x,count);if(rc){printf("FAIL op=%u width=%u count=%u x=%x rc=%d\n",op,w,count,x,rc);return rc;}checks++;}}for(unsigned op=0;op<3;op++)for(unsigned w=8;w<=32;w*=2)for(unsigned count=32;count<=33;count++)for(unsigned i=0;i<256;i++){state=state*1664525u+1013904223u;int rc=probe(op,w,state,count);if(rc){printf("MASK FAIL op=%u w=%u count=%u rc=%d\n",op,w,count,rc);return rc;}checks++;}printf("SAR SHR ROR native defined flags/results matched: %lu cases incl masked counts 32,33\n",checks);return 0;}
