#ifndef BX_NTVDM_DEM_HARD_ERROR_SERVICE_H
#define BX_NTVDM_DEM_HARD_ERROR_SERVICE_H

#include <stdint.h>

#include "bx_ntvdm_cpu_result_v2.h"
#include "bx_ntvdm_exception_abi.h"

/* Adapter-only equivalents of DEM's pHardErrPacket and pDeviceChain. */
typedef struct bx_ntvdm_dem_hard_error_registration_v1 {
    uint64_t hard_error_packet;
    uint64_t device_chain;
} bx_ntvdm_dem_hard_error_registration_v1;

int bx_ntvdm_dem_hard_error_service_v1_dispatch(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_dem_hard_error_registration_v1 *registration,
    bx_ntvdm_cpu_result_v2 *result);

#endif
