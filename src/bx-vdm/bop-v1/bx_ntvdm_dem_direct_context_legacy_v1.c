#include "bx_ntvdm_dem_direct_context_legacy_v1.h"

#include <string.h>

#include "bx_ntvdm_dem_file_session_v1.h"
#include "../bx_ntvdm_host_namespace.h"

static int lookup_handle(void *state, uint32_t token, HANDLE *handle_out)
{
    bx_ntvdm_dem_whole_provider_v1 *provider =
        (bx_ntvdm_dem_whole_provider_v1 *)state;
    return provider != 0 && bx_ntvdm_dem_whole_provider_v1_valid(provider) &&
        bx_ntvdm_dem_file_session_v1_lookup(&provider->files, token, handle_out);
}

static int release_handle(void *state, uint32_t token, DWORD *error_out)
{
    bx_ntvdm_dem_whole_provider_v1 *provider =
        (bx_ntvdm_dem_whole_provider_v1 *)state;
    if (error_out != 0) *error_out = ERROR_INVALID_HANDLE;
    if (provider == 0 || !bx_ntvdm_dem_whole_provider_v1_valid(provider) ||
        !bx_ntvdm_dem_file_session_v1_release(&provider->files, token)) {
        if (error_out != 0 && GetLastError() != ERROR_SUCCESS)
            *error_out = GetLastError();
        return 0;
    }
    if (error_out != 0) *error_out = ERROR_SUCCESS;
    return 1;
}

static int query_attributes(void *state, uint8_t drive,
    const wchar_t *relative_path, DWORD *attributes_out, DWORD *error_out)
{
    bx_ntvdm_dem_whole_provider_v1 *provider =
        (bx_ntvdm_dem_whole_provider_v1 *)state;
    return provider != 0 && bx_ntvdm_dem_whole_provider_v1_valid(provider) &&
        bx_ntvdm_host_namespace_v1_query_file_attributes(provider->host_namespace,
            drive, relative_path, attributes_out, error_out);
}

static int set_attributes(void *state, uint8_t drive,
    const wchar_t *relative_path, DWORD attributes, DWORD *error_out)
{
    bx_ntvdm_dem_whole_provider_v1 *provider =
        (bx_ntvdm_dem_whole_provider_v1 *)state;
    return provider != 0 && bx_ntvdm_dem_whole_provider_v1_valid(provider) &&
        bx_ntvdm_host_namespace_v1_set_file_attributes(provider->host_namespace,
            drive, relative_path, attributes, error_out);
}

int bx_ntvdm_dem_direct_context_legacy_v1_make(
    bx_ntvdm_dem_whole_provider_v1 *provider,
    bx_ntvdm_dem_direct_context *context)
{
    if (provider == 0 || context == 0 ||
        !bx_ntvdm_dem_whole_provider_v1_valid(provider)) return 0;
    memset(context, 0, sizeof(*context));
    context->magic = BX_NTVDM_DEM_DIRECT_CONTEXT_MAGIC;
    context->abi_version = BX_NTVDM_DEM_DIRECT_CONTEXT_VERSION;
    context->struct_bytes = sizeof(*context);
    context->state = provider;
    context->lookup_handle = lookup_handle;
    context->release_handle = release_handle;
    context->query_attributes = query_attributes;
    context->set_attributes = set_attributes;
    return bx_ntvdm_dem_direct_context_valid(context);
}
