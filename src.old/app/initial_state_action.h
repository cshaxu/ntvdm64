#ifndef RUNTIME_INITIAL_STATE_ACTION_H
#define RUNTIME_INITIAL_STATE_ACTION_H

#include "initial_state_abi.h"
#include "adapter-softpc/mechanical_action.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Converts an admitted profile-owned record to the selector-blind copied RAM
 * action consumed by the machine.  The caller owns both records. */
int runtime_initial_state_action_prepare(
    const runtime_initial_state *state,
    struct runtime_mechanical_action *action);

#ifdef __cplusplus
}
#endif

#endif
