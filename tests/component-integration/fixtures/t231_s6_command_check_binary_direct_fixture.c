#include <stdio.h>
#include <string.h>

#include "opennt-bop/command/opennt_command_composition.h"

typedef struct fixture_context { uint8_t guest[0x10000]; } fixture_context;
static int read_guest(void *s,uint32_t a,uint8_t *b,uint32_t n)
{ fixture_context *c=s; if(!c||a>sizeof(c->guest)||n>sizeof(c->guest)-a)return 0;memcpy(b,c->guest+a,n);return 1; }
static int write_guest(void *s,uint32_t a,const uint8_t *b,uint32_t n)
{ fixture_context *c=s; if(!c||a>sizeof(c->guest)||n>sizeof(c->guest)-a)return 0;memcpy(c->guest+a,b,n);return 1; }
static void event_init(runtime_exception_event *e)
{ memset(e,0,sizeof(*e));e->magic=RUNTIME_EXCEPTION_ABI_MAGIC;e->abi_version=RUNTIME_EXCEPTION_ABI_VERSION;e->struct_bytes=sizeof(*e);e->kind=RUNTIME_EXCEPTION_EVENT_CPU_EXCEPTION;e->vector=6u;e->fault_rip=0x500u; }
static int invoke(fixture_context *c,runtime_exception_event *e,runtime_cpu_state *cpu,runtime_cpu_result *r,runtime_command_misc_session *s)
{ runtime_command_misc_call x;memset(&x,0,sizeof(x));x.magic=RUNTIME_COMMAND_MISC_CALL_MAGIC;x.abi_version=RUNTIME_COMMAND_MISC_CALL_VERSION;x.struct_bytes=sizeof(x);x.service=RUNTIME_COMMAND_MISC_CHECK_BINARY;x.boundary=e;x.cpu=cpu;x.result=r;x.guest_state=c;x.guest_read=read_guest;x.guest_write=write_guest;x.session=s;return runtime_command_misc_invoke(&x); }
int main(void)
{
 fixture_context c;runtime_exception_event e;runtime_cpu_state cpu;runtime_cpu_result r;runtime_command_misc_session s;PARAMBLOCK p;CHAR path[MAX_PATH],converted[16]; ANSI_STRING ansi; UNICODE_STRING unicode; uint32_t scs=0x4000u,tail=0x2800u;
 memset(&c,0,sizeof(c));event_init(&e);runtime_cpu_state_initialize(&cpu,RUNTIME_CPU_EXECUTION_REAL);runtime_command_misc_session_initialize(&s);
 if(RtlNtStatusToDosError((NTSTATUS)0xc000000dL)!=ERROR_INVALID_PARAMETER)return 1;
 RtlInitUnicodeString(&unicode,L"ANSI");memset(&ansi,0,sizeof(ansi));ansi.Buffer=converted;ansi.MaximumLength=sizeof(converted);
 if(!NT_SUCCESS(RtlUnicodeStringToAnsiString(&ansi,&unicode,FALSE))||ansi.Buffer!=converted||ansi.MaximumLength!=sizeof(converted)||strcmp(converted,"ANSI"))return 2;
 if(!GetModuleFileNameA(NULL,path,MAX_PATH)||strlen(path)>=120u)return 3;
 strcpy((CHAR *)c.guest+0x1000u,path);memset(&p,0,sizeof(p));p.SegEnv=0x1234u;p.OffCmdTail=0u;p.SegCmdTail=(USHORT)(tail>>4);memcpy(c.guest+0x2000u,&p,sizeof(p));c.guest[tail]=3u;memcpy(c.guest+tail+1u," /x",3u);
 s.scs_info_address=scs;strcpy(s.comspec,"COMSPEC=C:\\RUNNER\\COMMAND.COM");s.comspec_bytes=(USHORT)(strlen(s.comspec)+1u);cpu.ds=0x100u;cpu.edx=0u;cpu.es=0x200u;cpu.ebx=0u;
 if(!invoke(&c,&e,&cpu,&r,&s)||r.disposition!=RUNTIME_CPU_RESULT_RESUME||r.eflags_values!=0u||r.cpu_delta.segment_write_mask!=((1u<<0)|(1u<<3))||r.cpu_delta.segment_values[3]!=(USHORT)(scs>>4)||r.cpu_delta.segment_values[0]!=(USHORT)((scs+offsetof(SCSINFO,SCS_ParamBlock))>>4))return 4;
 if(strcmp(s.scs_info.SCS_ComSpec,"C:\\RUNNER\\COMMAND.COM")||s.scs_info.SCS_CmdTail[0]<(UCHAR)(3u+strlen(path))||memcmp(s.scs_info.SCS_CmdTail+1,"/z ",3u)||memcmp(c.guest+scs,&s.scs_info,sizeof(s.scs_info)))return 5;
 DontCheckDosBinaryType=TRUE;if(!invoke(&c,&e,&cpu,&r,&s)||r.eflags_values!=0u){DontCheckDosBinaryType=FALSE;return 6;}DontCheckDosBinaryType=FALSE;
 puts("T231 S6 direct OpenNT cmdCheckBinary handoff, session writeback, and DOS bypass verified");return 0;
}
