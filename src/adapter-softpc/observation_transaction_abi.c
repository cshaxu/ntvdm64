#include "observation_transaction_abi.h"

#include <string.h>

void runtime_observation_transaction_initialize(
    runtime_observation_transaction *transaction,
    const runtime_exception_event *boundary,
    const runtime_cpu_state *cpu_before,
    const runtime_guest_range *guest_read)
{
    if (transaction == 0) return;
    memset(transaction, 0, sizeof(*transaction));
    transaction->magic = RUNTIME_OBSERVATION_TRANSACTION_ABI_MAGIC;
    transaction->abi_version = RUNTIME_OBSERVATION_TRANSACTION_ABI_VERSION;
    transaction->struct_bytes = sizeof(*transaction);
    if (boundary != 0) transaction->boundary = *boundary;
    if (cpu_before != 0) transaction->cpu_before = *cpu_before;
    if (guest_read != 0) transaction->guest_read = *guest_read;
}

int runtime_observation_transaction_preflight(
    const runtime_observation_transaction *transaction,
    uint64_t aperture_bytes, uint64_t output_bytes)
{
    return transaction != 0 &&
        transaction->magic == RUNTIME_OBSERVATION_TRANSACTION_ABI_MAGIC &&
        transaction->abi_version == RUNTIME_OBSERVATION_TRANSACTION_ABI_VERSION &&
        transaction->struct_bytes == sizeof(*transaction) &&
        transaction->flags == 0u &&
        runtime_exception_event_valid(&transaction->boundary) &&
        transaction->boundary.vector == 6u &&
        runtime_cpu_state_valid(&transaction->cpu_before) &&
        transaction->guest_read.length != 0u &&
        transaction->guest_read.length == output_bytes &&
        runtime_guest_range_within(aperture_bytes, &transaction->guest_read);
}
