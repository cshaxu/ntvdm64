#ifndef BX_NTVDM_DEM_CLOCK_SERVICE_V1_H
#define BX_NTVDM_DEM_CLOCK_SERVICE_V1_H

#include "bx_ntvdm_cpu_result_v2.h"
#include "bx_ntvdm_exception_abi.h"

/* Contained, read-only host-clock adaptation for OpenNT DEM GSET.  Date/time
 * setters deliberately report the original handler's AL=FF failure form and
 * never alter the ambient host clock. */
int bx_ntvdm_dem_clock_service_v1_dispatch(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result);

#endif
