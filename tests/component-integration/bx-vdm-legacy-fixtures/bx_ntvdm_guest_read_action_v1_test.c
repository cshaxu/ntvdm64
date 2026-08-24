#include <assert.h>
#include <stdio.h>
#include "guest_read_action.h"

int main(void)
{
    runtime_guest_read_action_v1 action;
    runtime_guest_read_action_v1_pass_through(&action);
    assert(runtime_guest_read_action_v1_valid(&action));
    assert(runtime_guest_read_action_v1_stop(&action));
    assert(action.disposition == RUNTIME_GUEST_READ_ACTION_V1_STOP);
    assert(action.guest_read.address == 0u && action.guest_read.length == 0u);
    assert(runtime_guest_read_action_v1_valid(&action));
    assert(runtime_guest_read_action_v1_resume(&action, 0x1234u));
    assert(runtime_guest_read_action_v1_valid(&action));
    assert(runtime_guest_read_action_v1_need_read(&action, 0x9a000u, 4u));
    assert(runtime_guest_read_action_v1_valid(&action));
    assert(!runtime_guest_read_action_v1_need_read(&action, 0u, 0u));
    assert(!runtime_guest_read_action_v1_need_read(&action, UINT64_MAX - 1u, 4u));
    assert(runtime_guest_read_action_v1_need_read(&action, 0x9a000u, 128u));
    assert(runtime_guest_read_action_v1_valid(&action));
    action.guest_read.length = 129u;
    assert(!runtime_guest_read_action_v1_valid(&action));
    puts("bx-ntvdm guest-read action v1: single bounded read contract verified");
    return 0;
}
