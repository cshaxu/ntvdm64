#ifndef RUNTIME_BULK_RESULT_TRANSACTION_H
#define RUNTIME_BULK_RESULT_TRANSACTION_H

#include <stdint.h>

#include "cpu_result.h"
#include "exception_abi.h"

#define RUNTIME_BULK_RESULT_TRANSACTION_V1_MAGIC 0x42584252u
#define RUNTIME_BULK_RESULT_TRANSACTION_V1_VERSION 1u
#define RUNTIME_BULK_RESULT_TRANSACTION_V1_MAX_PAYLOAD 65535u

/* One opaque, bounded result range. The payload is deliberately not embedded
 * and crosses only through the same-island take operation. */
typedef struct runtime_bulk_result_transaction_v1 {
    uint32_t magic, abi_version, struct_bytes, flags;
    runtime_exception_event_v1 boundary;
    runtime_cpu_state_v1 cpu_before;
    uint64_t guest_physical_address;
    uint32_t payload_bytes, reserved0;
    runtime_cpu_result_v2 result;
} runtime_bulk_result_transaction_v1;

void runtime_bulk_result_transaction_v1_initialize(
    runtime_bulk_result_transaction_v1 *value,
    const runtime_exception_event_v1 *boundary,
    const runtime_cpu_state_v1 *cpu_before);
int runtime_bulk_result_transaction_v1_prepare(
    runtime_bulk_result_transaction_v1 *value, uint64_t guest_physical_address,
    uint32_t payload_bytes, const runtime_cpu_result_v2 *result);
int runtime_bulk_result_transaction_v1_preflight(
    const runtime_bulk_result_transaction_v1 *value,
    uint64_t aperture_bytes, uint64_t supplied_payload_bytes);

#endif
