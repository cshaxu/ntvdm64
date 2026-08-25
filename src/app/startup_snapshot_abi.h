#ifndef RUNTIME_STARTUP_SNAPSHOT_ABI_H
#define RUNTIME_STARTUP_SNAPSHOT_ABI_H

#include <stdint.h>

#include "cpu_state_abi.h"
#include "exception_abi.h"
#include "guest_range.h"

#define RUNTIME_STARTUP_SNAPSHOT_ABI_MAGIC 0x42585353u
#define RUNTIME_STARTUP_SNAPSHOT_ABI_VERSION 1u
#define RUNTIME_STARTUP_SNAPSHOT_MAX_RANGES 4u

/* A profile-owned opaque range.  Neither its identifier nor bytes convey
 * firmware, BIOS, DOS, BOP, DEM or Bochs semantics. */
typedef struct runtime_startup_snapshot_range {
    uint32_t id;
    uint32_t flags;
    runtime_guest_range guest_read;
} runtime_startup_snapshot_range;

/* One all-or-nothing multi-range observation tied to one copied #UD boundary.
 * Output bytes and pointers remain solely in the adapter process island. */
typedef struct runtime_startup_snapshot_transaction {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t flags;
    runtime_exception_event boundary;
    runtime_cpu_state cpu_before;
    uint32_t range_count;
    uint32_t reserved0;
    uint64_t output_bytes;
    runtime_startup_snapshot_range ranges[RUNTIME_STARTUP_SNAPSHOT_MAX_RANGES];
} runtime_startup_snapshot_transaction;

#ifdef __cplusplus
extern "C" {
#endif

void runtime_startup_snapshot_transaction_initialize(
    runtime_startup_snapshot_transaction *transaction,
    const runtime_exception_event *boundary,
    const runtime_cpu_state *cpu_before,
    const runtime_startup_snapshot_range *ranges, uint32_t range_count);
int runtime_startup_snapshot_transaction_preflight(
    const runtime_startup_snapshot_transaction *transaction,
    uint64_t aperture_bytes, uint64_t output_capacity);

#ifdef __cplusplus
}
#endif

#endif
