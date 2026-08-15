#ifndef BX_NTVDM_DEM_BOOT_DRIVE_SERVICE_H
#define BX_NTVDM_DEM_BOOT_DRIVE_SERVICE_H

#include "bx_ntvdm_cpu_state_abi.h"
#include "bx_ntvdm_exception_abi.h"
#include "bx_ntvdm_host_drive_policy.h"

#ifdef __cplusplus
extern "C" {
#endif
int bx_ntvdm_dem_boot_drive_service_v1_dispatch(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_exception_result_v1 *result);
/* Selected-host replacement for the modern package path. `preferred_drive`
 * is the admitted CLI startup drive; UINT32_MAX requests a real fixed-drive
 * fallback. No branch invents C: when C: is excluded or absent. */
int bx_ntvdm_dem_boot_drive_service_v2_dispatch(
    const bx_ntvdm_host_drive_snapshot_v1 *drives, uint32_t preferred_drive,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_exception_result_v1 *result);
#ifdef __cplusplus
}
#endif

#endif
