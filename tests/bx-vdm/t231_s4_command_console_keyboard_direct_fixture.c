#include <stdio.h>
#include <string.h>

#include "bop/shim/command_misc_shim.h"

typedef struct fixture_context { uint8_t guest[0x20000]; } fixture_context;
static int guest_read(void *s,uint32_t a,uint8_t *b,uint32_t n)
{ fixture_context *c=(fixture_context *)s; if(!c||a>sizeof(c->guest)||n>sizeof(c->guest)-a)return 0;memcpy(b,c->guest+a,n);return 1; }
static int guest_write(void *s,uint32_t a,const uint8_t *b,uint32_t n)
{ fixture_context *c=(fixture_context *)s; if(!c||a>sizeof(c->guest)||n>sizeof(c->guest)-a)return 0;memcpy(c->guest+a,b,n);return 1; }
static void event_initialize(bx_ntvdm_exception_event_v1 *e)
{ memset(e,0,sizeof(*e));e->magic=BX_NTVDM_EXCEPTION_ABI_MAGIC;e->abi_version=BX_NTVDM_EXCEPTION_ABI_VERSION;e->struct_bytes=sizeof(*e);e->kind=BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION;e->vector=6u;e->fault_rip=0x500u; }
static int invoke(fixture_context *c,bx_ntvdm_exception_event_v1 *e,bx_ntvdm_cpu_state_v1 *cpu,bx_ntvdm_cpu_result_v2 *r,bx_ntvdm_command_misc_session *s,uint32_t service)
{ bx_ntvdm_command_misc_call call;memset(&call,0,sizeof(call));call.magic=BX_NTVDM_COMMAND_MISC_CALL_MAGIC;call.abi_version=BX_NTVDM_COMMAND_MISC_CALL_VERSION;call.struct_bytes=sizeof(call);call.service=service;call.boundary=e;call.cpu=cpu;call.result=r;call.guest_state=c;call.guest_read=guest_read;call.guest_write=guest_write;call.session=s;return bx_ntvdm_command_misc_invoke(&call)&&r->disposition==BX_NTVDM_CPU_RESULT_V2_RESUME&&r->resume_rip==e->fault_rip+4u; }
int main(void)
{
    fixture_context c; bx_ntvdm_exception_event_v1 e; bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_cpu_result_v2 r; bx_ntvdm_command_misc_session s;
    memset(&c,0,sizeof(c));event_initialize(&e);bx_ntvdm_cpu_state_v1_initialize(&cpu,BX_NTVDM_CPU_EXECUTION_REAL);bx_ntvdm_command_misc_session_initialize(&s);
    if(!invoke(&c,&e,&cpu,&r,&s,BX_NTVDM_COMMAND_MISC_INIT_CONSOLE)||s.console_initialized!=1u)return 1;
    bPifFastPaste=TRUE;cpu.edx=0u;cpu.ds=0x100u;cpu.esi=0u;cpu.ecx=0x80u;
    if(!invoke(&c,&e,&cpu,&r,&s,BX_NTVDM_COMMAND_MISC_GET_KBD_LAYOUT)||r.cpu_delta.gpr16_values[2]!=0u||s.console_initialized!=1u)return 2;
    s.redirection_token=1u;s.redirection_info.ri_hStdOut=CreateFileA("NUL",GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL);
    if(s.redirection_info.ri_hStdOut==INVALID_HANDLE_VALUE)return 3;
    cpu.eax=0u;cpu.ebx=1u;cpu.ecx=HANDLE_STDOUT;
    if(!invoke(&c,&e,&cpu,&r,&s,0x06u)||r.cpu_delta.gpr16_values[1]!=1u||r.cpu_delta.gpr16_values[3]!=0u)return 4;
    CloseHandle(s.redirection_info.ri_hStdOut);
    puts("T231 S4 direct OpenNT console, keyboard fallback, and standard-handle token ABI verified");return 0;
}
