#ifndef BX_NTVDM_INITIAL_STATE_ACTION_V1_H
#define BX_NTVDM_INITIAL_STATE_ACTION_V1_H

#include "bx_ntvdm_initial_state_abi.h"
#include "adapter-softpc/bx_ntvdm_mechanical_action_v1.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Converts an admitted profile-owned record to the selector-blind copied RAM
 * action consumed by the mantle.  The caller owns both records. */
int bx_ntvdm_initial_state_action_v1_prepare(
    const bx_ntvdm_initial_state_v1 *state,
    struct bx_ntvdm_mechanical_action_v1 *action);

#ifdef __cplusplus
}
#endif

#endif
