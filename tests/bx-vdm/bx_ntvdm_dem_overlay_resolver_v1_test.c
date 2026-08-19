#include "bop-v1/bx_ntvdm_dem_overlay_resolver_v1.h"

#include <wchar.h>
#include <wctype.h>

int main(void)
{
    wchar_t temporary_directory[MAX_PATH], path[MAX_PATH], short_path[MAX_PATH], relative[MAX_PATH];
    bx_ntvdm_host_drive_snapshot_v1 snapshot = {0};
    bx_ntvdm_host_namespace_v1 host; bx_ntvdm_dem_overlay_store_v1 store;
    bx_ntvdm_dem_overlay_file_v1 files; HANDLE raw = INVALID_HANDLE_VALUE;
    uint8_t drive, read[8]; DWORD wrote = 0u, error = 0u; uint32_t token, size, count;
    int failed = 0;
    if (GetTempPathW(MAX_PATH, temporary_directory) == 0u ||
        GetTempFileNameW(temporary_directory, L"n66", 0u, path) == 0u ||
        GetShortPathNameW(path, short_path, MAX_PATH) == 0u || short_path[1] != L':') return 1;
    raw = CreateFileW(path, GENERIC_WRITE, 0u, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, 0);
    if (raw == INVALID_HANDLE_VALUE || !WriteFile(raw, "base", 4u, &wrote, 0) || wrote != 4u) goto done;
    CloseHandle(raw); raw = INVALID_HANDLE_VALUE;
    drive = (uint8_t)(towupper((wint_t)short_path[0]) - L'A');
    snapshot.magic = BX_NTVDM_HOST_DRIVE_SNAPSHOT_V1_MAGIC;
    snapshot.version = BX_NTVDM_HOST_DRIVE_SNAPSHOT_V1_VERSION;
    snapshot.present_mask = snapshot.admitted_mask = UINT32_C(1) << drive;
    snapshot.types[drive] = DRIVE_FIXED;
    if (wcscpy_s(relative, MAX_PATH, short_path + 3u) != 0 ||
        !bx_ntvdm_host_namespace_v1_initialize(&host, &snapshot) ||
        !bx_ntvdm_dem_overlay_store_v1_initialize(&store) ||
        !bx_ntvdm_dem_overlay_file_v1_initialize(&files, &store) ||
        !bx_ntvdm_dem_overlay_resolver_v1_open(&files, &host, drive, relative, 3u,
            OPEN_EXISTING, &token, &size, &error) || size != 4u || error != ERROR_SUCCESS ||
        !bx_ntvdm_dem_overlay_file_v1_seek(&files, token, 2, 0u, &size) ||
        !bx_ntvdm_dem_overlay_file_v1_write(&files, token, (const uint8_t *)"XX", 2u, &count) ||
        count != 2u || !bx_ntvdm_dem_overlay_file_v1_close(&files, token)) { failed = 2; goto cleanup; }
    raw = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if (raw == INVALID_HANDLE_VALUE || !ReadFile(raw, read, 4u, &wrote, 0) || wrote != 4u ||
        memcmp(read, "base", 4u) != 0) failed = 3;
cleanup:
    if (raw != INVALID_HANDLE_VALUE) CloseHandle(raw);
    bx_ntvdm_dem_overlay_file_v1_teardown(&files);
    bx_ntvdm_dem_overlay_store_v1_teardown(&store);
    bx_ntvdm_host_namespace_v1_release(&host);
done:
    DeleteFileW(path);
    return failed;
}
