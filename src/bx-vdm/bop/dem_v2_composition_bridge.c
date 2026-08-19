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
#include "bop/observation/bx_ntvdm_bop_sequence_observation_v1.h"
#include "bop/observation/bx_ntvdm_generic_ud_sequence_observation_v1.h"

int bx_ntvdm_mantle_generic_ud_bridge_v1(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    struct bx_ntvdm_generic_ud_outcome_v1 *outcome)
{
    struct bx_ntvdm_generic_ud_outcome_v1 declined = {0};
    int accepted;
    if (!bx_ntvdm_dem_v2_generic_ud_recognizes(event)) {
        declined.abi_version = BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
        declined.disposition = BX_NTVDM_GENERIC_UD_PASS_THROUGH;
        bx_ntvdm_bop_sequence_observation_v1_consider(event, &declined);
        bx_ntvdm_generic_ud_sequence_observation_v1_consider(event, &declined);
        return 0;
    }
    accepted = bx_ntvdm_dem_v2_generic_ud_dispatch(event, outcome);
    if (accepted) {
        bx_ntvdm_bop_sequence_observation_v1_consider(event, outcome);
        bx_ntvdm_generic_ud_sequence_observation_v1_consider(event, outcome);
    } else {
        declined.abi_version = BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
        declined.disposition = BX_NTVDM_GENERIC_UD_PASS_THROUGH;
        bx_ntvdm_bop_sequence_observation_v1_consider(event, &declined);
        bx_ntvdm_generic_ud_sequence_observation_v1_consider(event, &declined);
    }
    return accepted;
}
