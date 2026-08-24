#include "bulk_result_transaction.h"

#include <assert.h>
#include <stdio.h>

int main(void)
{
    runtime_exception_event_v1 event = {
        RUNTIME_EXCEPTION_ABI_MAGIC, RUNTIME_EXCEPTION_ABI_VERSION,
        sizeof(event), RUNTIME_EXCEPTION_EVENT_CPU_EXCEPTION, 0u, 6u,
        0u, 0u, 0x100u };
    runtime_cpu_state_v1 cpu;
    runtime_cpu_result_v2 result;
    runtime_bulk_result_transaction_v1 tx;
    runtime_cpu_state_v1_initialize(&cpu, RUNTIME_CPU_EXECUTION_REAL);
    assert(runtime_cpu_result_v2_resume(&result, 0x104u));
    runtime_bulk_result_transaction_v1_initialize(&tx, &event, &cpu);
    assert(runtime_bulk_result_transaction_v1_prepare(&tx, 0x200u,
        RUNTIME_BULK_RESULT_TRANSACTION_V1_MAX_PAYLOAD, &result));
    assert(runtime_bulk_result_transaction_v1_preflight(&tx, 0x100000u,
        RUNTIME_BULK_RESULT_TRANSACTION_V1_MAX_PAYLOAD));
    assert(!runtime_bulk_result_transaction_v1_preflight(&tx, 0x100000u,
        1u));
    assert(runtime_bulk_result_transaction_v1_prepare(&tx, 0x100000u,
        1u, &result));
    assert(!runtime_bulk_result_transaction_v1_preflight(&tx, 0x100000u,
        1u));
    assert(runtime_bulk_result_transaction_v1_prepare(&tx, 0u, 0u, &result));
    assert(runtime_bulk_result_transaction_v1_preflight(&tx, 0x100000u, 0u));
    assert(!runtime_bulk_result_transaction_v1_prepare(&tx, 1u, 0u, &result));
    result.resume_rip = event.fault_rip;
    assert(runtime_bulk_result_transaction_v1_prepare(&tx, 0u, 0u, &result));
    assert(!runtime_bulk_result_transaction_v1_preflight(&tx, 0x100000u, 0u));
    puts("bx-ntvdm bulk-result transaction: bounded single-range contract verified");
    return 0;
}
