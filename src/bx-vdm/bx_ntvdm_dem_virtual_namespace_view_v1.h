#ifndef BX_NTVDM_DEM_VIRTUAL_NAMESPACE_VIEW_V1_H
#define BX_NTVDM_DEM_VIRTUAL_NAMESPACE_VIEW_V1_H

#include "bx_ntvdm_dem_overlay_namespace_view_v1.h"

/* A selector-blind private-root view.  Unlike the Overlay view it has no
 * host namespace input: every visible node comes from the private store. */
int bx_ntvdm_dem_virtual_namespace_view_v1_query(
    const bx_ntvdm_dem_overlay_store_v1 *store, uint8_t drive_index,
    const wchar_t *relative,
    bx_ntvdm_dem_overlay_namespace_node_v1 *node_out, DWORD *error_out);

int bx_ntvdm_dem_virtual_namespace_view_v1_enumerate(
    const bx_ntvdm_dem_overlay_store_v1 *store, uint8_t drive_index,
    const wchar_t *relative, bx_ntvdm_host_namespace_entry_v1 *entries,
    uint32_t entry_capacity, uint32_t *entry_count_out, DWORD *error_out);

#endif
