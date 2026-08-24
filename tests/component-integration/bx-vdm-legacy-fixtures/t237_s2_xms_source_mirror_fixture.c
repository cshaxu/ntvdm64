#include "opennt-bop/xms/opennt_xms_compat.h"
#include "opennt-bop/ingress/xms_v2_runtime_session.h"
#include "adapter-bop/bx_ntvdm_generic_ud_bridge.h"
#include "opennt-bop/ingress/opennt_bop_route.h"

#include <stdlib.h>
#include <string.h>

#define TEST_MEMORY_BYTES (16u * 1024u * 1024u)

typedef struct fixture_memory {
    uint8_t *bytes;
    uint32_t byte_count;
} fixture_memory;

static int read_guest(void *state, uint32_t address, uint8_t *buffer,
    uint32_t bytes)
{
    fixture_memory *memory = (fixture_memory *)state;
    return memory != NULL && buffer != NULL && bytes != 0u &&
        address <= memory->byte_count && bytes <= memory->byte_count - address &&
        (memcpy(buffer, memory->bytes + address, bytes), 1);
}

static int write_guest(void *state, uint32_t address, const uint8_t *buffer,
    uint32_t bytes)
{
    fixture_memory *memory = (fixture_memory *)state;
    return memory != NULL && buffer != NULL && bytes != 0u &&
        address <= memory->byte_count && bytes <= memory->byte_count - address &&
        (memcpy(memory->bytes + address, buffer, bytes), 1);
}

static void put_u32(uint8_t *bytes, uint32_t value)
{
    bytes[0] = (uint8_t)value; bytes[1] = (uint8_t)(value >> 8);
    bytes[2] = (uint8_t)(value >> 16); bytes[3] = (uint8_t)(value >> 24);
}

static int invoke(fixture_memory *memory, uint32_t service,
    uint16_t ax, uint16_t bx, uint16_t dx, bx_ntvdm_cpu_result_v2 *result)
{
    bx_ntvdm_xms_call call;
    bx_ntvdm_exception_event_v1 boundary;
    bx_ntvdm_cpu_state_v1 cpu;
    memset(&call, 0, sizeof(call));
    memset(&boundary, 0, sizeof(boundary));
    boundary.magic = BX_NTVDM_EXCEPTION_ABI_MAGIC;
    boundary.abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION;
    boundary.struct_bytes = sizeof(boundary);
    boundary.kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION;
    boundary.vector = 6u;
    boundary.fault_rip = UINT64_C(0x8000);
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.eax = ax; cpu.ebx = bx; cpu.edx = dx; cpu.ss = 0x100u; cpu.ebp = 0x10u;
    call.magic = BX_NTVDM_XMS_CALL_MAGIC;
    call.abi_version = BX_NTVDM_XMS_CALL_VERSION;
    call.struct_bytes = sizeof(call);
    call.service = service;
    call.boundary = &boundary;
    call.cpu = &cpu;
    call.result = result;
    call.guest_state = memory;
    call.guest_read = read_guest;
    call.guest_write = write_guest;
    return bx_ntvdm_xms_invoke(&call);
}

static uint16_t result_gpr16(const bx_ntvdm_cpu_result_v2 *result, uint32_t index)
{
    return result->cpu_delta.gpr16_values[index];
}

int main(void)
{
    if (!bx_ntvdm_bop_ingress_v1_bind(bx_ntvdm_opennt_bop_route_dispatch_v1, 0)) return 90;
    fixture_memory memory;
    bx_ntvdm_cpu_result_v2 result;
    uint16_t first_base, second_base;
    uint32_t descriptor = ((uint32_t)0x100u << 4) + 0x10u - 12u;
    uint8_t source[] = { 0x11u, 0x22u, 0x33u, 0x44u };

    memset(&memory, 0, sizeof(memory));
    memory.byte_count = TEST_MEMORY_BYTES;
    memory.bytes = (uint8_t *)calloc(memory.byte_count, 1u);
    if (memory.bytes == NULL || !bx_ntvdm_xms_configure_memory_kib(8192u) ||
        !bx_ntvdm_xms_initialize()) return 1;

    /* Original 52:05 returns the configured XMS size. */
    if (!invoke(&memory, 5u, 0u, 0u, 0u, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        result_gpr16(&result, 0u) != 8192u) return 2;

    /* 52:00 and 52:06 require the mantle A20 lifecycle, while 52:09
     * requires the keyboard/BIOS-owned UpdateKbdInt15 mechanism.  The
     * standalone source fixture has neither and must decline rather than
     * silently claim that a source action occurred. */
    if (invoke(&memory, 0u, 2u, 0u, 0u, &result) ||
        invoke(&memory, 9u, 0u, 0u, 0u, &result)) return 9;

    if (!invoke(&memory, 4u, 0u, 0u, 0u, &result) ||
        result_gpr16(&result, 0u) == 0u) return 10;

    if (!invoke(&memory, 2u, 0u, 0u, 1024u, &result) ||
        (first_base = result_gpr16(&result, 0u)) == 0u) return 3;
    if (!invoke(&memory, 10u, 0u, 0u, 0u, &result) ||
        result_gpr16(&result, 0u) == 0u || result_gpr16(&result, 2u) == 0u) return 4;
    if (!invoke(&memory, 11u, first_base, 2048u, 1024u, &result) ||
        (second_base = result_gpr16(&result, 1u)) == 0u) return 5;
    if (!invoke(&memory, 3u, second_base, 0u, 2048u, &result) ||
        result_gpr16(&result, 0u) != 1u) return 6;

    memcpy(memory.bytes + 0x2000u, source, sizeof(source));
    put_u32(memory.bytes + descriptor, 0x3000u);
    put_u32(memory.bytes + descriptor + 4u, 0x2000u);
    put_u32(memory.bytes + descriptor + 8u, 2u);
    if (!invoke(&memory, 1u, 0u, 0u, 0u, &result) ||
        result_gpr16(&result, 0u) != 1u ||
        memcmp(memory.bytes + 0x3000u, source, sizeof(source)) != 0) return 7;

    /* Without a UMB initialization, the original static list is empty and
     * request/release retain their B1/B2 source failure results. */
    if (!invoke(&memory, 7u, 0u, 0u, 1u, &result) ||
        result_gpr16(&result, 0u) != 0u || (result_gpr16(&result, 3u) & 0xffu) != 0xb1u)
        return 8;
    if (!invoke(&memory, 8u, 0u, 0u, 0x1234u, &result) ||
        result_gpr16(&result, 0u) != 0u || (result_gpr16(&result, 3u) & 0xffu) != 0xb2u)
        return 11;

    /* This is the bounded product ingress witness: a copied #UD record is
     * classified in bx-vdm, reaches the original XMS dispatcher through the
     * session seam, and returns its source AX result and four-byte resume.
     * Query needs no extended-RAM allocation, so this does not pretend that
     * the current one-MiB machine has acquired an XMS aperture. */
    {
        struct bx_ntvdm_generic_ud_event_v1 event;
        struct bx_ntvdm_generic_ud_outcome_v1 outcome;
        memset(&event, 0, sizeof(event));
        event.magic = BX_NTVDM_GENERIC_UD_EVENT_V1_MAGIC;
        event.abi_version = BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
        event.struct_bytes = sizeof(event); event.vector = 6u;
        event.execution_mode = BX_NTVDM_CPU_EXECUTION_REAL;
        event.fault_rip = UINT64_C(0x2600); event.window_bytes = 4u;
        event.window[0] = 0xc4u; event.window[1] = 0xc4u;
        event.window[2] = 0x52u; event.window[3] = 5u;
        memset(&outcome, 0, sizeof(outcome));
        if (!bx_ntvdm_xms_v2_runtime_session_bind(8192u) ||
            !bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
            outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME ||
            outcome.resume_rip != UINT64_C(0x2604) ||
            outcome.gpr16_values[0] != 8192u) return 12;
        bx_ntvdm_xms_v2_runtime_session_reset();
    }

    free(memory.bytes);
    return 0;
}
