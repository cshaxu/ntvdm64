#include "bx_ntvdm_dem_drive_provider_v1.h"

#include <stdint.h>

static int bx_ntvdm_dem_drive_provider_v1_count(
    const bx_ntvdm_host_drive_snapshot_v1 *snapshot, uint16_t *count)
{
    uint32_t index;
    uint16_t value = 2u;
    if (!bx_ntvdm_host_drive_snapshot_v1_valid(snapshot) || count == 0)
        return 0;
    /* OpenNT counts contiguous physical drives.  A filtered host projection
     * can intentionally contain a letter gap (for example C excluded but D
     * admitted), so preserve DOS letter identity by reporting the highest
     * admitted physical-letter bound rather than reindexing or hiding D:. */
    for (index = 2u; index < 26u; ++index) {
        uint8_t type = snapshot->types[index];
        if ((snapshot->admitted_mask & (UINT32_C(1) << index)) != 0u &&
            (type == 2u || type == 3u || type == 5u || type == 6u))
            value = (uint16_t)(index + 1u);
    }
    *count = value;
    return 1;
}

int bx_ntvdm_dem_drive_provider_v1_dispatch(
    const bx_ntvdm_bop_ingress_v1 *ingress,
    const bx_ntvdm_bop_provider_selection_v1 *selection,
    const bx_ntvdm_dem_plane_record_v1 *plane,
    const bx_ntvdm_host_drive_snapshot_v1 *snapshot,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    bx_ntvdm_cpu_result_v2 *result)
{
    uint16_t count;
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
        ingress->family != BX_NTVDM_BOP_FAMILY_DEM || ingress->service != 15u ||
        plane->service != ingress->service ||
        plane->component != BX_NTVDM_DEM_COMPONENT_GSET ||
        plane->disposition != BX_NTVDM_DEM_PLANE_DEFERRED ||
        event->kind != BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION ||
        event->vector != 6u || event->fault_rip > UINT64_MAX - 4u ||
        !bx_ntvdm_dem_drive_provider_v1_count(snapshot, &count))
        return 0;
    return bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 4u) &&
        bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 0u, count) &&
        bx_ntvdm_cpu_result_v2_set_cf(result, 0);
}
