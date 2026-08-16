#include "bx_ntvdm_dem_fcb_wildcard_partition_v1.h"

#include "bx_ntvdm_search_request_v1.h"
#include "bx_ntvdm_dem_overlay_namespace_view_v1.h"
#include "bx_ntvdm_dem_overlay_mutation_backend_v1.h"

#include <string.h>
#include <wctype.h>

#define DEM_ERROR_INVALID_FUNCTION 1u
#define DEM_ERROR_FILE_NOT_FOUND 2u
#define DEM_ERROR_ACCESS_DENIED 5u
#define DEM_ERROR_PATH_NOT_FOUND 3u
#define DEM_ERROR_NOT_SAME_DEVICE 17u

static int finish(const bx_ntvdm_exception_event_v1 *boundary,
    bx_ntvdm_cpu_result_v2 *result, uint16_t ax, int set_ax, int cf)
{
    return bx_ntvdm_cpu_result_v2_resume(result, boundary->fault_rip + 4u) &&
        (!set_ax || bx_ntvdm_cpu_delta_v1_set_gpr16(&result->cpu_delta, 0u, ax)) &&
        bx_ntvdm_cpu_result_v2_set_cf(result, cf);
}

static int fail(const bx_ntvdm_exception_event_v1 *boundary,
    bx_ntvdm_cpu_result_v2 *result, DWORD error)
{ return finish(boundary, result, (uint16_t)(error ? error : DEM_ERROR_INVALID_FUNCTION), 1, 1); }

static int query(const char *oem, bx_ntvdm_search_query_v1 *out)
{
    uint8_t copied[BX_NTVDM_SEARCH_REQUEST_V1_PATH_BYTES] = {0};
    size_t length;
    if (oem == 0 || out == 0) return 0;
    length = strnlen_s(oem, sizeof(copied));
    if (length == 0u || length == sizeof(copied)) return 0;
    memcpy(copied, oem, length + 1u);
    return bx_ntvdm_search_request_v1_decode_first_fcb(copied, 0u, 0u, out);
}

static int wildcard(const wchar_t *pattern, const wchar_t *name)
{
    while (*pattern != L'\0') {
        if (*pattern == L'*') {
            while (*pattern == L'*') ++pattern;
            if (*pattern == L'\0') return 1;
            while (*name != L'\0') if (wildcard(pattern, name++)) return 1;
            return wildcard(pattern, name);
        }
        if (*name == L'\0' || (*pattern != L'?' &&
            towupper((wint_t)*pattern) != towupper((wint_t)*name))) return 0;
        ++pattern; ++name;
    }
    return *name == L'\0';
}

static int relative_name(const bx_ntvdm_search_query_v1 *query,
    const wchar_t *name, wchar_t out[BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE])
{
    size_t directory;
    if (query == 0 || name == 0 || out == 0) return 0;
    directory = wcslen(query->relative_directory);
    if (directory + (directory ? 1u : 0u) + wcslen(name) >=
        BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE) return 0;
    wcscpy_s(out, BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE, query->relative_directory);
    if (directory) wcscat_s(out, BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE, L"\\");
    return wcscat_s(out, BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE, name) == 0;
}

static int destination_name(const wchar_t *source, const wchar_t *template_name,
    wchar_t out[13])
{
    const wchar_t *src = source, *dst = template_name;
    wchar_t *write = out;
    if (source == 0 || template_name == 0 || out == 0) return 0;
    while (*dst != L'\0') {
        if ((size_t)(write - out) >= 12u) return 0;
        if (*dst == L'?') {
            if (*src != L'.' && *src != L'\0') *write++ = *src++;
        } else if (*dst == L'.') {
            while (*src != L'.' && *src != L'\0') ++src;
            if (*src == L'.') ++src;
            *write++ = L'.';
        } else {
            if (*src != L'.' && *src != L'\0') ++src;
            *write++ = *dst;
        }
        ++dst;
    }
    *write = L'\0';
    return out[0] != L'\0';
}

static int mutation_view(const bx_ntvdm_dem_whole_provider_v1 *provider,
    const bx_ntvdm_exception_event_v1 *boundary, bx_ntvdm_cpu_result_v2 *result)
{
    uint32_t policy;
    if (!bx_ntvdm_dem_profile_consumer_v1_resolve(&provider->files.profile,
            BX_NTVDM_MUTATION_CLASS_V1_NAMESPACE_CONTENT, &policy)) return 0;
    if (policy == BX_NTVDM_MUTATION_POLICY_V1_DIRECT_HOST) return 1;
    if (policy == BX_NTVDM_MUTATION_POLICY_V1_USE_OVERLAY) return 2;
    return policy == BX_NTVDM_MUTATION_POLICY_V1_REJECT_READONLY ?
        -fail(boundary, result, DEM_ERROR_ACCESS_DENIED) :
        -fail(boundary, result, DEM_ERROR_INVALID_FUNCTION);
}

int bx_ntvdm_dem_fcb_wildcard_partition_v1_dispatch(
    bx_ntvdm_dem_whole_provider_v1 *provider, uint8_t service,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu, const char *source_oem,
    const char *destination_oem, bx_ntvdm_cpu_result_v2 *result)
{
    bx_ntvdm_search_query_v1 source, destination;
    bx_ntvdm_host_namespace_entry_v1 entries[BX_NTVDM_HOST_NAMESPACE_V1_MAX_ENTRIES];
    uint32_t count = 0u, index;
    int admitted, success = 0, rejected = 0;
    if (provider == 0 || boundary == 0 || cpu == 0 || result == 0 ||
        (service != 0x07u && service != 0x20u) ||
        !bx_ntvdm_dem_whole_provider_v1_valid(provider) ||
        !bx_ntvdm_exception_event_v1_valid(boundary) ||
        !bx_ntvdm_cpu_state_v1_valid(cpu) ||
        cpu->execution_mode != BX_NTVDM_CPU_EXECUTION_REAL ||
        boundary->fault_rip > UINT64_MAX - 4u || !query(source_oem, &source)) return 0;
    admitted = mutation_view(provider, boundary, result);
    if (admitted <= 0) return admitted < 0;
    if (service == 0x07u && (uint8_t)cpu->eax != 0u &&
        (uint8_t)cpu->edx == 0x08u) return fail(boundary, result, DEM_ERROR_INVALID_FUNCTION);
    /* demfcb.c accepts a different directory on the same DOS drive. */
    if (service == 0x20u && (!query(destination_oem, &destination) ||
        source.drive_index != destination.drive_index))
        return fail(boundary, result, DEM_ERROR_NOT_SAME_DEVICE);
    if (admitted == 2) {
        DWORD overlay_error = ERROR_SUCCESS;
        if (!bx_ntvdm_dem_overlay_namespace_view_v1_enumerate(&provider->overlay_store,
                provider->host_namespace, source.drive_index, source.relative_directory,
                entries, BX_NTVDM_HOST_NAMESPACE_V1_MAX_ENTRIES, &count, &overlay_error))
            return fail(boundary, result, overlay_error ? overlay_error : DEM_ERROR_PATH_NOT_FOUND);
    } else if (bx_ntvdm_host_namespace_v1_enumerate(provider->host_namespace,
            source.drive_index, source.relative_directory, entries,
            BX_NTVDM_HOST_NAMESPACE_V1_MAX_ENTRIES, &count) != BX_NTVDM_HOST_NAMESPACE_V1_OK)
        return fail(boundary, result, DEM_ERROR_PATH_NOT_FOUND);
    for (index = 0u; index < count; ++index) {
        wchar_t current[BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE];
        if (!wildcard(source.pattern, entries[index].dos_name) ||
            !relative_name(&source, entries[index].dos_name, current)) continue;
        if (service == 0x07u) {
            uint32_t special = entries[index].attributes & 0x07u;
            int extended = (uint8_t)cpu->eax != 0u;
            uint8_t allowed = (uint8_t)cpu->edx & 0x07u;
            DWORD error = ERROR_SUCCESS;
            if ((!extended && special != 0u) ||
                (extended && (special & allowed) != special)) { rejected = 1; continue; }
            if (admitted == 2) {
                if (!bx_ntvdm_dem_overlay_mutation_backend_v1_delete_file(
                        &provider->overlay_store, provider->host_namespace,
                        source.drive_index, current, &error))
                    return fail(boundary, result, error);
                if (error != ERROR_SUCCESS) { rejected = 1; continue; }
            } else if ((entries[index].attributes & FILE_ATTRIBUTE_READONLY) != 0u &&
                !bx_ntvdm_host_namespace_v1_set_file_attributes(provider->host_namespace,
                    source.drive_index, current, FILE_ATTRIBUTE_NORMAL, &error)) {
                rejected = 1; continue;
            }
            if (admitted != 2 && !bx_ntvdm_host_namespace_v1_delete_file(provider->host_namespace,
                    source.drive_index, current, &error)) return fail(boundary, result, error);
            success = 1;
        } else {
            wchar_t name[13], target[BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE];
            DWORD error = ERROR_SUCCESS;
            if (!destination_name(entries[index].dos_name, destination.pattern, name) ||
                !relative_name(&destination, name, target)) return fail(boundary, result, DEM_ERROR_INVALID_FUNCTION);
            if (_wcsicmp(current, target) == 0) return fail(boundary, result, DEM_ERROR_ACCESS_DENIED);
            if (admitted == 2) {
                if (!bx_ntvdm_dem_overlay_mutation_backend_v1_rename(&provider->overlay_store,
                        &provider->overlay_files, provider->host_namespace, source.drive_index,
                        current, destination.drive_index, target, &error)) return fail(boundary, result, error);
                if (error != ERROR_SUCCESS) return fail(boundary, result, error);
            } else if (!bx_ntvdm_host_namespace_v1_rename_file(provider->host_namespace,
                    source.drive_index, current, destination.drive_index, target, &error))
                return fail(boundary, result, error);
            success = 1;
        }
    }
    return success ? finish(boundary, result, 0u, 0, 0) :
        fail(boundary, result, rejected ? DEM_ERROR_ACCESS_DENIED : DEM_ERROR_FILE_NOT_FOUND);
}
