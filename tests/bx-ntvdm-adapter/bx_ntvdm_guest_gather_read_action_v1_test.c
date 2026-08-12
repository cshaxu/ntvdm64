#include <assert.h>

#include "bx_ntvdm_guest_gather_read_action_v1.h"

int main(void)
{
    bx_ntvdm_guest_gather_read_action_v1 action;
    bx_ntvdm_guest_range ranges[2] = {{0x700u, 128u}, {0x800u, 2u}};
    bx_ntvdm_guest_range bad[2] = {{0x700u, 255u}, {0x800u, 2u}};
    bx_ntvdm_guest_gather_read_action_v1_pass_through(&action);
    assert(bx_ntvdm_guest_gather_read_action_v1_valid(&action));
    assert(bx_ntvdm_guest_gather_read_action_v1_stop(&action));
    assert(action.disposition == BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_STOP);
    assert(action.range_count == 0u && action.total_bytes == 0u);
    assert(bx_ntvdm_guest_gather_read_action_v1_valid(&action));
    assert(bx_ntvdm_guest_gather_read_action_v1_resume(&action, 0x1234u));
    assert(bx_ntvdm_guest_gather_read_action_v1_valid(&action));
    assert(bx_ntvdm_guest_gather_read_action_v1_need_read(&action, ranges, 2u));
    assert(bx_ntvdm_guest_gather_read_action_v1_valid(&action));
    assert(action.total_bytes == 130u && action.range_count == 2u);
    assert(bx_ntvdm_guest_gather_read_action_v1_need_read_resume(&action,
        ranges, 2u, 0x1234u));
    assert(bx_ntvdm_guest_gather_read_action_v1_valid(&action));
    assert(!bx_ntvdm_guest_gather_read_action_v1_need_read(&action, bad, 2u));
    assert(!bx_ntvdm_guest_gather_read_action_v1_need_read(&action, ranges, 0u));
    action.ranges[1].length = 0u;
    assert(!bx_ntvdm_guest_gather_read_action_v1_valid(&action));
    return 0;
}
