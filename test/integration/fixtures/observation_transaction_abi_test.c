#include <stdint.h>
#include <string.h>

#include "observation_transaction_abi.h"

int main(void)
{
    runtime_exception_event event;
    runtime_cpu_state state;
    runtime_guest_range range = { UINT64_C(0x700), 8u };
    runtime_observation_transaction transaction;

    memset(&event, 0, sizeof(event));
    event.magic = RUNTIME_EXCEPTION_ABI_MAGIC;
    event.abi_version = RUNTIME_EXCEPTION_ABI_VERSION;
    event.struct_bytes = sizeof(event);
    event.kind = RUNTIME_EXCEPTION_EVENT_CPU_EXCEPTION;
    event.vector = 6u;
    event.fault_rip = UINT64_C(0xfff0);
    runtime_cpu_state_initialize(&state, RUNTIME_CPU_EXECUTION_REAL);
    runtime_observation_transaction_initialize(&transaction, &event, &state, &range);
    if (sizeof(transaction) != 144u || !runtime_observation_transaction_preflight(
        &transaction, UINT64_C(0x100000), 8u)) return 1;
    transaction.guest_read.length = 0u;
    if (runtime_observation_transaction_preflight(&transaction, UINT64_C(0x100000), 0u)) return 2;
    transaction.guest_read.length = 8u;
    transaction.guest_read.address = UINT64_C(0xfffff);
    if (runtime_observation_transaction_preflight(&transaction, UINT64_C(0x100000), 8u)) return 3;
    transaction.guest_read.address = UINT64_C(0x700);
    transaction.flags = 1u;
    if (runtime_observation_transaction_preflight(&transaction, UINT64_C(0x100000), 8u)) return 4;
    transaction.flags = 0u;
    transaction.boundary.vector = 5u;
    if (runtime_observation_transaction_preflight(&transaction, UINT64_C(0x100000), 8u)) return 5;
    runtime_observation_transaction_initialize(0, 0, 0, 0);
    return runtime_observation_transaction_preflight(0, 0u, 0u) ? 6 : 0;
}
