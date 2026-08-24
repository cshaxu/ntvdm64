#include "opennt-host/command/command_misc_shim.h"
#include <stdio.h>
#include <string.h>

static int no_read(void *state,uint32_t address,uint8_t *buffer,uint32_t bytes)
{ (void)state;(void)address;(void)buffer;(void)bytes;return 0; }
static int no_write(void *state,uint32_t address,const uint8_t *buffer,uint32_t bytes)
{ (void)state;(void)address;(void)buffer;(void)bytes;return 0; }
int main(void)
{
 bx_ntvdm_exception_event_v1 event;bx_ntvdm_cpu_state_v1 cpu;bx_ntvdm_cpu_result_v2 result;bx_ntvdm_command_misc_call call;
 memset(&event,0,sizeof(event));event.magic=BX_NTVDM_EXCEPTION_ABI_MAGIC;event.abi_version=BX_NTVDM_EXCEPTION_ABI_VERSION;event.struct_bytes=sizeof(event);event.kind=BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION;event.vector=6u;event.fault_rip=0x500u;bx_ntvdm_cpu_state_v1_initialize(&cpu,BX_NTVDM_CPU_EXECUTION_REAL);
 memset(&call,0,sizeof(call));call.magic=BX_NTVDM_COMMAND_MISC_CALL_MAGIC;call.abi_version=BX_NTVDM_COMMAND_MISC_CALL_VERSION;call.struct_bytes=sizeof(call);call.service=BX_NTVDM_COMMAND_MISC_EXIT;call.boundary=&event;call.cpu=&cpu;call.result=&result;call.guest_read=no_read;call.guest_write=no_write;
 if(!bx_ntvdm_command_misc_invoke(&call)||result.disposition!=BX_NTVDM_CPU_RESULT_V2_STOP||result.resume_rip!=0u||result.cpu_delta.gpr16_write_mask!=0u)return 1;
 puts("T231 S9 direct OpenNT cmdExitVDM typed controlled-stop behavior verified");return 0;
}
