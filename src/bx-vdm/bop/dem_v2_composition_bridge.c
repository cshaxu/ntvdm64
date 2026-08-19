/*
 * Direct-only DEM composition entry.
 *
 * The mantle knows only this fixed mechanical ABI.  BOP selector recognition
 * remains in bx-vdm.  A valid C4 C4 50 xx window is terminally owned by the
 * imported OpenNT DEM session: a missing or failing session declines to the
 * CPU exception path and must never enter bop-v1.  Non-DEM selectors also
 * decline until their own OpenNT-shaped package is composed here.
 */

#include "dem_v2_generic_ud_bridge.h"

int bx_ntvdm_mantle_generic_ud_bridge_v1(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    struct bx_ntvdm_generic_ud_outcome_v1 *outcome)
{
    if (!bx_ntvdm_dem_v2_generic_ud_recognizes(event)) return 0;
    return bx_ntvdm_dem_v2_generic_ud_dispatch(event, outcome);
}
