#include "nt4_ntio_bootstrap_plan.h"

#include <stdlib.h>
#include <string.h>

nt4_ntio_bootstrap_plan_result nt4_ntio_bootstrap_plan_build(
    const uint8_t *ntio_bytes,
    size_t ntio_byte_count,
    const uint8_t virtual_interrupt_state[NT4_NTIO_VIRTUAL_INTERRUPT_SIZE],
    size_t ordinary_ram_byte_count,
    nt4_ntio_bootstrap_plan *out_plan)
{
    nt4_ntio_bootstrap_plan candidate = { NULL, 0u, 0u, 0u, 0u };
    const size_t virtual_interrupt_offset =
        NT4_NTIO_VIRTUAL_INTERRUPT_PHYSICAL - NT4_NTIO_LOAD_PHYSICAL;

    if (ntio_bytes == NULL || virtual_interrupt_state == NULL || out_plan == NULL ||
        out_plan->preload_bytes != NULL || ntio_byte_count == 0u) {
        return NT4_NTIO_BOOTSTRAP_PLAN_INVALID_ARGUMENT;
    }
    if (ntio_byte_count < virtual_interrupt_offset + NT4_NTIO_VIRTUAL_INTERRUPT_SIZE) {
        return NT4_NTIO_BOOTSTRAP_PLAN_IMAGE_TOO_SHORT;
    }
    if (ordinary_ram_byte_count < NT4_NTIO_LOAD_PHYSICAL ||
        ntio_byte_count > ordinary_ram_byte_count - NT4_NTIO_LOAD_PHYSICAL) {
        return NT4_NTIO_BOOTSTRAP_PLAN_RAM_RANGE_REJECTED;
    }

    candidate.preload_bytes = (uint8_t *)malloc(ntio_byte_count);
    if (candidate.preload_bytes == NULL) {
        return NT4_NTIO_BOOTSTRAP_PLAN_NO_MEMORY;
    }
    memcpy(candidate.preload_bytes, ntio_bytes, ntio_byte_count);
    memcpy(candidate.preload_bytes + virtual_interrupt_offset, virtual_interrupt_state,
        NT4_NTIO_VIRTUAL_INTERRUPT_SIZE);
    candidate.preload_byte_count = ntio_byte_count;
    candidate.preload_physical = NT4_NTIO_LOAD_PHYSICAL;
    candidate.entry_cs = NT4_NTIO_LOAD_CS;
    candidate.entry_ip = NT4_NTIO_LOAD_IP;
    *out_plan = candidate;
    return NT4_NTIO_BOOTSTRAP_PLAN_OK;
}

void nt4_ntio_bootstrap_plan_release(nt4_ntio_bootstrap_plan *plan)
{
    if (plan == NULL) {
        return;
    }
    free(plan->preload_bytes);
    memset(plan, 0, sizeof(*plan));
}
