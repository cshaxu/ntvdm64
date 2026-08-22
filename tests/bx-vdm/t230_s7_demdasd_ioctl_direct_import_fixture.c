#include <stdio.h>
#include <string.h>

#include "bop/shim/demdisp_shim.h"
#include "bop/shim/demdasd_ioctl_shim.h"

typedef struct fixture_context { uint8_t guest[0x10000]; } fixture_context;

static int publish(void *s, HANDLE h, uint32_t *t, DWORD *e)
{ (void)s; (void)h; if (t) *t=1u; if (e) *e=ERROR_SUCCESS; return 1; }
static int lookup(void *s, uint32_t t, HANDLE *h)
{ (void)s; (void)t; if (h) *h=INVALID_HANDLE_VALUE; return 0; }
static int release(void *s, uint32_t t, DWORD *e)
{ (void)s; (void)t; if (e) *e=ERROR_INVALID_HANDLE; return 0; }
static int attr_get(void *s,uint8_t d,const wchar_t *p,DWORD *a,DWORD *e)
{ (void)s;(void)d;(void)p;if(a)*a=FILE_ATTRIBUTE_NORMAL;if(e)*e=ERROR_SUCCESS;return 1; }
static int attr_set(void *s,uint8_t d,const wchar_t *p,DWORD a,DWORD *e)
{ (void)s;(void)d;(void)p;(void)a;if(e)*e=ERROR_SUCCESS;return 1; }
static int read_guest(void *s,uint32_t a,uint8_t *b,uint32_t n)
{ fixture_context *c=s;if(!c||a>sizeof(c->guest)||n>sizeof(c->guest)-a)return 0;memcpy(b,c->guest+a,n);return 1; }
static int write_guest(void *s,uint32_t a,const uint8_t *b,uint32_t n)
{ fixture_context *c=s;if(!c||a>sizeof(c->guest)||n>sizeof(c->guest)-a)return 0;memcpy(c->guest+a,b,n);return 1; }
static int carry(const bx_ntvdm_cpu_result_v2 *r)
{ return (r->eflags_values & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) != 0u; }

static int invoke(fixture_context *state, bx_ntvdm_dem_direct_context *direct,
    bx_ntvdm_exception_event_v1 *event, bx_ntvdm_cpu_state_v1 *cpu,
    bx_ntvdm_cpu_result_v2 *result, uint32_t service)
{
    bx_ntvdm_demhndl_call call;
    memset(&call,0,sizeof(call));
    call.magic=BX_NTVDM_DEMHNDL_CALL_MAGIC;
    call.abi_version=BX_NTVDM_DEMHNDL_CALL_VERSION;
    call.struct_bytes=sizeof(call); call.service=service; call.direct=direct;
    call.boundary=event; call.cpu=cpu; call.result=result; call.guest_state=state;
    call.guest_read=read_guest; call.guest_write=write_guest;
    return bx_ntvdm_demdisp_invoke(&call) &&
        result->disposition==BX_NTVDM_CPU_RESULT_V2_RESUME;
}

int main(void)
{
    fixture_context state; bx_ntvdm_dem_direct_context direct;
    bx_ntvdm_exception_event_v1 event; bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_cpu_result_v2 result;

    memset(&state,0,sizeof(state)); memset(&direct,0,sizeof(direct));
    direct.magic=BX_NTVDM_DEM_DIRECT_CONTEXT_MAGIC;
    direct.abi_version=BX_NTVDM_DEM_DIRECT_CONTEXT_VERSION;
    direct.struct_bytes=sizeof(direct); direct.state=&state;
    direct.publish_handle=publish; direct.lookup_handle=lookup;
    direct.release_handle=release; direct.query_attributes=attr_get;
    direct.set_attributes=attr_set;
    memset(&event,0,sizeof(event)); event.magic=BX_NTVDM_EXCEPTION_ABI_MAGIC;
    event.abi_version=BX_NTVDM_EXCEPTION_ABI_VERSION; event.struct_bytes=sizeof(event);
    event.kind=BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION; event.fault_rip=0x1000u;
    if (!bx_ntvdm_dem_direct_context_valid(&direct)) return 1;

    /* Original demIoctlChangeable: host C: is a non-removable volume. */
    bx_ntvdm_cpu_state_v1_initialize(&cpu,BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.eax=IOCTL_CHANGEABLE; cpu.ebx=2u;
    if (!invoke(&state,&direct,&event,&cpu,&result,0x21u) || carry(&result) ||
        result.cpu_delta.gpr16_values[0] != 1u) return 2;

    /* The historical range check was DBG-only.  The modern copied boundary
     * must preserve demIoctlInvalid's error contract instead of indexing the
     * imported 18-entry dispatch table out of bounds. */
    bx_ntvdm_cpu_state_v1_initialize(&cpu,BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.eax=0x12u;
    if (!invoke(&state,&direct,&event,&cpu,&result,0x21u) || !carry(&result) ||
        result.cpu_delta.gpr16_values[0] != ERROR_INVALID_FUNCTION) return 3;

    /* Original demAbsRead/Write both reject an unregistered raw BDS before
     * any raw-device shim is reached, returning DOS_DRIVE_NOT_READY. */
    bx_ntvdm_cpu_state_v1_initialize(&cpu,BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.eax=2u; cpu.ecx=1u;
    if (!invoke(&state,&direct,&event,&cpu,&result,0x29u) || !carry(&result) ||
        result.cpu_delta.gpr16_values[0] != DOS_DRIVE_NOT_READY) return 4;
    bx_ntvdm_cpu_state_v1_initialize(&cpu,BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.eax=2u; cpu.ecx=1u;
    if (!invoke(&state,&direct,&event,&cpu,&result,0x2au) || !carry(&result) ||
        result.cpu_delta.gpr16_values[0] != DOS_DRIVE_NOT_READY) return 5;
    puts("T230 S7 direct OpenNT DASD/IOCTL import: IOCTL, boundary and raw-drive failure contracts verified");
    return 0;
}
