#ifndef BX_NTVDM_DEM_FCB_IO_ROUTE_PARTITION_V1_H
#define BX_NTVDM_DEM_FCB_IO_ROUTE_PARTITION_V1_H

#include "bx_ntvdm_dem_whole_provider_v1.h"
#include "bx_ntvdm_instruction_window_abi.h"
#include "bx-mantle/bx_ntvdm_mechanical_action_v1.h"

/* demfcb.c's FCBIO route.  The caller supplies the already registered DTA
 * guest-physical address; the route neither discovers DOS state nor accepts
 * a host pointer.  It claims only an opaque token issued by this provider. */
int bx_ntvdm_dem_fcb_io_route_partition_v1_claims_request(
    const bx_ntvdm_dem_whole_provider_v1 *provider, uint8_t service,
    const bx_ntvdm_cpu_state_v1 *cpu_before);
int bx_ntvdm_dem_fcb_io_route_partition_v1_dispatch(
    bx_ntvdm_dem_whole_provider_v1 *provider, uint8_t service,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window, uint64_t dta_address,
    struct bx_ntvdm_mechanical_action_v1 *action_out,
    bx_ntvdm_cpu_result_v2 *result_out);
int bx_ntvdm_dem_fcb_io_route_partition_v1_complete_write(
    bx_ntvdm_dem_whole_provider_v1 *provider, uint8_t service,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const struct bx_ntvdm_mechanical_action_v1 *read_action,
    bx_ntvdm_cpu_result_v2 *result_out);

#endif
