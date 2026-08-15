#include "bx_ntvdm_dem_cwd_context_v1.h"

int bx_ntvdm_dem_cwd_context_v1_initialize(bx_ntvdm_dem_cwd_context_v1 *context,
    const bx_ntvdm_mutation_profile_v1 *profile)
{
    /* The historical default context is C: when no CLI projection has yet
     * selected another admitted drive.  A later rooted namespace attachment
     * is explicit and required before a direct host operation can succeed. */
    return bx_ntvdm_session_host_context_v1_initialize(context, profile, 0, 2u);
}

int bx_ntvdm_dem_cwd_context_v1_valid(const bx_ntvdm_dem_cwd_context_v1 *context)
{ return bx_ntvdm_session_host_context_v1_valid(context); }

int bx_ntvdm_dem_cwd_context_v1_set(bx_ntvdm_dem_cwd_context_v1 *context,
    const bx_ntvdm_host_namespace_v1 *host_namespace, uint8_t drive_index,
    const wchar_t *relative_directory)
{
    if (host_namespace != 0 && !bx_ntvdm_session_host_context_v1_set_host_namespace(context, host_namespace))
        return BX_NTVDM_DEM_CWD_V1_REJECTED;
    return bx_ntvdm_session_host_context_v1_set(context, drive_index, relative_directory, 0);
}

int bx_ntvdm_dem_cwd_context_v1_query(const bx_ntvdm_dem_cwd_context_v1 *context,
    uint8_t drive_index, wchar_t *relative_directory, uint32_t capacity)
{ return bx_ntvdm_session_host_context_v1_query(context, drive_index, relative_directory, capacity); }

int bx_ntvdm_dem_cwd_context_v1_reconcile_direct(bx_ntvdm_dem_cwd_context_v1 *context,
    const bx_ntvdm_host_namespace_v1 *host_namespace, uint8_t drive_index)
{
    if (host_namespace != 0 && !bx_ntvdm_session_host_context_v1_set_host_namespace(context, host_namespace))
        return BX_NTVDM_DEM_CWD_V1_REJECTED;
    return bx_ntvdm_session_host_context_v1_reconcile_direct(context, drive_index, 0);
}

int bx_ntvdm_dem_cwd_context_v1_set_default_drive(bx_ntvdm_dem_cwd_context_v1 *context,
    const bx_ntvdm_host_namespace_v1 *host_namespace, uint8_t drive_index)
{
    if (host_namespace != 0 && !bx_ntvdm_session_host_context_v1_set_host_namespace(context, host_namespace))
        return BX_NTVDM_DEM_CWD_V1_REJECTED;
    return bx_ntvdm_session_host_context_v1_set_default_drive(context, drive_index);
}

int bx_ntvdm_dem_cwd_context_v1_query_default_drive(const bx_ntvdm_dem_cwd_context_v1 *context,
    uint8_t *drive_index_out)
{ return bx_ntvdm_session_host_context_v1_query_default_drive(context, drive_index_out); }
