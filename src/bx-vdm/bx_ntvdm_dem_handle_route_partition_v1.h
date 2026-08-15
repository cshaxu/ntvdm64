#ifndef BX_NTVDM_DEM_HANDLE_ROUTE_PARTITION_V1_H
#define BX_NTVDM_DEM_HANDLE_ROUTE_PARTITION_V1_H

#include "bx_ntvdm_dem_whole_provider_v1.h"
#include "bx_ntvdm_instruction_window_abi.h"
#include "bx-mantle/bx_ntvdm_mechanical_action_v1.h"

/* The only ingress-facing route for the complete demhndl.c local-file
 * family. It moves copied CPU state and ordinary guest-RAM requests, never a
 * native handle, DOS object, or Bochs semantic. */
int bx_ntvdm_dem_handle_route_partition_v1_owns_service(uint8_t service);
/* During package migration, only claims a token actually issued by this
 * provider. Legacy fixture tokens remain with their legacy owner until the
 * single final package switch. */
int bx_ntvdm_dem_handle_route_partition_v1_claims_request(
    const bx_ntvdm_dem_whole_provider_v1 *provider, uint8_t service,
    const bx_ntvdm_cpu_state_v1 *cpu_before);
int bx_ntvdm_dem_handle_route_partition_v1_dispatch(
    bx_ntvdm_dem_whole_provider_v1 *provider, uint8_t service,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    struct bx_ntvdm_mechanical_action_v1 *action_out,
    bx_ntvdm_cpu_result_v2 *result_out);
int bx_ntvdm_dem_handle_route_partition_v1_complete_read(
    bx_ntvdm_dem_whole_provider_v1 *provider, uint8_t service,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const struct bx_ntvdm_mechanical_action_v1 *read_action,
    bx_ntvdm_cpu_result_v2 *result_out);

#endif
