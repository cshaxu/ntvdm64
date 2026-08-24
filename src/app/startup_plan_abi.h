#ifndef BX_NTVDM_STARTUP_PLAN_ABI_H
#define BX_NTVDM_STARTUP_PLAN_ABI_H

#include <stdint.h>

#include "bx_ntvdm_cpu_state_abi.h"
#include "bx_ntvdm_guest_write_abi.h"

#define BX_NTVDM_STARTUP_PLAN_ABI_MAGIC 0x42585350u
#define BX_NTVDM_STARTUP_PLAN_ABI_VERSION 1u
#define BX_NTVDM_STARTUP_PLAN_MAX_PRESERVED_BYTES 64u

/* Pointer-free, one-shot guest start plan. Payload ownership stays in the
 * adapter session; the plan describes only a checked RAM write and an entry
 * delta. v1 permits only real-mode CS:IP; native Bochs reset supplies every
 * other CPU field. It contains no Bochs object or guest pointer. */
typedef struct bx_ntvdm_startup_plan_v1 {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t flags;
    bx_ntvdm_guest_write_v1 payload_write;
    bx_ntvdm_cpu_state_v1 entry_cpu;
    uint64_t preserved_state_address;
    uint64_t preserved_state_bytes;
} bx_ntvdm_startup_plan_v1;

#ifdef __cplusplus
extern "C" {
#endif

void bx_ntvdm_startup_plan_v1_initialize(bx_ntvdm_startup_plan_v1 *plan,
    const bx_ntvdm_guest_write_v1 *payload_write,
    const bx_ntvdm_cpu_state_v1 *entry_cpu,
    uint64_t preserved_state_address, uint64_t preserved_state_bytes);

/* Validates one complete plan before a backend sees any payload byte or CPU
 * mutation. The preserved range is a required capture-and-restore range: a
 * generic backend must copy it before the payload write and restore those
 * exact copied bytes before applying the CS:IP entry delta. It is not a
 * request to synthesize contents or a complete CPU-state image. */
int bx_ntvdm_startup_plan_v1_preflight(const bx_ntvdm_startup_plan_v1 *plan,
    uint64_t aperture_bytes, uint64_t payload_bytes);

#ifdef __cplusplus
}
#endif

#endif
