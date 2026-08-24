#include <stdint.h>
#include <string.h>

#include "startup_snapshot_abi.h"

int main(void)
{
    runtime_exception_event_v1 event;
    runtime_cpu_state_v1 state;
    runtime_startup_snapshot_range_v1 ranges[3] = {
        { 11u, 0u, { UINT64_C(0x40), 48u } },
        { 12u, 0u, { UINT64_C(0x496), 1u } },
        { 13u, 0u, { UINT64_C(0x714), 4u } }
    };
    runtime_startup_snapshot_transaction_v1 transaction;

    memset(&event, 0, sizeof(event));
    event.magic = RUNTIME_EXCEPTION_ABI_MAGIC;
    event.abi_version = RUNTIME_EXCEPTION_ABI_VERSION;
    event.struct_bytes = sizeof(event);
    event.kind = RUNTIME_EXCEPTION_EVENT_CPU_EXCEPTION;
    event.vector = 6u;
    event.fault_rip = UINT64_C(0x7c00);
    runtime_cpu_state_v1_initialize(&state, RUNTIME_CPU_EXECUTION_REAL);
    runtime_startup_snapshot_transaction_v1_initialize(
        &transaction, &event, &state, ranges, 3u);
    if (sizeof(transaction) != 240u ||
        !runtime_startup_snapshot_transaction_v1_preflight(
            &transaction, UINT64_C(0x100000), 53u)) return 1;
    transaction.ranges[2].id = 12u;
    if (runtime_startup_snapshot_transaction_v1_preflight(
            &transaction, UINT64_C(0x100000), 53u)) return 2;
    transaction.ranges[2].id = 13u;
    transaction.ranges[2].guest_read.address = UINT64_C(0x496);
    if (runtime_startup_snapshot_transaction_v1_preflight(
            &transaction, UINT64_C(0x100000), 53u)) return 3;
    transaction.ranges[2].guest_read.address = UINT64_C(0x714);
    transaction.output_bytes = 52u;
    if (runtime_startup_snapshot_transaction_v1_preflight(
            &transaction, UINT64_C(0x100000), 53u)) return 4;
    transaction.output_bytes = 53u;
    transaction.boundary.vector = 5u;
    if (runtime_startup_snapshot_transaction_v1_preflight(
            &transaction, UINT64_C(0x100000), 53u)) return 5;
    runtime_startup_snapshot_transaction_v1_initialize(0, 0, 0, 0, 0u);
    return runtime_startup_snapshot_transaction_v1_preflight(0, 0u, 0u) ? 6 : 0;
}
