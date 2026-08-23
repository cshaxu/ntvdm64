#ifndef BX_NTVDM_BULK_RESULT_TRANSACTION_H
#define BX_NTVDM_BULK_RESULT_TRANSACTION_H

#include <stdint.h>

#include "bx_ntvdm_cpu_result_v2.h"
#include "bx_ntvdm_exception_abi.h"

#define BX_NTVDM_BULK_RESULT_TRANSACTION_V1_MAGIC 0x42584252u
#define BX_NTVDM_BULK_RESULT_TRANSACTION_V1_VERSION 1u
#define BX_NTVDM_BULK_RESULT_TRANSACTION_V1_MAX_PAYLOAD 65535u

/* One opaque, bounded result range. The payload is deliberately not embedded
 * and crosses only through the same-island take operation. */
typedef struct bx_ntvdm_bulk_result_transaction_v1 {
    uint32_t magic, abi_version, struct_bytes, flags;
    bx_ntvdm_exception_event_v1 boundary;
    bx_ntvdm_cpu_state_v1 cpu_before;
    uint64_t guest_physical_address;
    uint32_t payload_bytes, reserved0;
    bx_ntvdm_cpu_result_v2 result;
} bx_ntvdm_bulk_result_transaction_v1;

void bx_ntvdm_bulk_result_transaction_v1_initialize(
    bx_ntvdm_bulk_result_transaction_v1 *value,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before);
int bx_ntvdm_bulk_result_transaction_v1_prepare(
    bx_ntvdm_bulk_result_transaction_v1 *value, uint64_t guest_physical_address,
    uint32_t payload_bytes, const bx_ntvdm_cpu_result_v2 *result);
int bx_ntvdm_bulk_result_transaction_v1_preflight(
    const bx_ntvdm_bulk_result_transaction_v1 *value,
    uint64_t aperture_bytes, uint64_t supplied_payload_bytes);

#endif
