#ifndef BX_NTVDM_DEM_VOLUME_PROVIDER_V1_H
#define BX_NTVDM_DEM_VOLUME_PROVIDER_V1_H

#include "bx_ntvdm_host_volume_snapshot_v1.h"
#include "bx_ntvdm_dem_package_facade_v1.h"

int bx_ntvdm_dem_volume_provider_v1_dispatch(
    const bx_ntvdm_bop_ingress_v1 *ingress,
    const bx_ntvdm_bop_provider_selection_v1 *selection,
    const bx_ntvdm_dem_plane_record_v1 *plane,
    const bx_ntvdm_host_volume_snapshot_v1 *volumes,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result);

/* OpenNT demGetDiskFreeSpace's DOS-visible reduction, shared by GSET
 * free-space and fake-DPB providers. */
int bx_ntvdm_dem_volume_provider_v1_dos_geometry(
    const bx_ntvdm_host_volume_record_v1 *record,
    uint16_t *bytes_per_sector, uint16_t *sectors_per_cluster,
    uint16_t *total_clusters, uint16_t *free_clusters);

#endif
