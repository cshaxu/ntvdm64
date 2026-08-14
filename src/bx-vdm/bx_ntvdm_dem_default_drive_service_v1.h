#ifndef BX_NTVDM_DEM_DEFAULT_DRIVE_SERVICE_V1_H
#define BX_NTVDM_DEM_DEFAULT_DRIVE_SERVICE_V1_H

#include "bx_ntvdm_guest_read_action_v1.h"
#include "bx_ntvdm_exception_abi.h"
#include "bx_ntvdm_instruction_window_abi.h"

/* OpenNT demSetDefaultDrive's observable first-byte validation.  A matching
 * path needs the historical NT current-directory capability and is therefore
 * deliberately terminated by the CLI completion branch. */
int bx_ntvdm_dem_default_drive_service_v1_prepare(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_guest_read_action_v1 *action);
int bx_ntvdm_dem_default_drive_service_v1_complete(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_guest_read_action_v1 *action,
    const uint8_t *bytes, uint64_t byte_count,
    bx_ntvdm_cpu_result_v2 *result);

#endif
