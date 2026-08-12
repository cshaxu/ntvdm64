#include "nt4_bootstrap_transaction.h"

#include <stdint.h>

static int range_fits(uint32_t physical, size_t byte_count, size_t capacity)
{
    return byte_count != 0u && (size_t)physical <= capacity &&
        byte_count <= capacity - (size_t)physical;
}

nt4_bootstrap_transaction_result nt4_bootstrap_transaction_build(
    const nt4_ntio_bootstrap_plan *ntio_plan,
    const nt4_ntdos_load_plan *ntdos_plan,
    size_t ordinary_ram_byte_count,
    nt4_bootstrap_transaction *out_transaction)
{
    nt4_bootstrap_transaction candidate = { { { 0u, NULL, 0u }, { 0u, NULL, 0u } }, 0u, 0u, 0u };
    uint64_t ntio_end;
    uint64_t ntdos_end;

    if (ntio_plan == NULL || ntdos_plan == NULL || out_transaction == NULL ||
        ntio_plan->preload_bytes == NULL || ntdos_plan->preload_bytes == NULL ||
        ntio_plan->preload_byte_count == 0u || ntdos_plan->preload_byte_count == 0u) {
        return NT4_BOOTSTRAP_TRANSACTION_INVALID_ARGUMENT;
    }
    if (!range_fits(ntio_plan->preload_physical, ntio_plan->preload_byte_count,
            ordinary_ram_byte_count) || !range_fits(ntdos_plan->preload_physical,
            ntdos_plan->preload_byte_count, ordinary_ram_byte_count)) {
        return NT4_BOOTSTRAP_TRANSACTION_RAM_RANGE_REJECTED;
    }
    ntio_end = (uint64_t)ntio_plan->preload_physical + ntio_plan->preload_byte_count;
    ntdos_end = (uint64_t)ntdos_plan->preload_physical + ntdos_plan->preload_byte_count;
    if ((uint64_t)ntio_plan->preload_physical < ntdos_end &&
        (uint64_t)ntdos_plan->preload_physical < ntio_end) {
        return NT4_BOOTSTRAP_TRANSACTION_OVERLAP_REJECTED;
    }

    candidate.preloads[0].physical = ntio_plan->preload_physical;
    candidate.preloads[0].bytes = ntio_plan->preload_bytes;
    candidate.preloads[0].byte_count = ntio_plan->preload_byte_count;
    candidate.preloads[1].physical = ntdos_plan->preload_physical;
    candidate.preloads[1].bytes = ntdos_plan->preload_bytes;
    candidate.preloads[1].byte_count = ntdos_plan->preload_byte_count;
    candidate.preload_count = 2u;
    candidate.entry_cs = ntio_plan->entry_cs;
    candidate.entry_ip = ntio_plan->entry_ip;
    *out_transaction = candidate;
    return NT4_BOOTSTRAP_TRANSACTION_OK;
}
