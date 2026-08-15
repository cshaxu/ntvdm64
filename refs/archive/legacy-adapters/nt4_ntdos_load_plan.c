#include "nt4_ntdos_load_plan.h"

#include <stdlib.h>
#include <string.h>

nt4_ntdos_load_plan_result nt4_ntdos_load_plan_build(
    const uint8_t *ntdos_bytes,
    size_t ntdos_byte_count,
    uint16_t load_segment,
    size_t ordinary_ram_byte_count,
    nt4_ntdos_load_plan *out_plan)
{
    nt4_ntdos_load_plan candidate = { NULL, 0u, 0u, 0u };
    const uint32_t physical = (uint32_t)load_segment << 4u;

    if (ntdos_bytes == NULL || out_plan == NULL || out_plan->preload_bytes != NULL ||
        ntdos_byte_count == 0u) {
        return NT4_NTDOS_LOAD_PLAN_INVALID_ARGUMENT;
    }
    if ((size_t)physical > ordinary_ram_byte_count ||
        ntdos_byte_count > ordinary_ram_byte_count - (size_t)physical) {
        return NT4_NTDOS_LOAD_PLAN_RAM_RANGE_REJECTED;
    }

    candidate.preload_bytes = (uint8_t *)malloc(ntdos_byte_count);
    if (candidate.preload_bytes == NULL) {
        return NT4_NTDOS_LOAD_PLAN_NO_MEMORY;
    }
    memcpy(candidate.preload_bytes, ntdos_bytes, ntdos_byte_count);
    candidate.preload_byte_count = ntdos_byte_count;
    candidate.preload_physical = physical;
    candidate.load_segment = load_segment;
    *out_plan = candidate;
    return NT4_NTDOS_LOAD_PLAN_OK;
}

void nt4_ntdos_load_plan_release(nt4_ntdos_load_plan *plan)
{
    if (plan == NULL) {
        return;
    }
    free(plan->preload_bytes);
    memset(plan, 0, sizeof(*plan));
}
