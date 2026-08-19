#ifndef BX_NTVDM_DEM_CLOCK_SERVICE_V1_H
#define BX_NTVDM_DEM_CLOCK_SERVICE_V1_H

#include "bx_ntvdm_cpu_result_v2.h"
#include "bx_ntvdm_exception_abi.h"
#include "bx_ntvdm_dem_profile_consumer_v1.h"

/* Source-derived OpenNT DEM GSET clock adapter. Readonly refuses date/time
 * mutation before Win32; Direct invokes SetLocalTime with the original AL
 * result rule. */
int bx_ntvdm_dem_clock_service_v1_dispatch_with_profile(
    const bx_ntvdm_dem_profile_consumer_v1 *profile,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result);

#endif
