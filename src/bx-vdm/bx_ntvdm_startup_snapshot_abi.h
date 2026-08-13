#ifndef BX_NTVDM_STARTUP_SNAPSHOT_ABI_H
#define BX_NTVDM_STARTUP_SNAPSHOT_ABI_H

#include <stdint.h>

#include "bx_ntvdm_cpu_state_abi.h"
#include "bx_ntvdm_exception_abi.h"
#include "bx_ntvdm_guest_range.h"

#define BX_NTVDM_STARTUP_SNAPSHOT_ABI_MAGIC 0x42585353u
#define BX_NTVDM_STARTUP_SNAPSHOT_ABI_VERSION 1u
#define BX_NTVDM_STARTUP_SNAPSHOT_V1_MAX_RANGES 4u

/* A profile-owned opaque range.  Neither its identifier nor bytes convey
 * firmware, BIOS, DOS, BOP, DEM or Bochs semantics. */
typedef struct bx_ntvdm_startup_snapshot_range_v1 {
    uint32_t id;
    uint32_t flags;
    bx_ntvdm_guest_range guest_read;
} bx_ntvdm_startup_snapshot_range_v1;

/* One all-or-nothing multi-range observation tied to one copied #UD boundary.
 * Output bytes and pointers remain solely in the adapter process island. */
typedef struct bx_ntvdm_startup_snapshot_transaction_v1 {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t flags;
    bx_ntvdm_exception_event_v1 boundary;
    bx_ntvdm_cpu_state_v1 cpu_before;
    uint32_t range_count;
    uint32_t reserved0;
    uint64_t output_bytes;
    bx_ntvdm_startup_snapshot_range_v1 ranges[BX_NTVDM_STARTUP_SNAPSHOT_V1_MAX_RANGES];
} bx_ntvdm_startup_snapshot_transaction_v1;

#ifdef __cplusplus
extern "C" {
#endif

void bx_ntvdm_startup_snapshot_transaction_v1_initialize(
    bx_ntvdm_startup_snapshot_transaction_v1 *transaction,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_startup_snapshot_range_v1 *ranges, uint32_t range_count);
int bx_ntvdm_startup_snapshot_transaction_v1_preflight(
    const bx_ntvdm_startup_snapshot_transaction_v1 *transaction,
    uint64_t aperture_bytes, uint64_t output_capacity);

#ifdef __cplusplus
}
#endif

#endif
