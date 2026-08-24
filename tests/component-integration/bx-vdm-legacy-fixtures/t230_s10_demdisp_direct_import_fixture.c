/* Direct table witness for src/opennt/base/mvdm/dos/dem/demdisp.c. */
#include <stdio.h>
#include <string.h>

#include "opennt-bop/dem/opennt_dem_dispatch_composition.h"

typedef struct fixture_context { uint8_t guest[16]; } fixture_context;
static int publish(void *s,HANDLE h,uint32_t*t,DWORD*e)
{ (void)s;(void)h;if(t)*t=1u;if(e)*e=ERROR_SUCCESS;return 1; }
static int lookup(void*s,uint32_t t,HANDLE*h)
{ (void)s;(void)t;if(h)*h=INVALID_HANDLE_VALUE;return 0; }
static int release(void*s,uint32_t t,DWORD*e)
{ (void)s;(void)t;if(e)*e=ERROR_INVALID_HANDLE;return 0; }
static int attr_get(void*s,uint8_t d,const wchar_t*p,DWORD*a,DWORD*e)
{ (void)s;(void)d;(void)p;if(a)*a=FILE_ATTRIBUTE_NORMAL;if(e)*e=ERROR_SUCCESS;return 1; }
static int attr_set(void*s,uint8_t d,const wchar_t*p,DWORD a,DWORD*e)
{ (void)s;(void)d;(void)p;(void)a;if(e)*e=ERROR_SUCCESS;return 1; }
static int read_guest(void*s,uint32_t a,uint8_t*b,uint32_t n)
{ fixture_context*c=s;if(!c||a>sizeof(c->guest)||n>sizeof(c->guest)-a)return 0;memcpy(b,c->guest+a,n);return 1; }
static int write_guest(void*s,uint32_t a,const uint8_t*b,uint32_t n)
{ fixture_context*c=s;if(!c||a>sizeof(c->guest)||n>sizeof(c->guest)-a)return 0;memcpy(c->guest+a,b,n);return 1; }

static int invoke(uint8_t service, fixture_context *state,
    runtime_dem_direct_context *direct, runtime_exception_event_v1 *event,
    runtime_cpu_state_v1 *cpu, runtime_cpu_result_v2 *result)
{
    runtime_demhndl_call call;
    memset(&call, 0, sizeof(call));
    call.magic=RUNTIME_DEMHNDL_CALL_MAGIC; call.abi_version=RUNTIME_DEMHNDL_CALL_VERSION;
    call.struct_bytes=sizeof(call); call.service=service; call.direct=direct;
    call.boundary=event; call.cpu=cpu; call.result=result; call.guest_state=state;
    call.guest_read=read_guest; call.guest_write=write_guest;
    return runtime_demdisp_invoke(&call) && CurrentISVC == service &&
        result->disposition == RUNTIME_CPU_RESULT_V2_RESUME &&
        result->resume_rip == event->fault_rip + 4u;
}

int main(void)
{
    static const uint8_t noops[] = { 0x1fu, 0x24u, 0x26u, 0x28u,
        0x2bu, 0x40u, 0x42u, 0x43u };
    size_t index;
    fixture_context state;
    runtime_dem_direct_context direct;
    runtime_exception_event_v1 event;
    runtime_cpu_state_v1 cpu;
    runtime_cpu_result_v2 result;

    if (runtime_demdisp_service_count() != SVC_DEMLASTSVC ||
        SVC_DEMLASTSVC != 73u) return 1;
    for (index = 0; index != SVC_DEMLASTSVC; ++index)
        if (runtime_demdisp_handler_at((uint8_t)index) == 0) return 2;
    for (index = 0; index != sizeof(noops); ++index)
        if (runtime_demdisp_handler_at(noops[index]) != demNotYetImplemented)
            return 3;
    if (runtime_demdisp_handler_at(0x1du) != demSetV86KernelAddr ||
        runtime_demdisp_handler_at(SVC_DEMLASTSVC) != 0) return 4;
    memset(&state, 0, sizeof(state)); memset(&direct, 0, sizeof(direct));
    direct.magic=RUNTIME_DEM_DIRECT_CONTEXT_MAGIC; direct.abi_version=RUNTIME_DEM_DIRECT_CONTEXT_VERSION;
    direct.struct_bytes=sizeof(direct); direct.state=&state; direct.publish_handle=publish;
    direct.lookup_handle=lookup; direct.release_handle=release; direct.query_attributes=attr_get;
    direct.set_attributes=attr_set;
    memset(&event, 0, sizeof(event)); event.magic=RUNTIME_EXCEPTION_ABI_MAGIC;
    event.abi_version=RUNTIME_EXCEPTION_ABI_VERSION; event.struct_bytes=sizeof(event);
    event.kind=RUNTIME_EXCEPTION_EVENT_CPU_EXCEPTION; event.fault_rip=0x1000u;
    if (!runtime_dem_direct_context_valid(&direct)) return 5;
    runtime_cpu_state_v1_initialize(&cpu, RUNTIME_CPU_EXECUTION_REAL);
    if (!invoke(0x1du, &state, &direct, &event, &cpu, &result)) return 6;
    for (index = 0; index != sizeof(noops); ++index) {
        runtime_cpu_state_v1_initialize(&cpu, RUNTIME_CPU_EXECUTION_REAL);
        cpu.eflags = RUNTIME_CPU_RESULT_V2_EFLAGS_CF;
        if (!invoke(noops[index], &state, &direct, &event, &cpu, &result) ||
            (result.eflags_values & RUNTIME_CPU_RESULT_V2_EFLAGS_CF) != 0u)
            return 7;
    }
    puts("T230 S10 direct OpenNT demdisp table: 73 slots and all ordinary/no-op bodies verified");
    return 0;
}
