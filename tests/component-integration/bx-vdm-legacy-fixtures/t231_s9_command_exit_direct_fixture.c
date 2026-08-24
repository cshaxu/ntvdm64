#include "opennt-bop/command/opennt_command_composition.h"
#include <stdio.h>
#include <string.h>

static int no_read(void *state,uint32_t address,uint8_t *buffer,uint32_t bytes)
{ (void)state;(void)address;(void)buffer;(void)bytes;return 0; }
static int no_write(void *state,uint32_t address,const uint8_t *buffer,uint32_t bytes)
{ (void)state;(void)address;(void)buffer;(void)bytes;return 0; }
int main(void)
{
 runtime_exception_event_v1 event;runtime_cpu_state_v1 cpu;runtime_cpu_result_v2 result;runtime_command_misc_call call;
 memset(&event,0,sizeof(event));event.magic=RUNTIME_EXCEPTION_ABI_MAGIC;event.abi_version=RUNTIME_EXCEPTION_ABI_VERSION;event.struct_bytes=sizeof(event);event.kind=RUNTIME_EXCEPTION_EVENT_CPU_EXCEPTION;event.vector=6u;event.fault_rip=0x500u;runtime_cpu_state_v1_initialize(&cpu,RUNTIME_CPU_EXECUTION_REAL);
 memset(&call,0,sizeof(call));call.magic=RUNTIME_COMMAND_MISC_CALL_MAGIC;call.abi_version=RUNTIME_COMMAND_MISC_CALL_VERSION;call.struct_bytes=sizeof(call);call.service=RUNTIME_COMMAND_MISC_EXIT;call.boundary=&event;call.cpu=&cpu;call.result=&result;call.guest_read=no_read;call.guest_write=no_write;
 if(!runtime_command_misc_invoke(&call)||result.disposition!=RUNTIME_CPU_RESULT_V2_STOP||result.resume_rip!=0u||result.cpu_delta.gpr16_write_mask!=0u)return 1;
 puts("T231 S9 direct OpenNT cmdExitVDM typed controlled-stop behavior verified");return 0;
}
