#include "initial_state_action.h"

#include <string.h>

int runtime_initial_state_action_prepare(
    const runtime_initial_state *state,
    struct runtime_mechanical_action *action)
{
    uint32_t index;

    if (state == 0 || action == 0 ||
        !runtime_initial_state_admitted(state, UINT64_C(0x100000)))
        return 0;

    runtime_mechanical_action_clear(action);
    action->action_id = 1u;
    action->kind = RUNTIME_MECHANICAL_ACTION_WRITE;
    action->range_count = state->range_count;
    action->payload_bytes = state->payload_bytes;
    for (index = 0u; index < state->range_count; ++index) {
        action->ranges[index].physical_address =
            state->ranges[index].guest_physical_address;
        action->ranges[index].byte_count =
            (uint32_t) state->ranges[index].byte_count;
        action->ranges[index].payload_offset =
            (uint32_t) state->ranges[index].payload_offset;
    }
    memcpy(action->payload, state->payload, state->payload_bytes);
    return runtime_mechanical_action_valid(action);
}
