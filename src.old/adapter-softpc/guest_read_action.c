#include "guest_read_action.h"

void runtime_guest_read_action_pass_through(
    runtime_guest_read_action *action)
{
    if (action == 0) return;
    action->magic = RUNTIME_GUEST_READ_ACTION_MAGIC;
    action->abi_version = RUNTIME_GUEST_READ_ACTION_VERSION;
    action->struct_bytes = sizeof(*action);
    action->disposition = RUNTIME_GUEST_READ_ACTION_PASS_THROUGH;
    runtime_cpu_result_pass_through(&action->cpu_result);
    action->guest_read.address = 0u;
    action->guest_read.length = 0u;
}

int runtime_guest_read_action_resume(
    runtime_guest_read_action *action, uint64_t resume_rip)
{
    if (action == 0) return 0;
    runtime_guest_read_action_pass_through(action);
    action->disposition = RUNTIME_GUEST_READ_ACTION_RESUME;
    return runtime_cpu_result_resume(&action->cpu_result, resume_rip);
}

int runtime_guest_read_action_stop(runtime_guest_read_action *action)
{
    if (action == 0) return 0;
    runtime_guest_read_action_pass_through(action);
    action->disposition = RUNTIME_GUEST_READ_ACTION_STOP;
    return runtime_cpu_result_stop(&action->cpu_result);
}

int runtime_guest_read_action_need_read(
    runtime_guest_read_action *action, uint64_t address, uint64_t length)
{
    if (action == 0 || length == 0u || length > RUNTIME_GUEST_READ_ACTION_MAX_BYTES ||
        address > UINT64_MAX - length) return 0;
    runtime_guest_read_action_pass_through(action);
    action->disposition = RUNTIME_GUEST_READ_ACTION_NEED_READ;
    action->guest_read.address = address;
    action->guest_read.length = length;
    return 1;
}

int runtime_guest_read_action_valid(
    const runtime_guest_read_action *action)
{
    if (action == 0 || action->magic != RUNTIME_GUEST_READ_ACTION_MAGIC ||
        action->abi_version != RUNTIME_GUEST_READ_ACTION_VERSION ||
        action->struct_bytes != sizeof(*action) ||
        !runtime_cpu_result_valid(&action->cpu_result)) return 0;
    if (action->disposition == RUNTIME_GUEST_READ_ACTION_PASS_THROUGH)
        return action->cpu_result.disposition == RUNTIME_CPU_RESULT_PASS_THROUGH &&
            action->guest_read.address == 0u && action->guest_read.length == 0u;
    if (action->disposition == RUNTIME_GUEST_READ_ACTION_RESUME)
        return action->cpu_result.disposition == RUNTIME_CPU_RESULT_RESUME &&
            action->guest_read.address == 0u && action->guest_read.length == 0u;
    if (action->disposition == RUNTIME_GUEST_READ_ACTION_STOP)
        return action->cpu_result.disposition == RUNTIME_CPU_RESULT_STOP &&
            action->guest_read.address == 0u && action->guest_read.length == 0u;
    return action->disposition == RUNTIME_GUEST_READ_ACTION_NEED_READ &&
        action->cpu_result.disposition == RUNTIME_CPU_RESULT_PASS_THROUGH &&
        action->guest_read.length > 0u &&
        action->guest_read.length <= RUNTIME_GUEST_READ_ACTION_MAX_BYTES &&
        action->guest_read.address <= UINT64_MAX - action->guest_read.length;
}
