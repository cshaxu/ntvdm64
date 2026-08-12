#include "bx_ntvdm_dem_ioctl_changeable_service.h"

int bx_ntvdm_dem_ioctl_changeable_service_v1_dispatch(
    const uint8_t inventory_types[26], const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result)
{
    uint8_t drive, type;
    uint16_t ax;
    if (inventory_types == 0 || event == 0 || cpu_before == 0 || window == 0 ||
        result == 0 || !bx_ntvdm_exception_event_v1_valid(event) ||
        !bx_ntvdm_cpu_state_v1_valid(cpu_before) ||
        !bx_ntvdm_instruction_window_v1_valid(window) || event->vector != 6u ||
        cpu_before->execution_mode != BX_NTVDM_CPU_EXECUTION_REAL ||
        window->valid_bytes < 4u || window->bytes[0] != 0xc4u ||
        window->bytes[1] != 0xc4u || window->bytes[2] != 0x50u ||
        window->bytes[3] != 0x21u || (cpu_before->eax & 0xffffu) != 0x4408u ||
        event->fault_rip > UINT64_MAX - 4u) return 0;
    drive = (uint8_t)cpu_before->ebx;
    if (drive >= 26u || (type = inventory_types[drive]) < 2u || type > 6u) return 0;
    ax = type == 2u ? 0u : 1u;
    if (!bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 4u) ||
        !bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 0u, ax) ||
        !bx_ntvdm_cpu_result_v2_set_cf(result, 0)) {
        bx_ntvdm_cpu_result_v2_pass_through(result); return 0;
    }
    return 1;
}
