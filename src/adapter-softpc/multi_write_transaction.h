#ifndef RUNTIME_MULTI_WRITE_TRANSACTION_H
#define RUNTIME_MULTI_WRITE_TRANSACTION_H

#include "cpu_result.h"
#include "exception_abi.h"
#include "multi_write_abi.h"

#define RUNTIME_MULTI_WRITE_TRANSACTION_MAGIC 0x4258544du
#define RUNTIME_MULTI_WRITE_TRANSACTION_VERSION 1u

typedef struct runtime_multi_write_transaction_v1 {
    uint32_t magic, abi_version, struct_bytes, flags;
    runtime_exception_event_v1 boundary;
    runtime_cpu_state_v1 cpu_before;
    runtime_multi_write_v1 writes;
    runtime_cpu_result_v2 result;
} runtime_multi_write_transaction_v1;

void runtime_multi_write_transaction_v1_initialize(
    runtime_multi_write_transaction_v1 *value,
    const runtime_exception_event_v1 *boundary,
    const runtime_cpu_state_v1 *cpu_before);
int runtime_multi_write_transaction_v1_preflight(
    const runtime_multi_write_transaction_v1 *value,
    uint64_t aperture_bytes, uint64_t payload_bytes);

#endif
