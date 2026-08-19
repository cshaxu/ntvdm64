#include "bx_ntvdm_dem_check_path_service_v1.h"

#define BX_NTVDM_DEM_CHECK_PATH_APERTURE UINT64_C(0x100000)
#define BX_NTVDM_DEM_CHECK_PATH_BYTES 128u
#define BX_NTVDM_DEM_ERROR_ACCESS_DENIED 5u

static int matches(const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu, const bx_ntvdm_instruction_window_v1 *window)
{
    return event && cpu && window && bx_ntvdm_exception_event_v1_valid(event) &&
        bx_ntvdm_cpu_state_v1_valid(cpu) && bx_ntvdm_instruction_window_v1_valid(window) &&
        event->kind == BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION && event->vector == 6u &&
        event->fault_rip <= UINT64_MAX - 4u &&
        cpu->execution_mode == BX_NTVDM_CPU_EXECUTION_REAL && window->valid_bytes >= 4u &&
        window->bytes[0] == 0xc4u && window->bytes[1] == 0xc4u &&
        window->bytes[2] == 0x50u && window->bytes[3] == 0x44u;
}

static int physical(uint16_t segment, uint16_t offset, uint64_t *address)
{
    uint64_t value = ((uint64_t)segment << 4) + offset;
    if (!address || value >= BX_NTVDM_DEM_CHECK_PATH_APERTURE ||
        BX_NTVDM_DEM_CHECK_PATH_BYTES > BX_NTVDM_DEM_CHECK_PATH_APERTURE - value)
        return 0;
    *address = value;
    return 1;
}

static int dev_directory(const uint8_t *value, uint64_t bytes)
{
    return value && bytes == BX_NTVDM_DEM_CHECK_PATH_BYTES &&
        value[0] == '\\' && (value[1] == 'D' || value[1] == 'd') &&
        (value[2] == 'E' || value[2] == 'e') &&
        (value[3] == 'V' || value[3] == 'v') && value[4] == '\\' && value[5] == 0u;
}

static int complete(const bx_ntvdm_exception_event_v1 *event,
    bx_ntvdm_cpu_result_v2 *result, uint16_t ax, int carry)
{
    return bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 4u) &&
        bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 2u, 0u) &&
        (!carry || bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 0u, ax)) &&
        bx_ntvdm_cpu_result_v2_set_cf(result, carry);
}

int bx_ntvdm_dem_check_path_service_v1_prepare(
    const bx_ntvdm_exception_event_v1 *event, const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window, bx_ntvdm_guest_read_action_v1 *action)
{
    uint64_t address;
    if (!action || !matches(event, cpu, window) ||
        !physical(cpu->ds, (uint16_t)cpu->esi, &address)) return 0;
    return bx_ntvdm_guest_read_action_v1_need_read(action, address,
        BX_NTVDM_DEM_CHECK_PATH_BYTES);
}

int bx_ntvdm_dem_check_path_service_v1_complete(
    const bx_ntvdm_exception_event_v1 *event, const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_guest_read_action_v1 *action, const uint8_t *path_bytes,
    uint64_t path_byte_count, bx_ntvdm_cpu_result_v2 *result)
{
    uint64_t address;
    if (!event || !cpu || !action || !path_bytes || !result ||
        !bx_ntvdm_exception_event_v1_valid(event) ||
        !bx_ntvdm_cpu_state_v1_valid(cpu) ||
        !bx_ntvdm_guest_read_action_v1_valid(action) ||
        action->disposition != BX_NTVDM_GUEST_READ_ACTION_V1_NEED_READ ||
        !physical(cpu->ds, (uint16_t)cpu->esi, &address) ||
        action->guest_read.address != address ||
        action->guest_read.length != BX_NTVDM_DEM_CHECK_PATH_BYTES ||
        path_byte_count != BX_NTVDM_DEM_CHECK_PATH_BYTES ||
        event->fault_rip > UINT64_MAX - 4u)
        return 0;
    if (dev_directory(path_bytes, path_byte_count)) return complete(event, result, 0u, 0);
    return complete(event, result, BX_NTVDM_DEM_ERROR_ACCESS_DENIED, 1);
}
