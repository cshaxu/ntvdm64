#ifndef RUNTIME_INITIAL_STATE_ACTION_V1_H
#define RUNTIME_INITIAL_STATE_ACTION_V1_H

#include "initial_state_abi.h"
#include "adapter-softpc/mechanical_action.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Converts an admitted profile-owned record to the selector-blind copied RAM
 * action consumed by the mantle.  The caller owns both records. */
int runtime_initial_state_action_v1_prepare(
    const runtime_initial_state_v1 *state,
    struct runtime_mechanical_action_v1 *action);

#ifdef __cplusplus
}
#endif

#endif
