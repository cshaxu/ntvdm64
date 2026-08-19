#ifndef BX_NTVDM_DEM_NAMESPACE_ROUTE_PARTITION_V1_H
#define BX_NTVDM_DEM_NAMESPACE_ROUTE_PARTITION_V1_H

#include "bx_ntvdm_dem_namespace_partition_v1.h"
#include "bx_ntvdm_instruction_window_abi.h"
#include "bx-mantle/bx_ntvdm_mechanical_action_v1.h"

/* Shared, provider-internal pathname transaction for the entire demfile.c /
 * demdir.c pathname partition. It transports bounded OEM strings only; it
 * neither inspects guest memory directly nor selects a BOP provider. */
int bx_ntvdm_dem_namespace_route_partition_v1_owns_service(uint8_t service);
int bx_ntvdm_dem_namespace_route_partition_v1_prepare(
    bx_ntvdm_dem_whole_provider_v1 *provider, uint8_t service,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    struct bx_ntvdm_mechanical_action_v1 *action_out);
int bx_ntvdm_dem_namespace_route_partition_v1_complete(
    bx_ntvdm_dem_whole_provider_v1 *provider, uint8_t service,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const struct bx_ntvdm_mechanical_action_v1 *read_action,
    bx_ntvdm_cpu_result_v2 *result_out);

#endif
