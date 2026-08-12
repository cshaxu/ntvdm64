#include "bx_ntvdm_dem_boot_provider_v1.h"

#include <stdint.h>

int bx_ntvdm_dem_boot_provider_v1_dispatch(
    const bx_ntvdm_bop_ingress_v1 *ingress,
    const bx_ntvdm_bop_provider_selection_v1 *selection,
    const bx_ntvdm_dem_plane_record_v1 *plane,
    uint32_t profile_drive_index,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    bx_ntvdm_cpu_result_v2 *result)
{
    uint16_t ax;
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
        ingress->family != BX_NTVDM_BOP_FAMILY_DEM || ingress->service != 13u ||
        plane->service != ingress->service ||
        plane->component != BX_NTVDM_DEM_COMPONENT_GSET ||
        plane->disposition != BX_NTVDM_DEM_PLANE_DEFERRED ||
        profile_drive_index >= 26u ||
        event->kind != BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION ||
        event->vector != 6u || event->fault_rip > UINT64_MAX - 4u)
        return 0;
    ax = (uint16_t)((cpu_before->eax & 0xff00u) | (profile_drive_index + 1u));
    return bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 4u) &&
        bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 0u, ax);
}
