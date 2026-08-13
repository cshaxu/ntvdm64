#ifndef BX_NTVDM_DEM_DTA_SERVICE_H
#define BX_NTVDM_DEM_DTA_SERVICE_H

#include <stdint.h>

#include "bx_ntvdm_cpu_result_v2.h"
#include "bx_ntvdm_exception_abi.h"
#include "bx_ntvdm_guest_read_action_v1.h"

/* Adapter-owned, source-derived registration of the four NTDOS locations
 * recorded by demSetDTALocation.  These are physical guest locators, never
 * host pointers. */
typedef struct bx_ntvdm_dem_dta_registration_v1 {
    uint64_t dta_location;
    uint64_t current_pdb;
    uint64_t extended_error;
    uint64_t sft_head;
} bx_ntvdm_dem_dta_registration_v1;

/* Recognizes only real-mode SVC_DEMSETDTALOCATION (C4 C4 50 1B).  It asks
 * for exactly the little-endian low word of DOSWOWDATA::lpSftAddr. */
int bx_ntvdm_dem_dta_service_v1_dispatch(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_guest_read_action_v1 *action);

/* Completes only the action produced above and returns the source-equivalent
 * resume plus an adapter-owned registration. */
int bx_ntvdm_dem_dta_service_v1_complete(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_guest_read_action_v1 *action,
    const uint8_t *bytes, uint64_t byte_count,
    bx_ntvdm_dem_dta_registration_v1 *registration,
    bx_ntvdm_cpu_result_v2 *result);

#endif
