#include <stdio.h>
#include <string.h>

#include "bop/shim/demdisp_shim.h"
#include "bop/shim/demerror_lock_shim.h"

extern SAVEDEMWORLD RetryInfo;

typedef struct fixture_context {
    uint8_t guest[0x10000];
    HANDLE file;
    uint32_t token;
} fixture_context;

static int publish(void *state, HANDLE file, uint32_t *token, DWORD *error)
{
    fixture_context *context = (fixture_context *)state;
    context->file = file;
    context->token = 0x12345678u;
    if (token != NULL) *token = context->token;
    if (error != NULL) *error = ERROR_SUCCESS;
    return 1;
}

static int lookup(void *state, uint32_t token, HANDLE *file)
{
    fixture_context *context = (fixture_context *)state;
    if (context == NULL || token != context->token || context->file == INVALID_HANDLE_VALUE)
        return 0;
    if (file != NULL) *file = context->file;
    return 1;
}

static int release(void *state, uint32_t token, DWORD *error)
{ (void)state; (void)token; if (error != NULL) *error = ERROR_INVALID_HANDLE; return 0; }
static int attr_get(void *state, uint8_t drive, const wchar_t *path, DWORD *attributes, DWORD *error)
{ (void)state; (void)drive; (void)path; if (attributes != NULL) *attributes=FILE_ATTRIBUTE_NORMAL; if(error!=NULL)*error=ERROR_SUCCESS; return 1; }
static int attr_set(void *state, uint8_t drive, const wchar_t *path, DWORD attributes, DWORD *error)
{ (void)state; (void)drive; (void)path; (void)attributes; if(error!=NULL)*error=ERROR_SUCCESS; return 1; }
static int read_guest(void *state, uint32_t address, uint8_t *buffer, uint32_t bytes)
{ fixture_context *c=state; if(c==NULL||address>sizeof(c->guest)||bytes>sizeof(c->guest)-address)return 0; memcpy(buffer,c->guest+address,bytes);return 1; }
static int write_guest(void *state, uint32_t address, const uint8_t *buffer, uint32_t bytes)
{ fixture_context *c=state; if(c==NULL||address>sizeof(c->guest)||bytes>sizeof(c->guest)-address)return 0; memcpy(c->guest+address,buffer,bytes);return 1; }

static int carry(const bx_ntvdm_cpu_result_v2 *result)
{ return (result->eflags_values & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) != 0u; }

static int invoke(fixture_context *state, bx_ntvdm_dem_direct_context *direct,
    bx_ntvdm_exception_event_v1 *event, bx_ntvdm_cpu_state_v1 *cpu,
    bx_ntvdm_cpu_result_v2 *result, uint32_t service)
{
    bx_ntvdm_demhndl_call call;
    memset(&call, 0, sizeof(call));
    call.magic=BX_NTVDM_DEMHNDL_CALL_MAGIC; call.abi_version=BX_NTVDM_DEMHNDL_CALL_VERSION;
    call.struct_bytes=sizeof(call); call.service=service; call.direct=direct;
    call.boundary=event; call.cpu=cpu; call.result=result; call.guest_state=state;
    call.guest_read=read_guest; call.guest_write=write_guest;
    return bx_ntvdm_demdisp_invoke(&call) &&
        result->disposition == BX_NTVDM_CPU_RESULT_V2_RESUME;
}

static void reset_cpu(bx_ntvdm_cpu_state_v1 *cpu)
{ bx_ntvdm_cpu_state_v1_initialize(cpu, BX_NTVDM_CPU_EXECUTION_REAL); cpu->ds=0x100u; }

int main(void)
{
    fixture_context state; bx_ntvdm_dem_direct_context direct;
    bx_ntvdm_exception_event_v1 event; bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_cpu_result_v2 result; char directory[MAX_PATH], path[MAX_PATH]; DWORD written;

    memset(&state,0,sizeof(state)); state.file=INVALID_HANDLE_VALUE;
    memset(&direct,0,sizeof(direct)); direct.magic=BX_NTVDM_DEM_DIRECT_CONTEXT_MAGIC;
    direct.abi_version=BX_NTVDM_DEM_DIRECT_CONTEXT_VERSION; direct.struct_bytes=sizeof(direct);
    direct.state=&state; direct.publish_handle=publish; direct.lookup_handle=lookup;
    direct.release_handle=release; direct.query_attributes=attr_get; direct.set_attributes=attr_set;
    memset(&event,0,sizeof(event)); event.magic=BX_NTVDM_EXCEPTION_ABI_MAGIC;
    event.abi_version=BX_NTVDM_EXCEPTION_ABI_VERSION; event.struct_bytes=sizeof(event);
    event.kind=BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION; event.fault_rip=0x1000u;
    if (!bx_ntvdm_dem_direct_context_valid(&direct) ||
        !GetTempPathA(MAX_PATH,directory) ||
        !GetTempFileNameA(directory,"s8",0u,path)) return 1;
    state.file=CreateFileA(path,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL);
    if (state.file==INVALID_HANDLE_VALUE || !WriteFile(state.file,"x",1u,&written,NULL) || written!=1u) return 2;
    state.token=0x12345678u;

    /* Original demSetHardErrorInfo records the DOS kernel's two guest
     * addresses.  This bounded invocation verifies that the source body is
     * reachable; hard-error continuation needs whole-DEM dispatcher binding. */
    reset_cpu(&cpu); cpu.edx=0x0200u; cpu.ebx=0x0220u;
    if (!invoke(&state,&direct,&event,&cpu,&result,0x32u) || carry(&result)) return 3;

    /* Original demRetry restores the saved service identity and reinvokes the
     * original table entry.  Retry a historical ordinary-return slot: this
     * proves the dispatcher-owned table is used, without inventing a retry
     * provider or taking an unrecorded host action. */
    memset(&RetryInfo, 0, sizeof(RetryInfo));
    RetryInfo.iSVC = 0x1fu;
    reset_cpu(&cpu);
    if (!invoke(&state,&direct,&event,&cpu,&result,0x33u) || carry(&result) ||
        CurrentISVC != 0x1fu) return 4;

    /* Original demlock.c obtains an opaque token through BX:BP and calls
     * Win32 LockFile/UnlockFile, preserving its own success/failure flow. */
    reset_cpu(&cpu); cpu.ebx=0x1234u; cpu.ebp=0x5678u; cpu.esi=0u; cpu.edi=1u;
    if (!invoke(&state,&direct,&event,&cpu,&result,0x3fu) || carry(&result)) return 5;
    reset_cpu(&cpu); cpu.eax=1u; cpu.ebx=0x1234u; cpu.ebp=0x5678u; cpu.esi=0u; cpu.edi=1u;
    if (!invoke(&state,&direct,&event,&cpu,&result,0x3fu) || carry(&result)) return 6;
    CloseHandle(state.file); DeleteFileA(path);
    puts("T230 S8 direct OpenNT demerror/demlock import: hard-error setup, dispatcher retry and file lock contracts verified");
    return 0;
}
