#ifndef BX_NTVDM_DEM_DIRECT_CONTEXT_LEGACY_V1_H
#define BX_NTVDM_DEM_DIRECT_CONTEXT_LEGACY_V1_H

/*
 * Retained v1 composition adapter.
 *
 * This belongs to bop-v1 because it knows the legacy whole-provider layout.
 * The v2 OpenNT mirror and bop/shim remain selector/profile/legacy blind.
 */

#include "../bop/shim/bx_ntvdm_dem_direct_context.h"
#include "bx_ntvdm_dem_whole_provider_v1.h"

int bx_ntvdm_dem_direct_context_legacy_v1_make(
    bx_ntvdm_dem_whole_provider_v1 *provider,
    bx_ntvdm_dem_direct_context *context);

#endif
