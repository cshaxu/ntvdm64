#ifndef BX_NTVDM_GUEST_READ_ACTION_V1_H
#define BX_NTVDM_GUEST_READ_ACTION_V1_H

#include <stdint.h>

#include "bx_ntvdm_cpu_result_v2.h"
#include "bx_ntvdm_guest_range.h"

#define BX_NTVDM_GUEST_READ_ACTION_V1_MAGIC 0x42584752u
#define BX_NTVDM_GUEST_READ_ACTION_V1_VERSION 1u
#define BX_NTVDM_GUEST_READ_ACTION_V1_MAX_BYTES 128u

enum bx_ntvdm_guest_read_action_v1_disposition {
    BX_NTVDM_GUEST_READ_ACTION_V1_PASS_THROUGH = 0u,
    BX_NTVDM_GUEST_READ_ACTION_V1_RESUME = 1u,
    BX_NTVDM_GUEST_READ_ACTION_V1_NEED_READ = 2u,
    BX_NTVDM_GUEST_READ_ACTION_V1_STOP = 3u
};

/* One generic adapter outcome. NEED_READ contains an opaque physical range;
 * neither this record nor its Bochs consumer carries a guest or host pointer. */
typedef struct bx_ntvdm_guest_read_action_v1 {
    uint32_t magic, abi_version, struct_bytes, disposition;
    bx_ntvdm_cpu_result_v2 cpu_result;
    bx_ntvdm_guest_range guest_read;
} bx_ntvdm_guest_read_action_v1;

#ifdef __cplusplus
extern "C" {
#endif
void bx_ntvdm_guest_read_action_v1_pass_through(
    bx_ntvdm_guest_read_action_v1 *action);
int bx_ntvdm_guest_read_action_v1_resume(
    bx_ntvdm_guest_read_action_v1 *action, uint64_t resume_rip);
int bx_ntvdm_guest_read_action_v1_stop(bx_ntvdm_guest_read_action_v1 *action);
int bx_ntvdm_guest_read_action_v1_need_read(
    bx_ntvdm_guest_read_action_v1 *action, uint64_t address, uint64_t length);
int bx_ntvdm_guest_read_action_v1_valid(
    const bx_ntvdm_guest_read_action_v1 *action);
#ifdef __cplusplus
}
#endif

#endif
