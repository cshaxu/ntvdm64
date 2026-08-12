/* SH-12: configured regular-image facade below original DEM DASD. */
#include <windows.h>
#include <string.h>
#include "demdasd.h"

#define NTDOS64_MEDIA_MAX_DRIVES 26
#define NTDOS64_BOOT_SECTOR_SIZE 512

typedef struct {
    HANDLE handle;
    LARGE_INTEGER length;
    BPB bpb;
    DISK_GEOMETRY geometry;
    BOOL writable;
    BOOL configured;
} NTDOS64_IMAGE;

static NTDOS64_IMAGE configured[NTDOS64_MEDIA_MAX_DRIVES];
static BYTE physical[NTDOS64_MEDIA_MAX_DRIVES];
static BYTE physical_count;

/* Wrapper/session teardown only; original DEM does not call this helper. */
VOID ntdos64_media_reset(VOID) {
    BYTE drive;
    for (drive = 0; drive < NTDOS64_MEDIA_MAX_DRIVES; ++drive) {
        if (configured[drive].configured) CloseHandle(configured[drive].handle);
    }
    ZeroMemory(configured, sizeof(configured));
    ZeroMemory(physical, sizeof(physical));
    physical_count = 0;
}

static BOOL range_ok(const NTDOS64_IMAGE *image, const LARGE_INTEGER *offset, ULONG size) {
    ULONGLONG start;
    if (offset == NULL || offset->QuadPart < 0) return FALSE;
    start = (ULONGLONG)offset->QuadPart;
    return start <= (ULONGLONG)image->length.QuadPart &&
        (ULONGLONG)size <= (ULONGLONG)image->length.QuadPart - start;
}

static NTDOS64_IMAGE *image_for_physical(BYTE drive) {
    if (drive >= physical_count) return NULL;
    return &configured[physical[drive]];
}

/* Future wrapper-only setup API. A DOS request never supplies this path. */
BOOL ntdos64_media_configure_fdisk_image(BYTE logical_drive, LPCSTR path, BOOL writable) {
    NTDOS64_IMAGE *image;
    BYTE sector[NTDOS64_BOOT_SECTOR_SIZE];
    DWORD read;
    LARGE_INTEGER zero;
    DWORD attributes;
    HANDLE handle;

    if (logical_drive >= NTDOS64_MEDIA_MAX_DRIVES || path == NULL || !*path ||
        strncmp(path, "\\\\.\\", 4) == 0) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    attributes = GetFileAttributesA(path);
    if (attributes == (DWORD)-1 || (attributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT))) return FALSE;
    handle = CreateFileA(path, writable ? (GENERIC_READ | GENERIC_WRITE) : GENERIC_READ,
        FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (handle == INVALID_HANDLE_VALUE) return FALSE;
    zero.QuadPart = 0;
    if (!SetFilePointerEx(handle, zero, NULL, FILE_BEGIN) || !ReadFile(handle, sector, sizeof(sector), &read, NULL) || read != sizeof(sector)) {
        CloseHandle(handle); return FALSE;
    }
    image = &configured[logical_drive];
    if (image->configured) CloseHandle(image->handle);
    ZeroMemory(image, sizeof(*image));
    memcpy(&image->bpb, sector + 11, sizeof(image->bpb));
    if (image->bpb.SectorSize != 512 || image->bpb.TrackSize == 0 || image->bpb.Heads == 0 ||
        !GetFileSizeEx(handle, &image->length) || image->length.QuadPart < sizeof(sector)) {
        CloseHandle(handle); ZeroMemory(image, sizeof(*image)); SetLastError(ERROR_BAD_FORMAT); return FALSE;
    }
    image->handle = handle; image->writable = writable; image->configured = TRUE;
    image->geometry.MediaType = FixedMedia;
    image->geometry.BytesPerSector = image->bpb.SectorSize;
    image->geometry.SectorsPerTrack = image->bpb.TrackSize;
    image->geometry.TracksPerCylinder = image->bpb.Heads;
    image->geometry.Cylinders.QuadPart = image->length.QuadPart /
        ((LONGLONG)image->bpb.SectorSize * image->bpb.TrackSize * image->bpb.Heads);
    return TRUE;
}

BOOL nt_fdisk_init(BYTE logical_drive, PBPB bpb, PDISK_GEOMETRY geometry) {
    NTDOS64_IMAGE *image;
    if (logical_drive >= NTDOS64_MEDIA_MAX_DRIVES || bpb == NULL || geometry == NULL || physical_count == NTDOS64_MEDIA_MAX_DRIVES) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    image = &configured[logical_drive];
    if (!image->configured) { SetLastError(ERROR_NOT_READY); return FALSE; }
    *bpb = image->bpb; *geometry = image->geometry;
    physical[physical_count++] = logical_drive;
    return TRUE;
}

static ULONG transfer(BYTE drive, PLARGE_INTEGER offset, ULONG size, PBYTE buffer, BOOL write) {
    NTDOS64_IMAGE *image = image_for_physical(drive);
    DWORD completed;
    if (image == NULL || buffer == NULL || !range_ok(image, offset, size) || (write && !image->writable)) {
        SetLastError(write && image != NULL && !image->writable ? ERROR_ACCESS_DENIED : ERROR_SECTOR_NOT_FOUND); return 0;
    }
    if (!SetFilePointerEx(image->handle, *offset, NULL, FILE_BEGIN)) return 0;
    if (!(write ? WriteFile(image->handle, buffer, size, &completed, NULL) : ReadFile(image->handle, buffer, size, &completed, NULL))) return 0;
    return completed;
}

ULONG nt_fdisk_read(BYTE drive, PLARGE_INTEGER offset, ULONG size, PBYTE buffer) { return transfer(drive, offset, size, buffer, FALSE); }
ULONG nt_fdisk_write(BYTE drive, PLARGE_INTEGER offset, ULONG size, PBYTE buffer) { return transfer(drive, offset, size, buffer, TRUE); }
BOOL nt_fdisk_verify(BYTE drive, PLARGE_INTEGER offset, ULONG size) { NTDOS64_IMAGE *image = image_for_physical(drive); if (image == NULL || !range_ok(image, offset, size)) { SetLastError(ERROR_SECTOR_NOT_FOUND); return FALSE; } return TRUE; }
BOOL nt_fdisk_close(BYTE drive) { return image_for_physical(drive) != NULL; }
