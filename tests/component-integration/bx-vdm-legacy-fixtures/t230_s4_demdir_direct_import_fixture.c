#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "opennt-bop/dem/opennt_dem_dispatch_composition.h"
#include "opennt-host/dem/demdir_shim.h"

typedef struct context { uint8_t guest[0x20000]; } context;
static int publish(void *s,HANDLE h,uint32_t*t,DWORD*e){(void)s;(void)h;if(t)*t=1;if(e)*e=0;return 1;}
static int lookup(void*s,uint32_t t,HANDLE*h){(void)s;(void)t;if(h)*h=INVALID_HANDLE_VALUE;return 0;}
static int release(void*s,uint32_t t,DWORD*e){(void)s;(void)t;if(e)*e=ERROR_INVALID_HANDLE;return 0;}
static int attrget(void*s,uint8_t d,const wchar_t*p,DWORD*a,DWORD*e){(void)s;(void)d;(void)p;if(a)*a=FILE_ATTRIBUTE_NORMAL;if(e)*e=0;return 1;}
static int attrset(void*s,uint8_t d,const wchar_t*p,DWORD a,DWORD*e){(void)s;(void)d;(void)p;(void)a;if(e)*e=0;return 1;}
static int readg(void*s,uint32_t a,uint8_t*b,uint32_t n){context*c=s;if(!c||a>sizeof(c->guest)||n>sizeof(c->guest)-a)return 0;memcpy(b,c->guest+a,n);return 1;}
static int writeg(void*s,uint32_t a,const uint8_t*b,uint32_t n){context*c=s;if(!c||a>sizeof(c->guest)||n>sizeof(c->guest)-a)return 0;memcpy(c->guest+a,b,n);return 1;}
static int cf(const bx_ntvdm_cpu_result_v2*r){return (r->eflags_values&BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF)!=0;}
static int invoke(context*c,bx_ntvdm_dem_direct_context*d,bx_ntvdm_exception_event_v1*e,bx_ntvdm_cpu_state_v1*cpu,bx_ntvdm_cpu_result_v2*r,uint32_t svc){bx_ntvdm_demhndl_call x;memset(&x,0,sizeof(x));x.magic=BX_NTVDM_DEMHNDL_CALL_MAGIC;x.abi_version=BX_NTVDM_DEMHNDL_CALL_VERSION;x.struct_bytes=sizeof(x);x.service=svc;x.direct=d;x.boundary=e;x.cpu=cpu;x.result=r;x.guest_state=c;x.guest_read=readg;x.guest_write=writeg;return bx_ntvdm_demdisp_invoke(&x)&&r->disposition==BX_NTVDM_CPU_RESULT_V2_RESUME;}
static void cpuinit(bx_ntvdm_cpu_state_v1*c){bx_ntvdm_cpu_state_v1_initialize(c,BX_NTVDM_CPU_EXECUTION_REAL);c->ds=0x100;c->es=0x100;}
int main(void){char tmp[MAX_PATH],dir[MAX_PATH],old[MAX_PATH];context c;bx_ntvdm_dem_direct_context d;bx_ntvdm_exception_event_v1 e;bx_ntvdm_cpu_state_v1 cpu;bx_ntvdm_cpu_result_v2 r;CDS cds;DWORD n;char drive;
 if(!GetCurrentDirectoryA(MAX_PATH,old)||!GetTempPathA(MAX_PATH,tmp))return 1;sprintf_s(dir,sizeof(dir),"%st230-dir",tmp);RemoveDirectoryA(dir);memset(&c,0,sizeof(c));memset(&d,0,sizeof(d));d.magic=BX_NTVDM_DEM_DIRECT_CONTEXT_MAGIC;d.abi_version=BX_NTVDM_DEM_DIRECT_CONTEXT_VERSION;d.struct_bytes=sizeof(d);d.state=&c;d.publish_handle=publish;d.lookup_handle=lookup;d.release_handle=release;d.query_attributes=attrget;d.set_attributes=attrset;memset(&e,0,sizeof(e));e.magic=BX_NTVDM_EXCEPTION_ABI_MAGIC;e.abi_version=BX_NTVDM_EXCEPTION_ABI_VERSION;e.struct_bytes=sizeof(e);e.kind=BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION;e.fault_rip=0x1000;
 memcpy(c.guest+0x1000,dir,strlen(dir)+1);cpuinit(&cpu);cpu.edx=0;if(!invoke(&c,&d,&e,&cpu,&r,BX_NTVDM_DEMDIR_CREATE)||cf(&r)||GetFileAttributesA(dir)==INVALID_FILE_ATTRIBUTES)return 2;
 cpuinit(&cpu);cpu.edx=0;if(!invoke(&c,&d,&e,&cpu,&r,BX_NTVDM_DEMDIR_SET_CURRENT)||cf(&r)||!SetCurrentDirectoryA(old))return 3;
 memset(&cds,0,sizeof(cds));strncpy_s(cds.CurDir_Text,sizeof(cds.CurDir_Text),dir,_TRUNCATE);memcpy(c.guest+0x1000,&cds,sizeof(cds));drive=(char)(toupper((unsigned char)dir[0])-'A');cpuinit(&cpu);cpu.esi=0;cpu.eax=(uint8_t)drive;if(!invoke(&c,&d,&e,&cpu,&r,BX_NTVDM_DEMDIR_QUERY_CURRENT)||cf(&r))return 4;n=GetFileAttributesA(dir);if(n==INVALID_FILE_ATTRIBUTES)return 5;
 memcpy(c.guest+0x1000,dir,strlen(dir)+1);cpuinit(&cpu);cpu.edx=0;if(!invoke(&c,&d,&e,&cpu,&r,BX_NTVDM_DEMDIR_DELETE)||cf(&r)||GetFileAttributesA(dir)!=INVALID_FILE_ATTRIBUTES)return 6;puts("T230 S4 direct OpenNT demdir import: directory and CDS services verified");return 0;}
