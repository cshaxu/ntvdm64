#include <stdio.h>
#include <string.h>
#include "opennt-bop/ingress/dem_ingress_shim.h"

typedef struct fixture_context { uint8_t guest[16]; } fixture_context;
static int publish_handle(void *s,HANDLE h,uint32_t *t,DWORD *e) { (void)s;(void)h;if(t)*t=0u;if(e)*e=ERROR_INVALID_HANDLE;return 0; }
static int lookup_handle(void *s,uint32_t t,HANDLE *h) { (void)s;(void)t;if(h)*h=INVALID_HANDLE_VALUE;return 0; }
static int release_handle(void *s,uint32_t t,DWORD *e) { (void)s;(void)t;if(e)*e=ERROR_INVALID_HANDLE;return 0; }
static int query_attributes(void *s,uint8_t d,const wchar_t *p,DWORD *a,DWORD *e) { (void)s;(void)d;(void)p;if(a)*a=0u;if(e)*e=ERROR_FILE_NOT_FOUND;return 0; }
static int set_attributes(void *s,uint8_t d,const wchar_t *p,DWORD a,DWORD *e) { (void)s;(void)d;(void)p;(void)a;if(e)*e=ERROR_FILE_NOT_FOUND;return 0; }
static int guest_read(void *s,uint32_t a,uint8_t *b,uint32_t n) { fixture_context *c=(fixture_context *)s;if(!c||a>sizeof(c->guest)||n>sizeof(c->guest)-a)return 0;memcpy(b,c->guest+a,n);return 1; }
static int guest_write(void *s,uint32_t a,const uint8_t *b,uint32_t n) { fixture_context *c=(fixture_context *)s;if(!c||a>sizeof(c->guest)||n>sizeof(c->guest)-a)return 0;memcpy(c->guest+a,b,n);return 1; }

int main(void)
{
    fixture_context state={{0}}; runtime_dem_direct_context direct;
    runtime_exception_event event; runtime_cpu_state cpu;
    runtime_cpu_result result; runtime_demhndl_call call;
    runtime_instruction_window window;
    memset(&direct,0,sizeof(direct)); direct.magic=RUNTIME_DEM_DIRECT_CONTEXT_MAGIC;
    direct.abi_version=RUNTIME_DEM_DIRECT_CONTEXT_VERSION; direct.struct_bytes=sizeof(direct);
    direct.state=&state; direct.publish_handle=publish_handle; direct.lookup_handle=lookup_handle;
    direct.release_handle=release_handle; direct.query_attributes=query_attributes; direct.set_attributes=set_attributes;
    memset(&event,0,sizeof(event)); event.magic=RUNTIME_EXCEPTION_ABI_MAGIC;
    event.abi_version=RUNTIME_EXCEPTION_ABI_VERSION; event.struct_bytes=sizeof(event);
    event.kind=RUNTIME_EXCEPTION_EVENT_CPU_EXCEPTION; event.fault_rip=0x2000u;
    runtime_cpu_state_initialize(&cpu,RUNTIME_CPU_EXECUTION_REAL);
    memset(&call,0,sizeof(call)); call.magic=RUNTIME_DEMHNDL_CALL_MAGIC;
    call.abi_version=RUNTIME_DEMHNDL_CALL_VERSION; call.struct_bytes=sizeof(call);
    call.service=0x1fu; call.direct=&direct; call.boundary=&event; call.cpu=&cpu;
    call.result=&result; call.guest_state=&state; call.guest_read=guest_read; call.guest_write=guest_write;
    runtime_instruction_window_capture(&window,(const uint8_t[]){0xc4u,0xc4u,0x50u,0x1fu},4u);
    if(!runtime_dem_ingress_dispatch(&window,&call)||CurrentISVC!=0x1fu||
       result.disposition!=RUNTIME_CPU_RESULT_RESUME||result.resume_rip!=event.fault_rip+4u||
       (result.eflags_values&RUNTIME_CPU_RESULT_EFLAGS_CF)!=0u)return 1;
    runtime_instruction_window_capture(&window,(const uint8_t[]){0xc4u,0xc4u,0x54u,0x1fu},4u);
    if(runtime_dem_ingress_dispatch(&window,&call))return 2;
    runtime_instruction_window_capture(&window,(const uint8_t[]){0xc4u,0xc4u,0x50u},3u);
    if(runtime_dem_ingress_dispatch(&window,&call))return 3;
    runtime_instruction_window_capture(&window,(const uint8_t[]){0xc4u,0xc4u,0x50u,0x49u},4u);
    if(runtime_dem_ingress_dispatch(&window,&call))return 4;
    puts("T230 S10 direct DEM ingress: 50:xx dispatch is bounded and v1-free"); return 0;
}
