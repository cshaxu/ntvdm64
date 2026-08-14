#ifndef BX_NTVDM_HOST_VOLUME_SNAPSHOT_V1_H
#define BX_NTVDM_HOST_VOLUME_SNAPSHOT_V1_H

#include <stdint.h>

#include "bx_ntvdm_host_drive_policy.h"

#define BX_NTVDM_HOST_VOLUME_SNAPSHOT_V1_MAGIC 0x42585653u
#define BX_NTVDM_HOST_VOLUME_SNAPSHOT_V1_VERSION 1u

/* Adapter-local, immutable observation.  It never contains a host path,
 * handle, or API object and is captured only during CLI session setup. */
typedef struct bx_ntvdm_host_volume_record_v1 {
    uint32_t available;
    uint32_t serial_number;
    uint32_t sectors_per_cluster;
    uint32_t bytes_per_sector;
    uint32_t free_clusters;
    uint32_t total_clusters;
    wchar_t label[12];
    wchar_t file_system[9];
} bx_ntvdm_host_volume_record_v1;

typedef struct bx_ntvdm_host_volume_snapshot_v1 {
    uint32_t magic;
    uint32_t version;
    bx_ntvdm_host_drive_snapshot_v1 drives;
    bx_ntvdm_host_volume_record_v1 volumes[26];
} bx_ntvdm_host_volume_snapshot_v1;

int bx_ntvdm_host_volume_snapshot_v1_apply(
    const bx_ntvdm_host_drive_snapshot_v1 *drives,
    const bx_ntvdm_host_volume_record_v1 records[26],
    bx_ntvdm_host_volume_snapshot_v1 *out);
int bx_ntvdm_host_volume_snapshot_v1_capture(
    const bx_ntvdm_host_drive_snapshot_v1 *drives,
    bx_ntvdm_host_volume_snapshot_v1 *out);
int bx_ntvdm_host_volume_snapshot_v1_valid(
    const bx_ntvdm_host_volume_snapshot_v1 *snapshot);

#endif
