#include "bx_ntvdm_initial_state_action_v1.h"

#include <string.h>

int bx_ntvdm_initial_state_action_v1_prepare(
    const bx_ntvdm_initial_state_v1 *state,
    struct bx_ntvdm_mechanical_action_v1 *action)
{
    uint32_t index;

    if (state == 0 || action == 0 ||
        !bx_ntvdm_initial_state_v1_admitted(state, UINT64_C(0x100000)))
        return 0;

    bx_ntvdm_mechanical_action_v1_clear(action);
    action->action_id = 1u;
    action->kind = BX_NTVDM_MECHANICAL_ACTION_V1_WRITE;
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
    return bx_ntvdm_mechanical_action_v1_valid(action);
}
