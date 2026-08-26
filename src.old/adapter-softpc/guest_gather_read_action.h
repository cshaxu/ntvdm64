#ifndef RUNTIME_GUEST_GATHER_READ_ACTION_H
#define RUNTIME_GUEST_GATHER_READ_ACTION_H

#include <stdint.h>

#include "cpu_result.h"
#include "guest_range.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Generic mechanical gather transport. It intentionally has no consumer,
 * selector, service, guest pointer, or host pointer field. */
#define RUNTIME_GUEST_GATHER_READ_ACTION_MAGIC 0x42584747u
#define RUNTIME_GUEST_GATHER_READ_ACTION_VERSION 2u
#define RUNTIME_GUEST_GATHER_READ_ACTION_MAX_RANGES 4u
/* Two bounded OpenNT MAX_PATH OEM strings are the largest admitted DEM
 * pathname transaction (demRename).  This remains far below the machine's
 * generic mechanical-action limit and does not admit unbounded inspection. */
#define RUNTIME_GUEST_GATHER_READ_ACTION_MAX_TOTAL_BYTES 520u

enum runtime_guest_gather_read_action_disposition {
    RUNTIME_GUEST_GATHER_READ_ACTION_PASS_THROUGH = 0u,
    RUNTIME_GUEST_GATHER_READ_ACTION_RESUME = 1u,
    RUNTIME_GUEST_GATHER_READ_ACTION_NEED_READ = 2u,
    RUNTIME_GUEST_GATHER_READ_ACTION_STOP = 3u
};

typedef struct runtime_guest_gather_read_action {
    uint32_t magic, abi_version, struct_bytes, disposition;
    runtime_cpu_result cpu_result;
    uint32_t range_count;
    uint32_t total_bytes;
    runtime_guest_range ranges[RUNTIME_GUEST_GATHER_READ_ACTION_MAX_RANGES];
} runtime_guest_gather_read_action;

void runtime_guest_gather_read_action_pass_through(
    runtime_guest_gather_read_action *action);
int runtime_guest_gather_read_action_resume(
    runtime_guest_gather_read_action *action, uint64_t resume_rip);
int runtime_guest_gather_read_action_stop(runtime_guest_gather_read_action *action);
int runtime_guest_gather_read_action_need_read(
    runtime_guest_gather_read_action *action,
    const runtime_guest_range *ranges, uint32_t range_count);
/* A gather request may carry an already checked resume candidate; the caller
 * must not apply it until the matching copied-range completion succeeds. */
int runtime_guest_gather_read_action_need_read_resume(
    runtime_guest_gather_read_action *action,
    const runtime_guest_range *ranges, uint32_t range_count,
    uint64_t resume_rip);
int runtime_guest_gather_read_action_valid(
    const runtime_guest_gather_read_action *action);

#ifdef __cplusplus
}
#endif

#endif
