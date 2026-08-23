#include "xms_shim.h"

#include "adapter-softpc/bx_ntvdm_a20_capability_v1.h"
#include "adapter-softpc/bx_ntvdm_ivt_watch_v1.h"
#include "softpc_int15_watch_shim.h"

#include <string.h>

typedef struct bx_ntvdm_xms_active_call {
    bx_ntvdm_xms_call *call;
    bx_ntvdm_cpu_state_v1 cpu;
} bx_ntvdm_xms_active_call;

static __declspec(thread) bx_ntvdm_xms_active_call *g_active_call;
static ULONG g_configured_memory_kib;
static int g_xms_initialized;
/* DIVERGENCE (T237 S5): OpenNT's xmsa20.c retained a host-mapped pointer
 * from GetVDMAddr.  A x64-safe composition retains the real-mode physical
 * byte address and writes it through the existing checked guest boundary. */
static __declspec(thread) uint32_t g_himem_a20_state_address;
static __declspec(thread) int g_himem_a20_state_bound;

/* The direct SoftPC fragment stages only its historical expected pair here;
 * its opaque storage is owned by the selector-blind mantle watch. */
int bx_ntvdm_softpc_int15_watch_source_begin(void);
int bx_ntvdm_softpc_int15_watch_source_end(void);
void bx_ntvdm_softpc_int15_watch_source_reset(void);

static bx_ntvdm_xms_active_call *active_call(void)
{
    return g_active_call;
}

static USHORT low16(uint32_t value)
{
    return (USHORT)(value & 0xffffu);
}

static void set_gpr(uint32_t index, USHORT value)
{
    bx_ntvdm_xms_active_call *active = active_call();
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
    (void)bx_ntvdm_cpu_delta_v1_set_gpr16(&active->call->result->cpu_delta,
        index, value);
}

int bx_ntvdm_xms_call_valid(const bx_ntvdm_xms_call *call)
{
    return call != NULL && call->magic == BX_NTVDM_XMS_CALL_MAGIC &&
        call->abi_version == BX_NTVDM_XMS_CALL_VERSION &&
        call->struct_bytes == sizeof(*call) && call->service < XMS_LASTSVC &&
        call->boundary != NULL && bx_ntvdm_exception_event_v1_valid(call->boundary) &&
        call->cpu != NULL && bx_ntvdm_cpu_state_v1_valid(call->cpu) &&
        call->cpu->execution_mode == BX_NTVDM_CPU_EXECUTION_REAL &&
        call->result != NULL && call->guest_read != NULL && call->guest_write != NULL;
}

USHORT bx_ntvdm_xms_get_ax(void) { return low16(active_call()->cpu.eax); }
USHORT bx_ntvdm_xms_get_bx(void) { return low16(active_call()->cpu.ebx); }
USHORT bx_ntvdm_xms_get_cx(void) { return low16(active_call()->cpu.ecx); }
USHORT bx_ntvdm_xms_get_dx(void) { return low16(active_call()->cpu.edx); }
USHORT bx_ntvdm_xms_get_bp(void) { return low16(active_call()->cpu.ebp); }
USHORT bx_ntvdm_xms_get_ss(void) { return active_call()->cpu.ss; }
USHORT bx_ntvdm_xms_get_cs(void) { return active_call()->cpu.cs; }
void bx_ntvdm_xms_set_ax(USHORT value) { set_gpr(0u, value); }
void bx_ntvdm_xms_set_bx(USHORT value) { set_gpr(3u, value); }
void bx_ntvdm_xms_set_bl(USHORT value)
{ bx_ntvdm_xms_set_bx((USHORT)((bx_ntvdm_xms_get_bx() & 0xff00u) | (value & 0xffu))); }
void bx_ntvdm_xms_set_cx(USHORT value) { set_gpr(1u, value); }
void bx_ntvdm_xms_set_dx(USHORT value) { set_gpr(2u, value); }
void bx_ntvdm_xms_set_cf(int value)
{ (void)bx_ntvdm_cpu_result_v2_set_cf(active_call()->call->result, value); }

PVOID bx_ntvdm_xms_get_vdm_addr(USHORT segment, USHORT offset)
{
    (void)segment; (void)offset;
    /* Deliberately unavailable until the checked bounce-span seam is wired.
     * A raw guest pointer is not permitted to escape this adapter boundary. */
    return NULL;
}

int bx_ntvdm_xms_bind_himem_a20_state(USHORT segment, USHORT offset)
{
    uint8_t value;
    uint32_t address = ((uint32_t)segment << 4) + (uint32_t)offset;
    bx_ntvdm_xms_active_call *active = active_call();
    if (active == NULL || active->call == NULL ||
        !active->call->guest_read(active->call->guest_state, address, &value, 1u))
        return 0;
    g_himem_a20_state_address = address;
    g_himem_a20_state_bound = 1;
    return 1;
}

void bx_ntvdm_xms_write_himem_a20_state(BYTE value)
{
    bx_ntvdm_xms_active_call *active = active_call();
    if (active == NULL || active->call == NULL || !g_himem_a20_state_bound)
        return;
    /* The source body has already performed the A20 transition.  A failed
     * publication is never treated as a host-pointer fallback. */
    (void)active->call->guest_write(active->call->guest_state,
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
    uint32_t offset = 0u;
    uint32_t chunk;
    bx_ntvdm_xms_active_call *active = active_call();
    if (active == NULL || active->call == NULL || bytes == 0u) return 0;
    /* The original xmsMoveBlock used RtlCopyMemory, whose only guaranteed
     * overlap direction is destination below source.  Retain that forward
     * traversal; no source-derived memmove upgrade is introduced here. */
    while (offset != bytes) {
        chunk = bytes - offset > (uint32_t)sizeof(buffer) ?
            (uint32_t)sizeof(buffer) : bytes - offset;
        if (!active->call->guest_read(active->call->guest_state, source + offset,
                buffer, chunk) || !active->call->guest_write(active->call->guest_state,
                destination + offset, buffer, chunk)) return 0;
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
    bx_ntvdm_xms_active_call *active = active_call();
    if (active == NULL || active->call == NULL || offset < 12u) return 0;
    base = ((uint32_t)segment << 4) + (uint32_t)offset - 12u;
    if (!active->call->guest_read(active->call->guest_state, base, descriptor,
            (uint32_t)sizeof(descriptor))) return 0;
    words = read_u32(descriptor + 8u);
    if (words > UINT32_MAX / 2u) return 0;
    source = read_u32(descriptor + 4u);
    destination = read_u32(descriptor);
    return words == 0u || bx_ntvdm_xms_copy_physical(source, destination,
        words * 2u);
}

WORD bx_ntvdm_xms_linear_to_segment(PVOID address)
{
    (void)address;
    /* The current explicit unavailable UMB capability never publishes a
     * host address.  When bx-mantle admits UMB reservation, this conversion
     * will consume its fixed guest-linear result, not cast a host pointer. */
    return 0u;
}

static int range_readable(uint32_t address, uint32_t bytes)
{
    uint8_t buffer[4096];
    uint32_t offset = 0u;
    uint32_t chunk;
    bx_ntvdm_xms_active_call *active = active_call();
    if (active == NULL || active->call == NULL || bytes == 0u) return 0;
    while (offset != bytes) {
        chunk = bytes - offset > (uint32_t)sizeof(buffer) ?
            (uint32_t)sizeof(buffer) : bytes - offset;
        if (!active->call->guest_read(active->call->guest_state, address + offset,
                buffer, chunk)) return 0;
        offset += chunk;
    }
    return 1;
}

/* The original suballocator owns allocation state.  Its callbacks only
 * ensure that an admitted native guest-RAM range is accessible; no host heap
 * or second allocation table is introduced here. */
NTSTATUS xmsCommitBlock(ULONG base, ULONG bytes)
{
    return range_readable(base, bytes) ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL;
}

NTSTATUS xmsDecommitBlock(ULONG base, ULONG bytes)
{
    return range_readable(base, bytes) ? STATUS_SUCCESS : STATUS_UNSUCCESSFUL;
}

VOID xmsMoveMemory(ULONG destination, ULONG source, ULONG count)
{
    (void)bx_ntvdm_xms_copy_physical(source, destination, count);
}

BOOL ReserveUMB(ULONG owner, PVOID *address, PULONG bytes)
{
    (void)owner;
    if (address != NULL) *address = NULL;
    if (bytes != NULL) *bytes = 0u;
    /* No UMB map exists in the current minimal mantle.  Returning no map is
     * the original XMS list's empty-input path, not an adapter allocation. */
    return FALSE;
}

int bx_ntvdm_xms_configure_memory_kib(ULONG kib)
{
    /* OpenNT's XMS module has no destruction entry.  This product supports
     * one process-local VDM session, so a later bind may only reaffirm the
     * original capacity; it must never reset/reallocate source-owned state. */
    if (kib < 1024u || (g_xms_initialized && kib != g_configured_memory_kib)) return 0;
    if (g_xms_initialized) return 1;
    g_configured_memory_kib = kib;
    xmsMemorySize = kib;
    return 1;
}

void bx_ntvdm_xms_reset(void)
{
    /* Test-only process teardown helper.  A live session may not call this:
     * OpenNT's imported suballocator has no standalone destruction entry. */
    g_configured_memory_kib = 0u;
    g_xms_initialized = 0;
    xmsMemorySize = 0u;
    ExtMemSA = NULL;
    bx_ntvdm_xms_clear_himem_a20_state();
    bx_ntvdm_softpc_int15_watch_source_reset();
}

int bx_ntvdm_xms_initialize(void)
{
    /* OpenNT's softpc startup calls XMSInit before it can dispatch BOP_XMS.
     * Keep that source lifecycle ordering here; a BOP must not lazily create
     * allocation state on its first guest request. */
    if (g_xms_initialized) return 1;
    if (g_configured_memory_kib == 0u || !XMSInit(0, NULL)) return 0;
    g_xms_initialized = 1;
    return 1;
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
    bx_ntvdm_xms_active_call *active = active_call();
    if (value == NULL || active == NULL || active->call == NULL ||
        (address != 0x54u && address != 0x56u) ||
        !active->call->guest_read(active->call->guest_state, address, bytes,
            (uint32_t)sizeof(bytes))) {
        if (value != NULL) *value = 0u;
        return;
    }
    *value = (WORD)((WORD)bytes[0] | ((WORD)bytes[1] << 8));
}

int bx_ntvdm_softpc_int15_watch_state_load(WORD *offset, WORD *segment)
{
    return bx_ntvdm_ivt_watch_v1_copy_expected(0x15u, offset, segment);
}

int bx_ntvdm_softpc_int15_watch_state_store(WORD offset, WORD segment)
{
    return bx_ntvdm_ivt_watch_v1_store_expected(0x15u, offset, segment);
}

void sas_enable_20_bit_wrapping(void) { bx_ntvdm_xms_a20_set(0); }
void sas_disable_20_bit_wrapping(void) { bx_ntvdm_xms_a20_set(1); }
BOOL sas_twenty_bit_wrapping_enabled(void) { return bx_ntvdm_xms_a20_enabled() ? FALSE : TRUE; }

int bx_ntvdm_xms_invoke(bx_ntvdm_xms_call *call)
{
    bx_ntvdm_xms_active_call active;
    int invoked;
    if (!bx_ntvdm_xms_call_valid(call) || g_active_call != NULL ||
        call->boundary->fault_rip > UINT64_MAX - 4u) return 0;
    /* 52:00 enters the original SAS A20 path.  The source assumes that
     * mechanism exists and supplies no failure register contract.  Do not
     * turn an inactive mantle lifecycle into a false source success. */
    if (call->service == 0u && !bx_ntvdm_xms_a20_available()) return 0;
    /* 52:06 stores the HIMEM state byte across future A20 calls.  Bind its
     * real-mode address before entering the source body; no raw host pointer
     * is exposed or retained. */
    if (call->service == 6u && !bx_ntvdm_xms_a20_available()) return 0;
    memset(&active, 0, sizeof(active));
    active.call = call;
    active.cpu = *call->cpu;
    bx_ntvdm_cpu_result_v2_pass_through(call->result);
    if (!bx_ntvdm_cpu_result_v2_resume(call->result, call->boundary->fault_rip + 4u))
        return 0;
    g_active_call = &active;
    if (!g_xms_initialized) {
        g_active_call = NULL;
        return 0;
    }
    if (call->service == 6u &&
        !bx_ntvdm_xms_bind_himem_a20_state(low16(active.cpu.eax),
            low16(active.cpu.ebx))) {
        g_active_call = NULL;
        return 0;
    }
    /* Original xmsNotifyHookI15 has no error register path.  Therefore the
     * typed provider must prove the configured IVT watch is live before it
     * enters XMSDispatch, rather than letting a missing state turn into a
     * silent source success. */
    if (call->service == 9u && !bx_ntvdm_softpc_int15_watch_source_begin()) {
        g_active_call = NULL;
        return 0;
    }
    invoked = XMSDispatch(call->service);
    if (call->service == 9u && !bx_ntvdm_softpc_int15_watch_source_end())
        invoked = 0;
    g_active_call = NULL;
    return invoked && bx_ntvdm_cpu_result_v2_valid(call->result);
}
