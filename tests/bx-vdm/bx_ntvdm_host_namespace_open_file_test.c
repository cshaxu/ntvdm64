#include "bx_ntvdm_host_namespace.h"

#include <wchar.h>
#include <wctype.h>

int main(void)
{
    wchar_t temporary_directory[MAX_PATH], temporary[MAX_PATH], temporary_short[MAX_PATH], relative[MAX_PATH];
    wchar_t temporary2[MAX_PATH], temporary2_short[MAX_PATH], relative2[MAX_PATH];
    bx_ntvdm_host_drive_snapshot_v1 snapshot = {0};
    bx_ntvdm_host_namespace_v1 space;
    uint8_t drive;
    HANDLE source = INVALID_HANDLE_VALUE, opened = INVALID_HANDLE_VALUE;
    DWORD written = 0u, read = 0u, attributes = 0u;
    DWORD error = ERROR_SUCCESS;
    char output = 0;
    int failed = 0;
    if (GetTempPathW(MAX_PATH, temporary_directory) == 0u ||
        GetTempFileNameW(temporary_directory, L"n64", 0u, temporary) == 0u ||
        GetShortPathNameW(temporary, temporary_short, MAX_PATH) == 0u ||
        temporary_short[1] != L':') return 1;
    source = CreateFileW(temporary, GENERIC_WRITE, 0u, 0, CREATE_ALWAYS,
        FILE_ATTRIBUTE_TEMPORARY, 0);
    if (source == INVALID_HANDLE_VALUE || !WriteFile(source, "x", 1u, &written, 0) ||
        written != 1u) { if (source != INVALID_HANDLE_VALUE) CloseHandle(source); DeleteFileW(temporary); return 2; }
    CloseHandle(source);
    drive = (uint8_t)(towupper((wint_t)temporary_short[0]) - L'A');
    snapshot.magic = BX_NTVDM_HOST_DRIVE_SNAPSHOT_V1_MAGIC;
    snapshot.version = BX_NTVDM_HOST_DRIVE_SNAPSHOT_V1_VERSION;
    snapshot.present_mask = UINT32_C(1) << drive;
    snapshot.admitted_mask = snapshot.present_mask;
    snapshot.types[drive] = DRIVE_FIXED;
    if (!bx_ntvdm_host_namespace_v1_initialize(&space, &snapshot)) {
        DeleteFileW(temporary); return 3;
    }
    if (wcscpy_s(relative, MAX_PATH, temporary_short + 3u) != 0) {
        bx_ntvdm_host_namespace_v1_release(&space);
        DeleteFileW(temporary);
        return 4;
    }
    failed |= !bx_ntvdm_host_namespace_v1_open_file_ex(&space, drive, relative,
        GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, &opened, &error) ||
        error != ERROR_SUCCESS ||
        !ReadFile(opened, &output, 1u, &read, 0) || read != 1u || output != 'x';
    if (opened != INVALID_HANDLE_VALUE) CloseHandle(opened);
    opened = INVALID_HANDLE_VALUE;
    failed |= bx_ntvdm_host_namespace_v1_open_file_ex(&space, drive, L"..\\X.TXT",
        GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, &opened, &error) != 0 ||
        error != ERROR_INVALID_PARAMETER;
    failed |= !bx_ntvdm_host_namespace_v1_query_file_attributes(&space, drive,
        relative, &attributes, &error) || error != ERROR_SUCCESS ||
        (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0u;
    failed |= !bx_ntvdm_host_namespace_v1_set_file_attributes(&space, drive,
        relative, FILE_ATTRIBUTE_HIDDEN, &error) || error != ERROR_SUCCESS ||
        !bx_ntvdm_host_namespace_v1_query_file_attributes(&space, drive,
            relative, &attributes, &error) ||
        (attributes & FILE_ATTRIBUTE_HIDDEN) == 0u;
    failed |= !bx_ntvdm_host_namespace_v1_set_file_attributes(&space, drive,
        relative, FILE_ATTRIBUTE_NORMAL, &error) || error != ERROR_SUCCESS;
    failed |= !bx_ntvdm_host_namespace_v1_delete_file(&space, drive, relative,
        &error) || error != ERROR_SUCCESS;
    temporary[0] = L'\0'; /* deletion was performed through the namespace. */
    if (!failed && (!bx_ntvdm_host_namespace_v1_create_directory(&space, drive,
            relative, &error) || error != ERROR_SUCCESS ||
            !bx_ntvdm_host_namespace_v1_remove_directory(&space, drive, relative,
                &error) || error != ERROR_SUCCESS)) failed = 6;
    if (!failed && (!bx_ntvdm_host_namespace_v1_open_file_ex(&space, drive, relative,
            GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE |
            FILE_SHARE_DELETE, CREATE_NEW, &opened, &error) ||
            error != ERROR_SUCCESS)) failed = 7;
    if (opened != INVALID_HANDLE_VALUE) { CloseHandle(opened); opened = INVALID_HANDLE_VALUE; }
    if (GetTempFileNameW(temporary_directory, L"n65", 0u, temporary2) == 0u ||
        GetShortPathNameW(temporary2, temporary2_short, MAX_PATH) == 0u ||
        temporary2_short[1] != L':' || !DeleteFileW(temporary2) ||
        wcscpy_s(relative2, MAX_PATH, temporary2_short + 3u) != 0) failed = 8;
    if (!failed) {
        if (!bx_ntvdm_host_namespace_v1_rename_file(&space, drive, relative,
            drive, relative2, &error) || error != ERROR_SUCCESS ||
            !bx_ntvdm_host_namespace_v1_query_file_attributes(&space, drive,
                relative2, &attributes, &error) || error != ERROR_SUCCESS ||
            !bx_ntvdm_host_namespace_v1_delete_file(&space, drive, relative2,
                &error) || error != ERROR_SUCCESS) failed = 9;
    }
    bx_ntvdm_host_namespace_v1_release(&space);
    if (temporary[0] != L'\0') DeleteFileW(temporary);
    return failed;
}
