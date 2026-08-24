#include <stdio.h>
#include <string.h>

#include "opennt-bop/dem/opennt_dem_dispatch_composition.h"
#include "opennt-bop/dem/opennt_demgset_composition.h"

/* The fixture crosses the already-imported search owner after DTA
 * registration, but it deliberately imports only the S6 compatibility
 * surface so the two original-layout headers remain independently owned. */
int runtime_demsrch_fcb_invoke(runtime_demhndl_call *call);

typedef struct fixture_context { uint8_t guest[0x30000]; } fixture_context;

static int publish(void *s, HANDLE h, uint32_t *t, DWORD *e)
{ (void)s; (void)h; if (t) *t = 1u; if (e) *e = ERROR_SUCCESS; return 1; }
static int lookup(void *s, uint32_t t, HANDLE *h)
{ (void)s; (void)t; if (h) *h = INVALID_HANDLE_VALUE; return 0; }
static int release(void *s, uint32_t t, DWORD *e)
{ (void)s; (void)t; if (e) *e = ERROR_INVALID_HANDLE; return 0; }
static int attr_get(void *s, uint8_t d, const wchar_t *p, DWORD *a, DWORD *e)
{ (void)s; (void)d; (void)p; if (a) *a = FILE_ATTRIBUTE_NORMAL; if (e) *e = ERROR_SUCCESS; return 1; }
static int attr_set(void *s, uint8_t d, const wchar_t *p, DWORD a, DWORD *e)
{ (void)s; (void)d; (void)p; (void)a; if (e) *e = ERROR_SUCCESS; return 1; }
static int read_guest(void *s, uint32_t a, uint8_t *b, uint32_t n)
{ fixture_context *c = s; if (!c || a > sizeof(c->guest) || n > sizeof(c->guest) - a) return 0; memcpy(b, c->guest + a, n); return 1; }
static int write_guest(void *s, uint32_t a, const uint8_t *b, uint32_t n)
{ fixture_context *c = s; if (!c || a > sizeof(c->guest) || n > sizeof(c->guest) - a) return 0; memcpy(c->guest + a, b, n); return 1; }
static int invoke(fixture_context *s, runtime_dem_direct_context *d,
    runtime_exception_event_v1 *e, runtime_cpu_state_v1 *cpu,
    runtime_cpu_result_v2 *r, uint32_t service, int search)
{ runtime_demhndl_call c; (void)search; memset(&c, 0, sizeof(c)); c.magic=RUNTIME_DEMHNDL_CALL_MAGIC; c.abi_version=RUNTIME_DEMHNDL_CALL_VERSION; c.struct_bytes=sizeof(c); c.service=service; c.direct=d; c.boundary=e; c.cpu=cpu; c.result=r; c.guest_state=s; c.guest_read=read_guest; c.guest_write=write_guest; return runtime_demdisp_invoke(&c) && r->disposition==RUNTIME_CPU_RESULT_V2_RESUME; }
static void reset_cpu(runtime_cpu_state_v1 *c)
{ runtime_cpu_state_v1_initialize(c, RUNTIME_CPU_EXECUTION_REAL); c->ds=0x100u; c->es=0x100u; }
static int carry(const runtime_cpu_result_v2 *r)
{ return (r->eflags_values & RUNTIME_CPU_RESULT_V2_EFLAGS_CF) != 0u; }
static SYSTEMTIME written_time;
static BOOL fake_clock_writer(const SYSTEMTIME *time)
{ if (time == NULL) return FALSE; written_time = *time; return TRUE; }

int main(void)
{
    fixture_context state; runtime_dem_direct_context direct;
    runtime_exception_event_v1 event; runtime_cpu_state_v1 cpu;
    runtime_cpu_result_v2 result; ULONG far_dta = 0x02000000u;
    USHORT pdb = 0x50u; char directory[MAX_PATH], file[MAX_PATH], pattern[MAX_PATH], old_directory[MAX_PATH];
    HANDLE seed; DWORD bytes;
    memset(&state,0,sizeof(state)); memset(&direct,0,sizeof(direct));
    direct.magic=RUNTIME_DEM_DIRECT_CONTEXT_MAGIC; direct.abi_version=RUNTIME_DEM_DIRECT_CONTEXT_VERSION; direct.struct_bytes=sizeof(direct); direct.state=&state; direct.publish_handle=publish; direct.lookup_handle=lookup; direct.release_handle=release; direct.query_attributes=attr_get; direct.set_attributes=attr_set;
    memset(&event,0,sizeof(event)); event.magic=RUNTIME_EXCEPTION_ABI_MAGIC; event.abi_version=RUNTIME_EXCEPTION_ABI_VERSION; event.struct_bytes=sizeof(event); event.kind=RUNTIME_EXCEPTION_EVENT_CPU_EXCEPTION; event.fault_rip=0x1000u;
    if (!runtime_dem_direct_context_valid(&direct)) return 1;
    memcpy(state.guest+0x1000u,&far_dta,sizeof(far_dta)); memcpy(state.guest+0x1100u,&pdb,sizeof(pdb));
    reset_cpu(&cpu); cpu.eax=0u; cpu.edx=0x100u; cpu.ecx=0x200u; cpu.esi=0x300u;
    if (!invoke(&state,&direct,&event,&cpu,&result,0x1bu,0) || carry(&result)) return 2;
    reset_cpu(&cpu); if (!invoke(&state,&direct,&event,&cpu,&result,0x14u,0) || result.cpu_delta.gpr16_values[1] < 1980u) return 3;
    reset_cpu(&cpu); if (!invoke(&state,&direct,&event,&cpu,&result,0x15u,0) || result.cpu_delta.gpr16_write_mask == 0u) return 4;
    reset_cpu(&cpu); if (!invoke(&state,&direct,&event,&cpu,&result,0x0du,0) || result.cpu_delta.gpr16_values[0] == 0u) return 5;
    reset_cpu(&cpu); cpu.eax=2u; if (!invoke(&state,&direct,&event,&cpu,&result,0x0eu,0) || carry(&result)) return 6;
    reset_cpu(&cpu); if (!invoke(&state,&direct,&event,&cpu,&result,0x0fu,0) || carry(&result) || result.cpu_delta.gpr16_values[0] < 2u) return 7;
    memset(state.guest + 0x1800u, 0, 25u); reset_cpu(&cpu); cpu.eax=0u; cpu.ebx=2u; cpu.edx=0x800u;
    if (!invoke(&state,&direct,&event,&cpu,&result,0x10u,0) || carry(&result) || state.guest[0x1800u+6u]==0u) return 8;
    memset(state.guest + 0x1900u, 0, 33u); reset_cpu(&cpu); cpu.eax=2u; cpu.edi=0x900u;
    if (!invoke(&state,&direct,&event,&cpu,&result,0x25u,0) || carry(&result) || state.guest[0x1900u+3u]==0u) {
        fprintf(stderr, "dpb: invoke=%u cf=%u ax=%u sector-lo=%u last=%lu\n",
            result.disposition == RUNTIME_CPU_RESULT_V2_RESUME, carry(&result),
            result.cpu_delta.gpr16_values[0], state.guest[0x1900u+3u],
            (unsigned long)GetLastError());
        return 9;
    }
    memset(state.guest + 0x1a00u, 0, 16u); reset_cpu(&cpu); cpu.edx=0xa00u;
    if (!invoke(&state,&direct,&event,&cpu,&result,0x41u,0) || state.guest[0x1a00u]==0u) return 10;
    memset(state.guest + 0x1b00u, 0, 33u * 26u); reset_cpu(&cpu); cpu.ebp=0xb00u;
    if (!invoke(&state,&direct,&event,&cpu,&result,0x46u,0) || result.cpu_delta.gpr16_values[5] <= 0xb00u) return 11;
    if (!GetCurrentDirectoryA(MAX_PATH,old_directory)) return 12;
    memcpy(state.guest+0x1f00u,old_directory,strlen(old_directory)+1u); reset_cpu(&cpu); cpu.esi=0xf00u; cpu.edx=(uint8_t)(toupper((unsigned char)old_directory[0])-'A');
    if (!invoke(&state,&direct,&event,&cpu,&result,0x1au,0) || carry(&result)) {
        fprintf(stderr, "default-drive: invoke=%u cf=%u ax=%u path=%s last=%lu\n",
            result.disposition == RUNTIME_CPU_RESULT_V2_RESUME, carry(&result),
            result.cpu_delta.gpr16_values[0], old_directory,
            (unsigned long)GetLastError());
        return 13;
    }
    runtime_demgset_set_clock_writer(fake_clock_writer);
    reset_cpu(&cpu); cpu.ecx=1999u; cpu.edx=0x031cu;
    if (!invoke(&state,&direct,&event,&cpu,&result,0x19u,0) ||
        (result.cpu_delta.gpr16_values[0]&0xffu)!=0u || written_time.wYear!=1999u ||
        written_time.wMonth!=3u || written_time.wDay!=28u) return 14;
    reset_cpu(&cpu); cpu.ecx=0x173au; cpu.edx=0x2d32u;
    if (!invoke(&state,&direct,&event,&cpu,&result,0x1cu,0) ||
        (result.cpu_delta.gpr16_values[0]&0xffu)!=0u || written_time.wHour!=23u ||
        written_time.wMinute!=58u || written_time.wSecond!=45u || written_time.wMilliseconds!=500u) return 15;
    runtime_demgset_set_clock_writer(NULL);
    /* GetTempFileName consumes the historical three-character prefix. */
    if (!GetTempPathA(MAX_PATH,directory) || GetTempFileNameA(directory,"gset",0u,file)==0u || sprintf_s(pattern,sizeof(pattern),"%sgse*.tmp",directory)<0) return 16;
    seed=CreateFileA(file,GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL); if(seed==INVALID_HANDLE_VALUE||!WriteFile(seed,"x",1u,&bytes,NULL)||bytes!=1u)return 16; CloseHandle(seed);
    memcpy(state.guest+0x1400u,pattern,strlen(pattern)+1u); reset_cpu(&cpu); cpu.edx=0x400u;
    if (!invoke(&state,&direct,&event,&cpu,&result,0x09u,1) || carry(&result) || state.guest[0x2000u+30u]==0u) {
        fprintf(stderr, "find-first: invoke=%u cf=%u ax=%u dta-name=%u last=%lu\n",
            result.disposition == RUNTIME_CPU_RESULT_V2_RESUME,
            carry(&result), result.cpu_delta.gpr16_values[0],
            state.guest[0x2000u+30u], (unsigned long)GetLastError());
        return 17;
    }
    DeleteFileA(file);
    puts("T230 S6 direct OpenNT demgset import: DTA registration, drive, clock and search continuation verified");
    return 0;
}
