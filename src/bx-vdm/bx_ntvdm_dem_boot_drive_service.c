#include "bx_ntvdm_dem_boot_drive_service.h"

#define BX_NTVDM_DEM_BOOT_DRIVE_FALLBACK_AL 0x03u

int bx_ntvdm_dem_boot_drive_service_v1_dispatch(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_exception_result_v1 *result)
{
    uint16_t ax;
    if (event == 0 || cpu_before == 0 || window == 0 || result == 0 ||
        !bx_ntvdm_exception_event_v1_valid(event) ||
        !bx_ntvdm_cpu_state_v1_valid(cpu_before) ||
        !bx_ntvdm_instruction_window_v1_valid(window) || event->vector != 6u ||
        cpu_before->execution_mode != BX_NTVDM_CPU_EXECUTION_REAL ||
        window->valid_bytes < 4u || window->bytes[0] != 0xc4u ||
        window->bytes[1] != 0xc4u || window->bytes[2] != 0x50u ||
        window->bytes[3] != 0x0du || event->fault_rip > UINT64_MAX - 4u)
        return 0;
    /* OpenNT demGetBootDrive defaults Drive to C: when the registry-derived
     * boot root is unavailable or unsuitable.  The contained CLI has no
     * registry relationship, so use that source-defined fallback only. */
    ax = (uint16_t)((cpu_before->eax & 0xff00u) |
        BX_NTVDM_DEM_BOOT_DRIVE_FALLBACK_AL);
    if (!bx_ntvdm_exception_result_v1_resume(result, event->fault_rip + 4u) ||
        !bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 0u, ax)) {
        bx_ntvdm_exception_result_v1_pass_through(result);
        return 0;
    }
    return 1;
}
