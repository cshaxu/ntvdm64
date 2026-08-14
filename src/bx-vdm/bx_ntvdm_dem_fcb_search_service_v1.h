#ifndef BX_NTVDM_DEM_FCB_SEARCH_SERVICE_V1_H
#define BX_NTVDM_DEM_FCB_SEARCH_SERVICE_V1_H

#include "bx_ntvdm_profile_search_snapshot_v1.h"
#include "bx_ntvdm_search_transaction_v1.h"
#include "bx_ntvdm_guest_gather_read_action_v1.h"
#include "bx_ntvdm_dem_dta_service.h"

int bx_ntvdm_dem_fcb_search_service_v1_prepare_first(
    const bx_ntvdm_dem_dta_registration_v1 *registration,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_guest_gather_read_action_v1 *action);
int bx_ntvdm_dem_fcb_search_service_v1_prepare_next(
    const bx_ntvdm_dem_dta_registration_v1 *registration,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_guest_gather_read_action_v1 *action);
int bx_ntvdm_dem_fcb_search_service_v1_complete_first(
    bx_ntvdm_search_transaction_v1 *search,
    const bx_ntvdm_profile_search_snapshot_v1 *snapshot,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_guest_gather_read_action_v1 *action,
    const uint8_t *bytes, uint64_t byte_count,
    bx_ntvdm_multi_write_transaction_v1 *transaction, uint8_t payload[51],
    uint32_t *payload_bytes);
int bx_ntvdm_dem_fcb_search_service_v1_complete_next(
    bx_ntvdm_search_transaction_v1 *search,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_guest_gather_read_action_v1 *action,
    const uint8_t *bytes, uint64_t byte_count,
    bx_ntvdm_multi_write_transaction_v1 *transaction, uint8_t payload[51],
    uint32_t *payload_bytes);

#endif
