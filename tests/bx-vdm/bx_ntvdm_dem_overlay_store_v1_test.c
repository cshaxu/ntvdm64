#include "bop-v1/bx_ntvdm_dem_overlay_store_v1.h"

#include <string.h>

int main(void)
{
    bx_ntvdm_dem_overlay_store_v1 store;
    const bx_ntvdm_dem_overlay_store_v1_entry *entry;
    static const uint8_t first[] = { 'o', 'n', 'e' };
    static const uint8_t second[] = { 't', 'w', 'o', '!' };
    if (!bx_ntvdm_dem_overlay_store_v1_initialize(&store) ||
        !bx_ntvdm_dem_overlay_store_v1_put_file(&store, 2u, L"WORK\\A.TXT",
            0x20u, first, sizeof(first)) ||
        (entry = bx_ntvdm_dem_overlay_store_v1_lookup(&store, 2u, L"work\\a.txt")) == 0 ||
        entry->state != BX_NTVDM_DEM_OVERLAY_STORE_V1_FILE ||
        entry->byte_count != sizeof(first) || memcmp(entry->bytes, first, sizeof(first)) != 0 ||
        !bx_ntvdm_dem_overlay_store_v1_put_file(&store, 2u, L"WORK\\A.TXT",
            0x21u, second, sizeof(second)) ||
        (entry = bx_ntvdm_dem_overlay_store_v1_lookup(&store, 2u, L"WORK\\A.TXT")) == 0 ||
        entry->attributes != 0x21u || entry->byte_count != sizeof(second) ||
        memcmp(entry->bytes, second, sizeof(second)) != 0 ||
        bx_ntvdm_dem_overlay_store_v1_lookup(&store, 3u, L"WORK\\A.TXT") != 0 ||
        !bx_ntvdm_dem_overlay_store_v1_tombstone(&store, 2u, L"WORK\\A.TXT") ||
        (entry = bx_ntvdm_dem_overlay_store_v1_lookup(&store, 2u, L"WORK\\A.TXT")) == 0 ||
        entry->state != BX_NTVDM_DEM_OVERLAY_STORE_V1_TOMBSTONE ||
        entry->bytes != 0 || entry->byte_count != 0u ||
        bx_ntvdm_dem_overlay_store_v1_put_file(&store, 26u, L"A.TXT", 0u, 0, 0u) ||
        bx_ntvdm_dem_overlay_store_v1_put_file(&store, 2u, L"", 0u, 0, 0u)) {
        bx_ntvdm_dem_overlay_store_v1_teardown(&store); return 1;
    }
    bx_ntvdm_dem_overlay_store_v1_teardown(&store);
    return bx_ntvdm_dem_overlay_store_v1_valid(&store) ? 2 : 0;
}
