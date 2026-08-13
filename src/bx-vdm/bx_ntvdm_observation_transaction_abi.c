#include "bx_ntvdm_observation_transaction_abi.h"

#include <string.h>

void bx_ntvdm_observation_transaction_v1_initialize(
    bx_ntvdm_observation_transaction_v1 *transaction,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_guest_range *guest_read)
{
    if (transaction == 0) return;
    memset(transaction, 0, sizeof(*transaction));
    transaction->magic = BX_NTVDM_OBSERVATION_TRANSACTION_ABI_MAGIC;
    transaction->abi_version = BX_NTVDM_OBSERVATION_TRANSACTION_ABI_VERSION;
    transaction->struct_bytes = sizeof(*transaction);
    if (boundary != 0) transaction->boundary = *boundary;
    if (cpu_before != 0) transaction->cpu_before = *cpu_before;
    if (guest_read != 0) transaction->guest_read = *guest_read;
}

int bx_ntvdm_observation_transaction_v1_preflight(
    const bx_ntvdm_observation_transaction_v1 *transaction,
    uint64_t aperture_bytes, uint64_t output_bytes)
{
    return transaction != 0 &&
        transaction->magic == BX_NTVDM_OBSERVATION_TRANSACTION_ABI_MAGIC &&
        transaction->abi_version == BX_NTVDM_OBSERVATION_TRANSACTION_ABI_VERSION &&
        transaction->struct_bytes == sizeof(*transaction) &&
        transaction->flags == 0u &&
        bx_ntvdm_exception_event_v1_valid(&transaction->boundary) &&
        transaction->boundary.vector == 6u &&
        bx_ntvdm_cpu_state_v1_valid(&transaction->cpu_before) &&
        transaction->guest_read.length != 0u &&
        transaction->guest_read.length == output_bytes &&
        bx_ntvdm_guest_range_within(aperture_bytes, &transaction->guest_read);
}
