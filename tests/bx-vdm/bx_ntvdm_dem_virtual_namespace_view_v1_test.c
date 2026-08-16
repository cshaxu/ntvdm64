#include "../../src/bx-vdm/bx_ntvdm_dem_virtual_namespace_view_v1.h"

#include <string.h>

int main(void)
{
    bx_ntvdm_dem_overlay_store_v1 store;
    bx_ntvdm_dem_overlay_namespace_node_v1 node;
    bx_ntvdm_host_namespace_entry_v1 entries[4];
    DWORD error = ERROR_GEN_FAILURE;
    uint32_t count = 0u;
    memset(&store, 0, sizeof(store));
    if (!bx_ntvdm_dem_overlay_store_v1_initialize(&store) ||
        !bx_ntvdm_dem_overlay_store_v1_put_directory(&store, 2u, L"BIN", FILE_ATTRIBUTE_DIRECTORY) ||
        !bx_ntvdm_dem_overlay_store_v1_put_file(&store, 2u, L"BIN\\TEST.COM", 0u,
            (const uint8_t *)"x", 1u) ||
        !bx_ntvdm_dem_virtual_namespace_view_v1_query(&store, 2u, L"", &node, &error) ||
        node.kind != BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_DIRECTORY || error != ERROR_SUCCESS ||
        !bx_ntvdm_dem_virtual_namespace_view_v1_enumerate(&store, 2u, L"", entries, 4u,
            &count, &error) || count != 1u || _wcsicmp(entries[0].dos_name, L"BIN") != 0 ||
        !bx_ntvdm_dem_virtual_namespace_view_v1_query(&store, 2u, L"BIN\\TEST.COM", &node, &error) ||
        node.kind != BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_FILE ||
        !bx_ntvdm_dem_virtual_namespace_view_v1_query(&store, 2u, L"MISSING.COM", &node, &error) ||
        node.kind != BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_ABSENT || error != ERROR_FILE_NOT_FOUND) {
        bx_ntvdm_dem_overlay_store_v1_teardown(&store); return 1;
    }
    bx_ntvdm_dem_overlay_store_v1_teardown(&store); return 0;
}
