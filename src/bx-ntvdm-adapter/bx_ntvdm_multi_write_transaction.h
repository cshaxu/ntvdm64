#ifndef BX_NTVDM_MULTI_WRITE_TRANSACTION_H
#define BX_NTVDM_MULTI_WRITE_TRANSACTION_H

#include "bx_ntvdm_cpu_result_v2.h"
#include "bx_ntvdm_exception_abi.h"
#include "bx_ntvdm_multi_write_abi.h"

#define BX_NTVDM_MULTI_WRITE_TRANSACTION_MAGIC 0x4258544du
#define BX_NTVDM_MULTI_WRITE_TRANSACTION_VERSION 1u

typedef struct bx_ntvdm_multi_write_transaction_v1 {
    uint32_t magic, abi_version, struct_bytes, flags;
    bx_ntvdm_exception_event_v1 boundary;
    bx_ntvdm_cpu_state_v1 cpu_before;
    bx_ntvdm_multi_write_v1 writes;
    bx_ntvdm_cpu_result_v2 result;
} bx_ntvdm_multi_write_transaction_v1;

void bx_ntvdm_multi_write_transaction_v1_initialize(
    bx_ntvdm_multi_write_transaction_v1 *value,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before);
int bx_ntvdm_multi_write_transaction_v1_preflight(
    const bx_ntvdm_multi_write_transaction_v1 *value,
    uint64_t aperture_bytes, uint64_t payload_bytes);

#endif
