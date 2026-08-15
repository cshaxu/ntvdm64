#include "bx_ntvdm_dem_volume_provider_v1.h"

#define BX_NTVDM_DEM_GET_DRIVE_FREE_SPACE 0x0eu
#define BX_NTVDM_DEM_ERROR_INVALID_DRIVE 15u

static int selected(const bx_ntvdm_bop_ingress_v1 *ingress,
    const bx_ntvdm_bop_provider_selection_v1 *selection,
    const bx_ntvdm_dem_plane_record_v1 *plane,
    const bx_ntvdm_host_volume_snapshot_v1 *volumes,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window)
{
    return ingress && selection && plane && volumes && event && cpu_before && window &&
        bx_ntvdm_bop_ingress_v1_valid(ingress) &&
        bx_ntvdm_bop_provider_selection_v1_valid(selection) &&
        bx_ntvdm_dem_plane_v1_valid(plane) &&
        bx_ntvdm_host_volume_snapshot_v1_valid(volumes) &&
        bx_ntvdm_exception_event_v1_valid(event) &&
        bx_ntvdm_cpu_state_v1_valid(cpu_before) &&
        bx_ntvdm_instruction_window_v1_valid(window) &&
        ingress->family == BX_NTVDM_BOP_FAMILY_DEM &&
        ingress->route == BX_NTVDM_BOP_ROUTE_MAPPED_DEFERRED &&
        selection->provider_family == BX_NTVDM_BOP_PROVIDER_DEM &&
        selection->disposition == BX_NTVDM_BOP_PROVIDER_DEFERRED &&
        plane->component == BX_NTVDM_DEM_COMPONENT_GSET &&
        plane->service == ingress->service &&
        event->kind == BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION && event->vector == 6u &&
        cpu_before->execution_mode == BX_NTVDM_CPU_EXECUTION_REAL &&
        event->fault_rip <= UINT64_MAX - 4u && window->valid_bytes >= 4u &&
        window->bytes[0] == 0xc4u && window->bytes[1] == 0xc4u &&
        window->bytes[2] == 0x50u && window->bytes[3] == ingress->service;
}

int bx_ntvdm_dem_volume_provider_v1_dos_geometry(
    const bx_ntvdm_host_volume_record_v1 *record,
    uint16_t *bytes_per_sector, uint16_t *sectors_per_cluster,
    uint16_t *total_clusters, uint16_t *free_clusters)
{
    uint64_t sectors = record->sectors_per_cluster;
    uint64_t total = record->total_clusters;
    uint64_t free = record->free_clusters;
    /* This is demGetDiskFreeSpace's DOS-visible 16-bit reduction: preserve
     * capacity as closely as possible while increasing cluster size. */
    if (record == 0 || bytes_per_sector == 0 || sectors_per_cluster == 0 ||
        total_clusters == 0 || free_clusters == 0 ||
        record->bytes_per_sector == 0u || record->sectors_per_cluster == 0u ||
        record->total_clusters == 0u) return 0;
    while (total > 0xffffu && sectors <= 0x7fffu) {
        sectors <<= 1u;
        total = (total + 1u) >> 1u;
        free = (free + 1u) >> 1u;
    }
    if (total > 0xffffu) total = 0xffffu;
    if (free > 0xffffu) free = 0xffffu;
    if (sectors > 0xffffu) sectors = 0xffffu;
    *bytes_per_sector = (uint16_t)record->bytes_per_sector;
    *sectors_per_cluster = (uint16_t)sectors;
    *total_clusters = (uint16_t)total;
    *free_clusters = (uint16_t)free;
    return 1;
}

int bx_ntvdm_dem_volume_provider_v1_dispatch(
    const bx_ntvdm_bop_ingress_v1 *ingress,
    const bx_ntvdm_bop_provider_selection_v1 *selection,
    const bx_ntvdm_dem_plane_record_v1 *plane,
    const bx_ntvdm_host_volume_snapshot_v1 *volumes,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result)
{
    uint8_t drive;
    const bx_ntvdm_host_volume_record_v1 *record;
    uint16_t bytes_per_sector, sectors_per_cluster, total_clusters, free_clusters;
    if (!result || !selected(ingress, selection, plane, volumes, event,
            cpu_before, window) || ingress->service != BX_NTVDM_DEM_GET_DRIVE_FREE_SPACE)
        return 0;
    drive = (uint8_t)cpu_before->eax;
    /* demGetDriveFreeSpace calls demClientError when its host volume query
     * fails.  An excluded or absent snapshot drive corresponds to the
     * ordinary ERROR_INVALID_DRIVE path, not the package's generic access
     * refusal.  The snapshot remains the sole host-observation source. */
    if (drive >= 26u || (volumes->drives.admitted_mask & (UINT32_C(1) << drive)) == 0u ||
        (record = &volumes->volumes[drive])->available == 0u)
        return bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 4u) &&
            bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 0u,
                BX_NTVDM_DEM_ERROR_INVALID_DRIVE) &&
            bx_ntvdm_cpu_result_v2_set_cf(result, 1);
    if (!bx_ntvdm_dem_volume_provider_v1_dos_geometry(record, &bytes_per_sector,
            &sectors_per_cluster, &total_clusters, &free_clusters)) return 0;
    return bx_ntvdm_cpu_result_v2_resume(result, event->fault_rip + 4u) &&
        bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 0u,
            (uint16_t)(cpu_before->eax & 0xff00u)) &&
        bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 1u, free_clusters) &&
        bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 2u, bytes_per_sector) &&
        bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 3u, total_clusters) &&
        bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 4u, sectors_per_cluster) &&
        bx_ntvdm_cpu_result_v2_set_cf(result, 0);
}
