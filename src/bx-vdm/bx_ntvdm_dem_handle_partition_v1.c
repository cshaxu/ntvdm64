#include "bx_ntvdm_dem_handle_partition_v1.h"

#define DEM_ERROR_INVALID_FUNCTION 1u
#define DEM_ERROR_INVALID_HANDLE 6u
#define DEM_ERROR_DISK_FULL 112u
#define DEM_ERROR_WRITE_PROTECT 19u
#define DEM_ERROR_GEN_FAILURE 31u
#define DEM_ERROR_WRONG_DISK 34u

static uint32_t token(const bx_ntvdm_cpu_state_v1 *cpu)
{ return ((cpu->eax & 0xffffu) << 16) | (cpu->ebp & 0xffffu); }
static int selected(uint8_t service)
{ return service == 0x00u || service == 0x02u || service == 0x08u ||
    service == 0x16u || service == 0x1eu || service == 0x27u; }
static int finish(const bx_ntvdm_exception_event_v1 *boundary,
    bx_ntvdm_cpu_result_v2 *result, uint16_t ax, int write_ax, int cf)
{
    return bx_ntvdm_cpu_result_v2_resume(result, boundary->fault_rip + 4u) &&
        (!write_ax || bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 0u, ax)) &&
        bx_ntvdm_cpu_result_v2_set_cf(result, cf);
}
static int error_result(const bx_ntvdm_exception_event_v1 *boundary,
    bx_ntvdm_cpu_result_v2 *result, DWORD error)
{
    /* The historic hard-error route arms INT 24.  Until that owner exists,
     * retain its explicit AX=FFFF/CF result rather than silently treating it
     * as a normal local-filesystem error. */
    if ((error >= DEM_ERROR_WRITE_PROTECT && error <= DEM_ERROR_GEN_FAILURE) ||
        error == DEM_ERROR_WRONG_DISK) return finish(boundary, result, 0xffffu, 1, 1);
    return finish(boundary, result, (uint16_t)(error == 0u ? DEM_ERROR_INVALID_FUNCTION : error), 1, 1);
}
static int local_handle(bx_ntvdm_dem_whole_provider_v1 *provider,
    const bx_ntvdm_cpu_state_v1 *cpu, HANDLE *out)
{
    return bx_ntvdm_dem_file_session_v1_lookup(&provider->files, token(cpu), out);
}
static int seek_if_requested(HANDLE handle, const bx_ntvdm_cpu_state_v1 *cpu)
{
    LARGE_INTEGER distance, position;
    if ((cpu->eflags & 0x40u) != 0u) return 1;
    distance.QuadPart = (LONG)(uint32_t)(((cpu->ebx & 0xffffu) << 16) |
        (cpu->esi & 0xffffu));
    return SetFilePointerEx(handle, distance, &position, FILE_BEGIN) != 0;
}

int bx_ntvdm_dem_handle_partition_v1_dispatch(
    bx_ntvdm_dem_whole_provider_v1 *provider, uint8_t service,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu,
    uint8_t *io_bytes, uint32_t io_capacity, uint32_t *io_byte_count,
    bx_ntvdm_cpu_result_v2 *result)
{
    HANDLE handle;
    DWORD transferred = 0u;
    if (io_byte_count != 0) *io_byte_count = 0u;
    if (!provider || !boundary || !cpu || !result || !selected(service) ||
        !bx_ntvdm_dem_whole_provider_v1_valid(provider) ||
        !bx_ntvdm_exception_event_v1_valid(boundary) ||
        !bx_ntvdm_cpu_state_v1_valid(cpu) || boundary->fault_rip > UINT64_MAX - 4u ||
        cpu->execution_mode != BX_NTVDM_CPU_EXECUTION_REAL) return 0;
    if (service == 0x02u && token(cpu) == 0u) return finish(boundary, result, 0u, 0, 0);
    /* demFileTimes deliberately does not consume AX:BP for its device-time
     * case.  Keep that historical distinction before resolving a local
     * opaque token. */
    if (service == 0x08u && (uint8_t)(cpu->ebx & 0xffu) != 0u &&
        (uint8_t)(cpu->ebx & 0xffu) != 1u) {
        FILETIME file_time, local_time;
        SYSTEMTIME now;
        WORD date, time;
        GetSystemTime(&now);
        if (!SystemTimeToFileTime(&now, &file_time) ||
            !FileTimeToLocalFileTime(&file_time, &local_time) ||
            !FileTimeToDosDateTime(&local_time, &date, &time))
            return finish(boundary, result, 0u, 0, 0);
        return finish(boundary, result, 0u, 0, 0) &&
            bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 1u, time) &&
            bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 2u, date);
    }
    if (!local_handle(provider, cpu, &handle))
        return error_result(boundary, result, DEM_ERROR_INVALID_HANDLE);
    if (service == 0x00u) {
        LARGE_INTEGER distance, position;
        uint8_t origin = (uint8_t)(cpu->ebx & 0xffu);
        if (origin > FILE_END) return error_result(boundary, result, DEM_ERROR_INVALID_FUNCTION);
        distance.QuadPart = (LONG)(uint32_t)(((cpu->ecx & 0xffffu) << 16) |
            (cpu->edx & 0xffffu));
        if (!SetFilePointerEx(handle, distance, &position, origin))
            return error_result(boundary, result, GetLastError());
        return finish(boundary, result, (uint16_t)position.LowPart, 1, 0) &&
            bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 2u,
                (uint16_t)((uint64_t)position.QuadPart >> 16));
    }
    if (service == 0x02u) {
        uint32_t location = ((cpu->ecx & 0xffffu) << 16) | (cpu->edx & 0xffffu);
        if (location != 0xffffffffu) {
            LARGE_INTEGER distance, ignored;
            distance.QuadPart = (LONG)location;
            if (!SetFilePointerEx(handle, distance, &ignored, FILE_BEGIN))
                return error_result(boundary, result, GetLastError());
        }
        if (!bx_ntvdm_dem_file_session_v1_release(&provider->files, token(cpu)))
            return error_result(boundary, result, GetLastError());
        return finish(boundary, result, 0u, 0, 0);
    }
    if (service == 0x27u) { (void)FlushFileBuffers(handle); return finish(boundary, result, 0u, 0, 0); }
    if (service == 0x08u) {
        uint8_t option = (uint8_t)(cpu->ebx & 0xffu);
        FILETIME file_time, local_time;
        WORD date, time;
        if (option == 0u) {
            if (!GetFileTime(handle, 0, 0, &file_time)) return error_result(boundary, result, GetLastError());
        } else if (option == 1u) {
            if (!DosDateTimeToFileTime((WORD)(cpu->edx & 0xffffu),
                    (WORD)(cpu->ecx & 0xffffu), &local_time) ||
                !LocalFileTimeToFileTime(&local_time, &file_time)) return finish(boundary, result, 0u, 0, 0);
            if (!SetFileTime(handle, 0, 0, &file_time)) return error_result(boundary, result, GetLastError());
            return finish(boundary, result, 0u, 0, 0);
        } else return 0;
        if (!FileTimeToLocalFileTime(&file_time, &local_time) ||
            !FileTimeToDosDateTime(&local_time, &date, &time)) return finish(boundary, result, 0u, 0, 0);
        return finish(boundary, result, 0u, 0, 0) &&
            bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 1u, time) &&
            bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 2u, date);
    }
    if (!seek_if_requested(handle, cpu)) return error_result(boundary, result, GetLastError());
    if (io_byte_count == 0 || (cpu->ecx & 0xffffu) > io_capacity ||
        ((cpu->ecx & 0xffffu) != 0u && io_bytes == 0)) return 0;
    if (service == 0x16u) {
        if (!ReadFile(handle, io_bytes, (DWORD)(cpu->ecx & 0xffffu), &transferred, 0))
            return error_result(boundary, result, GetLastError());
        *io_byte_count = transferred;
        return finish(boundary, result, (uint16_t)transferred, 1, 0);
    }
    if ((cpu->ecx & 0xffffu) == 0u) {
        if (!SetEndOfFile(handle)) return error_result(boundary, result, GetLastError());
        return finish(boundary, result, 0u, 0, 0);
    }
    if (!WriteFile(handle, io_bytes, (DWORD)(cpu->ecx & 0xffffu), &transferred, 0)) {
        DWORD error = GetLastError();
        if (error == DEM_ERROR_DISK_FULL) return finish(boundary, result, 0u, 1, 0);
        return error_result(boundary, result, error);
    }
    *io_byte_count = transferred;
    return finish(boundary, result, (uint16_t)transferred, 1, 0);
}
