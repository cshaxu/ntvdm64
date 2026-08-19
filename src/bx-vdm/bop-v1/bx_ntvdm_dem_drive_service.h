#ifndef BX_NTVDM_DEM_DRIVE_SERVICE_H
#define BX_NTVDM_DEM_DRIVE_SERVICE_H

#include <stdint.h>

#include "bx_ntvdm_cpu_result_v2.h"
#include "bx_ntvdm_exception_abi.h"

int bx_ntvdm_dem_drive_service_v1_dispatch(
    const uint8_t inventory_types[26],
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result);

#endif
