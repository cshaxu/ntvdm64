/* Current original callback bodies; bounded register/stack fixture. */
#include <stdlib.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>
typedef uint16_t word; typedef word MOUSE_CALL_MASK;
#define LOCAL static
#define IFN0() (void)
#define IFN1(a,b) (a b)
#define IFN3(a,b,c,d,e,f) (a b,c d,e f)
#define NTVDM 1
#define CPU_40_STYLE 1
/* Original host/inc/host_def.h callback adjustment for the negative control. */
#define HOST_BOP_IP_FUDGE (-2)
#define MOUSE_VERBOSE 0
#define note_trace0(a,b) ((void)0)
#define MOUSE_INT2_SEGMENT 0xf000
#define MOUSE_INT2_OFFSET 0x100
#define MOUSE_CALL_MASK_POSITION_BIT 1
#define MOUSE_CURSOR_DISPLAYED 1
static word reg[12]; enum { AX,BX,CX,DX,SI,DI,ES,BP,DS,SS,SP,CS };
#define REG(n) static word get##n(void){return reg[n];} static void set##n(word v){reg[n]=v;}
REG(AX) REG(BX) REG(CX) REG(DX) REG(SI) REG(DI) REG(ES) REG(BP) REG(DS) REG(SP)
static word getSS(void){return reg[SS];} static void setCS(word v){reg[CS]=v;}
static void setIP(word v){(void)v;} static void setIF(word v){(void)v;}
static unsigned char mem[65536];
static unsigned effective_addr(word seg,word off){(void)seg;return off;}
static void sas_storew(unsigned a,word v){mem[a]=v;mem[(a+1)&65535]=v>>8;}
static void sas_loadw(unsigned a,word *v){*v=(word)(mem[a]|mem[(a+1)&65535]<<8);}
static struct { struct {word x,y;} position; word button_status;} cursor_status;
static struct {word x,y;} mouse_motion;
static int cursor_flag,draws,flushes,resumes;
static word saved_AX,saved_BX,saved_CX,saved_DX,saved_SI,saved_DI,saved_ES,saved_BP,saved_DS,last_condition_mask;
static void cursor_display(void){draws++;} static void host_flush_screen(void){flushes++;}
static void ResumeMouseInterrupts(void){resumes++;}

#include "mouse-callback-return.inc"
#include "mouse-callback-enter.inc"

static void die(const char *msg){puts(msg);exit(1);}
#include <stdlib.h>
static void verify(const word *expected,const char *where){
 int i; for(i=AX;i<=DS;i++)if(reg[i]!=expected[i]){printf("%s: reg %d expected %04x got %04x\n",where,i,expected[i],reg[i]);exit(1);}
}
int main(void){
 word outer[9],inner[9]; int i; memset(mem,0,sizeof(mem));
const word frame_size=24;
 for(i=AX;i<=DS;i++)reg[i]=(word)(0x1100+i*0x111); reg[SI]=0x1f8f;reg[BP]=0x8f90;reg[SS]=0x3456;reg[SP]=0x1000;
 memcpy(outer,reg,sizeof outer);cursor_flag=MOUSE_CURSOR_DISPLAYED;
 mouse_motion.x=0x780;mouse_motion.y=1080;cursor_status.position.x=639;cursor_status.position.y=479;
 jump_to_user_subroutine(1,0x1234,0x4321);
 if(reg[SP]!=(word)(0x1000-frame_size)||reg[CS]!=0x1234)die("outer frame placement");
 memcpy(inner,reg,sizeof inner);jump_to_user_subroutine(0,0x5678,0x8765);
 if(reg[SP]!=(word)(0x1000-2*frame_size))die("nested frame placement");
 reg[SP]+=4;mouse_int2();verify(inner,"inner");if(reg[SP]!=(word)(0x1000-frame_size))die("inner stack");
 reg[SP]+=4;mouse_int2();verify(outer,"outer");
 if(reg[SP]!=0x1000||draws!=1||flushes!=2||resumes!=2)die("return effects");
 /* A normal callback and 16-bit stack wrapping retain the same contract. */
 reg[SP]=8;memcpy(outer,reg,sizeof outer);jump_to_user_subroutine(0,0x1234,0x4321);
 if(reg[SP]!=(word)(8-24))die("wrapped frame placement");
 reg[SP]+=4;mouse_int2();verify(outer,"wrapped single");
 if(reg[SP]!=8||draws!=1||flushes!=3||resumes!=3)die("wrapped return effects");
 puts("PASS: original callback bodies preserve independent nested guest frames");return 0;
}
