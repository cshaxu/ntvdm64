#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "ntvdm-exe/softpc/include/mvdm_softpc_termination.h"
static unsigned long ldt[16];
void *Ldt=ldt;
unsigned long IntelBase,Cpu40GdtShadowAddress,Cpu40LdtShadowAddress;
void host_applClose(void){abort();}
int session_terminate_current(uint32_t value){(void)value;abort();}
/* Any diagnostic access to guest address translation or SAS is a failure. */
int xtrn2phy(unsigned long linear,unsigned char access,unsigned long *physical)
{(void)linear;(void)access;(void)physical;abort();}
void c_sas_loads(unsigned long source,unsigned char *destination,unsigned long count)
{(void)source;(void)destination;(void)count;abort();}
#define CHECK(call) do { SetLastError(10004); call; if(GetLastError()!=10004) return 3; } while(0)
int main(int argc,char **argv){
 if(argc!=2)return 2;
 IntelBase=(unsigned long)(uintptr_t)ldt;
 Cpu40GdtShadowAddress=0x2000; Cpu40LdtShadowAddress=0x1000;
 SetEnvironmentVariableA("MVDM_WOW_CALLBACK_CPU_TRACE_PATH",NULL);
 SetEnvironmentVariableA("MVDM_WOW_CALLBACK_CPU_SELECTOR","8");
 CHECK(mvdm_softpc_report_wow_far_call_attempt(8,0,8,0x20,0,0,0));
 SetEnvironmentVariableA("MVDM_WOW_CALLBACK_CPU_TRACE_PATH",argv[1]);
 SetEnvironmentVariableA("MVDM_WOW_CALLBACK_CPU_SELECTOR","10");
 CHECK(mvdm_softpc_report_wow_far_call_attempt(8,0,8,0x20,0,0,0));
 SetEnvironmentVariableA("MVDM_WOW_CALLBACK_CPU_SELECTOR","8");
 CHECK(mvdm_softpc_report_wow_far_call_attempt(8,0,8,0x20,0,0,0));
 CHECK(mvdm_softpc_report_dpmi_fault_dispatch(13,2,8,0x20,8,0x20,16,0x30,0,8,0x20));
 CHECK(mvdm_softpc_initialize_default_off_traces());
 CHECK(mvdm_softpc_report_wow_source_descriptor_store(8,0x1234,4,8,0x20,16,0x30));
 puts("SOFTPC_TRACE_NO_GUEST_READS_OK disabled=1 mismatch=1 enabled=3 error_preserved=1");
 return 0;
}
