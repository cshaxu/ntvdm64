#include "bx_ntvdm_dem_hard_error_service.h"

#define BX_NTVDM_DEM_HARD_ERROR_REAL_APERTURE UINT64_C(0x100000)

static int bx_ntvdm_dem_hard_error_physical(uint16_t segment, uint16_t offset,
    uint64_t *physical)
{
    uint64_t value;
    if (physical == 0) return 0;
    value = ((uint64_t)segment << 4) + (uint64_t)offset;
    if (value >= BX_NTVDM_DEM_HARD_ERROR_REAL_APERTURE) return 0;
    *physical = value;
    return 1;
}

int bx_ntvdm_dem_hard_error_service_v1_dispatch(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_dem_hard_error_registration_v1 *registration,
    bx_ntvdm_cpu_result_v2 *result)
{
    if (event == 0 || cpu_before == 0 || window == 0 || registration == 0 ||
        result == 0 || !bx_ntvdm_exception_event_v1_valid(event) ||
        !bx_ntvdm_cpu_state_v1_valid(cpu_before) ||
        !bx_ntvdm_instruction_window_v1_valid(window) || event->vector != 6u ||
        cpu_before->execution_mode != BX_NTVDM_CPU_EXECUTION_REAL ||
        window->valid_bytes < 4u || window->bytes[0] != 0xc4u ||
        window->bytes[1] != 0xc4u || window->bytes[2] != 0x50u ||
        window->bytes[3] != 0x32u || event->fault_rip > UINT64_MAX - 4u ||
        !bx_ntvdm_dem_hard_error_physical(cpu_before->ds,
            (uint16_t)cpu_before->edx, &registration->hard_error_packet) ||
        !bx_ntvdm_dem_hard_error_physical(cpu_before->ds,
            (uint16_t)cpu_before->ebx, &registration->device_chain)) return 0;
    return bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 4u);
}
