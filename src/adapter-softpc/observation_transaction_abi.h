#ifndef RUNTIME_OBSERVATION_TRANSACTION_ABI_H
#define RUNTIME_OBSERVATION_TRANSACTION_ABI_H

#include <stdint.h>

#include "cpu_state_abi.h"
#include "exception_abi.h"
#include "guest_range.h"

#define RUNTIME_OBSERVATION_TRANSACTION_ABI_MAGIC 0x42584f54u
#define RUNTIME_OBSERVATION_TRANSACTION_ABI_VERSION 1u

/* A bounded read requested at one copied #UD exception boundary. The output
 * bytes are deliberately outside this record and remain adapter-owned in the
 * same process island. This ABI contains no guest, host or Bochs pointer. */
typedef struct runtime_observation_transaction_v1 {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t flags;
    runtime_exception_event_v1 boundary;
    runtime_cpu_state_v1 cpu_before;
    runtime_guest_range guest_read;
} runtime_observation_transaction_v1;

#ifdef __cplusplus
extern "C" {
#endif

void runtime_observation_transaction_v1_initialize(
    runtime_observation_transaction_v1 *transaction,
    const runtime_exception_event_v1 *boundary,
    const runtime_cpu_state_v1 *cpu_before,
    const runtime_guest_range *guest_read);
int runtime_observation_transaction_v1_preflight(
    const runtime_observation_transaction_v1 *transaction,
    uint64_t aperture_bytes, uint64_t output_bytes);

#ifdef __cplusplus
}
#endif

#endif
