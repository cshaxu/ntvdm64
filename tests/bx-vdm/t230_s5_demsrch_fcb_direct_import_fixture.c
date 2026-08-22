#include <stdio.h>
#include <string.h>

#include "bop/shim/demdisp_shim.h"
#include "bop/shim/demsrch_fcb_shim.h"
#include "bop/shim/bx_ntvdm_host_handle_manager.h"

typedef struct fixture_context { uint8_t guest[0x20000]; bx_ntvdm_host_handle_manager handles; } fixture_context;
void DemHeartBeat(void);
static int publish(void *s, HANDLE h, uint32_t *t, DWORD *e)
{ fixture_context *c = s; uint32_t guest_handle; if (!c || !bx_ntvdm_host_handle_manager_publish(&c->handles, h, BX_NTVDM_HOST_HANDLE_OWNED, &guest_handle, e)) return 0; if (t) *t = guest_handle; return 1; }
static int lookup(void *s, uint32_t t, HANDLE *h)
{ fixture_context *c = s; if (h) *h = INVALID_HANDLE_VALUE; return c != NULL && t != 0u && t != UINT32_MAX && bx_ntvdm_host_handle_manager_lookup_handle(&c->handles, t, h); }
static int release(void *s, uint32_t t, DWORD *e)
{ fixture_context *c = s; if (!c || t == 0u || t == UINT32_MAX) { if (e) *e = ERROR_INVALID_HANDLE; return 0; } return bx_ntvdm_host_handle_manager_release(&c->handles, t, e); }
static int attribute_get(void *s, uint8_t d, const wchar_t *p, DWORD *a, DWORD *e)
{ (void)s; (void)d; (void)p; if (a) *a = FILE_ATTRIBUTE_NORMAL; if (e) *e = ERROR_SUCCESS; return 1; }
static int attribute_set(void *s, uint8_t d, const wchar_t *p, DWORD a, DWORD *e)
{ (void)s; (void)d; (void)p; (void)a; if (e) *e = ERROR_SUCCESS; return 1; }
static int read_guest(void *s, uint32_t a, uint8_t *b, uint32_t n)
{ fixture_context *c = s; if (!c || a > sizeof(c->guest) || n > sizeof(c->guest) - a) return 0; memcpy(b, c->guest + a, n); return 1; }
static int write_guest(void *s, uint32_t a, const uint8_t *b, uint32_t n)
{ fixture_context *c = s; if (!c || a > sizeof(c->guest) || n > sizeof(c->guest) - a) return 0; memcpy(c->guest + a, b, n); return 1; }

int main(void)
{
    fixture_context state; bx_ntvdm_dem_direct_context direct;
    bx_ntvdm_exception_event_v1 event; bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_cpu_result_v2 result; bx_ntvdm_demhndl_call call;
    CHAR temporary[MAX_PATH], temporary_second[MAX_PATH], temporary_directory[MAX_PATH], pattern[MAX_PATH], renamed[MAX_PATH], created[MAX_PATH], first_name[14]; ULONG dta_location; USHORT current_pdb = 0x50u; uint32_t fcb_handle; HANDLE seed; DWORD seed_bytes; unsigned tick;
    memset(&state, 0, sizeof(state)); memset(&direct, 0, sizeof(direct));
    if (!bx_ntvdm_host_handle_manager_initialize(&state.handles)) return 1;
    direct.magic = BX_NTVDM_DEM_DIRECT_CONTEXT_MAGIC;
    direct.abi_version = BX_NTVDM_DEM_DIRECT_CONTEXT_VERSION;
    direct.struct_bytes = sizeof(direct); direct.state = &state;
    direct.publish_handle = publish; direct.lookup_handle = lookup;
    direct.release_handle = release; direct.query_attributes = attribute_get;
    direct.set_attributes = attribute_set;
    memset(&event, 0, sizeof(event)); event.magic = BX_NTVDM_EXCEPTION_ABI_MAGIC;
    event.abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION; event.struct_bytes = sizeof(event);
    event.kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION; event.fault_rip = 0x1000u;
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    memset(&call, 0, sizeof(call)); call.magic = BX_NTVDM_DEMHNDL_CALL_MAGIC;
    call.abi_version = BX_NTVDM_DEMHNDL_CALL_VERSION; call.struct_bytes = sizeof(call);
    call.service = 0x30u; call.direct = &direct; call.boundary = &event;
    call.cpu = &cpu; call.result = &result; call.guest_state = &state;
    call.guest_read = read_guest; call.guest_write = write_guest;
    if (!bx_ntvdm_demdisp_invoke(&call) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        (result.cpu_delta.gpr16_write_mask & ((1u << 0) | (1u << 2))) !=
            ((1u << 0) | (1u << 2)) ||
        result.cpu_delta.gpr16_values[0] == 0u ||
        result.cpu_delta.gpr16_values[2] == 0u) return 1;
    if (GetTempPathA(MAX_PATH, temporary_directory) == 0u ||
        GetTempFileNameA(temporary_directory, "s5f", 0u, temporary) == 0u ||
        GetTempFileNameA(temporary_directory, "s5f", 0u, temporary_second) == 0u) return 2;
    seed = CreateFileA(temporary, GENERIC_WRITE, 0u, NULL, OPEN_EXISTING, 0u, NULL);
    if (seed == INVALID_HANDLE_VALUE || !WriteFile(seed, "OK", 2u, &seed_bytes, NULL) ||
        seed_bytes != 2u) return 2;
    CloseHandle(seed);
    if (sprintf_s(pattern, sizeof(pattern), "%ss5f*.tmp", temporary_directory) < 0) return 2;
    memcpy(state.guest + 0x1000u, pattern, strlen(pattern) + 1u);
    dta_location = 0x02000000u;
    pulDTALocation = &dta_location;
    pusCurrentPDB = &current_pdb;
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.ds = 0x100u;
    cpu.edx = 0u;
    cpu.ecx = 0u;
    call.service = 0x09u;
    if (!bx_ntvdm_demdisp_invoke(&call) ||
        (result.eflags_values & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) != 0u ||
        state.guest[0x2000u + 30u] == 0u) return 3;
    memcpy(first_name, state.guest + 0x2000u + 30u, sizeof(first_name));
    /* OpenNT's search heartbeat evicts an idle directory handle after 8640
     * ticks.  The next original FindNext must reopen and FileFindReset.  On
     * this product NtVdmControl(VdmQueryDir) is explicitly unavailable, so
     * that original reset takes its in-source slow enumeration path. */
    for (tick = 0u; tick < 8640u; ++tick) DemHeartBeat();
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    call.service = 0x0bu;
    if (!bx_ntvdm_demdisp_invoke(&call) ||
        (result.eflags_values & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) != 0u ||
        state.guest[0x2000u + 30u] == 0u ||
        memcmp(first_name, state.guest + 0x2000u + 30u, sizeof(first_name)) == 0) return 4;
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    if (!bx_ntvdm_demdisp_invoke(&call) ||
        (result.eflags_values & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) == 0u ||
        result.cpu_delta.gpr16_values[0] != ERROR_NO_MORE_FILES) return 4;
    memcpy(state.guest + 0x1000u, pattern, strlen(pattern) + 1u);
    memset(state.guest + 0x1200u, 0, 52u);
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.ds = 0x100u; cpu.es = 0x100u; cpu.esi = 0x200u;
    call.service = 0x0au;
    if (!bx_ntvdm_demdisp_invoke(&call) ||
        (result.eflags_values & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) != 0u ||
        state.guest[0x1201u] == 0u) return 5;
    memcpy(first_name, state.guest + 0x1201u, 11u);
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.ds = 0x100u; cpu.esi = 0x200u;
    call.service = 0x0cu;
    if (!bx_ntvdm_demdisp_invoke(&call) ||
        (result.eflags_values & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) != 0u ||
        state.guest[0x1201u] == 0u ||
        memcmp(first_name, state.guest + 0x1201u, 11u) == 0) return 6;
    state.handles.next_guest_handle = 0x10000u;
    memcpy(state.guest + 0x1000u, temporary, strlen(temporary) + 1u);
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.ds = 0x100u;
    call.service = 0x2du;
    if (!bx_ntvdm_demdisp_invoke(&call) ||
        (result.eflags_values & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) != 0u ||
        (result.cpu_delta.gpr16_write_mask & ((1u << 0) | (1u << 5))) !=
            ((1u << 0) | (1u << 5)) ||
        !bx_ntvdm_host_handle_manager_valid(&state.handles)) return 7;
    fcb_handle = ((uint32_t)result.cpu_delta.gpr16_values[0] << 16u) |
        result.cpu_delta.gpr16_values[5];
    if (fcb_handle != 0x10000u) return 7;
    memset(state.guest + 0x2000u, 0, 2u);
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.eax = fcb_handle >> 16u;
    cpu.ebp = fcb_handle;
    cpu.ebx = 1u;
    cpu.ecx = 2u;
    call.service = 0x2fu;
    if (!bx_ntvdm_demdisp_invoke(&call) ||
        (result.eflags_values & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) != 0u ||
        memcmp(state.guest + 0x2000u, "OK", 2u) != 0) return 7;
    if (!bx_ntvdm_host_handle_manager_release(&state.handles, fcb_handle, NULL)) return 7;
    memcpy(state.guest + 0x1000u, temporary, strlen(temporary) + 1u);
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.ds = 0x100u;
    call.service = 0x31u;
    if (!bx_ntvdm_demdisp_invoke(&call) ||
        (result.eflags_values & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) != 0u) return 8;
    if (sprintf_s(renamed, sizeof(renamed), "%s.ren", temporary_second) < 0) return 8;
    memcpy(state.guest + 0x1000u, temporary_second, strlen(temporary_second) + 1u);
    memcpy(state.guest + 0x1100u, renamed, strlen(renamed) + 1u);
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.ds = 0x100u; cpu.es = 0x100u; cpu.edi = 0x100u;
    call.service = 0x20u;
    if (!bx_ntvdm_demdisp_invoke(&call) ||
        (result.eflags_values & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) != 0u ||
        GetFileAttributesA(renamed) == INVALID_FILE_ATTRIBUTES) return 9;
    memcpy(state.guest + 0x1000u, renamed, strlen(renamed) + 1u);
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.es = 0x100u;
    call.service = 0x07u;
    if (!bx_ntvdm_demdisp_invoke(&call) ||
        (result.eflags_values & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) != 0u ||
        GetFileAttributesA(renamed) != INVALID_FILE_ATTRIBUTES) return 10;
    if (sprintf_s(created, sizeof(created), "%s.new", temporary_second) < 0) return 10;
    memcpy(state.guest + 0x1000u, created, strlen(created) + 1u);
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.ds = 0x100u;
    call.service = 0x2cu;
    if (!bx_ntvdm_demdisp_invoke(&call) ||
        (result.eflags_values & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) != 0u ||
        GetFileAttributesA(created) == INVALID_FILE_ATTRIBUTES ||
        !bx_ntvdm_host_handle_manager_valid(&state.handles)) return 11;
    fcb_handle = ((uint32_t)result.cpu_delta.gpr16_values[0] << 16u) |
        result.cpu_delta.gpr16_values[5];
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.eax = fcb_handle >> 16u;
    cpu.esi = fcb_handle;
    call.service = 0x2eu;
    if (!bx_ntvdm_demdisp_invoke(&call) ||
        (result.eflags_values & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) != 0u ||
        bx_ntvdm_host_handle_manager_lookup_handle(&state.handles,
            fcb_handle, &seed)) return 12;
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    call.service = 0x3cu;
    if (!bx_ntvdm_demdisp_invoke(&call) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME) return 13;
    bx_ntvdm_host_handle_manager_reset(&state.handles);
    DeleteFileA(temporary);
    DeleteFileA(temporary_second);
    DeleteFileA(created);
    puts("T230 S5 direct OpenNT demsrch/demfcb import: slow-reset search lifecycle, tokenized FCB operations and PDB cleanup verified");
    return 0;
}
