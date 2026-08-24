#include "initial_state_action.h"

int main(void)
{
    runtime_initial_state_v1 state;
    struct runtime_mechanical_action_v1 action;

    runtime_initial_state_v1_clear(&state);
    if (runtime_initial_state_action_v1_prepare(&state, &action)) return 1;

    state.disposition = RUNTIME_INITIAL_STATE_V1_PRESENT;
    state.range_count = 1u;
    state.payload_bytes = 1u;
    state.evidence_sha256[0] = 1u;
    state.payload[0] = 7u;
    runtime_guest_write_v1_initialize(&state.ranges[0], 0x40u, 1u, 0u);
    if (!runtime_initial_state_action_v1_prepare(&state, &action) ||
        action.kind != RUNTIME_MECHANICAL_ACTION_V1_WRITE ||
        action.range_count != 1u || action.payload[0] != 7u)
        return 2;
    return 0;
}
