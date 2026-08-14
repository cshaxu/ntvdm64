#include "bx_ntvdm_dem_default_drive_service_v1.h"

#define BX_NTVDM_DEM_ERROR_INVALID_FUNCTION 1u
#define BX_NTVDM_DEM_ERROR_ACCESS_DENIED 5u

static int event_cpu_valid(const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu)
{
    return event && cpu && bx_ntvdm_exception_event_v1_valid(event) &&
        bx_ntvdm_cpu_state_v1_valid(cpu) &&
        event->vector == 6u && cpu->execution_mode == BX_NTVDM_CPU_EXECUTION_REAL &&
        event->fault_rip <= UINT64_MAX - 4u;
}

static int selected(const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu, const bx_ntvdm_instruction_window_v1 *window)
{
    return event_cpu_valid(event, cpu) && window &&
        bx_ntvdm_instruction_window_v1_valid(window) && window->valid_bytes >= 4u &&
        window->bytes[0] == 0xc4u && window->bytes[1] == 0xc4u &&
        window->bytes[2] == 0x50u && window->bytes[3] == 0x1au;
}

int bx_ntvdm_dem_default_drive_service_v1_prepare(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_guest_read_action_v1 *action)
{
    uint64_t address;
    if (!action || !selected(event, cpu, window)) return 0;
    address = ((uint64_t)cpu->ds << 4) + (uint16_t)cpu->esi;
    if (address >= UINT64_C(0x100000)) return 0;
    return bx_ntvdm_guest_read_action_v1_need_read(action, address, 1u);
}

int bx_ntvdm_dem_default_drive_service_v1_complete(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_guest_read_action_v1 *action,
    const uint8_t *bytes, uint64_t byte_count,
    bx_ntvdm_cpu_result_v2 *result)
{
    uint64_t address;
    uint16_t error;
    if (!result || !bytes || byte_count != 1u || !event_cpu_valid(event, cpu) ||
        !bx_ntvdm_guest_read_action_v1_valid(action) ||
        action->disposition != BX_NTVDM_GUEST_READ_ACTION_V1_NEED_READ) return 0;
    address = ((uint64_t)cpu->ds << 4) + (uint16_t)cpu->esi;
    if (action->guest_read.address != address || action->guest_read.length != 1u)
        return 0;
    error = bytes[0] == (uint8_t)((uint8_t)cpu->edx + (uint8_t)'A') ?
        BX_NTVDM_DEM_ERROR_ACCESS_DENIED : BX_NTVDM_DEM_ERROR_INVALID_FUNCTION;
    return bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 4u) &&
        bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 0u, error) &&
        bx_ntvdm_cpu_result_v2_set_cf(result, 1);
}
