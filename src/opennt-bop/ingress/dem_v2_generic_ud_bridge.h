#ifndef BX_NTVDM_BOP_DEM_V2_GENERIC_UD_BRIDGE_H
#define BX_NTVDM_BOP_DEM_V2_GENERIC_UD_BRIDGE_H

/* v2 DEM-only generic-UD route.  It is intentionally independent of every
 * earlier provider: a recognized DEM window is accepted only by the imported
 * OpenNT DEM session, never by a legacy fallback. */

#include "adapter-bop/bx_ntvdm_generic_ud_bridge.h"

/* Recognition is exported separately so the composition entry can make DEM
 * ownership terminal before any other BOP family is considered. */
int bx_ntvdm_dem_v2_generic_ud_recognizes(
    const struct bx_ntvdm_generic_ud_event_v1 *event);

int bx_ntvdm_dem_v2_generic_ud_dispatch(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    struct bx_ntvdm_generic_ud_outcome_v1 *outcome);

#endif
