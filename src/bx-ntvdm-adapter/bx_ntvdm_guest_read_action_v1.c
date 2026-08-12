#include "bx_ntvdm_guest_read_action_v1.h"

void bx_ntvdm_guest_read_action_v1_pass_through(
    bx_ntvdm_guest_read_action_v1 *action)
{
    if (action == 0) return;
    action->magic = BX_NTVDM_GUEST_READ_ACTION_V1_MAGIC;
    action->abi_version = BX_NTVDM_GUEST_READ_ACTION_V1_VERSION;
    action->struct_bytes = sizeof(*action);
    action->disposition = BX_NTVDM_GUEST_READ_ACTION_V1_PASS_THROUGH;
    bx_ntvdm_cpu_result_v2_pass_through(&action->cpu_result);
    action->guest_read.address = 0u;
    action->guest_read.length = 0u;
}

int bx_ntvdm_guest_read_action_v1_resume(
    bx_ntvdm_guest_read_action_v1 *action, uint64_t resume_rip)
{
    if (action == 0) return 0;
    bx_ntvdm_guest_read_action_v1_pass_through(action);
    action->disposition = BX_NTVDM_GUEST_READ_ACTION_V1_RESUME;
    return bx_ntvdm_cpu_result_v2_resume(&action->cpu_result, resume_rip);
}

int bx_ntvdm_guest_read_action_v1_stop(bx_ntvdm_guest_read_action_v1 *action)
{
    if (action == 0) return 0;
    bx_ntvdm_guest_read_action_v1_pass_through(action);
    action->disposition = BX_NTVDM_GUEST_READ_ACTION_V1_STOP;
    return bx_ntvdm_cpu_result_v2_stop(&action->cpu_result);
}

int bx_ntvdm_guest_read_action_v1_need_read(
    bx_ntvdm_guest_read_action_v1 *action, uint64_t address, uint64_t length)
{
    if (action == 0 || length == 0u || length > BX_NTVDM_GUEST_READ_ACTION_V1_MAX_BYTES ||
        address > UINT64_MAX - length) return 0;
    bx_ntvdm_guest_read_action_v1_pass_through(action);
    action->disposition = BX_NTVDM_GUEST_READ_ACTION_V1_NEED_READ;
    action->guest_read.address = address;
    action->guest_read.length = length;
    return 1;
}

int bx_ntvdm_guest_read_action_v1_valid(
    const bx_ntvdm_guest_read_action_v1 *action)
{
    if (action == 0 || action->magic != BX_NTVDM_GUEST_READ_ACTION_V1_MAGIC ||
        action->abi_version != BX_NTVDM_GUEST_READ_ACTION_V1_VERSION ||
        action->struct_bytes != sizeof(*action) ||
        !bx_ntvdm_cpu_result_v2_valid(&action->cpu_result)) return 0;
    if (action->disposition == BX_NTVDM_GUEST_READ_ACTION_V1_PASS_THROUGH)
        return action->cpu_result.disposition == BX_NTVDM_CPU_RESULT_V2_PASS_THROUGH &&
            action->guest_read.address == 0u && action->guest_read.length == 0u;
    if (action->disposition == BX_NTVDM_GUEST_READ_ACTION_V1_RESUME)
        return action->cpu_result.disposition == BX_NTVDM_CPU_RESULT_V2_RESUME &&
            action->guest_read.address == 0u && action->guest_read.length == 0u;
    if (action->disposition == BX_NTVDM_GUEST_READ_ACTION_V1_STOP)
        return action->cpu_result.disposition == BX_NTVDM_CPU_RESULT_V2_STOP &&
            action->guest_read.address == 0u && action->guest_read.length == 0u;
    return action->disposition == BX_NTVDM_GUEST_READ_ACTION_V1_NEED_READ &&
        action->cpu_result.disposition == BX_NTVDM_CPU_RESULT_V2_PASS_THROUGH &&
        action->guest_read.length > 0u &&
        action->guest_read.length <= BX_NTVDM_GUEST_READ_ACTION_V1_MAX_BYTES &&
        action->guest_read.address <= UINT64_MAX - action->guest_read.length;
}
