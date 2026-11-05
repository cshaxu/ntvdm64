#ifndef RUNTIME_HOST_TRANSACTION_ABI_H
#define RUNTIME_HOST_TRANSACTION_ABI_H

#include <stdint.h>

#include "cpu_state_abi.h"
#include "guest_write_abi.h"
#include "runtime_host_service_contract.h"

#define RUNTIME_HOST_TRANSACTION_ABI_MAGIC 0x42585458u
#define RUNTIME_HOST_TRANSACTION_ABI_VERSION 1u

/* One preflighted request. The payload remains a separate same-island byte
 * region; this record contains neither payload nor any pointer. */
typedef struct runtime_host_transaction {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t flags;
    runtime_host_service_request request;
    uint32_t reserved0;
    runtime_cpu_state cpu_before;
    runtime_guest_write guest_write;
} runtime_host_transaction;

#ifdef __cplusplus
extern "C" {
#endif

void runtime_host_transaction_initialize(
    runtime_host_transaction *transaction,
    const runtime_host_service_request *request,
    const runtime_cpu_state *cpu_before,
    const runtime_guest_write *guest_write);
int runtime_host_transaction_preflight(
    const runtime_host_transaction *transaction,
    uint64_t aperture_bytes, uint64_t payload_bytes);

#ifdef __cplusplus
}
#endif

#endif
