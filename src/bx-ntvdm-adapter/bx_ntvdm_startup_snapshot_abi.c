#include "bx_ntvdm_startup_snapshot_abi.h"

#include <string.h>

static int ranges_overlap(const bx_ntvdm_guest_range *left,
    const bx_ntvdm_guest_range *right)
{
    return left->address < right->address + right->length &&
        right->address < left->address + left->length;
}

void bx_ntvdm_startup_snapshot_transaction_v1_initialize(
    bx_ntvdm_startup_snapshot_transaction_v1 *transaction,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_startup_snapshot_range_v1 *ranges, uint32_t range_count)
{
    uint32_t index;
    uint64_t output_bytes = 0u;

    if (transaction == 0) return;
    memset(transaction, 0, sizeof(*transaction));
    transaction->magic = BX_NTVDM_STARTUP_SNAPSHOT_ABI_MAGIC;
    transaction->abi_version = BX_NTVDM_STARTUP_SNAPSHOT_ABI_VERSION;
    transaction->struct_bytes = sizeof(*transaction);
    if (boundary != 0) transaction->boundary = *boundary;
    if (cpu_before != 0) transaction->cpu_before = *cpu_before;
    if (ranges == 0 || range_count == 0u ||
        range_count > BX_NTVDM_STARTUP_SNAPSHOT_V1_MAX_RANGES) return;
    for (index = 0u; index < range_count; ++index) {
        if (UINT64_MAX - output_bytes < ranges[index].guest_read.length) return;
        transaction->ranges[index] = ranges[index];
        output_bytes += ranges[index].guest_read.length;
    }
    transaction->range_count = range_count;
    transaction->output_bytes = output_bytes;
}

int bx_ntvdm_startup_snapshot_transaction_v1_preflight(
    const bx_ntvdm_startup_snapshot_transaction_v1 *transaction,
    uint64_t aperture_bytes, uint64_t output_capacity)
{
    uint32_t index;
    uint32_t other;
    uint64_t output_bytes = 0u;

    if (transaction == 0 ||
        transaction->magic != BX_NTVDM_STARTUP_SNAPSHOT_ABI_MAGIC ||
        transaction->abi_version != BX_NTVDM_STARTUP_SNAPSHOT_ABI_VERSION ||
        transaction->struct_bytes != sizeof(*transaction) ||
        transaction->flags != 0u || transaction->reserved0 != 0u ||
        transaction->range_count == 0u ||
        transaction->range_count > BX_NTVDM_STARTUP_SNAPSHOT_V1_MAX_RANGES ||
        !bx_ntvdm_exception_event_v1_valid(&transaction->boundary) ||
        transaction->boundary.vector != 6u ||
        !bx_ntvdm_cpu_state_v1_valid(&transaction->cpu_before)) return 0;
    for (index = 0u; index < transaction->range_count; ++index) {
        const bx_ntvdm_startup_snapshot_range_v1 *range = &transaction->ranges[index];
        if (range->id == 0u || range->flags != 0u ||
            range->guest_read.length == 0u ||
            !bx_ntvdm_guest_range_within(aperture_bytes, &range->guest_read) ||
            UINT64_MAX - output_bytes < range->guest_read.length) return 0;
        for (other = 0u; other < index; ++other) {
            if (range->id == transaction->ranges[other].id ||
                ranges_overlap(&range->guest_read, &transaction->ranges[other].guest_read)) return 0;
        }
        output_bytes += range->guest_read.length;
    }
    return output_bytes == transaction->output_bytes && output_bytes == output_capacity;
}
