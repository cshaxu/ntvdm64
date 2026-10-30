#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "ntvdm-exe/softpc/include/mvdm_softpc_termination.h"
static unsigned long ldt[16]; void *Ldt=ldt;
unsigned long IntelBase,Cpu40GdtShadowAddress,Cpu40LdtShadowAddress;
static unsigned maps, reads; static unsigned long last_map,last_read;
void host_applClose(void){abort();}
int session_terminate_current(uint32_t value){(void)value;abort();}
int xtrn2phy(unsigned long linear,unsigned char access,unsigned long *physical)
{(void)access;maps++;last_map=linear;*physical=linear+0x9000;return 1;}
void c_sas_loads(unsigned long source,unsigned char *destination,unsigned long count)
{reads++;last_read=source;while(count--)*destination++=0;}
int main(int argc,char **argv){
 if(argc!=2)return 2;
 SetEnvironmentVariableA("MVDM_WOW_CALLBACK_CPU_TRACE_PATH",NULL);
 SetEnvironmentVariableA("MVDM_WOW_CALLBACK_CPU_SELECTOR","8");
 mvdm_softpc_report_wow_far_call_attempt(8,0,8,0x20,0,0,0);
 if(maps||reads)return 3;
 SetEnvironmentVariableA("MVDM_WOW_CALLBACK_CPU_TRACE_PATH",argv[1]);
 SetEnvironmentVariableA("MVDM_WOW_CALLBACK_CPU_SELECTOR","10");
 mvdm_softpc_report_wow_far_call_attempt(8,0,8,0x20,0,0,0);
 if(maps||reads)return 4;
 SetEnvironmentVariableA("MVDM_WOW_CALLBACK_CPU_SELECTOR","8");
 mvdm_softpc_report_wow_far_call_attempt(8,0,8,0x20,0,0,0);
 printf("maps=%u reads=%u linear=%08lx read_arg=%08lx\n",maps,reads,last_map,last_read);
 return !(maps==1 && reads==1 && last_map==0x18 && last_read==0x9018);
}
