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
    HANDLE pipe_read, pipe_write; DWORD bytes; CHAR received[2];
    HKEY root,key; HANDLE kb16, keyboard; CHAR directory[MAX_PATH], kb16_path[MAX_PATH], keyboard_path[MAX_PATH]; DWORD disposition;
    memset(&c,0,sizeof(c));event_initialize(&e);bx_ntvdm_cpu_state_v1_initialize(&cpu,BX_NTVDM_CPU_EXECUTION_REAL);bx_ntvdm_command_misc_session_initialize(&s);
    if(!invoke(&c,&e,&cpu,&r,&s,BX_NTVDM_COMMAND_MISC_INIT_CONSOLE)||s.console_initialized!=1u)return 1;
    bPifFastPaste=TRUE;cpu.edx=0u;cpu.ds=0x100u;cpu.esi=0u;cpu.ecx=0x80u;
    if(!invoke(&c,&e,&cpu,&r,&s,BX_NTVDM_COMMAND_MISC_GET_KBD_LAYOUT)||r.cpu_delta.gpr16_values[2]!=0u||s.console_initialized!=1u)return 2;
    s.redirection_token=1u;s.redirection_info.ri_hStdOut=CreateFileA("NUL",GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL);
    if(s.redirection_info.ri_hStdOut==INVALID_HANDLE_VALUE)return 3;
    cpu.eax=0u;cpu.ebx=1u;cpu.ecx=HANDLE_STDOUT;
    if(!invoke(&c,&e,&cpu,&r,&s,0x06u)||r.cpu_delta.gpr16_values[1]!=1u||r.cpu_delta.gpr16_values[3]!=0u)return 4;
    CloseHandle(s.redirection_info.ri_hStdOut);
    bx_ntvdm_command_misc_session_initialize(&s);
    if(!CreatePipe(&pipe_read,&pipe_write,NULL,0u))return 5;
    s.redirection_token=1u;s.redirection_info.ri_hStdOut=pipe_write;
    cpu.eax=0u;cpu.ebx=1u;cpu.ecx=HANDLE_STDOUT;
    if(!invoke(&c,&e,&cpu,&r,&s,0x06u)||s.redirection_info.ri_pPipeStdOut==NULL)return 6;
    if(!WriteFile(s.handle_tokens[0],"X",1u,&bytes,NULL)||bytes!=1u)return 7;
    SetEvent(s.redirection_info.ri_pPipeStdOut->hExitEvent);
    if(WaitForSingleObject(s.redirection_info.ri_hStdOutThread,2000u)!=WAIT_OBJECT_0)return 8;
    if(!ReadFile(pipe_read,received,1u,&bytes,NULL)||bytes!=1u||received[0]!='X')return 9;
    CloseHandle(s.redirection_info.ri_hStdOutThread);CloseHandle(pipe_read);
    if(!GetTempPathA(MAX_PATH,directory))return 10;
    sprintf(kb16_path,"%sKB16.COM",directory);kb16=CreateFileA(kb16_path,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,NULL);if(kb16==INVALID_HANDLE_VALUE)return 11;
    sprintf(keyboard_path,"%sKEYBOARD.SYS",directory);keyboard=CreateFileA(keyboard_path,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,NULL);if(keyboard==INVALID_HANDLE_VALUE)return 12;
    if(RegCreateKeyExA(HKEY_CURRENT_USER,"Software\\ntdos64-t231-kbd",0,NULL,0,KEY_ALL_ACCESS,NULL,&root,&disposition)!=ERROR_SUCCESS)return 13;
    if(RegCreateKeyExA(root,"System\\CurrentControlSet\\Control\\Keyboard Layout\\DosKeybCodes",0,NULL,0,KEY_ALL_ACCESS,NULL,&key,&disposition)!=ERROR_SUCCESS)return 14;
    if(RegSetValueExA(key,"00000409",0,REG_SZ,(const BYTE *)"US",3u)!=ERROR_SUCCESS)return 15;RegCloseKey(key);
    if(RegOverridePredefKey(HKEY_LOCAL_MACHINE,root)!=ERROR_SUCCESS)return 16;
    bx_ntvdm_command_misc_set_test_system_directory(directory);bPifFastPaste=FALSE;cpu.edx=1u;cpu.ds=0x100u;cpu.esi=0x100u;cpu.ecx=0x200u;
    if(!invoke(&c,&e,&cpu,&r,&s,BX_NTVDM_COMMAND_MISC_GET_KBD_LAYOUT)||r.cpu_delta.gpr16_values[2]!=1u||strstr((CHAR *)c.guest+0x1100,"KB16.COM")==NULL){fprintf(stderr,"kbd dx=%u program=%s options=%s\n",r.cpu_delta.gpr16_values[2],c.guest+0x1100,c.guest+0x1200);return 17;}
    RegOverridePredefKey(HKEY_LOCAL_MACHINE,NULL);RegCloseKey(root);RegDeleteTreeA(HKEY_CURRENT_USER,"Software\\ntdos64-t231-kbd");bx_ntvdm_command_misc_set_test_system_directory(NULL);CloseHandle(kb16);CloseHandle(keyboard);DeleteFileA(kb16_path);DeleteFileA(keyboard_path);
    puts("T231 S4 direct OpenNT console, keyboard fallback/success, and standard-handle token ABI verified");return 0;
}
