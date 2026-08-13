#ifndef BX_NTVDM_OBSERVATION_TRANSACTION_ABI_H
#define BX_NTVDM_OBSERVATION_TRANSACTION_ABI_H

#include <stdint.h>

#include "bx_ntvdm_cpu_state_abi.h"
#include "bx_ntvdm_exception_abi.h"
#include "bx_ntvdm_guest_range.h"

#define BX_NTVDM_OBSERVATION_TRANSACTION_ABI_MAGIC 0x42584f54u
#define BX_NTVDM_OBSERVATION_TRANSACTION_ABI_VERSION 1u

/* A bounded read requested at one copied #UD exception boundary. The output
 * bytes are deliberately outside this record and remain adapter-owned in the
 * same process island. This ABI contains no guest, host or Bochs pointer. */
typedef struct bx_ntvdm_observation_transaction_v1 {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t flags;
    bx_ntvdm_exception_event_v1 boundary;
    bx_ntvdm_cpu_state_v1 cpu_before;
    bx_ntvdm_guest_range guest_read;
} bx_ntvdm_observation_transaction_v1;

#ifdef __cplusplus
extern "C" {
#endif

void bx_ntvdm_observation_transaction_v1_initialize(
    bx_ntvdm_observation_transaction_v1 *transaction,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_guest_range *guest_read);
int bx_ntvdm_observation_transaction_v1_preflight(
    const bx_ntvdm_observation_transaction_v1 *transaction,
    uint64_t aperture_bytes, uint64_t output_bytes);

#ifdef __cplusplus
}
#endif

#endif
