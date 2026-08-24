#include <stdint.h>

#include "bx_ntvdm_host_transaction_abi.h"

int main(void)
{
    bx_ntvdm_host_service_request_v1 request;
    bx_ntvdm_cpu_state_v1 state;
    bx_ntvdm_guest_write_v1 write;
    bx_ntvdm_host_transaction_v1 transaction;

    request.selector = 0x50u;
    request.service = 0x11u;
    request.family = BX_NTVDM_HOST_SERVICE_FAMILY_DEM;
    bx_ntvdm_cpu_state_v1_initialize(&state, BX_NTVDM_CPU_EXECUTION_REAL);
    bx_ntvdm_guest_write_v1_initialize(&write, 0x8b00u, 0x200u, 0u);
    bx_ntvdm_host_transaction_v1_initialize(&transaction, &request, &state,
        &write);
    if (sizeof(transaction) != 136u ||
        !bx_ntvdm_host_transaction_v1_preflight(&transaction, 0x100000u,
            0x200u)) return 1;
    transaction.request.service = 0x49u;
    if (bx_ntvdm_host_transaction_v1_preflight(&transaction, 0x100000u,
        0x200u)) return 2;
    transaction.request.service = 0x11u;
    transaction.guest_write.byte_count = 0x200u;
    transaction.guest_write.payload_offset = 1u;
    if (bx_ntvdm_host_transaction_v1_preflight(&transaction, 0x100000u,
        0x200u)) return 3;
    transaction.guest_write.payload_offset = 0u;
    transaction.cpu_before.execution_mode = 0u;
    if (bx_ntvdm_host_transaction_v1_preflight(&transaction, 0x100000u,
        0x200u)) return 4;
    return bx_ntvdm_host_transaction_v1_preflight(0, 0u, 0u) ? 5 : 0;
}
