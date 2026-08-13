#include "bx_ntvdm_guest_gather_read_action_v1.h"

#include <string.h>

void bx_ntvdm_guest_gather_read_action_v1_pass_through(
    bx_ntvdm_guest_gather_read_action_v1 *action)
{
    if (action == 0) return;
    memset(action, 0, sizeof(*action));
    action->magic = BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_MAGIC;
    action->abi_version = BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_VERSION;
    action->struct_bytes = sizeof(*action);
    action->disposition = BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_PASS_THROUGH;
    bx_ntvdm_cpu_result_v2_pass_through(&action->cpu_result);
}

int bx_ntvdm_guest_gather_read_action_v1_resume(
    bx_ntvdm_guest_gather_read_action_v1 *action, uint64_t resume_rip)
{
    if (action == 0) return 0;
    bx_ntvdm_guest_gather_read_action_v1_pass_through(action);
    action->disposition = BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_RESUME;
    return bx_ntvdm_cpu_result_v2_resume(&action->cpu_result, resume_rip);
}

int bx_ntvdm_guest_gather_read_action_v1_stop(
    bx_ntvdm_guest_gather_read_action_v1 *action)
{
    if (action == 0) return 0;
    bx_ntvdm_guest_gather_read_action_v1_pass_through(action);
    action->disposition = BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_STOP;
    return bx_ntvdm_cpu_result_v2_stop(&action->cpu_result);
}

int bx_ntvdm_guest_gather_read_action_v1_need_read(
    bx_ntvdm_guest_gather_read_action_v1 *action,
    const bx_ntvdm_guest_range *ranges, uint32_t range_count)
{
    uint32_t index, total = 0u;
    if (action == 0 || ranges == 0 || range_count == 0u ||
        range_count > BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_MAX_RANGES) return 0;
    for (index = 0u; index < range_count; ++index) {
        if (ranges[index].length == 0u ||
            ranges[index].length > BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_MAX_TOTAL_BYTES ||
            ranges[index].address > UINT64_MAX - ranges[index].length ||
            total > BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_MAX_TOTAL_BYTES - ranges[index].length)
            return 0;
        total += (uint32_t)ranges[index].length;
    }
    bx_ntvdm_guest_gather_read_action_v1_pass_through(action);
    action->disposition = BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_NEED_READ;
    action->range_count = range_count;
    action->total_bytes = total;
    memcpy(action->ranges, ranges, (size_t)range_count * sizeof(*ranges));
    return 1;
}

int bx_ntvdm_guest_gather_read_action_v1_need_read_resume(
    bx_ntvdm_guest_gather_read_action_v1 *action,
    const bx_ntvdm_guest_range *ranges, uint32_t range_count,
    uint64_t resume_rip)
{
    return bx_ntvdm_guest_gather_read_action_v1_need_read(action, ranges,
        range_count) && bx_ntvdm_cpu_result_v2_resume(&action->cpu_result,
            resume_rip);
}

int bx_ntvdm_guest_gather_read_action_v1_valid(
    const bx_ntvdm_guest_gather_read_action_v1 *action)
{
    uint32_t index, total = 0u;
    if (action == 0 || action->magic != BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_MAGIC ||
        action->abi_version != BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_VERSION ||
        action->struct_bytes != sizeof(*action) ||
        !bx_ntvdm_cpu_result_v2_valid(&action->cpu_result)) return 0;
    if (action->disposition == BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_PASS_THROUGH)
        return action->cpu_result.disposition == BX_NTVDM_CPU_RESULT_V2_PASS_THROUGH &&
            action->range_count == 0u && action->total_bytes == 0u;
    if (action->disposition == BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_RESUME)
        return action->cpu_result.disposition == BX_NTVDM_CPU_RESULT_V2_RESUME &&
            action->range_count == 0u && action->total_bytes == 0u;
    if (action->disposition == BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_STOP)
        return action->cpu_result.disposition == BX_NTVDM_CPU_RESULT_V2_STOP &&
            action->range_count == 0u && action->total_bytes == 0u;
    if (action->disposition != BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_NEED_READ ||
        (action->cpu_result.disposition != BX_NTVDM_CPU_RESULT_V2_PASS_THROUGH &&
         action->cpu_result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME) ||
        action->range_count == 0u ||
        action->range_count > BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_MAX_RANGES) return 0;
    for (index = 0u; index < action->range_count; ++index) {
        const bx_ntvdm_guest_range *range = &action->ranges[index];
        if (range->length == 0u || range->address > UINT64_MAX - range->length ||
            range->length > BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_MAX_TOTAL_BYTES ||
            total > BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_MAX_TOTAL_BYTES - range->length)
            return 0;
        total += (uint32_t)range->length;
    }
    return action->total_bytes == total;
}
