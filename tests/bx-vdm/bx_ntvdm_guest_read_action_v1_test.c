#include <assert.h>
#include <stdio.h>
#include "bx_ntvdm_guest_read_action_v1.h"

int main(void)
{
    bx_ntvdm_guest_read_action_v1 action;
    bx_ntvdm_guest_read_action_v1_pass_through(&action);
    assert(bx_ntvdm_guest_read_action_v1_valid(&action));
    assert(bx_ntvdm_guest_read_action_v1_stop(&action));
    assert(action.disposition == BX_NTVDM_GUEST_READ_ACTION_V1_STOP);
    assert(action.guest_read.address == 0u && action.guest_read.length == 0u);
    assert(bx_ntvdm_guest_read_action_v1_valid(&action));
    assert(bx_ntvdm_guest_read_action_v1_resume(&action, 0x1234u));
    assert(bx_ntvdm_guest_read_action_v1_valid(&action));
    assert(bx_ntvdm_guest_read_action_v1_need_read(&action, 0x9a000u, 4u));
    assert(bx_ntvdm_guest_read_action_v1_valid(&action));
    assert(!bx_ntvdm_guest_read_action_v1_need_read(&action, 0u, 0u));
    assert(!bx_ntvdm_guest_read_action_v1_need_read(&action, UINT64_MAX - 1u, 4u));
    assert(bx_ntvdm_guest_read_action_v1_need_read(&action, 0x9a000u, 128u));
    assert(bx_ntvdm_guest_read_action_v1_valid(&action));
    action.guest_read.length = 129u;
    assert(!bx_ntvdm_guest_read_action_v1_valid(&action));
    puts("bx-ntvdm guest-read action v1: single bounded read contract verified");
    return 0;
}
