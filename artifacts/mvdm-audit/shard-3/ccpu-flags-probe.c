/* Independent harness links the actual product x86 CCPU object code. */
#include <stdint.h>
#include <stdio.h>
uint32_t CCPU_FLAGS[32];
void CLC(void); void CMC(void); void CLD(void);
static unsigned host_cf(unsigned input_flag, unsigned op) {
 unsigned result_bits;
 __asm {
  cmp op,0
  jne case_cmc
  clc
  jmp save_cf
 case_cmc:
  cmp input_flag,0
  je zero_cf
  stc
  jmp toggle_cf
 zero_cf:
  clc
 toggle_cf:
  cmc
 save_cf:
  pushfd
  pop eax
  mov result_bits,eax
 }
 return result_bits&1;
}
static unsigned host_cld(unsigned input_flag) {
 unsigned result_bits;
 __asm {
  cmp input_flag,0
  je zero_df
  std
  jmp clear_df
 zero_df:
  cld
 clear_df:
  cld
  pushfd
  pop eax
  mov result_bits,eax
 }
 return (result_bits>>10)&1;
}
int main(void) {
 unsigned input_flag,op,i, failures=0,cases=0;
 for(op=0;op<3;op++) for(input_flag=0;input_flag<2;input_flag++) {
  for(i=0;i<32;i++) CCPU_FLAGS[i]=0x100+i;
  CCPU_FLAGS[0]=input_flag; CCPU_FLAGS[10]=input_flag;
  if(op==0) CLC(); else if(op==1) CMC(); else CLD();
  if(op<2) { if(CCPU_FLAGS[0]!=host_cf(input_flag,op)) failures++; }
  else if(CCPU_FLAGS[10]!=host_cld(input_flag)) failures++;
  for(i=0;i<32;i++) if(i!=0 && i!=10 && CCPU_FLAGS[i]!=0x100+i) failures++;
  if(op<2 && CCPU_FLAGS[10]!=input_flag) failures++;
  if(op==2 && CCPU_FLAGS[0]!=input_flag) failures++;
  cases++;
 }
 printf("CLC CMC CLD native-reference cases=%u failures=%u\n",cases,failures);
 return failures?1:0;
}
