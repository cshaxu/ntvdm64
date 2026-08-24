#include "opennt_xms_softpc_facade.h"

#include "adapter-softpc/bx_ntvdm_a20_capability_v1.h"
#include "adapter-softpc/bx_ntvdm_ivt_watch_v1.h"
#include "softpc_int15_watch_shim.h"

#include <string.h>

typedef struct bx_ntvdm_xms_active_context {
    bx_ntvdm_xms_softpc_context context;
    bx_ntvdm_cpu_state_v1 cpu;
} bx_ntvdm_xms_active_context;

static __declspec(thread) bx_ntvdm_xms_active_context *g_active_context;
static __declspec(thread) uint32_t g_himem_a20_state_address;
static __declspec(thread) int g_himem_a20_state_bound;

static bx_ntvdm_xms_active_context *active_context(void)
{
    return g_active_context;
}

static USHORT low16(uint32_t value)
{
    return (USHORT)(value & 0xffffu);
}

static void set_gpr(uint32_t index, USHORT value)
{
    bx_ntvdm_xms_active_context *active = active_context();
    uint32_t *target = NULL;
    if (active == NULL) return;
    switch (index) {
    case 0u: target = &active->cpu.eax; break;
    case 1u: target = &active->cpu.ecx; break;
    case 2u: target = &active->cpu.edx; break;
    case 3u: target = &active->cpu.ebx; break;
    default: return;
    }
    *target = (*target & 0xffff0000u) | value;
    (void)bx_ntvdm_cpu_delta_v1_set_gpr16(&active->context.result->cpu_delta,
        index, value);
}

int bx_ntvdm_xms_softpc_context_valid(const bx_ntvdm_xms_softpc_context *context)
{
    return context != NULL && context->cpu != NULL &&
        bx_ntvdm_cpu_state_v1_valid(context->cpu) &&
        context->cpu->execution_mode == BX_NTVDM_CPU_EXECUTION_REAL &&
        context->result != NULL && context->guest_read != NULL &&
        context->guest_write != NULL;
}

int bx_ntvdm_xms_softpc_context_begin(const bx_ntvdm_xms_softpc_context *context)
{
    static __declspec(thread) bx_ntvdm_xms_active_context active;
    if (!bx_ntvdm_xms_softpc_context_valid(context) || g_active_context != NULL)
        return 0;
    memset(&active, 0, sizeof(active));
    active.context = *context;
    active.cpu = *context->cpu;
    g_active_context = &active;
    return 1;
}

void bx_ntvdm_xms_softpc_context_end(void)
{
    g_active_context = NULL;
}

USHORT bx_ntvdm_xms_get_ax(void) { return low16(active_context()->cpu.eax); }
USHORT bx_ntvdm_xms_get_bx(void) { return low16(active_context()->cpu.ebx); }
USHORT bx_ntvdm_xms_get_cx(void) { return low16(active_context()->cpu.ecx); }
USHORT bx_ntvdm_xms_get_dx(void) { return low16(active_context()->cpu.edx); }
USHORT bx_ntvdm_xms_get_bp(void) { return low16(active_context()->cpu.ebp); }
USHORT bx_ntvdm_xms_get_ss(void) { return active_context()->cpu.ss; }
USHORT bx_ntvdm_xms_get_cs(void) { return active_context()->cpu.cs; }
void bx_ntvdm_xms_set_ax(USHORT value) { set_gpr(0u, value); }
void bx_ntvdm_xms_set_bx(USHORT value) { set_gpr(3u, value); }
void bx_ntvdm_xms_set_bl(USHORT value)
{ bx_ntvdm_xms_set_bx((USHORT)((bx_ntvdm_xms_get_bx() & 0xff00u) | (value & 0xffu))); }
void bx_ntvdm_xms_set_cx(USHORT value) { set_gpr(1u, value); }
void bx_ntvdm_xms_set_dx(USHORT value) { set_gpr(2u, value); }
void bx_ntvdm_xms_set_cf(int value)
{ (void)bx_ntvdm_cpu_result_v2_set_cf(active_context()->context.result, value); }

PVOID bx_ntvdm_xms_get_vdm_addr(USHORT segment, USHORT offset)
{
    (void)segment; (void)offset;
    /* DIVERGENCE (T261 S8): this reached i386 API returned a process pointer.
     * The imported xms block route uses the checked copy helper below; no
     * unbounded guest pointer may escape the adapter-softpc call epoch. */
    return NULL;
}

int bx_ntvdm_xms_bind_himem_a20_state(USHORT segment, USHORT offset)
{
    uint8_t value;
    uint32_t address = ((uint32_t)segment << 4) + (uint32_t)offset;
    bx_ntvdm_xms_active_context *active = active_context();
    if (active == NULL || !active->context.guest_read(active->context.guest_state,
        address, &value, 1u)) return 0;
    g_himem_a20_state_address = address;
    g_himem_a20_state_bound = 1;
    return 1;
}

void bx_ntvdm_xms_write_himem_a20_state(BYTE value)
{
    bx_ntvdm_xms_active_context *active = active_context();
    if (active == NULL || !g_himem_a20_state_bound) return;
    (void)active->context.guest_write(active->context.guest_state,
        g_himem_a20_state_address, &value, 1u);
}

void bx_ntvdm_xms_clear_himem_a20_state(void)
{
    g_himem_a20_state_address = 0u;
    g_himem_a20_state_bound = 0;
}

int bx_ntvdm_xms_copy_physical(uint32_t source, uint32_t destination,
    uint32_t bytes)
{
    uint8_t buffer[4096];
    uint32_t offset = 0u, chunk;
    bx_ntvdm_xms_active_context *active = active_context();
    if (active == NULL || bytes == 0u) return 0;
    while (offset != bytes) {
        chunk = bytes - offset > (uint32_t)sizeof(buffer) ?
            (uint32_t)sizeof(buffer) : bytes - offset;
        if (!active->context.guest_read(active->context.guest_state, source + offset,
                buffer, chunk) || !active->context.guest_write(
                active->context.guest_state, destination + offset, buffer, chunk)) return 0;
        offset += chunk;
    }
    return 1;
}

static uint32_t read_u32(const uint8_t *bytes)
{
    return (uint32_t)bytes[0] | ((uint32_t)bytes[1] << 8) |
        ((uint32_t)bytes[2] << 16) | ((uint32_t)bytes[3] << 24);
}

int bx_ntvdm_xms_move_block_from_guest(USHORT segment, USHORT offset)
{
    uint8_t descriptor[12];
    uint32_t base, words, source, destination;
    bx_ntvdm_xms_active_context *active = active_context();
    if (active == NULL || offset < 12u) return 0;
    base = ((uint32_t)segment << 4) + (uint32_t)offset - 12u;
    if (!active->context.guest_read(active->context.guest_state, base, descriptor,
            (uint32_t)sizeof(descriptor))) return 0;
    words = read_u32(descriptor + 8u);
    if (words > UINT32_MAX / 2u) return 0;
    source = read_u32(descriptor + 4u);
    destination = read_u32(descriptor);
    return words == 0u || bx_ntvdm_xms_copy_physical(source, destination, words * 2u);
}

WORD bx_ntvdm_xms_linear_to_segment(PVOID address)
{
    (void)address;
    return 0u; /* Current mantle has no admitted UMB map. */
}

static int range_readable(uint32_t address, uint32_t bytes)
{
    uint8_t buffer[4096];
    uint32_t offset = 0u, chunk;
    bx_ntvdm_xms_active_context *active = active_context();
    if (active == NULL || bytes == 0u) return 0;
    while (offset != bytes) {
        chunk = bytes - offset > (uint32_t)sizeof(buffer) ?
            (uint32_t)sizeof(buffer) : bytes - offset;
        if (!active->context.guest_read(active->context.guest_state, address + offset,
                buffer, chunk)) return 0;
        offset += chunk;
    }
    return 1;
}

NTSTATUS xmsCommitBlock(ULONG base, ULONG bytes)
{ return range_readable(base, bytes) ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL; }
NTSTATUS xmsDecommitBlock(ULONG base, ULONG bytes)
{ return range_readable(base, bytes) ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL; }
VOID xmsMoveMemory(ULONG destination, ULONG source, ULONG count)
{ (void)bx_ntvdm_xms_copy_physical(source, destination, count); }

BOOL ReserveUMB(ULONG owner, PVOID *address, PULONG bytes)
{
    (void)owner;
    if (address != NULL) *address = NULL;
    if (bytes != NULL) *bytes = 0u;
    return FALSE; /* Native XMS empty-UMB-list path. */
}

void bx_ntvdm_xms_a20_set(int enabled)
{
    struct bx_ntvdm_a20_capability_request_v1 request = {0};
    struct bx_ntvdm_a20_capability_result_v1 result = {0};
    request.version = BX_NTVDM_A20_CAPABILITY_V1_VERSION;
    request.operation = BX_NTVDM_A20_CAPABILITY_SET;
    request.requested_enabled = enabled ? 1u : 0u;
    bx_ntvdm_a20_capability_v1_dispatch(&request, &result);
}

int bx_ntvdm_xms_a20_enabled(void)
{
    struct bx_ntvdm_a20_capability_request_v1 request = {0};
    struct bx_ntvdm_a20_capability_result_v1 result = {0};
    request.version = BX_NTVDM_A20_CAPABILITY_V1_VERSION;
    request.operation = BX_NTVDM_A20_CAPABILITY_QUERY;
    bx_ntvdm_a20_capability_v1_dispatch(&request, &result);
    return result.status == BX_NTVDM_A20_CAPABILITY_OK && result.enabled != 0u;
}

int bx_ntvdm_xms_a20_available(void)
{
    struct bx_ntvdm_a20_capability_request_v1 request = {0};
    struct bx_ntvdm_a20_capability_result_v1 result = {0};
    request.version = BX_NTVDM_A20_CAPABILITY_V1_VERSION;
    request.operation = BX_NTVDM_A20_CAPABILITY_QUERY;
    bx_ntvdm_a20_capability_v1_dispatch(&request, &result);
    return result.status == BX_NTVDM_A20_CAPABILITY_OK;
}

void bx_ntvdm_softpc_int15_sas_loadw(uint32_t address, WORD *value)
{
    uint8_t bytes[2];
    bx_ntvdm_xms_active_context *active = active_context();
    if (value == NULL || active == NULL || (address != 0x54u && address != 0x56u) ||
        !active->context.guest_read(active->context.guest_state, address, bytes,
            (uint32_t)sizeof(bytes))) {
        if (value != NULL) *value = 0u;
        return;
    }
    *value = (WORD)((WORD)bytes[0] | ((WORD)bytes[1] << 8));
}

int bx_ntvdm_softpc_int15_watch_state_load(WORD *offset, WORD *segment)
{ return bx_ntvdm_ivt_watch_v1_copy_expected(0x15u, offset, segment); }
int bx_ntvdm_softpc_int15_watch_state_store(WORD offset, WORD segment)
{ return bx_ntvdm_ivt_watch_v1_store_expected(0x15u, offset, segment); }

void sas_enable_20_bit_wrapping(void) { bx_ntvdm_xms_a20_set(0); }
void sas_disable_20_bit_wrapping(void) { bx_ntvdm_xms_a20_set(1); }
BOOL sas_twenty_bit_wrapping_enabled(void)
{ return bx_ntvdm_xms_a20_enabled() ? FALSE : TRUE; }

void bx_ntvdm_xms_softpc_reset(void)
{
    bx_ntvdm_xms_clear_himem_a20_state();
    bx_ntvdm_xms_softpc_context_end();
}
