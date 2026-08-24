#include <stdio.h>
#include <string.h>

#include "opennt-host/dem/demdasd_ioctl_shim.h"

UINT GetDriveTypeOem(LPSTR root);
UCHAR GetPhysicalDriveType(UCHAR drive);

static uint32_t bit(unsigned index) { return UINT32_C(1) << index; }

int main(void)
{
    uint8_t types[26] = {0};
    bx_ntvdm_host_drive_snapshot_v1 snapshot;
    BPB bpb;
    DISK_GEOMETRY geometry;

    /* This is a synthetic observed inventory: C/D/E exist and are fixed.
     * Include admits C/D, while exclusion wins for D.  It never consults the
     * machine running the fixture. */
    types[2] = DRIVE_FIXED;
    types[3] = DRIVE_FIXED;
    types[4] = DRIVE_FIXED;
    if (!bx_ntvdm_host_drive_snapshot_v1_apply(bit(2u) | bit(3u) | bit(4u),
            types, bit(2u) | bit(3u), bit(3u), &snapshot) ||
        !bx_ntvdm_demdasd_drive_policy_bind(&snapshot) ||
        !bx_ntvdm_demdasd_drive_policy_admits(2u) ||
        bx_ntvdm_demdasd_drive_policy_admits(3u) ||
        bx_ntvdm_demdasd_drive_policy_admits(4u)) return 1;

    /* Both OpenNT discovery paths must suppress D: without invoking either
     * the public drive query or the original NT-native physical classifier. */
    if (GetDriveTypeOem("D:\\") != DRIVE_UNKNOWN ||
        GetPhysicalDriveType(3u) != DRIVE_UNKNOWN) return 2;

    /* The excluded D: rejection occurs before GetDriveType/CreateFile, so
     * it is deterministic and cannot inspect the fixture host's D: volume. */
    memset(&bpb, 0, sizeof(bpb));
    memset(&geometry, 0, sizeof(geometry));
    SetLastError(ERROR_SUCCESS);
    if (nt_fdisk_init(3u, &bpb, &geometry) ||
        GetLastError() != ERROR_INVALID_DRIVE) return 3;

    bx_ntvdm_demdasd_drive_policy_reset();
    SetLastError(ERROR_SUCCESS);
    if (bx_ntvdm_demdasd_drive_policy_admits(2u) ||
        nt_fdisk_init(2u, &bpb, &geometry) ||
        GetLastError() != ERROR_INVALID_DRIVE) return 4;

    puts("T245 S9 DEM DASD: immutable include/exclude admission guards raw-volume probes");
    return 0;
}
