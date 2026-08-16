#include "bx_ntvdm_dem_overlay_file_v1.h"

#include <string.h>

int main(void)
{
    bx_ntvdm_dem_overlay_store_v1 store;
    bx_ntvdm_dem_overlay_file_v1 files;
    const bx_ntvdm_dem_overlay_store_v1_entry *entry;
    static const uint8_t base[] = { 'b', 'a', 's', 'e' };
    uint8_t read[8]; uint32_t token, count, position;
    if (!bx_ntvdm_dem_overlay_store_v1_initialize(&store) ||
        !bx_ntvdm_dem_overlay_file_v1_initialize(&files, &store) ||
        !bx_ntvdm_dem_overlay_file_v1_open(&files, 2u, L"WORK\\A.TXT",
            BX_NTVDM_DEM_OVERLAY_FILE_V1_READ | BX_NTVDM_DEM_OVERLAY_FILE_V1_WRITE,
            base, sizeof(base), 0x20u, 1, 0, &token) ||
        !bx_ntvdm_dem_overlay_file_v1_seek(&files, token, 2, BX_NTVDM_DEM_OVERLAY_FILE_V1_SEEK_BEGIN, &position) ||
        position != 2u || !bx_ntvdm_dem_overlay_file_v1_write(&files, token,
            (const uint8_t *)"XX", 2u, &count) || count != 2u ||
        memcmp(base, "base", sizeof(base)) != 0 ||
        !bx_ntvdm_dem_overlay_file_v1_seek(&files, token, 0, BX_NTVDM_DEM_OVERLAY_FILE_V1_SEEK_BEGIN, &position) ||
        !bx_ntvdm_dem_overlay_file_v1_read(&files, token, read, sizeof(read), &count) ||
        count != 4u || memcmp(read, "baXX", 4u) != 0 ||
        !bx_ntvdm_dem_overlay_file_v1_seek(&files, token, 2, BX_NTVDM_DEM_OVERLAY_FILE_V1_SEEK_BEGIN, &position) ||
        !bx_ntvdm_dem_overlay_file_v1_truncate(&files, token) ||
        (entry = bx_ntvdm_dem_overlay_store_v1_lookup(&store, 2u, L"work\\a.txt")) == 0 ||
        entry->byte_count != 2u || memcmp(entry->bytes, "ba", 2u) != 0 ||
        !bx_ntvdm_dem_overlay_file_v1_close(&files, token) ||
        bx_ntvdm_dem_overlay_file_v1_read(&files, token, read, sizeof(read), &count) ||
        !bx_ntvdm_dem_overlay_file_v1_open(&files, 2u, L"WORK\\NEW.TXT",
            BX_NTVDM_DEM_OVERLAY_FILE_V1_WRITE, 0, 0u, 0x20u, 0, 1, &token) ||
        !bx_ntvdm_dem_overlay_file_v1_write(&files, token, (const uint8_t *)"N", 1u, &count) ||
        count != 1u || !bx_ntvdm_dem_overlay_file_v1_close(&files, token) ||
        bx_ntvdm_dem_overlay_file_v1_open(&files, 2u, L"WORK\\GONE.TXT",
            BX_NTVDM_DEM_OVERLAY_FILE_V1_READ, 0, 0u, 0u, 0, 0, &token)) {
        bx_ntvdm_dem_overlay_file_v1_teardown(&files);
        bx_ntvdm_dem_overlay_store_v1_teardown(&store);
        return 1;
    }
    bx_ntvdm_dem_overlay_file_v1_teardown(&files);
    bx_ntvdm_dem_overlay_store_v1_teardown(&store);
    return 0;
}
