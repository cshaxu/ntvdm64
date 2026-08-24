#include "bx_ntvdm_host_namespace.h"

#include <stdio.h>
#include <string.h>

static int write_file(const wchar_t *path)
{
    HANDLE file = CreateFileW(path, GENERIC_WRITE, 0u, 0, CREATE_NEW,
        FILE_ATTRIBUTE_NORMAL, 0);
    DWORD written;
    if (file == INVALID_HANDLE_VALUE) return 0;
    if (!WriteFile(file, "x", 1u, &written, 0) || written != 1u) {
        CloseHandle(file); return 0;
    }
    CloseHandle(file);
    return 1;
}

int main(void)
{
    wchar_t temporary[MAX_PATH], root[MAX_PATH], short_root[MAX_PATH];
    wchar_t alpha[MAX_PATH], zeta[MAX_PATH], subdir[MAX_PATH], child[MAX_PATH];
    bx_ntvdm_host_drive_snapshot_v1 snapshot = {0};
    bx_ntvdm_host_namespace_v1 space;
    bx_ntvdm_host_namespace_entry_v1 entries[8];
    uint32_t count = 0u, first_count = 0u;
    wchar_t first_names[3][13] = {{0}};
    uint8_t drive;
    int first_result = 0, nested_result = 0;
    int failed = 0;

    if (GetTempPathW(MAX_PATH, temporary) == 0u ||
        GetTempFileNameW(temporary, L"n64", 0u, root) == 0u ||
        !DeleteFileW(root) || !CreateDirectoryW(root, 0)) return 2;
    swprintf(alpha, MAX_PATH, L"%ls\\ALPHA.TXT", root);
    swprintf(zeta, MAX_PATH, L"%ls\\ZETA.TXT", root);
    swprintf(subdir, MAX_PATH, L"%ls\\SUBDIR", root);
    swprintf(child, MAX_PATH, L"%ls\\CHILD.TXT", subdir);
    if (!write_file(alpha) || !write_file(zeta) || !CreateDirectoryW(subdir, 0) ||
        !write_file(child) || GetShortPathNameW(root, short_root, MAX_PATH) == 0u ||
        short_root[1] != L':') failed = 1;
    drive = (uint8_t)(towupper((wint_t)short_root[0]) - L'A');
    snapshot.magic = BX_NTVDM_HOST_DRIVE_SNAPSHOT_V1_MAGIC;
    snapshot.version = BX_NTVDM_HOST_DRIVE_SNAPSHOT_V1_VERSION;
    snapshot.present_mask = UINT32_C(1) << drive;
    snapshot.admitted_mask = snapshot.present_mask;
    snapshot.types[drive] = DRIVE_FIXED;
    failed |= !bx_ntvdm_host_namespace_v1_initialize(&space, &snapshot);
    if (!failed) {
        const wchar_t *relative = short_root + 3u;
        first_result = bx_ntvdm_host_namespace_v1_enumerate(&space, drive, relative,
            entries, 8u, &count);
        first_count = count;
        if (count >= 3u) {
            memcpy(first_names[0], entries[0].dos_name, sizeof(first_names[0]));
            memcpy(first_names[1], entries[1].dos_name, sizeof(first_names[1]));
            memcpy(first_names[2], entries[2].dos_name, sizeof(first_names[2]));
        }
        failed |= first_result != BX_NTVDM_HOST_NAMESPACE_V1_OK || first_count != 3u ||
            wcscmp(entries[0].dos_name, L"ALPHA.TXT") != 0 ||
            wcscmp(entries[1].dos_name, L"SUBDIR") != 0 ||
            wcscmp(entries[2].dos_name, L"ZETA.TXT") != 0;
        failed |= bx_ntvdm_host_namespace_v1_enumerate(&space, drive,
            L"..", entries, 8u, &count) != BX_NTVDM_HOST_NAMESPACE_V1_REJECTED;
        failed |= !bx_ntvdm_host_namespace_v1_directory_exists(&space, drive,
            relative);
        failed |= !bx_ntvdm_host_namespace_v1_directory_exists(&space, drive,
            L"");
        failed |= bx_ntvdm_host_namespace_v1_directory_exists(&space, drive,
            L"..");
        if (wcslen(relative) + wcslen(L"\\SUBDIR") + 1u < MAX_PATH) {
            wchar_t nested[MAX_PATH];
            swprintf(nested, MAX_PATH, L"%ls\\SUBDIR", relative);
            failed |= !bx_ntvdm_host_namespace_v1_directory_exists(&space,
                drive, nested);
            nested_result = bx_ntvdm_host_namespace_v1_enumerate(&space, drive, nested,
                entries, 8u, &count);
            failed |= nested_result != BX_NTVDM_HOST_NAMESPACE_V1_OK || count != 1u ||
                wcscmp(entries[0].dos_name, L"CHILD.TXT") != 0;
        } else failed = 1;
        bx_ntvdm_host_namespace_v1_release(&space);
    }
    DeleteFileW(child); RemoveDirectoryW(subdir); DeleteFileW(alpha); DeleteFileW(zeta); RemoveDirectoryW(root);
    if (failed) {
        wprintf(L"namespace diagnostic: root=%ls first=%d first_count=%u names=%ls,%ls,%ls nested=%d count=%u available=%08lx error=%lu\n",
            short_root, first_result, (unsigned)first_count, first_names[0], first_names[1], first_names[2], nested_result, (unsigned)count,
            (unsigned long)space.available_mask, (unsigned long)GetLastError());
        return 1;
    }
    puts("bx-ntvdm-host-namespace-test: contained DOS projection and deterministic enumeration verified");
    return 0;
}
