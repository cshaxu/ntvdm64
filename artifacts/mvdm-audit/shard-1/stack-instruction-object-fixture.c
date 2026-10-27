#include <stdint.h>
#include <stdio.h>
#include <setjmp.h>
uint32_t CCPU_GR[8];uint16_t *CCPU_WR[8];static uint32_t pushed[8],n,stage,lastsp;static int fail;static jmp_buf env;
void validate_stack_space(int bp,int nitems){if(bp!=0||nitems!=8)stage=99;else stage=1;if(fail)longjmp(env,1);}
void validate_stack_exists(int bp,int nitems){if(bp!=1||nitems!=1)stage=99;else stage=1;if(fail)longjmp(env,1);}
void spush(uint32_t x){if(stage!=1)stage=99;pushed[n++]=x;CCPU_GR[4]-=4;}
void set_current_SP(uint32_t x){if(stage!=1)stage=99;lastsp=x;stage=2;}
uint32_t spop(void){if(stage!=2)stage=99;else stage=3;return 0x76543210;}
extern void PUSHA(void),LEAVE16(void),LEAVE32(void);
#define CHECK(x) do{if(!(x)){printf("FAIL %d: %s\n",__LINE__,#x);return 1;}}while(0)
int main(void){for(int i=0;i<8;i++){CCPU_GR[i]=0xaaaa0000+i;CCPU_WR[i]=(uint16_t*)&CCPU_GR[i];}fail=1;stage=n=0;if(!setjmp(env))PUSHA();CHECK(stage==1&&n==0&&CCPU_GR[4]==0xaaaa0004);fail=0;stage=n=0;PUSHA();CHECK(n==8&&stage==1);for(int i=0;i<8;i++)CHECK(pushed[i]==0xaaaa0000u+(unsigned)i);
CCPU_GR[5]=0xabcd9876;fail=1;stage=0;lastsp=0;if(!setjmp(env))LEAVE16();CHECK(stage==1&&lastsp==0&&CCPU_GR[5]==0xabcd9876);fail=0;stage=0;LEAVE16();CHECK(stage==3&&lastsp==0xabcd9876&&CCPU_GR[5]==0xabcd3210);
CCPU_GR[5]=0xabcd9876;stage=0;LEAVE32();CHECK(stage==3&&lastsp==0xabcd9876&&CCPU_GR[5]==0x76543210);puts("PASS PUSHA order/validation and LEAVE16/32 fault-before-mutation");return 0;}
