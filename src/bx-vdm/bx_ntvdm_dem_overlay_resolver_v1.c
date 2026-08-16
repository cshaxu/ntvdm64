#include "bx_ntvdm_dem_overlay_resolver_v1.h"

#include <string.h>

static int load_base(const bx_ntvdm_host_namespace_v1 *space, uint8_t drive,
    const wchar_t *relative, uint8_t **bytes_out, uint32_t *count_out,
    uint32_t *attributes_out, DWORD *error_out)
{
    HANDLE handle = INVALID_HANDLE_VALUE;
    BY_HANDLE_FILE_INFORMATION info;
    LARGE_INTEGER size;
    uint8_t *bytes = 0; DWORD read = 0u, error = ERROR_SUCCESS;
    *bytes_out = 0; *count_out = *attributes_out = 0u;
    if (!bx_ntvdm_host_namespace_v1_open_file_ex(space, drive, relative,
            GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
            OPEN_EXISTING, &handle, &error)) { *error_out = error; return 0; }
    if (!GetFileInformationByHandle(handle, &info) || !GetFileSizeEx(handle, &size) ||
        size.QuadPart < 0 || (uint64_t)size.QuadPart > UINT32_MAX) {
        error = GetLastError(); if (error == ERROR_SUCCESS) error = ERROR_FILE_TOO_LARGE;
        CloseHandle(handle); *error_out = error; return 0;
    }
    if (size.LowPart != 0u && (bytes = (uint8_t *)HeapAlloc(GetProcessHeap(), 0u,
            size.LowPart)) == 0) { CloseHandle(handle); *error_out = ERROR_NOT_ENOUGH_MEMORY; return 0; }
    if (size.LowPart != 0u && (!ReadFile(handle, bytes, size.LowPart, &read, 0) || read != size.LowPart)) {
        error = GetLastError(); if (error == ERROR_SUCCESS) error = ERROR_READ_FAULT;
        HeapFree(GetProcessHeap(), 0u, bytes); CloseHandle(handle); *error_out = error; return 0;
    }
    CloseHandle(handle); *bytes_out = bytes; *count_out = size.LowPart;
    *attributes_out = info.dwFileAttributes & 0x3fu; *error_out = ERROR_SUCCESS;
    return 1;
}

int bx_ntvdm_dem_overlay_resolver_v1_open_shared(bx_ntvdm_dem_overlay_file_v1 *files,
    const bx_ntvdm_host_namespace_v1 *space, uint8_t drive, const wchar_t *relative,
    uint32_t access, uint32_t share_access, DWORD disposition, uint32_t *token_out, uint32_t *size_out,
    DWORD *error_out)
{
    const bx_ntvdm_dem_overlay_store_v1_entry *entry;
    uint8_t *base = 0; uint32_t base_count = 0u, attributes = 0u, token = 0u;
    int exists, truncate = 0;
    if (token_out) *token_out = 0u; if (size_out) *size_out = 0u;
    if (error_out) *error_out = ERROR_INVALID_PARAMETER;
    if (!bx_ntvdm_dem_overlay_file_v1_valid(files) || !bx_ntvdm_host_namespace_v1_valid(space) ||
        !relative || !token_out || !size_out || !error_out || access == 0u || (access & ~3u) != 0u ||
        (share_access & ~3u) != 0u)
        return 0;
    entry = bx_ntvdm_dem_overlay_store_v1_lookup(files->store, drive, relative);
    exists = entry != 0 && entry->state == BX_NTVDM_DEM_OVERLAY_STORE_V1_FILE;
    if (entry == 0 && !load_base(space, drive, relative, &base, &base_count, &attributes, error_out)) {
        if (*error_out != ERROR_FILE_NOT_FOUND && *error_out != ERROR_PATH_NOT_FOUND) return 0;
        *error_out = ERROR_SUCCESS;
    } else if (entry == 0) exists = 1;
    switch (disposition) {
    case OPEN_EXISTING: if (!exists) { *error_out = ERROR_FILE_NOT_FOUND; goto done; } break;
    case CREATE_NEW: if (exists) { *error_out = ERROR_FILE_EXISTS; goto done; } break;
    case CREATE_ALWAYS: truncate = 1; break;
    case OPEN_ALWAYS: break;
    case TRUNCATE_EXISTING:
        if (!exists) { *error_out = ERROR_FILE_NOT_FOUND; goto done; }
        if ((access & BX_NTVDM_DEM_OVERLAY_FILE_V1_WRITE) == 0u) { *error_out = ERROR_ACCESS_DENIED; goto done; }
        truncate = 1; break;
    default: *error_out = ERROR_INVALID_PARAMETER; goto done;
    }
    if (!bx_ntvdm_dem_overlay_file_v1_open_shared(files, drive, relative, access, share_access, base,
            base_count, attributes, exists, !exists, &token)) { *error_out = ERROR_NOT_ENOUGH_MEMORY; goto done; }
    if (truncate && !bx_ntvdm_dem_overlay_file_v1_truncate(files, token)) {
        (void)bx_ntvdm_dem_overlay_file_v1_close(files, token); *error_out = ERROR_WRITE_FAULT; goto done;
    }
    entry = bx_ntvdm_dem_overlay_store_v1_lookup(files->store, drive, relative);
    if (entry == 0 || entry->state != BX_NTVDM_DEM_OVERLAY_STORE_V1_FILE) {
        (void)bx_ntvdm_dem_overlay_file_v1_close(files, token); *error_out = ERROR_INVALID_DATA; goto done;
    }
    *token_out = token; *size_out = entry->byte_count; *error_out = ERROR_SUCCESS;
done:
    if (base) HeapFree(GetProcessHeap(), 0u, base);
    return *token_out != 0u;
}

int bx_ntvdm_dem_overlay_resolver_v1_open(bx_ntvdm_dem_overlay_file_v1 *files,
    const bx_ntvdm_host_namespace_v1 *space, uint8_t drive, const wchar_t *relative,
    uint32_t access, DWORD disposition, uint32_t *token_out, uint32_t *size_out,
    DWORD *error_out)
{ return bx_ntvdm_dem_overlay_resolver_v1_open_shared(files, space, drive, relative,
    access, 3u, disposition, token_out, size_out, error_out); }
