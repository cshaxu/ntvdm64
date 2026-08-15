#include "bx_ntvdm_dem_namespace_partition_v1.h"

#include <string.h>

#define DEM_ERROR_INVALID_FUNCTION 1u
#define DEM_ERROR_ACCESS_DENIED 5u
#define DEM_ERROR_WRITE_PROTECT 19u
#define DEM_ERROR_GEN_FAILURE 31u
#define DEM_ERROR_WRONG_DISK 34u

static int finish(const bx_ntvdm_exception_event_v1 *boundary,
    bx_ntvdm_cpu_result_v2 *result, uint16_t ax, int write_ax, int cf)
{
    return bx_ntvdm_cpu_result_v2_resume(result, boundary->fault_rip + 4u) &&
        (!write_ax || bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 0u, ax)) &&
        bx_ntvdm_cpu_result_v2_set_cf(result, cf);
}

static int fail(const bx_ntvdm_exception_event_v1 *boundary,
    bx_ntvdm_cpu_result_v2 *result, DWORD error)
{
    /* demClientErrorEx owns an INT 24 route for these errors.  No such owner
     * is admitted in S4, so retain the existing explicit FFFF/CF limitation. */
    if ((error >= DEM_ERROR_WRITE_PROTECT && error <= DEM_ERROR_GEN_FAILURE) ||
        error == DEM_ERROR_WRONG_DISK) return finish(boundary, result, 0xffffu, 1, 1);
    return finish(boundary, result,
        (uint16_t)(error == ERROR_SUCCESS ? DEM_ERROR_INVALID_FUNCTION : error), 1, 1);
}

static int valid_call(const bx_ntvdm_dem_whole_provider_v1 *provider,
    uint8_t service, const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu, bx_ntvdm_cpu_result_v2 *result)
{
    return provider != 0 && boundary != 0 && cpu != 0 && result != 0 &&
        bx_ntvdm_dem_whole_provider_v1_valid(provider) &&
        bx_ntvdm_dem_namespace_partition_v1_owns_service(service) &&
        bx_ntvdm_exception_event_v1_valid(boundary) &&
        bx_ntvdm_cpu_state_v1_valid(cpu) &&
        cpu->execution_mode == BX_NTVDM_CPU_EXECUTION_REAL &&
        boundary->fault_rip <= UINT64_MAX - 4u;
}

static int resolve_path(const bx_ntvdm_dem_whole_provider_v1 *provider,
    const char *oem_path, uint8_t *drive_out,
    wchar_t relative_out[BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE])
{
    return oem_path != 0 && bx_ntvdm_dem_path_v1_resolve(oem_path,
        provider->cwd, drive_out, relative_out) == BX_NTVDM_DEM_PATH_V1_OK;
}

static int mutation(const bx_ntvdm_dem_whole_provider_v1 *provider,
    uint32_t mutation_class, const bx_ntvdm_exception_event_v1 *boundary,
    bx_ntvdm_cpu_result_v2 *result)
{
    uint32_t policy = 0u;
    if (!bx_ntvdm_dem_profile_consumer_v1_resolve(&provider->files.profile,
            mutation_class, &policy)) return 0;
    if (policy == BX_NTVDM_MUTATION_POLICY_V1_DIRECT_HOST) return 1;
    if (policy == BX_NTVDM_MUTATION_POLICY_V1_REJECT_READONLY)
        return fail(boundary, result, DEM_ERROR_ACCESS_DENIED) ? -1 : 0;
    /* Overlay and virtual backends are deliberately not faked by direct host
     * calls; their current provider disposition is explicit unavailable. */
    return fail(boundary, result, DEM_ERROR_INVALID_FUNCTION) ? -1 : 0;
}

static ULONG open_share(uint8_t mode)
{
    switch (mode & 0x70u) {
    case 0x10u: return FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
    case 0x20u: return 0u;
    case 0x30u: return FILE_SHARE_READ;
    case 0x40u: return FILE_SHARE_WRITE;
    case 0x50u: return FILE_SHARE_READ | FILE_SHARE_WRITE;
    default: return FILE_SHARE_READ | FILE_SHARE_WRITE;
    }
}

int bx_ntvdm_dem_namespace_partition_v1_owns_service(uint8_t service)
{
    return service == 0x01u || service == 0x03u || service == 0x04u ||
        service == 0x05u || service == 0x06u || service == 0x12u ||
        service == 0x17u || service == 0x22u || service == 0x44u;
}

int bx_ntvdm_dem_namespace_partition_v1_resolve(
    const bx_ntvdm_dem_whole_provider_v1 *provider, uint8_t service,
    const char *oem_path, uint8_t *drive_out,
    wchar_t relative_out[BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE],
    uint32_t mutation_class, uint32_t *policy_out)
{
    if (drive_out != 0) *drive_out = 0u;
    if (policy_out != 0) *policy_out = 0u;
    if (!provider || !oem_path || !drive_out || !relative_out || !policy_out ||
        !bx_ntvdm_dem_whole_provider_v1_valid(provider) ||
        !bx_ntvdm_dem_namespace_partition_v1_owns_service(service)) return 0;
    if (!bx_ntvdm_dem_profile_consumer_v1_resolve(&provider->files.profile,
            mutation_class, policy_out)) return 0;
    return bx_ntvdm_dem_path_v1_resolve(oem_path, provider->cwd, drive_out,
        relative_out) == BX_NTVDM_DEM_PATH_V1_OK;
}

int bx_ntvdm_dem_namespace_partition_v1_dispatch(
    bx_ntvdm_dem_whole_provider_v1 *provider, uint8_t service,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu, const char *oem_path,
    const char *oem_second_path, bx_ntvdm_cpu_result_v2 *result)
{
    wchar_t relative[BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE];
    wchar_t second_relative[BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE];
    uint8_t drive, second_drive;
    DWORD error = ERROR_SUCCESS;
    int admitted, startup_path = 0;
    uint64_t startup_size = 0u;
    if (!valid_call(provider, service, boundary, cpu, result) || oem_path == 0)
        return 0;

    if (service == 0x44u && _stricmp(oem_path, "\\\\DEV\\") == 0)
        return finish(boundary, result, 0u, 0, 0) &&
            bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 2u, 0u);
    if (!resolve_path(provider, oem_path, &drive, relative))
        return fail(boundary, result, ERROR_PATH_NOT_FOUND);
    if (provider->startup_namespace != 0)
        startup_path = bx_ntvdm_readonly_namespace_v1_match_startup_path(
            provider->startup_namespace, drive, relative, &startup_size);
    if (service == 0x44u) {
        /* demCheckPath's pathname has no drive prefix; DL is its one-based
         * DOS drive value and must therefore select the admitted root. */
        uint8_t dl = (uint8_t)(cpu->edx & 0xffu);
        if (dl == 0u || dl > 26u) return fail(boundary, result, ERROR_PATH_NOT_FOUND);
        drive = (uint8_t)(dl - 1u);
    }

    if (service == 0x01u) {
        DWORD attributes;
        if (startup_path) {
            if ((cpu->eax & 0xffu) != 0u)
                return fail(boundary, result, DEM_ERROR_ACCESS_DENIED);
            /* The image is provider-owned and immutable, but preserves the
             * ordinary DOS attribute projection expected by the startup
             * reader; no real host file is queried or modified. */
            return finish(boundary, result, 0u, 0, 0) &&
                bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 2u, 0u);
        }
        if ((cpu->eax & 0xffu) != 0u) {
            admitted = mutation(provider, BX_NTVDM_MUTATION_CLASS_V1_FILE_METADATA,
                boundary, result);
            if (admitted <= 0) return admitted < 0;
            attributes = cpu->ecx & 0xffffu;
            if (attributes == 0u) attributes = FILE_ATTRIBUTE_NORMAL;
            if (!bx_ntvdm_host_namespace_v1_set_file_attributes(
                    provider->host_namespace, drive, relative, attributes, &error))
                return fail(boundary, result, error);
            return finish(boundary, result, 0u, 0, 0);
        }
        if (!bx_ntvdm_host_namespace_v1_query_file_attributes(
                provider->host_namespace, drive, relative, &attributes, &error))
            return fail(boundary, result, error);
        if (attributes == FILE_ATTRIBUTE_NORMAL) attributes = 0u;
        else attributes &= 0x37u; /* historical DOS_ATTR_MASK projection */
        return finish(boundary, result, 0u, 0, 0) &&
            bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 2u,
                (uint16_t)attributes);
    }
    if (service == 0x03u || service == 0x22u || service == 0x12u) {
        uint32_t access = BX_NTVDM_DEM_LOCAL_FILE_ACCESS_V1_READ |
            BX_NTVDM_DEM_LOCAL_FILE_ACCESS_V1_WRITE;
        uint32_t token = 0u;
        DWORD disposition = service == 0x03u ? CREATE_ALWAYS :
            service == 0x22u ? CREATE_NEW : OPEN_EXISTING;
        if (service == 0x12u) {
            uint8_t mode = (uint8_t)(cpu->ebx & 0xffu);
            if ((cpu->eax & 0xffu) != 0u) return fail(boundary, result, DEM_ERROR_INVALID_FUNCTION);
            if ((mode & 0x07u) == 0u) access = BX_NTVDM_DEM_LOCAL_FILE_ACCESS_V1_READ;
            else if ((mode & 0x07u) == 1u) access = BX_NTVDM_DEM_LOCAL_FILE_ACCESS_V1_WRITE;
            else if ((mode & 0x07u) != 2u) return fail(boundary, result, DEM_ERROR_INVALID_FUNCTION);
        }
        if (startup_path) {
            uint32_t startup_token = 0u;
            if (service != 0x12u || (cpu->ebx & 0x07u) != 0u ||
                !bx_ntvdm_readonly_namespace_v1_open(provider->startup_namespace,
                    drive, relative, &startup_token, &startup_size))
                return fail(boundary, result, DEM_ERROR_ACCESS_DENIED);
            return finish(boundary, result, (uint16_t)(startup_token >> 16), 1, 0) &&
                bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 5u,
                    (uint16_t)startup_token) &&
                bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 2u,
                    (uint16_t)startup_size) &&
                bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 1u,
                    (uint16_t)(startup_size >> 16)) &&
                bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 3u, 0u);
        }
        admitted = bx_ntvdm_dem_local_file_backend_v1_open_ex(&provider->local_files,
            oem_path, access, service == 0x12u ? open_share((uint8_t)cpu->ebx) :
                FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
            disposition, &token, &error);
        if (admitted != BX_NTVDM_DEM_LOCAL_FILE_BACKEND_V1_OK) {
            if (admitted == BX_NTVDM_DEM_LOCAL_FILE_BACKEND_V1_READONLY)
                error = DEM_ERROR_ACCESS_DENIED;
            else if (admitted == BX_NTVDM_DEM_LOCAL_FILE_BACKEND_V1_OVERLAY ||
                admitted == BX_NTVDM_DEM_LOCAL_FILE_BACKEND_V1_VIRTUAL)
                error = DEM_ERROR_INVALID_FUNCTION;
            return fail(boundary, result, error);
        }
        {
            HANDLE handle;
            LARGE_INTEGER size;
            if (!bx_ntvdm_dem_file_session_v1_lookup(&provider->files, token, &handle) ||
                !GetFileSizeEx(handle, &size) || size.QuadPart > UINT32_MAX)
                return fail(boundary, result, GetLastError());
            if (!finish(boundary, result, (uint16_t)(token >> 16), 1, 0) ||
                !bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 5u,
                    (uint16_t)token) ||
                !bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 2u,
                    (uint16_t)size.LowPart) ||
                !bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 1u,
                    (uint16_t)((uint32_t)size.LowPart >> 16))) return 0;
            if (service == 0x12u)
                return bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 3u, 0u);
            return 1;
        }
    }
    if (service == 0x04u || service == 0x05u || service == 0x06u || service == 0x17u) {
        if (startup_path) return fail(boundary, result, DEM_ERROR_ACCESS_DENIED);
        admitted = mutation(provider, BX_NTVDM_MUTATION_CLASS_V1_NAMESPACE_CONTENT,
            boundary, result);
        if (admitted <= 0) return admitted < 0;
    }
    if (service == 0x04u) {
        if ((cpu->ebx & 0xffffu) != 0u || (cpu->esi & 0xffffu) != 0u)
            return fail(boundary, result, DEM_ERROR_INVALID_FUNCTION);
        if (!bx_ntvdm_host_namespace_v1_create_directory(provider->host_namespace,
                drive, relative, &error)) return fail(boundary, result, error);
        return finish(boundary, result, 0u, 0, 0);
    }
    if (service == 0x05u) {
        if (!bx_ntvdm_host_namespace_v1_delete_file(provider->host_namespace,
                drive, relative, &error)) return fail(boundary, result, error);
        return finish(boundary, result, 0u, 0, 0);
    }
    if (service == 0x06u) {
        if (!bx_ntvdm_host_namespace_v1_remove_directory(provider->host_namespace,
                drive, relative, &error)) return fail(boundary, result, error);
        return finish(boundary, result, 0u, 0, 0);
    }
    if (service == 0x17u) {
        if (!resolve_path(provider, oem_second_path, &second_drive, second_relative))
            return fail(boundary, result, ERROR_PATH_NOT_FOUND);
        if (provider->startup_namespace != 0 &&
            bx_ntvdm_readonly_namespace_v1_match_startup_path(
                provider->startup_namespace, second_drive, second_relative, 0))
            return fail(boundary, result, DEM_ERROR_ACCESS_DENIED);
        if (drive != second_drive) return fail(boundary, result, ERROR_NOT_SAME_DEVICE);
        if (_wcsicmp(relative, second_relative) == 0)
            return fail(boundary, result, DEM_ERROR_ACCESS_DENIED);
        if (!bx_ntvdm_host_namespace_v1_rename_file(provider->host_namespace,
                drive, relative, second_drive, second_relative, &error))
            return fail(boundary, result, error);
        return finish(boundary, result, 0u, 0, 0);
    }
    if (service == 0x44u) {
        /* OpenNT probes with a temporary NUL create.  The bounded adapter
         * instead verifies the directory directly, so this check has no host
         * side effect while preserving success/failure meaning. */
        if (!bx_ntvdm_host_namespace_v1_directory_exists(provider->host_namespace,
                drive, relative)) return fail(boundary, result, ERROR_PATH_NOT_FOUND);
        return finish(boundary, result, 0u, 0, 0) &&
            bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 3u, 0u);
    }
    return 0;
}
