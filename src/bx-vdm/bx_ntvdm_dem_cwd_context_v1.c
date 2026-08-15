#include "bx_ntvdm_dem_cwd_context_v1.h"

#include <string.h>
#include <wctype.h>

static int valid_component(const wchar_t *source, size_t length)
{
    size_t index, base = 0u, extension = 0u;
    int saw_dot = 0;
    if (source == 0 || length == 0u || length >= 13u) return 0;
    for (index = 0u; index < length; ++index) {
        wchar_t value = towupper((wint_t)source[index]);
        int allowed;
        if (value == L'.') {
            if (saw_dot || base == 0u) return 0;
            saw_dot = 1;
            continue;
        }
        allowed = (value >= L'A' && value <= L'Z') ||
            (value >= L'0' && value <= L'9') || value == L'$' ||
            value == L'%' || value == L'\'' || value == L'-' ||
            value == L'_' || value == L'@' || value == L'~' ||
            value == L'!' || value == L'#' || value == L'(' ||
            value == L')' || value == L'{' || value == L'}' || value == L'^';
        if (!allowed) return 0;
        if (saw_dot) ++extension; else ++base;
        if (base > 8u || extension > 3u) return 0;
    }
    return base != 0u && (!saw_dot || extension != 0u);
}

static int valid_relative(const wchar_t *relative)
{
    const wchar_t *component;
    if (relative == 0 || wcslen(relative) >= BX_NTVDM_DEM_CWD_CONTEXT_V1_MAX_RELATIVE)
        return 0;
    component = relative;
    while (*component != L'\0') {
        const wchar_t *end = wcschr(component, L'\\');
        size_t length = end == 0 ? wcslen(component) : (size_t)(end - component);
        if (!valid_component(component, length)) return 0;
        component = end == 0 ? component + length : end + 1u;
        if (end != 0 && *component == L'\0') return 0;
    }
    return 1;
}

static int policy(const bx_ntvdm_dem_cwd_context_v1 *context, uint32_t *out)
{
    return bx_ntvdm_dem_cwd_context_v1_valid(context) &&
        bx_ntvdm_dem_profile_consumer_v1_resolve(&context->profile,
            BX_NTVDM_MUTATION_CLASS_V1_SESSION_CONTEXT, out);
}

int bx_ntvdm_dem_cwd_context_v1_valid(const bx_ntvdm_dem_cwd_context_v1 *context)
{
    return context != 0 && context->magic == BX_NTVDM_DEM_CWD_CONTEXT_V1_MAGIC &&
        context->abi_version == BX_NTVDM_DEM_CWD_CONTEXT_V1_VERSION &&
        context->struct_bytes == sizeof(*context) && context->default_drive < 26u &&
        bx_ntvdm_dem_profile_consumer_v1_valid(&context->profile) &&
        (context->profile.profile.mode == BX_NTVDM_MUTATION_MODE_V1_OVERLAY ?
            bx_ntvdm_mutation_overlay_v1_valid(&context->overlay) :
            context->overlay.magic == 0u && context->overlay.abi_version == 0u &&
            context->overlay.struct_bytes == 0u && context->overlay.record_count == 0u &&
            context->overlay.reserved0 == 0u);
}

int bx_ntvdm_dem_cwd_context_v1_initialize(bx_ntvdm_dem_cwd_context_v1 *context,
    const bx_ntvdm_mutation_profile_v1 *profile)
{
    if (context == 0 || profile == 0) return 0;
    memset(context, 0, sizeof(*context));
    context->magic = BX_NTVDM_DEM_CWD_CONTEXT_V1_MAGIC;
    context->abi_version = BX_NTVDM_DEM_CWD_CONTEXT_V1_VERSION;
    context->struct_bytes = sizeof(*context);
    if (!bx_ntvdm_dem_profile_consumer_v1_initialize(&context->profile, profile))
        return 0;
    if (profile->mode == BX_NTVDM_MUTATION_MODE_V1_OVERLAY &&
        !bx_ntvdm_mutation_overlay_v1_initialize(&context->overlay, profile)) return 0;
    return bx_ntvdm_dem_cwd_context_v1_valid(context);
}

int bx_ntvdm_dem_cwd_context_v1_set(bx_ntvdm_dem_cwd_context_v1 *context,
    const bx_ntvdm_host_namespace_v1 *host_namespace, uint8_t drive_index,
    const wchar_t *relative_directory)
{
    uint32_t result;
    uint32_t bytes;
    if (!policy(context, &result) || drive_index >= 26u ||
        !valid_relative(relative_directory)) return BX_NTVDM_DEM_CWD_V1_REJECTED;
    bytes = (uint32_t)((wcslen(relative_directory) + 1u) * sizeof(wchar_t));
    if (result == BX_NTVDM_MUTATION_POLICY_V1_DIRECT_HOST) {
        if (!host_namespace || !bx_ntvdm_host_namespace_v1_directory_exists(
                host_namespace, drive_index, relative_directory))
            return BX_NTVDM_DEM_CWD_V1_NOT_FOUND;
        memcpy(context->direct_relative[drive_index], relative_directory, bytes);
        return BX_NTVDM_DEM_CWD_V1_OK;
    }
    if (result == BX_NTVDM_MUTATION_POLICY_V1_REJECT_READONLY)
        return BX_NTVDM_DEM_CWD_V1_READONLY;
    if (result == BX_NTVDM_MUTATION_POLICY_V1_USE_OVERLAY)
        return bx_ntvdm_mutation_overlay_v1_replace(&context->overlay,
            BX_NTVDM_MUTATION_OWNER_V1_DEM,
            BX_NTVDM_MUTATION_CLASS_V1_SESSION_CONTEXT, drive_index,
            (const uint8_t *)relative_directory, bytes) ?
            BX_NTVDM_DEM_CWD_V1_OK : BX_NTVDM_DEM_CWD_V1_CAPACITY;
    if (result == BX_NTVDM_MUTATION_POLICY_V1_USE_VIRTUAL) {
        memcpy(context->virtual_relative[drive_index], relative_directory, bytes);
        return BX_NTVDM_DEM_CWD_V1_OK;
    }
    return BX_NTVDM_DEM_CWD_V1_REJECTED;
}

int bx_ntvdm_dem_cwd_context_v1_query(const bx_ntvdm_dem_cwd_context_v1 *context,
    uint8_t drive_index, wchar_t *relative_directory, uint32_t capacity)
{
    uint32_t result, bytes = 0u;
    const wchar_t *source;
    if (!policy(context, &result) || drive_index >= 26u || relative_directory == 0 ||
        capacity == 0u) return BX_NTVDM_DEM_CWD_V1_REJECTED;
    source = context->direct_relative[drive_index];
    if (result == BX_NTVDM_MUTATION_POLICY_V1_USE_OVERLAY) {
        if (!bx_ntvdm_mutation_overlay_v1_lookup(&context->overlay,
                BX_NTVDM_MUTATION_OWNER_V1_DEM,
                BX_NTVDM_MUTATION_CLASS_V1_SESSION_CONTEXT, drive_index,
                (uint8_t *)relative_directory, capacity * sizeof(wchar_t), &bytes)) {
            relative_directory[0] = L'\0';
            return BX_NTVDM_DEM_CWD_V1_OK;
        }
        return bytes >= sizeof(wchar_t) && bytes % sizeof(wchar_t) == 0u &&
            relative_directory[bytes / sizeof(wchar_t) - 1u] == L'\0' ?
            BX_NTVDM_DEM_CWD_V1_OK : BX_NTVDM_DEM_CWD_V1_REJECTED;
    }
    if (result == BX_NTVDM_MUTATION_POLICY_V1_USE_VIRTUAL)
        source = context->virtual_relative[drive_index];
    bytes = (uint32_t)((wcslen(source) + 1u) * sizeof(wchar_t));
    if (bytes / sizeof(wchar_t) > capacity) return BX_NTVDM_DEM_CWD_V1_CAPACITY;
    memcpy(relative_directory, source, bytes);
    return BX_NTVDM_DEM_CWD_V1_OK;
}

int bx_ntvdm_dem_cwd_context_v1_set_default_drive(
    bx_ntvdm_dem_cwd_context_v1 *context,
    const bx_ntvdm_host_namespace_v1 *host_namespace, uint8_t drive_index)
{
    uint32_t result;
    if (!policy(context, &result) || drive_index >= 26u) return BX_NTVDM_DEM_CWD_V1_REJECTED;
    if (result == BX_NTVDM_MUTATION_POLICY_V1_DIRECT_HOST &&
        (!host_namespace || !bx_ntvdm_host_namespace_v1_directory_exists(
            host_namespace, drive_index, context->direct_relative[drive_index])))
        return BX_NTVDM_DEM_CWD_V1_NOT_FOUND;
    if (result == BX_NTVDM_MUTATION_POLICY_V1_REJECT_READONLY)
        return BX_NTVDM_DEM_CWD_V1_READONLY;
    context->default_drive = drive_index;
    return BX_NTVDM_DEM_CWD_V1_OK;
}

int bx_ntvdm_dem_cwd_context_v1_query_default_drive(
    const bx_ntvdm_dem_cwd_context_v1 *context, uint8_t *drive_index_out)
{
    if (!bx_ntvdm_dem_cwd_context_v1_valid(context) || drive_index_out == 0)
        return 0;
    *drive_index_out = (uint8_t)context->default_drive;
    return 1;
}
