#include <stdio.h>
#include <setjmp.h>
#include <insignia.h>
#include <host_def.h>
#include <xt.h>
#include <sas.h>
#include <c_main.h>
#include <c_reg.h>
#include <c_xfer.h>
#include "ntvdm-exe/session/session.h"
extern void sas_init(PHY_ADDR),sas_term(void),c_cpu_init(void),phy_w32(IU32,IU32);
static jmp_buf boundary; static int vector, err;
BOOL host_exint_hook(IS32 number,IS32 error) {vector=number;err=error;longjmp(boundary,1);return FALSE;}
int main(int argc,char **argv) {
 session owner; IU16 selector=8; IU32 ip=0,desc=0; IU8 count=0; ISM32 type=0; unsigned cr2;
 if(argc>1) {
  SetEnvironmentVariableA("MVDM_WOW_CALLBACK_CPU_TRACE_PATH","O:\\repos.hobby\\ntvdm64\\build\\t473-s10\\shard-3-ccpu\\far-trace.log");
  SetEnvironmentVariableA("MVDM_WOW_CALLBACK_CPU_SELECTOR",argv[1]);
 } else {
  SetEnvironmentVariableA("MVDM_WOW_CALLBACK_CPU_TRACE_PATH",NULL);
  SetEnvironmentVariableA("MVDM_WOW_CALLBACK_CPU_SELECTOR",NULL);
 }
 session_initialize(&owner,1);if(!session_activate(&owner)||!session_thread_bind(&owner))return 2;
 sas_init(0x200000);c_cpu_init();
 /* linear page 0 absent; linear page 1 maps RAM 0x12000 */
 phy_w32(0x10000,0x11003); phy_w32(0x11000,argc>2?0x13003:0); phy_w32(0x11004,0x12003); phy_w32(0x12000,argc>2?0x9b:0);
 SET_CR(CR_PDBR,0x10000); SET_GDT_BASE(0x0ff3); SET_GDT_LIMIT(0x000f); SET_CPL(0);SET_PE(1);SET_PG(1);
 if(!setjmp(boundary))validate_far_dest(&selector,&ip,&desc,&count,&type,0);
 cr2=GET_CR(CR_PFLA);SET_PG(0);
 fprintf(stderr,"vector=%d error=%d cr2=%08x gdt=00000ff3 selector=0008 descriptor=00000ffb pde=00011003 pte0=%08x pte1=00012003 ar=%02x\n",vector,err,cr2,argc>2?0x13003:0,argc>2?0x9b:0);
 sas_term();(void)session_thread_unbind(&owner);(void)session_dispose(&owner);return vector?vector:42;
}
