#include "bx_ntvdm_command_stream_child_v1.h"

#include <windows.h>
#include <string.h>

#define BX_NTVDM_COMMAND_STREAM_TOKEN_PREFIX 0xb0000000u
#define BX_NTVDM_COMMAND_STREAM_TOKEN_GENERATION_MASK 0x03ffffffu

static uint32_t next_generation = 1u;

static int selected(const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window)
{
    return event != 0 && cpu != 0 && window != 0 &&
        bx_ntvdm_exception_event_v1_valid(event) &&
        bx_ntvdm_cpu_state_v1_valid(cpu) &&
        bx_ntvdm_instruction_window_v1_valid(window) && event->vector == 6u &&
        cpu->execution_mode == BX_NTVDM_CPU_EXECUTION_REAL &&
        event->fault_rip <= UINT64_MAX - 4u && window->valid_bytes >= 4u &&
        window->bytes[0] == 0xc4u && window->bytes[1] == 0xc4u &&
        window->bytes[2] == 0x54u && window->bytes[3] == 6u;
}

static uint32_t token(const bx_ntvdm_command_stream_child_v1 *value,
    uint32_t slot)
{
    return BX_NTVDM_COMMAND_STREAM_TOKEN_PREFIX |
        ((value->generation & BX_NTVDM_COMMAND_STREAM_TOKEN_GENERATION_MASK) << 2u) |
        slot;
}

void bx_ntvdm_command_stream_child_v1_initialize(
    bx_ntvdm_command_stream_child_v1 *value)
{
    static const DWORD identifiers[BX_NTVDM_COMMAND_STREAM_CHILD_V1_SLOT_COUNT] = {
        STD_INPUT_HANDLE, STD_OUTPUT_HANDLE, STD_ERROR_HANDLE
    };
    uint32_t slot;
    if (value == 0) return;
    memset(value, 0, sizeof(*value));
    value->magic = BX_NTVDM_COMMAND_STREAM_CHILD_V1_MAGIC;
    value->abi_version = BX_NTVDM_COMMAND_STREAM_CHILD_V1_VERSION;
    value->struct_bytes = sizeof(*value);
    value->generation = next_generation++ & BX_NTVDM_COMMAND_STREAM_TOKEN_GENERATION_MASK;
    if (value->generation == 0u) value->generation = 1u;
    for (slot = 0u; slot < BX_NTVDM_COMMAND_STREAM_CHILD_V1_SLOT_COUNT; ++slot) {
        HANDLE handle = GetStdHandle(identifiers[slot]);
        if (handle != NULL && handle != INVALID_HANDLE_VALUE) {
            value->private_handle_values[slot] = (uintptr_t)handle;
            value->available_mask |= 1u << slot;
        }
    }
}

int bx_ntvdm_command_stream_child_v1_valid(
    const bx_ntvdm_command_stream_child_v1 *value)
{
    uint32_t slot;
    if (value == 0 || value->magic != BX_NTVDM_COMMAND_STREAM_CHILD_V1_MAGIC ||
        value->abi_version != BX_NTVDM_COMMAND_STREAM_CHILD_V1_VERSION ||
        value->struct_bytes != sizeof(*value) || value->generation == 0u ||
        (value->generation & ~BX_NTVDM_COMMAND_STREAM_TOKEN_GENERATION_MASK) != 0u ||
        (value->available_mask & ~0x07u) != 0u || value->reserved0 != 0u)
        return 0;
    for (slot = 0u; slot < BX_NTVDM_COMMAND_STREAM_CHILD_V1_SLOT_COUNT; ++slot) {
        if ((value->available_mask & (1u << slot)) == 0u &&
            value->private_handle_values[slot] != 0u) return 0;
    }
    return 1;
}

int bx_ntvdm_command_stream_child_v1_dispatch_stream(
    const bx_ntvdm_command_stream_child_v1 *value, int direct_granted,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result)
{
    uint32_t slot, opaque;
    if (!bx_ntvdm_command_stream_child_v1_valid(value) || result == 0 ||
        !selected(event, cpu, window)) return 0;
    slot = cpu->ecx & 0xffffu;
    if (!direct_granted || slot >= BX_NTVDM_COMMAND_STREAM_CHILD_V1_SLOT_COUNT ||
        (value->available_mask & (1u << slot)) == 0u) {
        return bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 4u) &&
            bx_ntvdm_cpu_result_v2_set_cf(result, 1) &&
            bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 0u, 0u) &&
            bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 2u, 0u);
    }
    opaque = token(value, slot);
    return bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 4u) &&
        bx_ntvdm_cpu_result_v2_set_cf(result, 0) &&
        bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 0u, 0u) &&
        bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 1u,
            (uint16_t)opaque) &&
        bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 2u, 0u) &&
        bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 3u,
            (uint16_t)(opaque >> 16u));
}
