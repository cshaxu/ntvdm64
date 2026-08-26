#include "opennt_xms_softpc_facade.h"

#include "adapter-softpc/a20_capability.h"
#include "adapter-softpc/guest_pointer_manager.h"
#include "adapter-softpc/ivt_watch.h"
#include "softpc_int15_watch_shim.h"

#include <string.h>

typedef struct runtime_xms_active_context {
    runtime_xms_softpc_context context;
    runtime_cpu_state cpu;
} runtime_xms_active_context;

static __declspec(thread) runtime_xms_active_context *g_active_context;
static __declspec(thread) uint32_t g_himem_a20_state_address;
static __declspec(thread) int g_himem_a20_state_bound;

static runtime_xms_active_context *active_context(void)
{
    return g_active_context;
}

static USHORT low16(uint32_t value)
{
    return (USHORT)(value & 0xffffu);
}

static void set_gpr(uint32_t index, USHORT value)
{
    runtime_xms_active_context *active = active_context();
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
    (void)runtime_cpu_delta_set_gpr16(&active->context.result->cpu_delta,
        index, value);
}

int runtime_xms_softpc_context_valid(const runtime_xms_softpc_context *context)
{
    return context != NULL && context->cpu != NULL &&
        runtime_cpu_state_valid(context->cpu) &&
        context->cpu->execution_mode == RUNTIME_CPU_EXECUTION_REAL &&
        context->result != NULL && context->guest_read != NULL &&
        context->guest_write != NULL;
}

int runtime_xms_softpc_context_begin(const runtime_xms_softpc_context *context)
{
    static __declspec(thread) runtime_xms_active_context active;
    if (!runtime_xms_softpc_context_valid(context) || g_active_context != NULL)
        return 0;
    memset(&active, 0, sizeof(active));
    active.context = *context;
    active.cpu = *context->cpu;
    /* DIVERGENCE(SOFTPC-DIV-006): the original XMS body obtained a process-wide
     * SAS alias.  The sole session guest-memory instance instead owns a
     * bounded call epoch; no pointer may survive this source invocation. */
    if (!runtime_guest_pointer_manager_begin(
            runtime_guest_pointer_manager_session(), context->guest_state,
            context->guest_read, context->guest_write)) return 0;
    g_active_context = &active;
    return 1;
}

void runtime_xms_softpc_context_end(void)
{
    runtime_guest_pointer_manager_end(runtime_guest_pointer_manager_session());
    g_active_context = NULL;
}

USHORT runtime_xms_get_ax(void) { return low16(active_context()->cpu.eax); }
USHORT runtime_xms_get_bx(void) { return low16(active_context()->cpu.ebx); }
USHORT runtime_xms_get_cx(void) { return low16(active_context()->cpu.ecx); }
USHORT runtime_xms_get_dx(void) { return low16(active_context()->cpu.edx); }
USHORT runtime_xms_get_bp(void) { return low16(active_context()->cpu.ebp); }
USHORT runtime_xms_get_ss(void) { return active_context()->cpu.ss; }
USHORT runtime_xms_get_cs(void) { return active_context()->cpu.cs; }
void runtime_xms_set_ax(USHORT value) { set_gpr(0u, value); }
void runtime_xms_set_bx(USHORT value) { set_gpr(3u, value); }
void runtime_xms_set_bl(USHORT value)
{ runtime_xms_set_bx((USHORT)((runtime_xms_get_bx() & 0xff00u) | (value & 0xffu))); }
void runtime_xms_set_cx(USHORT value) { set_gpr(1u, value); }
void runtime_xms_set_dx(USHORT value) { set_gpr(2u, value); }
void runtime_xms_set_cf(int value)
{ (void)runtime_cpu_result_set_cf(active_context()->context.result, value); }

PVOID runtime_xms_get_vdm_addr(USHORT segment, USHORT offset)
{
    (void)segment; (void)offset;
    /* DIVERGENCE (T261 S8): this reached i386 API returned a process pointer.
     * The imported xms block route uses the checked copy helper below; no
     * unbounded guest pointer may escape the adapter-softpc call epoch. */
    return NULL;
}

int runtime_xms_bind_himem_a20_state(USHORT segment, USHORT offset)
{
    runtime_guest_pointer_lease *lease;
    void *pointer;
    runtime_xms_active_context *active = active_context();
    if (active == NULL || !runtime_guest_pointer_manager_acquire_real_mode(
            runtime_guest_pointer_manager_session(), segment, offset, 1u,
            RUNTIME_GUEST_POINTER_READ, &lease, &pointer)) return 0;
    g_himem_a20_state_address = lease->address;
    g_himem_a20_state_bound = 1;
    return runtime_guest_pointer_manager_release(
        runtime_guest_pointer_manager_session(), lease, 0);
}

void runtime_xms_write_himem_a20_state(BYTE value)
{
    runtime_guest_pointer_lease *lease;
    void *pointer;
    runtime_xms_active_context *active = active_context();
    if (active == NULL || !g_himem_a20_state_bound) return;
    if (!runtime_guest_pointer_manager_acquire_linear(
            runtime_guest_pointer_manager_session(), g_himem_a20_state_address,
            1u, RUNTIME_GUEST_POINTER_WRITE, &lease, &pointer)) return;
    *(BYTE *)pointer = value;
    (void)runtime_guest_pointer_manager_release(
        runtime_guest_pointer_manager_session(), lease, 1);
}

void runtime_xms_clear_himem_a20_state(void)
{
    g_himem_a20_state_address = 0u;
    g_himem_a20_state_bound = 0;
}

int runtime_xms_copy_physical(uint32_t source, uint32_t destination,
    uint32_t bytes)
{
    uint8_t buffer[4096];
    uint32_t offset = 0u, chunk;
    runtime_xms_active_context *active = active_context();
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

int runtime_xms_move_block_from_guest(USHORT segment, USHORT offset)
{
    uint8_t descriptor[12];
    uint32_t base, words, source, destination;
    runtime_xms_active_context *active = active_context();
    if (active == NULL || offset < 12u) return 0;
    base = ((uint32_t)segment << 4) + (uint32_t)offset - 12u;
    if (!active->context.guest_read(active->context.guest_state, base, descriptor,
            (uint32_t)sizeof(descriptor))) return 0;
    words = read_u32(descriptor + 8u);
    if (words > UINT32_MAX / 2u) return 0;
    source = read_u32(descriptor + 4u);
    destination = read_u32(descriptor);
    return words == 0u || runtime_xms_copy_physical(source, destination, words * 2u);
}

WORD runtime_xms_linear_to_segment(PVOID address)
{
    (void)address;
    return 0u; /* Current machine has no admitted UMB map. */
}

static int range_readable(uint32_t address, uint32_t bytes)
{
    uint8_t buffer[4096];
    uint32_t offset = 0u, chunk;
    runtime_xms_active_context *active = active_context();
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
{ (void)runtime_xms_copy_physical(source, destination, count); }

BOOL ReserveUMB(ULONG owner, PVOID *address, PULONG bytes)
{
    (void)owner;
    if (address != NULL) *address = NULL;
    if (bytes != NULL) *bytes = 0u;
    return FALSE; /* Native XMS empty-UMB-list path. */
}

void runtime_xms_a20_set(int enabled)
{
    struct runtime_a20_capability_request request = {0};
    struct runtime_a20_capability_result result = {0};
    request.version = RUNTIME_A20_CAPABILITY_VERSION;
    request.operation = RUNTIME_A20_CAPABILITY_SET;
    request.requested_enabled = enabled ? 1u : 0u;
    runtime_a20_capability_dispatch(&request, &result);
}

int runtime_xms_a20_enabled(void)
{
    struct runtime_a20_capability_request request = {0};
    struct runtime_a20_capability_result result = {0};
    request.version = RUNTIME_A20_CAPABILITY_VERSION;
    request.operation = RUNTIME_A20_CAPABILITY_QUERY;
    runtime_a20_capability_dispatch(&request, &result);
    return result.status == RUNTIME_A20_CAPABILITY_OK && result.enabled != 0u;
}

int runtime_xms_a20_available(void)
{
    struct runtime_a20_capability_request request = {0};
    struct runtime_a20_capability_result result = {0};
    request.version = RUNTIME_A20_CAPABILITY_VERSION;
    request.operation = RUNTIME_A20_CAPABILITY_QUERY;
    runtime_a20_capability_dispatch(&request, &result);
    return result.status == RUNTIME_A20_CAPABILITY_OK;
}

void runtime_softpc_int15_sas_loadw(uint32_t address, WORD *value)
{
    uint8_t bytes[2];
    runtime_xms_active_context *active = active_context();
    if (value == NULL || active == NULL || (address != 0x54u && address != 0x56u) ||
        !active->context.guest_read(active->context.guest_state, address, bytes,
            (uint32_t)sizeof(bytes))) {
        if (value != NULL) *value = 0u;
        return;
    }
    *value = (WORD)((WORD)bytes[0] | ((WORD)bytes[1] << 8));
}

int runtime_softpc_int15_watch_state_load(WORD *offset, WORD *segment)
{ return runtime_ivt_watch_copy_expected(0x15u, offset, segment); }
int runtime_softpc_int15_watch_state_store(WORD offset, WORD segment)
{ return runtime_ivt_watch_store_expected(0x15u, offset, segment); }

void sas_enable_20_bit_wrapping(void) { runtime_xms_a20_set(0); }
void sas_disable_20_bit_wrapping(void) { runtime_xms_a20_set(1); }
BOOL sas_twenty_bit_wrapping_enabled(void)
{ return runtime_xms_a20_enabled() ? FALSE : TRUE; }

void runtime_xms_softpc_reset(void)
{
    runtime_xms_clear_himem_a20_state();
    runtime_xms_softpc_context_end();
}
