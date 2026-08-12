#include <stdint.h>
#include <string.h>

#include "bx_ntvdm_observation_transaction_abi.h"

int main(void)
{
    bx_ntvdm_exception_event_v1 event;
    bx_ntvdm_cpu_state_v1 state;
    bx_ntvdm_guest_range range = { UINT64_C(0x700), 8u };
    bx_ntvdm_observation_transaction_v1 transaction;

    memset(&event, 0, sizeof(event));
    event.magic = BX_NTVDM_EXCEPTION_ABI_MAGIC;
    event.abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION;
    event.struct_bytes = sizeof(event);
    event.kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION;
    event.vector = 6u;
    event.fault_rip = UINT64_C(0xfff0);
    bx_ntvdm_cpu_state_v1_initialize(&state, BX_NTVDM_CPU_EXECUTION_REAL);
    bx_ntvdm_observation_transaction_v1_initialize(&transaction, &event, &state, &range);
    if (sizeof(transaction) != 144u || !bx_ntvdm_observation_transaction_v1_preflight(
        &transaction, UINT64_C(0x100000), 8u)) return 1;
    transaction.guest_read.length = 0u;
    if (bx_ntvdm_observation_transaction_v1_preflight(&transaction, UINT64_C(0x100000), 0u)) return 2;
    transaction.guest_read.length = 8u;
    transaction.guest_read.address = UINT64_C(0xfffff);
    if (bx_ntvdm_observation_transaction_v1_preflight(&transaction, UINT64_C(0x100000), 8u)) return 3;
    transaction.guest_read.address = UINT64_C(0x700);
    transaction.flags = 1u;
    if (bx_ntvdm_observation_transaction_v1_preflight(&transaction, UINT64_C(0x100000), 8u)) return 4;
    transaction.flags = 0u;
    transaction.boundary.vector = 5u;
    if (bx_ntvdm_observation_transaction_v1_preflight(&transaction, UINT64_C(0x100000), 8u)) return 5;
    bx_ntvdm_observation_transaction_v1_initialize(0, 0, 0, 0);
    return bx_ntvdm_observation_transaction_v1_preflight(0, 0u, 0u) ? 6 : 0;
}
