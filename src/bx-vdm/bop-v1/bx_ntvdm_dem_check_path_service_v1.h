#ifndef BX_NTVDM_DEM_CHECK_PATH_SERVICE_V1_H
#define BX_NTVDM_DEM_CHECK_PATH_SERVICE_V1_H

#include "bx_ntvdm_guest_read_action_v1.h"
#include "bx_ntvdm_exception_abi.h"
#include "bx_ntvdm_cpu_state_abi.h"

/* Source-derived `demCheckPath` subset.  It consumes a checked copied path
 * and exposes only the historical DOS `\DEV\` success branch; it never probes
 * or creates a host NUL device. */
int bx_ntvdm_dem_check_path_service_v1_prepare(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_guest_read_action_v1 *action);
int bx_ntvdm_dem_check_path_service_v1_complete(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_guest_read_action_v1 *action,
    const uint8_t *path_bytes, uint64_t path_byte_count,
    bx_ntvdm_cpu_result_v2 *result);

#endif
