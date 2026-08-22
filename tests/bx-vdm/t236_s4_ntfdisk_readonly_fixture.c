#include <stdio.h>
#include <string.h>

#include "bop/shim/demdasd_ioctl_shim.h"

/* The test caller must explicitly name a disposable, mounted fixed volume.
 * It performs one 512-byte round trip at 8 MiB, then verifies that the exact
 * pre-existing sector was restored.  This exercises the original nt_fdisk
 * lifecycle used by DEM INT 25h/26h, without changing boot, FAT, or root
 * directory sectors.  It is intentionally a host-capability fixture, not a
 * normal product-path test. */
int main(void)
{
    BPB bpb;
    DISK_GEOMETRY geometry;
    LARGE_INTEGER offset;
    unsigned char original[512];
    unsigned char observed[512];
    unsigned char probe[512];
    CHAR selected[2];
    BYTE logical_drive;
    DWORD error;

    if (GetEnvironmentVariableA("NTDOS64_T236_S4_TEST_DRIVE", selected,
            sizeof(selected)) != 1u || selected[0] < 'A' ||
        selected[0] > 'Z') {
        fputs("T236 S4 nt_fdisk: set NTDOS64_T236_S4_TEST_DRIVE to the ", stderr);
        fputs("authorized disposable fixed-volume letter\n", stderr);
        return 10;
    }
    logical_drive = (BYTE)(selected[0] - 'A');
    memset(&bpb, 0, sizeof(bpb));
    memset(&geometry, 0, sizeof(geometry));
    if (!nt_fdisk_init(logical_drive, &bpb, &geometry)) {
        error = GetLastError();
        fprintf(stderr, "T236 S4 nt_fdisk: %c: init failed error=%lu\n",
            selected[0], (unsigned long)error);
        return 1;
    }
    if (geometry.BytesPerSector != sizeof(original) ||
        bpb.SectorSize != sizeof(original)) {
        fprintf(stderr, "T236 S4 nt_fdisk: %c: requires 512-byte sectors\n",
            selected[0]);
        nt_fdisk_close(0u);
        return 2;
    }
    offset.QuadPart = 8ll * 1024ll * 1024ll;
    if (nt_fdisk_read(0u, &offset, sizeof(original), original) != sizeof(original)) {
        error = GetLastError();
        nt_fdisk_close(0u);
        fprintf(stderr, "T236 S4 nt_fdisk: %c: read failed error=%lu\n",
            selected[0], (unsigned long)error);
        return 3;
    }
    if (!nt_fdisk_verify(0u, &offset, sizeof(original))) {
        error = GetLastError();
        nt_fdisk_close(0u);
        fprintf(stderr, "T236 S4 nt_fdisk: %c: verify failed error=%lu\n",
            selected[0], (unsigned long)error);
        return 4;
    }
    memcpy(probe, original, sizeof(probe));
    probe[0] ^= 0x5au;
    probe[511] ^= 0xa5u;
    if (nt_fdisk_write(0u, &offset, sizeof(probe), probe) != sizeof(probe)) {
        error = GetLastError();
        nt_fdisk_close(0u);
        fprintf(stderr, "T236 S4 nt_fdisk: %c: write failed error=%lu\n",
            selected[0], (unsigned long)error);
        return 5;
    }
    if (nt_fdisk_read(0u, &offset, sizeof(observed), observed) != sizeof(observed) ||
        memcmp(probe, observed, sizeof(probe)) != 0) {
        error = GetLastError();
        (void)nt_fdisk_write(0u, &offset, sizeof(original), original);
        nt_fdisk_close(0u);
        fprintf(stderr, "T236 S4 nt_fdisk: %c: write/readback failed error=%lu\n",
            selected[0], (unsigned long)error);
        return 6;
    }
    if (nt_fdisk_write(0u, &offset, sizeof(original), original) != sizeof(original) ||
        nt_fdisk_read(0u, &offset, sizeof(observed), observed) != sizeof(observed) ||
        memcmp(original, observed, sizeof(original)) != 0) {
        error = GetLastError();
        nt_fdisk_close(0u);
        fprintf(stderr, "T236 S4 nt_fdisk: %c: restore failed error=%lu\n",
            selected[0], (unsigned long)error);
        return 7;
    }
    if (!nt_fdisk_close(0u)) return 8;
    printf("T236 S4 nt_fdisk: %c: init/read/verify/write/restore/close verified\n",
        selected[0]);
    return 0;
}
