#include "bx_ntvdm_session_host_context_v1.h"

#include <string.h>
#include <wctype.h>
#include <windows.h>

static int valid_component(const wchar_t *source, size_t length)
{
    size_t index, base = 0u, extension = 0u;
    int saw_dot = 0;
    if (source == 0 || length == 0u || length >= 13u) return 0;
    for (index = 0u; index < length; ++index) {
        wchar_t value = towupper((wint_t)source[index]);
        int allowed;
        if (value == L'.') { if (saw_dot || base == 0u) return 0; saw_dot = 1; continue; }
        allowed = (value >= L'A' && value <= L'Z') || (value >= L'0' && value <= L'9') ||
            value == L'$' || value == L'%' || value == L'\'' || value == L'-' ||
            value == L'_' || value == L'@' || value == L'~' || value == L'!' ||
            value == L'#' || value == L'(' || value == L')' || value == L'{' ||
            value == L'}' || value == L'^';
        if (!allowed) return 0;
        if (saw_dot) ++extension; else ++base;
        if (base > 8u || extension > 3u) return 0;
    }
    return base != 0u && (!saw_dot || extension != 0u);
}

static int valid_relative(const wchar_t *relative)
{
    const wchar_t *component;
    if (relative == 0 || wcslen(relative) >= BX_NTVDM_SESSION_HOST_CONTEXT_V1_MAX_RELATIVE) return 0;
    /* The empty relative portion denotes the root of the selected DOS drive.
       It is the valid result of a host-CWD fallback such as `C:\\`; treating
       it as an empty 8.3 component prevents COMMAND and DEM from sharing the
       selected root context. */
    if (relative[0] == L'\0') return 1;
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

static int policy(const bx_ntvdm_session_host_context_v1 *context, uint32_t *out)
{
    return bx_ntvdm_session_host_context_v1_valid(context) &&
        bx_ntvdm_dem_profile_consumer_v1_resolve(&context->profile,
            BX_NTVDM_MUTATION_CLASS_V1_SESSION_CONTEXT, out);
}

static int namespace_available(const bx_ntvdm_session_host_context_v1 *context,
    uint8_t drive)
{
    return context->host_namespace != 0 &&
        bx_ntvdm_host_namespace_v1_valid(context->host_namespace) &&
        (context->host_namespace->available_mask & (UINT32_C(1) << drive)) != 0u;
}

static int full_path(uint8_t drive, const wchar_t *relative, wchar_t output[MAX_PATH])
{
    size_t length;
    if (drive >= 26u || relative == 0 || output == 0) return 0;
    length = wcslen(relative);
    if (length + 4u > MAX_PATH) return 0;
    output[0] = (wchar_t)(L'A' + drive); output[1] = L':'; output[2] = L'\\';
    memcpy(output + 3u, relative, (length + 1u) * sizeof(*output));
    return 1;
}

static int direct_set(bx_ntvdm_session_host_context_v1 *context,
    uint8_t drive, const wchar_t *relative, uint32_t *win32_error_out)
{
    wchar_t path[MAX_PATH];
    if (!namespace_available(context, drive) ||
        !bx_ntvdm_host_namespace_v1_directory_exists(context->host_namespace, drive, relative)) {
        if (win32_error_out != 0) *win32_error_out = ERROR_PATH_NOT_FOUND;
        return BX_NTVDM_SESSION_HOST_CONTEXT_V1_NOT_FOUND;
    }
    if (!full_path(drive, relative, path) || !SetCurrentDirectoryW(path)) {
        if (win32_error_out != 0) *win32_error_out = GetLastError();
        return BX_NTVDM_SESSION_HOST_CONTEXT_V1_HOST_FAILURE;
    }
    memcpy(context->direct_relative[drive], relative,
        (wcslen(relative) + 1u) * sizeof(*relative));
    if (win32_error_out != 0) *win32_error_out = ERROR_SUCCESS;
    return BX_NTVDM_SESSION_HOST_CONTEXT_V1_OK;
}

int bx_ntvdm_session_host_context_v1_valid(const bx_ntvdm_session_host_context_v1 *context)
{
    return context != 0 && context->magic == BX_NTVDM_SESSION_HOST_CONTEXT_V1_MAGIC &&
        context->abi_version == BX_NTVDM_SESSION_HOST_CONTEXT_V1_VERSION &&
        context->struct_bytes == sizeof(*context) && context->default_drive < 26u &&
        context->environment_bytes <= BX_NTVDM_COMMAND_HOST_CONTEXT_V1_ENVIRONMENT_BYTES &&
        bx_ntvdm_dem_profile_consumer_v1_valid(&context->profile) &&
        (context->host_namespace == 0 || bx_ntvdm_host_namespace_v1_valid(context->host_namespace)) &&
        (context->profile.profile.mode != BX_NTVDM_MUTATION_MODE_V1_OVERLAY ?
            context->overlay.magic == 0u && context->overlay.abi_version == 0u &&
            context->overlay.struct_bytes == 0u && context->overlay.record_count == 0u &&
            context->overlay.reserved0 == 0u : bx_ntvdm_mutation_overlay_v1_valid(&context->overlay));
}

int bx_ntvdm_session_host_context_v1_initialize(bx_ntvdm_session_host_context_v1 *context,
    const bx_ntvdm_mutation_profile_v1 *profile_value,
    const bx_ntvdm_host_namespace_v1 *host_namespace, uint8_t initial_drive)
{
    if (context == 0 || profile_value == 0 || initial_drive >= 26u) return 0;
    memset(context, 0, sizeof(*context));
    context->magic = BX_NTVDM_SESSION_HOST_CONTEXT_V1_MAGIC;
    context->abi_version = BX_NTVDM_SESSION_HOST_CONTEXT_V1_VERSION;
    context->struct_bytes = sizeof(*context);
    context->default_drive = initial_drive;
    context->host_namespace = host_namespace;
    if (!bx_ntvdm_dem_profile_consumer_v1_initialize(&context->profile, profile_value) ||
        (profile_value->mode == BX_NTVDM_MUTATION_MODE_V1_OVERLAY &&
         !bx_ntvdm_mutation_overlay_v1_initialize(&context->overlay, profile_value))) {
        memset(context, 0, sizeof(*context)); return 0;
    }
    return bx_ntvdm_session_host_context_v1_valid(context);
}

void bx_ntvdm_session_host_context_v1_teardown(bx_ntvdm_session_host_context_v1 *context)
{
    if (context == 0) return;
    bx_ntvdm_mutation_overlay_v1_teardown(&context->overlay);
    memset(context, 0, sizeof(*context));
}

int bx_ntvdm_session_host_context_v1_set_environment(bx_ntvdm_session_host_context_v1 *context,
    const uint8_t *environment, uint32_t environment_bytes)
{
    bx_ntvdm_command_host_context_v1 checked;
    uint8_t root[] = "C:\\";
    if (!bx_ntvdm_session_host_context_v1_valid(context)) return 0;
    root[0] = (uint8_t)('A' + context->default_drive);
    if (!bx_ntvdm_command_host_context_v1_initialize(&checked, context->default_drive, root, 3u) ||
        !bx_ntvdm_command_host_context_v1_set_environment(&checked, environment, environment_bytes)) return 0;
    memset(context->environment, 0, sizeof(context->environment));
    if (environment_bytes != 0u) memcpy(context->environment, environment, environment_bytes);
    context->environment_bytes = environment_bytes;
    return bx_ntvdm_session_host_context_v1_valid(context);
}

int bx_ntvdm_session_host_context_v1_seed_command(
    bx_ntvdm_session_host_context_v1 *context,
    const bx_ntvdm_command_host_context_v1 *projection)
{
    wchar_t relative[BX_NTVDM_SESSION_HOST_CONTEXT_V1_MAX_RELATIVE];
    uint32_t result, index, bytes;
    if (!policy(context, &result) || projection == 0 ||
        !bx_ntvdm_command_host_context_v1_valid(projection) ||
        projection->directory_bytes < 4u) return 0;
    for (index = 3u; index + 1u < projection->directory_bytes; ++index) {
        if (projection->selected_directory[index] > 0x7fu || index - 3u + 1u >=
            BX_NTVDM_SESSION_HOST_CONTEXT_V1_MAX_RELATIVE) return 0;
        relative[index - 3u] = (wchar_t)projection->selected_directory[index];
    }
    relative[index - 3u] = L'\0';
    if (!valid_relative(relative) || !bx_ntvdm_session_host_context_v1_set_environment(context,
            projection->environment, projection->environment_bytes)) return 0;
    context->default_drive = projection->selected_drive;
    bytes = (uint32_t)((wcslen(relative) + 1u) * sizeof(*relative));
    if (result == BX_NTVDM_MUTATION_POLICY_V1_USE_OVERLAY)
        return bx_ntvdm_mutation_overlay_v1_replace(&context->overlay,
            BX_NTVDM_MUTATION_OWNER_V1_DEM, BX_NTVDM_MUTATION_CLASS_V1_SESSION_CONTEXT,
            projection->selected_drive, (const uint8_t *)relative, bytes);
    if (result == BX_NTVDM_MUTATION_POLICY_V1_USE_VIRTUAL)
        memcpy(context->virtual_relative[projection->selected_drive], relative, bytes);
    else memcpy(context->direct_relative[projection->selected_drive], relative, bytes);
    return bx_ntvdm_session_host_context_v1_valid(context);
}

int bx_ntvdm_session_host_context_v1_set_host_namespace(
    bx_ntvdm_session_host_context_v1 *context,
    const bx_ntvdm_host_namespace_v1 *host_namespace)
{
    if (!bx_ntvdm_session_host_context_v1_valid(context) ||
        (host_namespace != 0 && !bx_ntvdm_host_namespace_v1_valid(host_namespace))) return 0;
    context->host_namespace = host_namespace;
    return bx_ntvdm_session_host_context_v1_valid(context);
}

int bx_ntvdm_session_host_context_v1_set(bx_ntvdm_session_host_context_v1 *context,
    uint8_t drive, const wchar_t *relative, uint32_t *win32_error_out)
{
    uint32_t result, bytes;
    if (win32_error_out != 0) *win32_error_out = ERROR_INVALID_PARAMETER;
    if (!policy(context, &result) || drive >= 26u || !valid_relative(relative))
        return BX_NTVDM_SESSION_HOST_CONTEXT_V1_REJECTED;
    if (result == BX_NTVDM_MUTATION_POLICY_V1_DIRECT_HOST)
        return direct_set(context, drive, relative, win32_error_out);
    if (result == BX_NTVDM_MUTATION_POLICY_V1_REJECT_READONLY)
        return BX_NTVDM_SESSION_HOST_CONTEXT_V1_READONLY;
    bytes = (uint32_t)((wcslen(relative) + 1u) * sizeof(*relative));
    if (result == BX_NTVDM_MUTATION_POLICY_V1_USE_OVERLAY)
        return bx_ntvdm_mutation_overlay_v1_replace(&context->overlay,
            BX_NTVDM_MUTATION_OWNER_V1_DEM, BX_NTVDM_MUTATION_CLASS_V1_SESSION_CONTEXT,
            drive, (const uint8_t *)relative, bytes) ? BX_NTVDM_SESSION_HOST_CONTEXT_V1_OK :
            BX_NTVDM_SESSION_HOST_CONTEXT_V1_CAPACITY;
    if (result == BX_NTVDM_MUTATION_POLICY_V1_USE_VIRTUAL) {
        memcpy(context->virtual_relative[drive], relative, bytes);
        return BX_NTVDM_SESSION_HOST_CONTEXT_V1_OK;
    }
    return BX_NTVDM_SESSION_HOST_CONTEXT_V1_REJECTED;
}

int bx_ntvdm_session_host_context_v1_query(const bx_ntvdm_session_host_context_v1 *context,
    uint8_t drive, wchar_t *relative, uint32_t capacity)
{
    uint32_t result, bytes = 0u;
    const wchar_t *source;
    if (!policy(context, &result) || drive >= 26u || relative == 0 || capacity == 0u)
        return BX_NTVDM_SESSION_HOST_CONTEXT_V1_REJECTED;
    source = context->direct_relative[drive];
    if (result == BX_NTVDM_MUTATION_POLICY_V1_USE_OVERLAY) {
        if (!bx_ntvdm_mutation_overlay_v1_lookup(&context->overlay,
                BX_NTVDM_MUTATION_OWNER_V1_DEM, BX_NTVDM_MUTATION_CLASS_V1_SESSION_CONTEXT,
                drive, (uint8_t *)relative, capacity * sizeof(*relative), &bytes)) {
            relative[0] = L'\0'; return BX_NTVDM_SESSION_HOST_CONTEXT_V1_OK;
        }
        return bytes >= sizeof(*relative) && bytes % sizeof(*relative) == 0u &&
            relative[bytes / sizeof(*relative) - 1u] == L'\0' ?
            BX_NTVDM_SESSION_HOST_CONTEXT_V1_OK : BX_NTVDM_SESSION_HOST_CONTEXT_V1_REJECTED;
    }
    if (result == BX_NTVDM_MUTATION_POLICY_V1_USE_VIRTUAL) source = context->virtual_relative[drive];
    bytes = (uint32_t)((wcslen(source) + 1u) * sizeof(*source));
    if (bytes / sizeof(*source) > capacity) return BX_NTVDM_SESSION_HOST_CONTEXT_V1_CAPACITY;
    memcpy(relative, source, bytes);
    return BX_NTVDM_SESSION_HOST_CONTEXT_V1_OK;
}

int bx_ntvdm_session_host_context_v1_reconcile_direct(bx_ntvdm_session_host_context_v1 *context,
    uint8_t drive, uint32_t *win32_error_out)
{
    uint32_t result;
    if (!policy(context, &result) || drive >= 26u) return BX_NTVDM_SESSION_HOST_CONTEXT_V1_REJECTED;
    if (result != BX_NTVDM_MUTATION_POLICY_V1_DIRECT_HOST) return BX_NTVDM_SESSION_HOST_CONTEXT_V1_OK;
    if (!namespace_available(context, drive)) return BX_NTVDM_SESSION_HOST_CONTEXT_V1_NOT_FOUND;
    if (!bx_ntvdm_host_namespace_v1_directory_exists(context->host_namespace, drive,
            context->direct_relative[drive])) context->direct_relative[drive][0] = L'\0';
    if (win32_error_out != 0) *win32_error_out = ERROR_SUCCESS;
    return BX_NTVDM_SESSION_HOST_CONTEXT_V1_OK;
}

int bx_ntvdm_session_host_context_v1_set_default_drive(bx_ntvdm_session_host_context_v1 *context,
    uint8_t drive)
{
    uint32_t result;
    if (!policy(context, &result) || drive >= 26u) return BX_NTVDM_SESSION_HOST_CONTEXT_V1_REJECTED;
    if (result == BX_NTVDM_MUTATION_POLICY_V1_DIRECT_HOST && !namespace_available(context, drive))
        return BX_NTVDM_SESSION_HOST_CONTEXT_V1_NOT_FOUND;
    if (result == BX_NTVDM_MUTATION_POLICY_V1_REJECT_READONLY)
        return BX_NTVDM_SESSION_HOST_CONTEXT_V1_READONLY;
    context->default_drive = drive;
    return BX_NTVDM_SESSION_HOST_CONTEXT_V1_OK;
}

int bx_ntvdm_session_host_context_v1_query_default_drive(const bx_ntvdm_session_host_context_v1 *context,
    uint8_t *drive_out)
{
    if (!bx_ntvdm_session_host_context_v1_valid(context) || drive_out == 0) return 0;
    *drive_out = (uint8_t)context->default_drive; return 1;
}

int bx_ntvdm_session_host_context_v1_project_command(const bx_ntvdm_session_host_context_v1 *context,
    bx_ntvdm_command_host_context_v1 *projection)
{
    wchar_t relative[BX_NTVDM_SESSION_HOST_CONTEXT_V1_MAX_RELATIVE];
    uint8_t path[BX_NTVDM_COMMAND_HOST_CONTEXT_V1_DIRECTORY_BYTES];
    uint32_t index = 0u;
    if (projection == 0 || !bx_ntvdm_session_host_context_v1_valid(context) ||
        bx_ntvdm_session_host_context_v1_query(context, (uint8_t)context->default_drive,
            relative, BX_NTVDM_SESSION_HOST_CONTEXT_V1_MAX_RELATIVE) != BX_NTVDM_SESSION_HOST_CONTEXT_V1_OK)
        return 0;
    path[index++] = (uint8_t)('A' + context->default_drive); path[index++] = ':'; path[index++] = '\\';
    while (relative[index - 3u] != L'\0') {
        wchar_t value = relative[index - 3u];
        if (value > 0x7fu || index + 1u >= sizeof(path)) return 0;
        path[index++] = (uint8_t)value;
    }
    return bx_ntvdm_command_host_context_v1_initialize(projection, context->default_drive,
        path, index) && bx_ntvdm_command_host_context_v1_set_environment(projection,
        context->environment, context->environment_bytes);
}
