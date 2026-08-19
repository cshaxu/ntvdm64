#ifndef BX_NTVDM_DEM_SEARCH_PARTITION_V1_H
#define BX_NTVDM_DEM_SEARCH_PARTITION_V1_H

#include "bx_ntvdm_dem_dta_service.h"
#include "bx_ntvdm_dem_whole_provider_v1.h"
#include "bx_ntvdm_instruction_window_abi.h"
#include "bx_ntvdm_multi_write_transaction.h"

/* The four demsrch.c service forms are one DEM partition.  This boundary
 * owns no BOP ingress, host handle, or guest memory access: it receives a
 * copied DTA registration, emits a checked gather, and accepts only its
 * matching copied bytes through the whole-provider continuation. */
int bx_ntvdm_dem_search_partition_v1_owns_service(uint8_t service);
int bx_ntvdm_dem_search_partition_v1_prepare(
    bx_ntvdm_dem_whole_provider_v1 *provider,
    const bx_ntvdm_dem_dta_registration_v1 *registration, uint8_t service,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_guest_gather_read_action_v1 *action_out);
int bx_ntvdm_dem_search_partition_v1_complete(
    bx_ntvdm_dem_whole_provider_v1 *provider, uint8_t service,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_guest_gather_read_action_v1 *action,
    const uint8_t *bytes, uint32_t byte_count,
    bx_ntvdm_multi_write_transaction_v1 *transaction,
    uint8_t payload[51], uint32_t *payload_bytes);

#endif
