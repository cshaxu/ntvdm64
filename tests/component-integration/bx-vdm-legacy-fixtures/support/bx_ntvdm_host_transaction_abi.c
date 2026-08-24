#include "bx_ntvdm_host_transaction_abi.h"

#include <string.h>

static int runtime_host_service_request_v1_valid(
    const runtime_host_service_request_v1 *request)
{
    return request != 0 &&
        ((request->selector == 0x50u &&
          request->family == RUNTIME_HOST_SERVICE_FAMILY_DEM &&
          request->service < 0x49u) ||
         (request->selector == 0x54u &&
          request->family == RUNTIME_HOST_SERVICE_FAMILY_COMMAND &&
          request->service < 17u));
}

void runtime_host_transaction_v1_initialize(
    runtime_host_transaction_v1 *transaction,
    const runtime_host_service_request_v1 *request,
    const runtime_cpu_state_v1 *cpu_before,
    const runtime_guest_write_v1 *guest_write)
{
    if (transaction == 0) return;
    memset(transaction, 0, sizeof(*transaction));
    transaction->magic = RUNTIME_HOST_TRANSACTION_ABI_MAGIC;
    transaction->abi_version = RUNTIME_HOST_TRANSACTION_ABI_VERSION;
    transaction->struct_bytes = sizeof(*transaction);
    if (request != 0) transaction->request = *request;
    if (cpu_before != 0) transaction->cpu_before = *cpu_before;
    if (guest_write != 0) transaction->guest_write = *guest_write;
}

int runtime_host_transaction_v1_preflight(
    const runtime_host_transaction_v1 *transaction,
    uint64_t aperture_bytes, uint64_t payload_bytes)
{
    return transaction != 0 &&
        transaction->magic == RUNTIME_HOST_TRANSACTION_ABI_MAGIC &&
        transaction->abi_version == RUNTIME_HOST_TRANSACTION_ABI_VERSION &&
        transaction->struct_bytes == sizeof(*transaction) &&
        transaction->flags == 0u && transaction->reserved0 == 0u &&
        runtime_host_service_request_v1_valid(&transaction->request) &&
        runtime_cpu_state_v1_valid(&transaction->cpu_before) &&
        runtime_guest_write_v1_preflight(&transaction->guest_write,
            aperture_bytes, payload_bytes);
}
