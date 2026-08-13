#ifndef BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_H
#define BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_H

#include <stdint.h>

#include "bx_ntvdm_cpu_result_v2.h"
#include "bx_ntvdm_guest_range.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Generic mechanical gather transport. It intentionally has no consumer,
 * selector, service, guest pointer, or host pointer field. */
#define BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_MAGIC 0x42584747u
#define BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_VERSION 1u
#define BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_MAX_RANGES 4u
#define BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_MAX_TOTAL_BYTES 256u

enum bx_ntvdm_guest_gather_read_action_v1_disposition {
    BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_PASS_THROUGH = 0u,
    BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_RESUME = 1u,
    BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_NEED_READ = 2u,
    BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_STOP = 3u
};

typedef struct bx_ntvdm_guest_gather_read_action_v1 {
    uint32_t magic, abi_version, struct_bytes, disposition;
    bx_ntvdm_cpu_result_v2 cpu_result;
    uint32_t range_count;
    uint32_t total_bytes;
    bx_ntvdm_guest_range ranges[BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_MAX_RANGES];
} bx_ntvdm_guest_gather_read_action_v1;

void bx_ntvdm_guest_gather_read_action_v1_pass_through(
    bx_ntvdm_guest_gather_read_action_v1 *action);
int bx_ntvdm_guest_gather_read_action_v1_resume(
    bx_ntvdm_guest_gather_read_action_v1 *action, uint64_t resume_rip);
int bx_ntvdm_guest_gather_read_action_v1_stop(bx_ntvdm_guest_gather_read_action_v1 *action);
int bx_ntvdm_guest_gather_read_action_v1_need_read(
    bx_ntvdm_guest_gather_read_action_v1 *action,
    const bx_ntvdm_guest_range *ranges, uint32_t range_count);
/* A gather request may carry an already checked resume candidate; the caller
 * must not apply it until the matching copied-range completion succeeds. */
int bx_ntvdm_guest_gather_read_action_v1_need_read_resume(
    bx_ntvdm_guest_gather_read_action_v1 *action,
    const bx_ntvdm_guest_range *ranges, uint32_t range_count,
    uint64_t resume_rip);
int bx_ntvdm_guest_gather_read_action_v1_valid(
    const bx_ntvdm_guest_gather_read_action_v1 *action);

#ifdef __cplusplus
}
#endif

#endif
