#include "bx_ntvdm_dem_file_view_v1.h"

#include <string.h>

static uint32_t kind_for_mode(uint32_t mode)
{
    switch (mode) {
    case BX_NTVDM_MUTATION_MODE_V1_DIRECT:
        return (uint32_t)BX_NTVDM_DEM_FILE_VIEW_V1_DIRECT;
    case BX_NTVDM_MUTATION_MODE_V1_READONLY:
        return (uint32_t)BX_NTVDM_DEM_FILE_VIEW_V1_READONLY;
    case BX_NTVDM_MUTATION_MODE_V1_OVERLAY:
        return (uint32_t)BX_NTVDM_DEM_FILE_VIEW_V1_OVERLAY;
    case BX_NTVDM_MUTATION_MODE_V1_VIRTUAL:
        return (uint32_t)BX_NTVDM_DEM_FILE_VIEW_V1_VIRTUAL;
    default:
        return 0u;
    }
}

int bx_ntvdm_dem_file_view_v1_valid(const bx_ntvdm_dem_file_view_v1 *view)
{
    return view != 0 && view->magic == BX_NTVDM_DEM_FILE_VIEW_V1_MAGIC &&
        view->abi_version == BX_NTVDM_DEM_FILE_VIEW_V1_VERSION &&
        view->struct_bytes == sizeof(*view) &&
        view->kind == kind_for_mode(view->profile.profile.mode) &&
        bx_ntvdm_dem_profile_consumer_v1_valid(&view->profile);
}

int bx_ntvdm_dem_file_view_v1_initialize(bx_ntvdm_dem_file_view_v1 *view,
    const bx_ntvdm_mutation_profile_v1 *profile)
{
    if (view == 0 || profile == 0 || !bx_ntvdm_mutation_profile_v1_valid(profile) ||
        kind_for_mode(profile->mode) == 0u) return 0;
    memset(view, 0, sizeof(*view));
    view->magic = BX_NTVDM_DEM_FILE_VIEW_V1_MAGIC;
    view->abi_version = BX_NTVDM_DEM_FILE_VIEW_V1_VERSION;
    view->struct_bytes = sizeof(*view);
    view->kind = kind_for_mode(profile->mode);
    if (!bx_ntvdm_dem_profile_consumer_v1_initialize(&view->profile, profile)) {
        memset(view, 0, sizeof(*view));
        return 0;
    }
    return bx_ntvdm_dem_file_view_v1_valid(view);
}

int bx_ntvdm_dem_file_view_v1_admit(const bx_ntvdm_dem_file_view_v1 *view,
    uint32_t mutation_class)
{
    uint32_t policy;
    if (!bx_ntvdm_dem_file_view_v1_valid(view))
        return BX_NTVDM_DEM_FILE_VIEW_V1_REJECTED;
    if (mutation_class == BX_NTVDM_MUTATION_CLASS_V1_NONE_MECHANICAL) {
        if (view->kind == BX_NTVDM_DEM_FILE_VIEW_V1_DIRECT ||
            view->kind == BX_NTVDM_DEM_FILE_VIEW_V1_READONLY)
            return BX_NTVDM_DEM_FILE_VIEW_V1_OK;
        return view->kind == BX_NTVDM_DEM_FILE_VIEW_V1_OVERLAY ?
            BX_NTVDM_DEM_FILE_VIEW_V1_NEEDS_OVERLAY_BACKEND :
            BX_NTVDM_DEM_FILE_VIEW_V1_NEEDS_VIRTUAL_BACKEND;
    }
    if (!bx_ntvdm_dem_profile_consumer_v1_resolve(&view->profile,
            mutation_class, &policy))
        return BX_NTVDM_DEM_FILE_VIEW_V1_REJECTED;
    switch (policy) {
    case BX_NTVDM_MUTATION_POLICY_V1_DIRECT_HOST:
        return BX_NTVDM_DEM_FILE_VIEW_V1_OK;
    case BX_NTVDM_MUTATION_POLICY_V1_REJECT_READONLY:
        return BX_NTVDM_DEM_FILE_VIEW_V1_DENIED_READONLY;
    case BX_NTVDM_MUTATION_POLICY_V1_USE_OVERLAY:
        return BX_NTVDM_DEM_FILE_VIEW_V1_NEEDS_OVERLAY_BACKEND;
    case BX_NTVDM_MUTATION_POLICY_V1_USE_VIRTUAL:
        return BX_NTVDM_DEM_FILE_VIEW_V1_NEEDS_VIRTUAL_BACKEND;
    default:
        return BX_NTVDM_DEM_FILE_VIEW_V1_REJECTED;
    }
}
