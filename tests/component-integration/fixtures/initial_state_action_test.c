#include "initial_state_action.h"

int main(void)
{
    runtime_initial_state state;
    struct runtime_mechanical_action action;

    runtime_initial_state_clear(&state);
    if (runtime_initial_state_action_prepare(&state, &action)) return 1;

    state.disposition = RUNTIME_INITIAL_STATE_PRESENT;
    state.range_count = 1u;
    state.payload_bytes = 1u;
    state.evidence_sha256[0] = 1u;
    state.payload[0] = 7u;
    runtime_guest_write_initialize(&state.ranges[0], 0x40u, 1u, 0u);
    if (!runtime_initial_state_action_prepare(&state, &action) ||
        action.kind != RUNTIME_MECHANICAL_ACTION_WRITE ||
        action.range_count != 1u || action.payload[0] != 7u)
        return 2;
    return 0;
}
