#include "bx_ntvdm_dem_provider_v1.h"

#include <stdint.h>

static int bx_ntvdm_dem_provider_v1_original_noop(uint8_t service)
{
    switch (service) {
    case 31u: case 36u: case 38u: case 40u:
    case 43u: case 64u: case 67u:
        return 1;
    default:
        return 0;
    }
}

int bx_ntvdm_dem_provider_v1_dispatch(
    const bx_ntvdm_bop_ingress_v1 *ingress,
    const bx_ntvdm_bop_provider_selection_v1 *selection,
    const bx_ntvdm_dem_plane_record_v1 *plane,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    bx_ntvdm_cpu_result_v2 *result)
{
    if (result == 0 || !bx_ntvdm_bop_ingress_v1_valid(ingress) ||
        !bx_ntvdm_bop_provider_selection_v1_valid(selection) ||
        !bx_ntvdm_dem_plane_v1_valid(plane) ||
        !bx_ntvdm_exception_event_v1_valid(event) ||
        !bx_ntvdm_cpu_state_v1_valid(cpu_before)) return 0;
    bx_ntvdm_cpu_result_v2_pass_through(result);
    if (selection->disposition != BX_NTVDM_BOP_PROVIDER_DEFERRED ||
        selection->provider_family != BX_NTVDM_BOP_PROVIDER_DEM ||
        selection->precedence != BX_NTVDM_BOP_PROVIDER_PRECEDENCE_ORIGINAL_OPENNT ||
        ingress->route != BX_NTVDM_BOP_ROUTE_MAPPED_DEFERRED ||
        ingress->family != BX_NTVDM_BOP_FAMILY_DEM ||
        plane->service != ingress->service ||
        plane->component != BX_NTVDM_DEM_COMPONENT_ORIGINAL_NOOP ||
        plane->disposition != BX_NTVDM_DEM_PLANE_ORIGINAL_NOOP ||
        !bx_ntvdm_dem_provider_v1_original_noop(ingress->service) ||
        event->kind != BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION ||
        event->vector != 6u || event->fault_rip > UINT64_MAX - 4u)
        return 0;
    return bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 4u) &&
        bx_ntvdm_cpu_result_v2_set_cf(result, 0);
}
