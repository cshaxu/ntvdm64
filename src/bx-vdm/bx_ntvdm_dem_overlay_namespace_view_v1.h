#ifndef BX_NTVDM_DEM_OVERLAY_NAMESPACE_VIEW_V1_H
#define BX_NTVDM_DEM_OVERLAY_NAMESPACE_VIEW_V1_H

#include "bx_ntvdm_dem_overlay_store_v1.h"
#include "bx_ntvdm_host_namespace.h"

enum bx_ntvdm_dem_overlay_namespace_node_v1_kind {
    BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_ABSENT = 0u,
    BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_FILE = 1u,
    BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_DIRECTORY = 2u
};

typedef struct bx_ntvdm_dem_overlay_namespace_node_v1 {
    uint32_t kind;
    uint32_t attributes;
} bx_ntvdm_dem_overlay_namespace_node_v1;

/* Selector-blind visible-node query for the Overlay view. It applies private
 * exact entries/tombstones and lazy directory relocation before making a
 * read-only admitted-root query. No host handle or host path escapes. */
int bx_ntvdm_dem_overlay_namespace_view_v1_query(
    const bx_ntvdm_dem_overlay_store_v1 *store,
    const bx_ntvdm_host_namespace_v1 *host_namespace, uint8_t drive_index,
    const wchar_t *visible_relative,
    bx_ntvdm_dem_overlay_namespace_node_v1 *node_out, DWORD *error_out);

#endif
