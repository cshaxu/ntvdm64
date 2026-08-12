#include "bx_ntvdm_bulk_result_transaction.h"

#include <assert.h>
#include <stdio.h>

int main(void)
{
    bx_ntvdm_exception_event_v1 event = {
        BX_NTVDM_EXCEPTION_ABI_MAGIC, BX_NTVDM_EXCEPTION_ABI_VERSION,
        sizeof(event), BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION, 0u, 6u,
        0u, 0u, 0x100u };
    bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_cpu_result_v2 result;
    bx_ntvdm_bulk_result_transaction_v1 tx;
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    assert(bx_ntvdm_cpu_result_v2_resume(&result, 0x104u));
    bx_ntvdm_bulk_result_transaction_v1_initialize(&tx, &event, &cpu);
    assert(bx_ntvdm_bulk_result_transaction_v1_prepare(&tx, 0x200u,
        BX_NTVDM_BULK_RESULT_TRANSACTION_V1_MAX_PAYLOAD, &result));
    assert(bx_ntvdm_bulk_result_transaction_v1_preflight(&tx, 0x100000u,
        BX_NTVDM_BULK_RESULT_TRANSACTION_V1_MAX_PAYLOAD));
    assert(!bx_ntvdm_bulk_result_transaction_v1_preflight(&tx, 0x100000u,
        1u));
    assert(bx_ntvdm_bulk_result_transaction_v1_prepare(&tx, 0x100000u,
        1u, &result));
    assert(!bx_ntvdm_bulk_result_transaction_v1_preflight(&tx, 0x100000u,
        1u));
    assert(bx_ntvdm_bulk_result_transaction_v1_prepare(&tx, 0u, 0u, &result));
    assert(bx_ntvdm_bulk_result_transaction_v1_preflight(&tx, 0x100000u, 0u));
    assert(!bx_ntvdm_bulk_result_transaction_v1_prepare(&tx, 1u, 0u, &result));
    result.resume_rip = event.fault_rip;
    assert(bx_ntvdm_bulk_result_transaction_v1_prepare(&tx, 0u, 0u, &result));
    assert(!bx_ntvdm_bulk_result_transaction_v1_preflight(&tx, 0x100000u, 0u));
    puts("bx-ntvdm bulk-result transaction: bounded single-range contract verified");
    return 0;
}
