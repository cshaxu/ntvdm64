#include "multi_write_transaction.h"
#include <string.h>

void runtime_multi_write_transaction_initialize(
    runtime_multi_write_transaction *v,
    const runtime_exception_event *e,
    const runtime_cpu_state *c)
{
    if (!v) return;
    memset(v, 0, sizeof(*v));
    v->magic = RUNTIME_MULTI_WRITE_TRANSACTION_MAGIC;
    v->abi_version = RUNTIME_MULTI_WRITE_TRANSACTION_VERSION;
    v->struct_bytes = sizeof(*v);
    if (e) v->boundary = *e;
    if (c) v->cpu_before = *c;
    runtime_multi_write_initialize(&v->writes);
    runtime_cpu_result_pass_through(&v->result);
}

int runtime_multi_write_transaction_preflight(
    const runtime_multi_write_transaction *v,
    uint64_t aperture_bytes, uint64_t payload_bytes)
{
    return v != 0 && v->magic == RUNTIME_MULTI_WRITE_TRANSACTION_MAGIC &&
        v->abi_version == RUNTIME_MULTI_WRITE_TRANSACTION_VERSION &&
        v->struct_bytes == sizeof(*v) && v->flags == 0u &&
        runtime_exception_event_valid(&v->boundary) &&
        runtime_cpu_state_valid(&v->cpu_before) &&
        runtime_multi_write_preflight(&v->writes, aperture_bytes,
            payload_bytes) && runtime_cpu_result_valid(&v->result) &&
        v->result.disposition == RUNTIME_CPU_RESULT_RESUME &&
        v->result.resume_rip > v->boundary.fault_rip;
}
