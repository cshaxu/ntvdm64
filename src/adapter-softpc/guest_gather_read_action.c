#include "guest_gather_read_action.h"

#include <string.h>

void runtime_guest_gather_read_action_pass_through(
    runtime_guest_gather_read_action *action)
{
    if (action == 0) return;
    memset(action, 0, sizeof(*action));
    action->magic = RUNTIME_GUEST_GATHER_READ_ACTION_MAGIC;
    action->abi_version = RUNTIME_GUEST_GATHER_READ_ACTION_VERSION;
    action->struct_bytes = sizeof(*action);
    action->disposition = RUNTIME_GUEST_GATHER_READ_ACTION_PASS_THROUGH;
    runtime_cpu_result_pass_through(&action->cpu_result);
}

int runtime_guest_gather_read_action_resume(
    runtime_guest_gather_read_action *action, uint64_t resume_rip)
{
    if (action == 0) return 0;
    runtime_guest_gather_read_action_pass_through(action);
    action->disposition = RUNTIME_GUEST_GATHER_READ_ACTION_RESUME;
    return runtime_cpu_result_resume(&action->cpu_result, resume_rip);
}

int runtime_guest_gather_read_action_stop(
    runtime_guest_gather_read_action *action)
{
    if (action == 0) return 0;
    runtime_guest_gather_read_action_pass_through(action);
    action->disposition = RUNTIME_GUEST_GATHER_READ_ACTION_STOP;
    return runtime_cpu_result_stop(&action->cpu_result);
}

int runtime_guest_gather_read_action_need_read(
    runtime_guest_gather_read_action *action,
    const runtime_guest_range *ranges, uint32_t range_count)
{
    uint32_t index, total = 0u;
    if (action == 0 || ranges == 0 || range_count == 0u ||
        range_count > RUNTIME_GUEST_GATHER_READ_ACTION_MAX_RANGES) return 0;
    for (index = 0u; index < range_count; ++index) {
        if (ranges[index].length == 0u ||
            ranges[index].length > RUNTIME_GUEST_GATHER_READ_ACTION_MAX_TOTAL_BYTES ||
            ranges[index].address > UINT64_MAX - ranges[index].length ||
            total > RUNTIME_GUEST_GATHER_READ_ACTION_MAX_TOTAL_BYTES - ranges[index].length)
            return 0;
        total += (uint32_t)ranges[index].length;
    }
    runtime_guest_gather_read_action_pass_through(action);
    action->disposition = RUNTIME_GUEST_GATHER_READ_ACTION_NEED_READ;
    action->range_count = range_count;
    action->total_bytes = total;
    memcpy(action->ranges, ranges, (size_t)range_count * sizeof(*ranges));
    return 1;
}

int runtime_guest_gather_read_action_need_read_resume(
    runtime_guest_gather_read_action *action,
    const runtime_guest_range *ranges, uint32_t range_count,
    uint64_t resume_rip)
{
    return runtime_guest_gather_read_action_need_read(action, ranges,
        range_count) && runtime_cpu_result_resume(&action->cpu_result,
            resume_rip);
}

int runtime_guest_gather_read_action_valid(
    const runtime_guest_gather_read_action *action)
{
    uint32_t index, total = 0u;
    if (action == 0 || action->magic != RUNTIME_GUEST_GATHER_READ_ACTION_MAGIC ||
        action->abi_version != RUNTIME_GUEST_GATHER_READ_ACTION_VERSION ||
        action->struct_bytes != sizeof(*action) ||
        !runtime_cpu_result_valid(&action->cpu_result)) return 0;
    if (action->disposition == RUNTIME_GUEST_GATHER_READ_ACTION_PASS_THROUGH)
        return action->cpu_result.disposition == RUNTIME_CPU_RESULT_PASS_THROUGH &&
            action->range_count == 0u && action->total_bytes == 0u;
    if (action->disposition == RUNTIME_GUEST_GATHER_READ_ACTION_RESUME)
        return action->cpu_result.disposition == RUNTIME_CPU_RESULT_RESUME &&
            action->range_count == 0u && action->total_bytes == 0u;
    if (action->disposition == RUNTIME_GUEST_GATHER_READ_ACTION_STOP)
        return action->cpu_result.disposition == RUNTIME_CPU_RESULT_STOP &&
            action->range_count == 0u && action->total_bytes == 0u;
    if (action->disposition != RUNTIME_GUEST_GATHER_READ_ACTION_NEED_READ ||
        (action->cpu_result.disposition != RUNTIME_CPU_RESULT_PASS_THROUGH &&
         action->cpu_result.disposition != RUNTIME_CPU_RESULT_RESUME) ||
        action->range_count == 0u ||
        action->range_count > RUNTIME_GUEST_GATHER_READ_ACTION_MAX_RANGES) return 0;
    for (index = 0u; index < action->range_count; ++index) {
        const runtime_guest_range *range = &action->ranges[index];
        if (range->length == 0u || range->address > UINT64_MAX - range->length ||
            range->length > RUNTIME_GUEST_GATHER_READ_ACTION_MAX_TOTAL_BYTES ||
            total > RUNTIME_GUEST_GATHER_READ_ACTION_MAX_TOTAL_BYTES - range->length)
            return 0;
        total += (uint32_t)range->length;
    }
    return action->total_bytes == total;
}
