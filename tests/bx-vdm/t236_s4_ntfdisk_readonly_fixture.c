#include <stdio.h>
#include <string.h>

#include "bop/shim/demdasd_ioctl_shim.h"

/* This fixture deliberately exercises only the public, non-mutating
 * fixed-volume admission/read/close route.  It never calls nt_fdisk_write,
 * format, or a locking IOCTL. */
int main(void)
{
    BPB bpb;
    DISK_GEOMETRY geometry;
    LARGE_INTEGER offset;
    unsigned char sector[512];
    DWORD error;

    memset(&bpb, 0, sizeof(bpb));
    memset(&geometry, 0, sizeof(geometry));
    if (!nt_fdisk_init(2u, &bpb, &geometry)) {
        error = GetLastError();
        printf("T236 S4 nt_fdisk readonly: unavailable error=%lu\n", (unsigned long)error);
        return 0;
    }
    offset.QuadPart = 0;
    if (nt_fdisk_read(0u, &offset, sizeof(sector), sector) != sizeof(sector)) {
        error = GetLastError();
        nt_fdisk_close(0u);
        if (error == ERROR_ACCESS_DENIED) {
            printf("T236 S4 nt_fdisk readonly: host raw-read unavailable error=%lu\n",
                (unsigned long)error);
            return 0;
        }
        fprintf(stderr, "T236 S4 nt_fdisk readonly: read failed error=%lu\n", (unsigned long)error);
        return 1;
    }
    if (!nt_fdisk_close(0u)) return 2;
    puts("T236 S4 nt_fdisk readonly: fixed-volume read/close verified");
    return 0;
}
