#include "bx_ntvdm_multi_write_transaction.h"
#include <string.h>

void bx_ntvdm_multi_write_transaction_v1_initialize(
    bx_ntvdm_multi_write_transaction_v1 *v,
    const bx_ntvdm_exception_event_v1 *e,
    const bx_ntvdm_cpu_state_v1 *c)
{
    if (!v) return;
    memset(v, 0, sizeof(*v));
    v->magic = BX_NTVDM_MULTI_WRITE_TRANSACTION_MAGIC;
    v->abi_version = BX_NTVDM_MULTI_WRITE_TRANSACTION_VERSION;
    v->struct_bytes = sizeof(*v);
    if (e) v->boundary = *e;
    if (c) v->cpu_before = *c;
    bx_ntvdm_multi_write_v1_initialize(&v->writes);
    bx_ntvdm_cpu_result_v2_pass_through(&v->result);
}

int bx_ntvdm_multi_write_transaction_v1_preflight(
    const bx_ntvdm_multi_write_transaction_v1 *v,
    uint64_t aperture_bytes, uint64_t payload_bytes)
{
    return v != 0 && v->magic == BX_NTVDM_MULTI_WRITE_TRANSACTION_MAGIC &&
        v->abi_version == BX_NTVDM_MULTI_WRITE_TRANSACTION_VERSION &&
        v->struct_bytes == sizeof(*v) && v->flags == 0u &&
        bx_ntvdm_exception_event_v1_valid(&v->boundary) &&
        bx_ntvdm_cpu_state_v1_valid(&v->cpu_before) &&
        bx_ntvdm_multi_write_v1_preflight(&v->writes, aperture_bytes,
            payload_bytes) && bx_ntvdm_cpu_result_v2_valid(&v->result) &&
        v->result.disposition == BX_NTVDM_CPU_RESULT_V2_RESUME &&
        v->result.resume_rip > v->boundary.fault_rip;
}
