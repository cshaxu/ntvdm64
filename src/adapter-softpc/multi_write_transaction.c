#include "multi_write_transaction.h"
#include <string.h>

void runtime_multi_write_transaction_v1_initialize(
    runtime_multi_write_transaction_v1 *v,
    const runtime_exception_event_v1 *e,
    const runtime_cpu_state_v1 *c)
{
    if (!v) return;
    memset(v, 0, sizeof(*v));
    v->magic = RUNTIME_MULTI_WRITE_TRANSACTION_MAGIC;
    v->abi_version = RUNTIME_MULTI_WRITE_TRANSACTION_VERSION;
    v->struct_bytes = sizeof(*v);
    if (e) v->boundary = *e;
    if (c) v->cpu_before = *c;
    runtime_multi_write_v1_initialize(&v->writes);
    runtime_cpu_result_v2_pass_through(&v->result);
}

int runtime_multi_write_transaction_v1_preflight(
    const runtime_multi_write_transaction_v1 *v,
    uint64_t aperture_bytes, uint64_t payload_bytes)
{
    return v != 0 && v->magic == RUNTIME_MULTI_WRITE_TRANSACTION_MAGIC &&
        v->abi_version == RUNTIME_MULTI_WRITE_TRANSACTION_VERSION &&
        v->struct_bytes == sizeof(*v) && v->flags == 0u &&
        runtime_exception_event_v1_valid(&v->boundary) &&
        runtime_cpu_state_v1_valid(&v->cpu_before) &&
        runtime_multi_write_v1_preflight(&v->writes, aperture_bytes,
            payload_bytes) && runtime_cpu_result_v2_valid(&v->result) &&
        v->result.disposition == RUNTIME_CPU_RESULT_V2_RESUME &&
        v->result.resume_rip > v->boundary.fault_rip;
}
