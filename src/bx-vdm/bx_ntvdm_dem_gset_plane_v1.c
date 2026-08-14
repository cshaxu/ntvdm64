#include "bx_ntvdm_dem_gset_plane_v1.h"
#include "bx_ntvdm_dem_clock_service_v1.h"
#include "bx_ntvdm_dem_volume_provider_v1.h"

#include <string.h>

void bx_ntvdm_dem_gset_plane_v1_clear(bx_ntvdm_dem_gset_plane_v1 *plane)
{
    if (plane == 0) return;
    plane->has_drive_snapshot = 0u;
    memset(&plane->drive_snapshot, 0, sizeof(plane->drive_snapshot));
    plane->has_volume_snapshot = 0u;
    memset(&plane->volume_snapshot, 0, sizeof(plane->volume_snapshot));
}

int bx_ntvdm_dem_gset_plane_v1_set_volume_snapshot(
    bx_ntvdm_dem_gset_plane_v1 *plane,
    const bx_ntvdm_host_volume_snapshot_v1 *snapshot)
{
    if (plane == 0 || snapshot == 0 ||
        !bx_ntvdm_host_volume_snapshot_v1_valid(snapshot)) return 0;
    plane->volume_snapshot = *snapshot;
    plane->has_volume_snapshot = 1u;
    return 1;
}

int bx_ntvdm_dem_gset_plane_v1_set_drive_snapshot(
    bx_ntvdm_dem_gset_plane_v1 *plane,
    const bx_ntvdm_host_drive_snapshot_v1 *snapshot)
{
    if (plane == 0 || snapshot == 0 ||
        !bx_ntvdm_host_drive_snapshot_v1_valid(snapshot)) return 0;
    plane->drive_snapshot = *snapshot;
    plane->has_drive_snapshot = 1u;
    return 1;
}

int bx_ntvdm_dem_gset_plane_v1_dispatch(
    bx_ntvdm_dem_gset_plane_v1 *plane,
    const bx_ntvdm_bop_ingress_v1 *ingress,
    const bx_ntvdm_bop_provider_selection_v1 *selection,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result)
{
    bx_ntvdm_dem_plane_record_v1 record;
    if (plane == 0 || ingress == 0 || selection == 0 || event == 0 ||
        cpu_before == 0 || window == 0 || result == 0 ||
        !plane->has_drive_snapshot ||
        !bx_ntvdm_host_drive_snapshot_v1_valid(&plane->drive_snapshot) ||
        !bx_ntvdm_dem_plane_v1_classify(ingress, selection, &record) ||
        record.component != BX_NTVDM_DEM_COMPONENT_GSET ||
        record.disposition != BX_NTVDM_DEM_PLANE_DEFERRED) return 0;
    if (bx_ntvdm_dem_clock_service_v1_dispatch(event, cpu_before, window, result))
        return 1;
    if (plane->has_volume_snapshot &&
        bx_ntvdm_dem_volume_provider_v1_dispatch(ingress, selection, &record,
            &plane->volume_snapshot, event, cpu_before, window, result))
        return 1;
    if (record.service != 0x0fu) return 0;
    return bx_ntvdm_dem_drive_provider_v1_dispatch(ingress, selection,
        &record, &plane->drive_snapshot, event, cpu_before, result);
}
