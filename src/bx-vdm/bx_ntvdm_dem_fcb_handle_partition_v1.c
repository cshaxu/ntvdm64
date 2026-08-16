#include "bx_ntvdm_dem_fcb_handle_partition_v1.h"

#define DEM_ERROR_INVALID_FUNCTION 1u
#define DEM_ERROR_ACCESS_DENIED 5u
#define DEM_ERROR_INVALID_HANDLE 6u
#define DEM_ERROR_DISK_FULL 112u
#define DEM_ERROR_WRITE_PROTECT 19u
#define DEM_ERROR_GEN_FAILURE 31u
#define DEM_ERROR_WRONG_DISK 34u

#include "bx_ntvdm_dem_fcb_overlay_backend_v1.h"

static uint32_t token_bp(const bx_ntvdm_cpu_state_v1 *cpu)
{ return ((cpu->eax & 0xffffu) << 16) | (cpu->ebp & 0xffffu); }
static uint32_t token_si(const bx_ntvdm_cpu_state_v1 *cpu)
{ return ((cpu->eax & 0xffffu) << 16) | (cpu->esi & 0xffffu); }
static int selected(uint8_t service)
{ return service == 0x2cu || service == 0x2du || service == 0x2eu ||
    service == 0x2fu || service == 0x30u || service == 0x31u; }
static int finish(const bx_ntvdm_exception_event_v1 *boundary,
    bx_ntvdm_cpu_result_v2 *result, uint16_t ax, int set_ax, int cf)
{
    return bx_ntvdm_cpu_result_v2_resume(result, boundary->fault_rip + 4u) &&
        (!set_ax || bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 0u, ax)) &&
        bx_ntvdm_cpu_result_v2_set_cf(result, cf);
}
static int error_result(const bx_ntvdm_exception_event_v1 *boundary,
    bx_ntvdm_cpu_result_v2 *result, DWORD error)
{
    if ((error >= DEM_ERROR_WRITE_PROTECT && error <= DEM_ERROR_GEN_FAILURE) ||
        error == DEM_ERROR_WRONG_DISK) return finish(boundary, result, 0xffffu, 1, 1);
    return finish(boundary, result, (uint16_t)(error ? error : DEM_ERROR_INVALID_FUNCTION), 1, 1);
}
static int backend_error(int backend_result, DWORD error)
{
    if (backend_result == BX_NTVDM_DEM_LOCAL_FILE_BACKEND_V1_READONLY)
        return DEM_ERROR_ACCESS_DENIED;
    if (backend_result == BX_NTVDM_DEM_LOCAL_FILE_BACKEND_V1_OVERLAY ||
        backend_result == BX_NTVDM_DEM_LOCAL_FILE_BACKEND_V1_VIRTUAL)
        return DEM_ERROR_INVALID_FUNCTION;
    return (int)(error == ERROR_SUCCESS ? DEM_ERROR_INVALID_FUNCTION : error);
}
static int file_info(HANDLE handle, uint16_t *time_out, uint16_t *date_out,
    uint32_t *size_out)
{
    FILETIME time, local;
    LARGE_INTEGER size;
    return GetFileTime(handle, 0, 0, &time) && FileTimeToLocalFileTime(&time, &local) &&
        FileTimeToDosDateTime(&local, date_out, time_out) && GetFileSizeEx(handle, &size) &&
        size.QuadPart >= 0 && (uint64_t)size.QuadPart <= UINT32_MAX &&
        ((*size_out = (uint32_t)size.QuadPart), 1);
}
static ULONG share_mode(uint8_t mode)
{
    switch (mode & 0x70u) {
    case 0x10u: return 0u;
    case 0x20u: return FILE_SHARE_READ;
    case 0x30u: return FILE_SHARE_WRITE;
    default: return FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
    }
}
/* The Overlay backend has a deliberately small, provider-private share ABI:
 * bit 0 permits readers and bit 1 permits writers.  Do not leak Win32's
 * FILE_SHARE_DELETE flag into it. */
static uint32_t overlay_share_mode(uint8_t mode)
{
    switch (mode & 0x70u) {
    case 0x10u: return 0u;
    case 0x20u: return BX_NTVDM_DEM_OVERLAY_FILE_V1_READ;
    case 0x30u: return BX_NTVDM_DEM_OVERLAY_FILE_V1_WRITE;
    default: return BX_NTVDM_DEM_OVERLAY_FILE_V1_READ |
        BX_NTVDM_DEM_OVERLAY_FILE_V1_WRITE;
    }
}
static uint32_t access_mode(uint8_t mode)
{
    if ((mode & 0x0fu) == 0u) return BX_NTVDM_DEM_LOCAL_FILE_ACCESS_V1_READ;
    if ((mode & 0x0fu) == 1u) return BX_NTVDM_DEM_LOCAL_FILE_ACCESS_V1_WRITE;
    if ((mode & 0x0fu) == 2u || (mode & 0x0fu) == 3u)
        return BX_NTVDM_DEM_LOCAL_FILE_ACCESS_V1_READ | BX_NTVDM_DEM_LOCAL_FILE_ACCESS_V1_WRITE;
    return 0u;
}

int bx_ntvdm_dem_fcb_handle_partition_v1_dispatch(
    bx_ntvdm_dem_whole_provider_v1 *provider, uint8_t service,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu, const char *oem_path,
    uint8_t *io_bytes, uint32_t io_capacity, uint32_t *io_byte_count,
    bx_ntvdm_cpu_result_v2 *result)
{
    HANDLE handle;
    uint16_t time, date;
    uint32_t size, opaque;
    DWORD error = ERROR_SUCCESS, transferred = 0u;
    if (io_byte_count) *io_byte_count = 0u;
    if (!provider || !boundary || !cpu || !result || !selected(service) ||
        !bx_ntvdm_dem_whole_provider_v1_valid(provider) ||
        !bx_ntvdm_exception_event_v1_valid(boundary) ||
        !bx_ntvdm_cpu_state_v1_valid(cpu) ||
        cpu->execution_mode != BX_NTVDM_CPU_EXECUTION_REAL ||
        boundary->fault_rip > UINT64_MAX - 4u) return 0;
    if (service == 0x30u) {
        SYSTEMTIME now; GetLocalTime(&now);
        return finish(boundary, result, (uint16_t)((now.wYear << 9) |
            ((now.wMonth & 0x0fu) << 5) | (now.wDay & 0x1fu)), 1, 0) &&
            bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 2u,
                (uint16_t)((now.wHour << 11) | ((now.wMinute & 0x3fu) << 5) |
                ((now.wSecond / 2u) & 0x1fu)));
    }
    if (service == 0x2cu || service == 0x2du) {
        uint8_t mode = (uint8_t)(cpu->eax & 0xffu);
        uint32_t access = service == 0x2cu ?
            (BX_NTVDM_DEM_LOCAL_FILE_ACCESS_V1_READ | BX_NTVDM_DEM_LOCAL_FILE_ACCESS_V1_WRITE) : access_mode(mode);
        DWORD disposition = service == 0x2cu ? CREATE_ALWAYS : OPEN_EXISTING;
        if (!oem_path || access == 0u) return error_result(boundary, result, DEM_ERROR_INVALID_FUNCTION);
        if (mode == 0x08u && service == 0x2cu) return error_result(boundary, result, DEM_ERROR_INVALID_FUNCTION);
        if (provider->file_view.kind == BX_NTVDM_DEM_FILE_VIEW_V1_OVERLAY) {
            uint8_t drive; wchar_t relative[BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE];
            if (bx_ntvdm_dem_path_v1_resolve(oem_path, provider->cwd, &drive, relative) !=
                BX_NTVDM_DEM_PATH_V1_OK || !bx_ntvdm_dem_fcb_overlay_backend_v1_open(
                    &provider->files, &provider->overlay_files, provider->host_namespace,
                    drive, relative, access, service == 0x2cu ? 3u :
                    overlay_share_mode(mode), disposition, mode, &opaque, &size,
                    &time, &date, &error))
                return error_result(boundary, result, error);
            return finish(boundary, result, (uint16_t)(opaque >> 16), 1, 0) &&
                bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 3u, time) &&
                bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 1u, date) &&
                bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 5u, (uint16_t)opaque) &&
                bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 6u, (uint16_t)size) &&
                bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 2u, (uint16_t)(size >> 16));
        }
        { int backend = bx_ntvdm_dem_local_file_backend_v1_open_ex(
                &provider->local_files, oem_path, access, share_mode(mode),
                disposition, &opaque, &error);
          if (backend != BX_NTVDM_DEM_LOCAL_FILE_BACKEND_V1_OK)
              return error_result(boundary, result,
                  (DWORD)backend_error(backend, error)); }
        if (!bx_ntvdm_dem_file_session_v1_lookup(&provider->files, opaque, &handle) ||
            !file_info(handle, &time, &date, &size)) return error_result(boundary, result, GetLastError());
        return finish(boundary, result, (uint16_t)(opaque >> 16), 1, 0) &&
            bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 3u, time) &&
            bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 1u, date) &&
            bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 5u, (uint16_t)opaque) &&
            bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 6u, (uint16_t)size) &&
            bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 2u, (uint16_t)(size >> 16));
    }
    if (service == 0x2eu) {
        opaque = token_si(cpu);
        if (opaque == 0u) return finish(boundary, result, 0u, 0, 0);
        if (provider->file_view.kind == BX_NTVDM_DEM_FILE_VIEW_V1_OVERLAY &&
            bx_ntvdm_dem_overlay_handle_backend_v1_close(&provider->files,
                &provider->overlay_files, opaque)) return finish(boundary, result, 0u, 0, 0);
        if (!bx_ntvdm_dem_file_session_v1_release(&provider->files, opaque))
            return error_result(boundary, result, DEM_ERROR_INVALID_HANDLE);
        return finish(boundary, result, 0u, 0, 0);
    }
    if (service == 0x31u) {
        uint8_t drive; wchar_t relative[BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE]; DWORD attributes;
        int backend;
        if (!oem_path || bx_ntvdm_dem_path_v1_resolve(oem_path, provider->cwd, &drive, relative) !=
                BX_NTVDM_DEM_PATH_V1_OK)
            return error_result(boundary, result, error);
        if (provider->file_view.kind == BX_NTVDM_DEM_FILE_VIEW_V1_OVERLAY) {
            if (bx_ntvdm_dem_fcb_overlay_backend_v1_open(&provider->files,
                    &provider->overlay_files, provider->host_namespace, drive, relative,
                    BX_NTVDM_DEM_OVERLAY_FILE_V1_READ, 3u, OPEN_EXISTING, 0u,
                    &opaque, &size, &time, &date, &error)) {
                uint32_t overlay_attributes;
                if (!bx_ntvdm_dem_fcb_overlay_backend_v1_info(&provider->files,
                        &provider->overlay_files, opaque, &overlay_attributes, &size,
                        &time, &date) || !bx_ntvdm_dem_overlay_handle_backend_v1_close(
                        &provider->files, &provider->overlay_files, opaque))
                    return error_result(boundary, result, DEM_ERROR_INVALID_FUNCTION);
                attributes = overlay_attributes;
                if (attributes == FILE_ATTRIBUTE_NORMAL) attributes = 0u; else attributes &= 0x37u;
                return finish(boundary, result, (uint16_t)attributes, 1, 0) &&
                    bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 1u, time) &&
                    bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 2u, date) &&
                    bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 7u, (uint16_t)size) &&
                    bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 3u, (uint16_t)(size >> 16));
            }
            return error_result(boundary, result, error);
        }
        backend = bx_ntvdm_dem_local_file_backend_v1_open_ex(&provider->local_files, oem_path,
            BX_NTVDM_DEM_LOCAL_FILE_ACCESS_V1_READ, FILE_SHARE_READ, OPEN_EXISTING,
            &opaque, &error);
        if (backend != BX_NTVDM_DEM_LOCAL_FILE_BACKEND_V1_OK)
            return error_result(boundary, result, (DWORD)backend_error(backend, error));
        if (
            !bx_ntvdm_dem_file_session_v1_lookup(&provider->files, opaque, &handle) ||
            !file_info(handle, &time, &date, &size) ||
            !bx_ntvdm_host_namespace_v1_query_file_attributes(provider->host_namespace,
                drive, relative, &attributes, &error)) return error_result(boundary, result, error);
        (void)bx_ntvdm_dem_file_session_v1_release(&provider->files, opaque);
        if (attributes == FILE_ATTRIBUTE_NORMAL) attributes = 0u; else attributes &= 0x37u;
        return finish(boundary, result, (uint16_t)attributes, 1, 0) &&
            bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 1u, time) &&
            bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 2u, date) &&
            bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 7u, (uint16_t)size) &&
            bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 3u, (uint16_t)(size >> 16));
    }
    opaque = token_bp(cpu);
    {
        uint32_t kind = BX_NTVDM_DEM_FILE_TOKEN_KIND_V1_NONE;
        if (bx_ntvdm_dem_file_session_v1_token_kind(&provider->files, opaque, &kind) &&
            kind == BX_NTVDM_DEM_FILE_TOKEN_KIND_V1_OVERLAY_FILE) {
            uint32_t position, overlay_attributes, overlay_transferred;
            int32_t distance = (int32_t)(uint32_t)(((cpu->edi & 0xffffu) << 16) |
                (cpu->edx & 0xffffu));
            if (io_byte_count == 0 || (cpu->ecx & 0xffffu) > io_capacity ||
                ((cpu->ecx & 0xffffu) && !io_bytes) ||
                !bx_ntvdm_dem_overlay_handle_backend_v1_seek(&provider->files,
                    &provider->overlay_files, opaque, distance,
                    BX_NTVDM_DEM_OVERLAY_FILE_V1_SEEK_BEGIN, &position))
                return error_result(boundary, result, DEM_ERROR_INVALID_HANDLE);
            if ((cpu->ebx & 0xffffu) != 0u) {
                if (!bx_ntvdm_dem_overlay_handle_backend_v1_read(&provider->files,
                        &provider->overlay_files, opaque, io_bytes,
                        (uint16_t)cpu->ecx, &overlay_transferred))
                    return error_result(boundary, result, DEM_ERROR_INVALID_HANDLE);
            } else if (!bx_ntvdm_dem_overlay_handle_backend_v1_write(&provider->files,
                    &provider->overlay_files, opaque, io_bytes,
                    (uint16_t)cpu->ecx, &overlay_transferred))
                return error_result(boundary, result, DEM_ERROR_INVALID_HANDLE);
            if (!bx_ntvdm_dem_fcb_overlay_backend_v1_info(&provider->files,
                    &provider->overlay_files, opaque, &overlay_attributes, &size,
                    &time, &date)) return error_result(boundary, result, DEM_ERROR_INVALID_HANDLE);
            transferred = overlay_transferred; *io_byte_count = transferred;
            return finish(boundary, result, (uint16_t)(size >> 16), 1, 0) &&
                bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 3u, (uint16_t)size) &&
                bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 1u, (uint16_t)transferred);
        }
    }
    if (!bx_ntvdm_dem_file_session_v1_lookup(&provider->files, opaque, &handle))
        return error_result(boundary, result, DEM_ERROR_INVALID_HANDLE);
    if (io_byte_count == 0 || (cpu->ecx & 0xffffu) > io_capacity ||
        ((cpu->ecx & 0xffffu) && !io_bytes)) return 0;
    {
        LARGE_INTEGER offset, ignored;
        offset.QuadPart = (LONG)(uint32_t)(((cpu->edi & 0xffffu) << 16) | (cpu->edx & 0xffffu));
        if (!SetFilePointerEx(handle, offset, &ignored, FILE_BEGIN)) return error_result(boundary, result, GetLastError());
    }
    if ((cpu->ebx & 0xffffu) != 0u) {
        if (!ReadFile(handle, io_bytes, (DWORD)(cpu->ecx & 0xffffu), &transferred, 0)) return error_result(boundary, result, GetLastError());
    } else if (!WriteFile(handle, io_bytes, (DWORD)(cpu->ecx & 0xffffu), &transferred, 0)) {
        error = GetLastError(); if (error == DEM_ERROR_DISK_FULL) return finish(boundary, result, 1u, 1, 1) && bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 1u, (uint16_t)transferred);
        return error_result(boundary, result, error);
    }
    if (!file_info(handle, &time, &date, &size)) return error_result(boundary, result, GetLastError());
    *io_byte_count = transferred;
    return finish(boundary, result, (uint16_t)(size >> 16), 1, 0) &&
        bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 3u, (uint16_t)size) &&
        bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 1u, (uint16_t)transferred);
}
