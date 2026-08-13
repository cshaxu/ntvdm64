#ifndef BX_NTVDM_PENDING_ACTION_V1_H
#define BX_NTVDM_PENDING_ACTION_V1_H

#include <stdint.h>
#include "bx_ntvdm_cpu_result_v2.h"
#include "bx_ntvdm_guest_range.h"
#include "bx_ntvdm_guest_gather_read_action_v1.h"
#include "bx_ntvdm_multi_write_abi.h"

#define BX_NTVDM_PENDING_ACTION_V1_MAGIC 0x42585041u
#define BX_NTVDM_PENDING_ACTION_V1_VERSION 1u

enum bx_ntvdm_pending_action_v1_kind {
    BX_NTVDM_PENDING_ACTION_V1_NONE = 0u,
    BX_NTVDM_PENDING_ACTION_V1_READ = 1u,
    BX_NTVDM_PENDING_ACTION_V1_GATHER_READ = 2u,
    BX_NTVDM_PENDING_ACTION_V1_MULTI_WRITE = 3u,
    BX_NTVDM_PENDING_ACTION_V1_BULK_WRITE = 4u
};

/* Fixed copied mechanics request. Payload bytes are deliberately absent: the
 * selected same-island producer retains them until mantle acknowledges the
 * exact action id. */
typedef struct bx_ntvdm_pending_action_v1 {
    uint32_t magic, abi_version, struct_bytes, kind;
    uint32_t action_id, range_count;
    uint64_t total_bytes;
    bx_ntvdm_cpu_result_v2 result;
    bx_ntvdm_guest_range ranges[BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_MAX_RANGES];
    bx_ntvdm_multi_write_v1 writes;
} bx_ntvdm_pending_action_v1;

#ifdef __cplusplus
extern "C" {
#endif
void bx_ntvdm_pending_action_v1_clear(bx_ntvdm_pending_action_v1 *action);
int bx_ntvdm_pending_action_v1_valid(const bx_ntvdm_pending_action_v1 *action,
    uint64_t aperture_bytes);
#ifdef __cplusplus
}
#endif
#endif
