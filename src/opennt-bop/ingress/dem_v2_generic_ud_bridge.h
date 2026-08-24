#ifndef RUNTIME_BOP_DEM_V2_GENERIC_UD_BRIDGE_H
#define RUNTIME_BOP_DEM_V2_GENERIC_UD_BRIDGE_H

/* v2 DEM-only generic-UD route.  It is intentionally independent of every
 * earlier provider: a recognized DEM window is accepted only by the imported
 * OpenNT DEM session, never by a legacy fallback. */

#include "adapter-bop/generic_ud_bridge.h"

/* Recognition is exported separately so the composition entry can make DEM
 * ownership terminal before any other BOP family is considered. */
int runtime_dem_v2_generic_ud_recognizes(
    const struct runtime_generic_ud_event_v1 *event);

int runtime_dem_v2_generic_ud_dispatch(
    const struct runtime_generic_ud_event_v1 *event,
    struct runtime_generic_ud_outcome_v1 *outcome);

#endif
