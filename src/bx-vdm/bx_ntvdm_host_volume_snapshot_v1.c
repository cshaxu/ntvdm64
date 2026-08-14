#include "bx_ntvdm_host_volume_snapshot_v1.h"

#include <string.h>
#include <windows.h>

static uint32_t bit(uint32_t index)
{
    return UINT32_C(1) << index;
}

static int text_valid(const wchar_t *text, uint32_t capacity)
{
    uint32_t index;
    if (!text || capacity == 0u) return 0;
    for (index = 0u; index < capacity; ++index) {
        if (text[index] == L'\0') return 1;
    }
    return 0;
}

int bx_ntvdm_host_volume_snapshot_v1_valid(
    const bx_ntvdm_host_volume_snapshot_v1 *snapshot)
{
    uint32_t index;
    if (!snapshot || snapshot->magic != BX_NTVDM_HOST_VOLUME_SNAPSHOT_V1_MAGIC ||
        snapshot->version != BX_NTVDM_HOST_VOLUME_SNAPSHOT_V1_VERSION ||
        !bx_ntvdm_host_drive_snapshot_v1_valid(&snapshot->drives)) return 0;
    for (index = 0u; index < 26u; ++index) {
        const bx_ntvdm_host_volume_record_v1 *record = &snapshot->volumes[index];
        if ((snapshot->drives.admitted_mask & bit(index)) == 0u) {
            if (record->available != 0u || record->serial_number != 0u ||
                record->sectors_per_cluster != 0u || record->bytes_per_sector != 0u ||
                record->free_clusters != 0u || record->total_clusters != 0u ||
                record->label[0] != L'\0' || record->file_system[0] != L'\0') return 0;
        } else if (record->available > 1u ||
            !text_valid(record->label, 12u) || !text_valid(record->file_system, 9u) ||
            (record->available != 0u && (record->sectors_per_cluster == 0u ||
                record->bytes_per_sector == 0u || record->total_clusters == 0u))) return 0;
    }
    return 1;
}

int bx_ntvdm_host_volume_snapshot_v1_apply(
    const bx_ntvdm_host_drive_snapshot_v1 *drives,
    const bx_ntvdm_host_volume_record_v1 records[26],
    bx_ntvdm_host_volume_snapshot_v1 *out)
{
    uint32_t index;
    if (!drives || !records || !out || !bx_ntvdm_host_drive_snapshot_v1_valid(drives))
        return 0;
    memset(out, 0, sizeof(*out));
    out->magic = BX_NTVDM_HOST_VOLUME_SNAPSHOT_V1_MAGIC;
    out->version = BX_NTVDM_HOST_VOLUME_SNAPSHOT_V1_VERSION;
    out->drives = *drives;
    for (index = 0u; index < 26u; ++index) {
        if ((drives->admitted_mask & bit(index)) != 0u)
            out->volumes[index] = records[index];
    }
    return bx_ntvdm_host_volume_snapshot_v1_valid(out);
}

int bx_ntvdm_host_volume_snapshot_v1_capture(
    const bx_ntvdm_host_drive_snapshot_v1 *drives,
    bx_ntvdm_host_volume_snapshot_v1 *out)
{
    bx_ntvdm_host_volume_record_v1 records[26];
    uint32_t index;
    if (!drives || !out || !bx_ntvdm_host_drive_snapshot_v1_valid(drives)) return 0;
    memset(records, 0, sizeof(records));
    for (index = 0u; index < 26u; ++index) {
        wchar_t root[] = { (wchar_t)(L'A' + index), L':', L'\\', L'\0' };
        bx_ntvdm_host_volume_record_v1 *record = &records[index];
        DWORD sectors_per_cluster, bytes_per_sector, free_clusters;
        DWORD total_clusters, serial_number;
        if ((drives->admitted_mask & bit(index)) == 0u) continue;
        if (!GetDiskFreeSpaceW(root, &sectors_per_cluster, &bytes_per_sector,
                &free_clusters, &total_clusters) ||
            !GetVolumeInformationW(root, record->label, 12u,
                &serial_number, 0, 0, record->file_system, 9u)) {
            memset(record, 0, sizeof(*record));
            continue;
        }
        record->available = 1u;
        record->sectors_per_cluster = sectors_per_cluster;
        record->bytes_per_sector = bytes_per_sector;
        record->free_clusters = free_clusters;
        record->total_clusters = total_clusters;
        record->serial_number = serial_number;
    }
    return bx_ntvdm_host_volume_snapshot_v1_apply(drives, records, out);
}
