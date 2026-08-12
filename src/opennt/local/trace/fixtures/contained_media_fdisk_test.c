#include <windows.h>
#include <string.h>
#include "demdasd.h"

BOOL ntdos64_media_configure_fdisk_image(BYTE, LPCSTR, BOOL);
VOID ntdos64_media_reset(VOID);

int main(void) {
    char path[MAX_PATH]; BYTE sector[512], write_buf[512], read_buf[512];
    DWORD written, read; HANDLE file; LARGE_INTEGER size, offset; BPB bpb; DISK_GEOMETRY geometry;
    if (!GetTempPathA(MAX_PATH, path) || !GetTempFileNameA(path, "ndm", 0, path)) return 1;
    file = CreateFileA(path, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return 2;
    ZeroMemory(sector, sizeof(sector)); sector[11] = 0; sector[12] = 2; sector[19] = 8; sector[24] = 18; sector[26] = 2;
    size.QuadPart = 4096;
    if (!WriteFile(file, sector, sizeof(sector), &written, NULL) || written != sizeof(sector) || !SetFilePointerEx(file, size, NULL, FILE_BEGIN) || !SetEndOfFile(file)) { CloseHandle(file); DeleteFileA(path); return 3; }
    CloseHandle(file);
    if (ntdos64_media_configure_fdisk_image(3, "\\\\.\\PhysicalDrive0", FALSE)) { DeleteFileA(path); return 4; }
    if (!ntdos64_media_configure_fdisk_image(2, path, TRUE) || !nt_fdisk_init(2, &bpb, &geometry) || bpb.SectorSize != 512 || geometry.BytesPerSector != 512) { DeleteFileA(path); return 5; }
    memset(write_buf, 0x5a, sizeof(write_buf)); offset.QuadPart = 512;
    if (nt_fdisk_write(0, &offset, sizeof(write_buf), write_buf) != sizeof(write_buf)) { DeleteFileA(path); return 6; }
    ZeroMemory(read_buf, sizeof(read_buf));
    if (nt_fdisk_read(0, &offset, sizeof(read_buf), read_buf) != sizeof(read_buf) || memcmp(write_buf, read_buf, sizeof(write_buf)) != 0) { DeleteFileA(path); return 7; }
    offset.QuadPart = 4096;
    if (nt_fdisk_read(0, &offset, 1, read_buf) != 0) { DeleteFileA(path); return 8; }
    nt_fdisk_close(0);
    ntdos64_media_reset();
    return DeleteFileA(path) ? 0 : 9;
}
