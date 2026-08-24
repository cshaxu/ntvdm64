#ifndef RUNTIME_PENDING_ACTION_V1_H
#define RUNTIME_PENDING_ACTION_V1_H

#include <stdint.h>
#include "cpu_result.h"
#include "guest_range.h"
#include "guest_gather_read_action.h"
#include "multi_write_abi.h"

#define RUNTIME_PENDING_ACTION_V1_MAGIC 0x42585041u
#define RUNTIME_PENDING_ACTION_V1_VERSION 1u

enum runtime_pending_action_v1_kind {
    RUNTIME_PENDING_ACTION_V1_NONE = 0u,
    RUNTIME_PENDING_ACTION_V1_READ = 1u,
    RUNTIME_PENDING_ACTION_V1_GATHER_READ = 2u,
    RUNTIME_PENDING_ACTION_V1_MULTI_WRITE = 3u,
    RUNTIME_PENDING_ACTION_V1_BULK_WRITE = 4u
};

/* Fixed copied mechanics request. Payload bytes are deliberately absent: the
 * selected same-island producer retains them until mantle acknowledges the
 * exact action id. */
typedef struct runtime_pending_action_v1 {
    uint32_t magic, abi_version, struct_bytes, kind;
    uint32_t action_id, range_count;
    uint64_t total_bytes;
    runtime_cpu_result_v2 result;
    runtime_guest_range ranges[RUNTIME_GUEST_GATHER_READ_ACTION_V1_MAX_RANGES];
    runtime_multi_write_v1 writes;
} runtime_pending_action_v1;

#ifdef __cplusplus
extern "C" {
#endif
void runtime_pending_action_v1_clear(runtime_pending_action_v1 *action);
int runtime_pending_action_v1_valid(const runtime_pending_action_v1 *action,
    uint64_t aperture_bytes);
#ifdef __cplusplus
}
#endif
#endif
