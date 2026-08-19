#include "bop-v1/bx_ntvdm_dem_overlay_file_v1.h"

#include <string.h>

int main(void)
{
    bx_ntvdm_dem_overlay_store_v1 store;
    bx_ntvdm_dem_overlay_file_v1 files;
    const bx_ntvdm_dem_overlay_store_v1_entry *entry;
    wchar_t effective[BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE];
    static const uint8_t base[] = { 'b', 'a', 's', 'e' };
    uint8_t read[8]; uint32_t token, second_token, count, position, attributes, size;
    uint16_t dos_time, dos_date;
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
        !bx_ntvdm_dem_overlay_file_v1_info(&files, token, &attributes, &size,
            &dos_time, &dos_date) || attributes != 0x20u || size != 4u ||
        (dos_time == 0u && dos_date == 0u) ||
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
        !bx_ntvdm_dem_overlay_file_v1_open_shared(&files, 2u, L"WORK\\SHARE.TXT",
            BX_NTVDM_DEM_OVERLAY_FILE_V1_READ, BX_NTVDM_DEM_OVERLAY_FILE_V1_READ,
            base, sizeof(base), 0x20u, 1, 0, &token) ||
        bx_ntvdm_dem_overlay_file_v1_open_shared(&files, 2u, L"WORK\\SHARE.TXT",
            BX_NTVDM_DEM_OVERLAY_FILE_V1_WRITE, 3u, 0, 0u, 0x20u, 1, 0, &second_token) ||
        !bx_ntvdm_dem_overlay_file_v1_close(&files, token) ||
        !bx_ntvdm_dem_overlay_file_v1_open_shared(&files, 2u, L"WORK\\SHARE.TXT",
            BX_NTVDM_DEM_OVERLAY_FILE_V1_WRITE, 3u, 0, 0u, 0x20u, 1, 0, &second_token) ||
        !bx_ntvdm_dem_overlay_file_v1_close(&files, second_token) ||
        !bx_ntvdm_dem_overlay_store_v1_put_directory(&store, 2u, L"WORK\\DIR", 0x10u) ||
        !bx_ntvdm_dem_overlay_store_v1_put_file(&store, 2u, L"WORK\\DIR\\CHILD", 0u, 0, 0u) ||
        !bx_ntvdm_dem_overlay_store_v1_has_descendant(&store, 2u, L"WORK\\DIR") ||
        (entry = bx_ntvdm_dem_overlay_store_v1_lookup(&store, 2u, L"work\\dir")) == 0 ||
        entry->state != BX_NTVDM_DEM_OVERLAY_STORE_V1_DIRECTORY ||
        !bx_ntvdm_dem_overlay_store_v1_tombstone_directory(&store, 2u, L"WORK\\DIR") ||
        (entry = bx_ntvdm_dem_overlay_store_v1_lookup(&store, 2u, L"work\\dir")) == 0 ||
        entry->state != BX_NTVDM_DEM_OVERLAY_STORE_V1_DIRECTORY_TOMBSTONE ||
        bx_ntvdm_dem_overlay_file_v1_open(&files, 2u, L"WORK\\GONE.TXT",
            BX_NTVDM_DEM_OVERLAY_FILE_V1_READ, 0, 0u, 0u, 0, 0, &token)) {
        bx_ntvdm_dem_overlay_file_v1_teardown(&files);
        bx_ntvdm_dem_overlay_store_v1_teardown(&store);
        return 1;
    }
    if (!bx_ntvdm_dem_overlay_store_v1_add_relocation(&store, 2u,
            L"WORK\\MOVED", L"WORK\\DIR")) return 2;
    if (!bx_ntvdm_dem_overlay_store_v1_resolve_relocation(&store, 2u,
            L"WORK\\MOVED\\CHILD", effective) ||
        _wcsicmp(effective, L"WORK\\DIR\\CHILD") != 0) return 3;
    if (!bx_ntvdm_dem_overlay_store_v1_add_relocation(&store, 2u,
            L"WORK\\FINAL", L"WORK\\MOVED") ||
        !bx_ntvdm_dem_overlay_store_v1_resolve_relocation(&store, 2u,
            L"WORK\\FINAL\\CHILD", effective) ||
        _wcsicmp(effective, L"WORK\\DIR\\CHILD") != 0) return 4;
    if (bx_ntvdm_dem_overlay_store_v1_add_relocation(&store, 2u,
            L"WORK\\DIR\\LOOP", L"WORK\\DIR")) return 5;
    if (bx_ntvdm_dem_overlay_store_v1_add_relocation(&store, 2u,
            L"WORK\\DIR", L"WORK\\MOVED")) return 6;
    bx_ntvdm_dem_overlay_file_v1_teardown(&files);
    bx_ntvdm_dem_overlay_store_v1_teardown(&store);
    return 0;
}
