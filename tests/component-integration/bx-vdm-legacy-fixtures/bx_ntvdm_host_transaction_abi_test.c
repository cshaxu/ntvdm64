#include <stdint.h>

#include "support/bx_ntvdm_host_transaction_abi.h"

/* This historical transaction classifier is intentionally test-local.  It
 * models the old 50:xx/54:xx experiment and must not re-enter the
 * selector-blind adapter production library. */
#include "support/bx_ntvdm_host_transaction_abi.c"

int main(void)
{
    runtime_host_service_request_v1 request;
    runtime_cpu_state_v1 state;
    runtime_guest_write_v1 write;
    runtime_host_transaction_v1 transaction;

    request.selector = 0x50u;
    request.service = 0x11u;
    request.family = RUNTIME_HOST_SERVICE_FAMILY_DEM;
    runtime_cpu_state_v1_initialize(&state, RUNTIME_CPU_EXECUTION_REAL);
    runtime_guest_write_v1_initialize(&write, 0x8b00u, 0x200u, 0u);
    runtime_host_transaction_v1_initialize(&transaction, &request, &state,
        &write);
    if (sizeof(transaction) != 136u ||
        !runtime_host_transaction_v1_preflight(&transaction, 0x100000u,
            0x200u)) return 1;
    transaction.request.service = 0x49u;
    if (runtime_host_transaction_v1_preflight(&transaction, 0x100000u,
        0x200u)) return 2;
    transaction.request.service = 0x11u;
    transaction.guest_write.byte_count = 0x200u;
    transaction.guest_write.payload_offset = 1u;
    if (runtime_host_transaction_v1_preflight(&transaction, 0x100000u,
        0x200u)) return 3;
    transaction.guest_write.payload_offset = 0u;
    transaction.cpu_before.execution_mode = 0u;
    if (runtime_host_transaction_v1_preflight(&transaction, 0x100000u,
        0x200u)) return 4;
    return runtime_host_transaction_v1_preflight(0, 0u, 0u) ? 5 : 0;
}
