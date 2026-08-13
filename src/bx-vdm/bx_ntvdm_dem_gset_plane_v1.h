#ifndef BX_NTVDM_DEM_GSET_PLANE_V1_H
#define BX_NTVDM_DEM_GSET_PLANE_V1_H

#include "bx_ntvdm_dem_drive_provider_v1.h"

typedef struct bx_ntvdm_dem_gset_plane_v1 {
    uint32_t has_drive_snapshot;
    bx_ntvdm_host_drive_snapshot_v1 drive_snapshot;
} bx_ntvdm_dem_gset_plane_v1;

void bx_ntvdm_dem_gset_plane_v1_clear(bx_ntvdm_dem_gset_plane_v1 *plane);
int bx_ntvdm_dem_gset_plane_v1_set_drive_snapshot(
    bx_ntvdm_dem_gset_plane_v1 *plane,
    const bx_ntvdm_host_drive_snapshot_v1 *snapshot);
int bx_ntvdm_dem_gset_plane_v1_dispatch(
    bx_ntvdm_dem_gset_plane_v1 *plane,
    const bx_ntvdm_bop_ingress_v1 *ingress,
    const bx_ntvdm_bop_provider_selection_v1 *selection,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result);

#endif
