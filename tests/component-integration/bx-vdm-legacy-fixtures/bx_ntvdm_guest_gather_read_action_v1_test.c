#include <assert.h>

#include "guest_gather_read_action.h"

int main(void)
{
    runtime_guest_gather_read_action_v1 action;
    runtime_guest_range ranges[2] = {{0x700u, 128u}, {0x800u, 2u}};
    runtime_guest_range bad[2] = {{0x700u, 255u}, {0x800u, 2u}};
    runtime_guest_gather_read_action_v1_pass_through(&action);
    assert(runtime_guest_gather_read_action_v1_valid(&action));
    assert(runtime_guest_gather_read_action_v1_stop(&action));
    assert(action.disposition == RUNTIME_GUEST_GATHER_READ_ACTION_V1_STOP);
    assert(action.range_count == 0u && action.total_bytes == 0u);
    assert(runtime_guest_gather_read_action_v1_valid(&action));
    assert(runtime_guest_gather_read_action_v1_resume(&action, 0x1234u));
    assert(runtime_guest_gather_read_action_v1_valid(&action));
    assert(runtime_guest_gather_read_action_v1_need_read(&action, ranges, 2u));
    assert(runtime_guest_gather_read_action_v1_valid(&action));
    assert(action.total_bytes == 130u && action.range_count == 2u);
    assert(runtime_guest_gather_read_action_v1_need_read_resume(&action,
        ranges, 2u, 0x1234u));
    assert(runtime_guest_gather_read_action_v1_valid(&action));
    assert(!runtime_guest_gather_read_action_v1_need_read(&action, bad, 2u));
    assert(!runtime_guest_gather_read_action_v1_need_read(&action, ranges, 0u));
    action.ranges[1].length = 0u;
    assert(!runtime_guest_gather_read_action_v1_valid(&action));
    return 0;
}
