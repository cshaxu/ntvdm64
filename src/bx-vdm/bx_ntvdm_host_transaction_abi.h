#ifndef BX_NTVDM_HOST_TRANSACTION_ABI_H
#define BX_NTVDM_HOST_TRANSACTION_ABI_H

#include <stdint.h>

#include "bx_ntvdm_cpu_state_abi.h"
#include "bx_ntvdm_guest_write_abi.h"
#include "bop-v1/bx_ntvdm_host_service_catalog.h"

#define BX_NTVDM_HOST_TRANSACTION_ABI_MAGIC 0x42585458u
#define BX_NTVDM_HOST_TRANSACTION_ABI_VERSION 1u

/* One preflighted request. The payload remains a separate same-island byte
 * region; this record contains neither payload nor any pointer. */
typedef struct bx_ntvdm_host_transaction_v1 {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t flags;
    bx_ntvdm_host_service_request_v1 request;
    uint32_t reserved0;
    bx_ntvdm_cpu_state_v1 cpu_before;
    bx_ntvdm_guest_write_v1 guest_write;
} bx_ntvdm_host_transaction_v1;

#ifdef __cplusplus
extern "C" {
#endif

void bx_ntvdm_host_transaction_v1_initialize(
    bx_ntvdm_host_transaction_v1 *transaction,
    const bx_ntvdm_host_service_request_v1 *request,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_guest_write_v1 *guest_write);
int bx_ntvdm_host_transaction_v1_preflight(
    const bx_ntvdm_host_transaction_v1 *transaction,
    uint64_t aperture_bytes, uint64_t payload_bytes);

#ifdef __cplusplus
}
#endif

#endif
