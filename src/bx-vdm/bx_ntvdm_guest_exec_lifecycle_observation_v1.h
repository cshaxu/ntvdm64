#ifndef BX_NTVDM_GUEST_EXEC_LIFECYCLE_OBSERVATION_V1_H
#define BX_NTVDM_GUEST_EXEC_LIFECYCLE_OBSERVATION_V1_H

#include "bx_ntvdm_generic_ud_bridge.h"
#include <stdint.h>

#define BX_NTVDM_GUEST_EXEC_LIFECYCLE_OBSERVATION_V1_MAGIC UINT32_C(0x4258454c)
#define BX_NTVDM_GUEST_EXEC_LIFECYCLE_OBSERVATION_V1_VERSION UINT32_C(1)

/* Test-only copied evidence of the original NTDOS/COMMAND lifecycle.  This
 * observer neither reads guest memory nor participates in BOP routing. */
struct bx_ntvdm_guest_exec_lifecycle_observation_v1 {
    uint32_t magic, abi_version, struct_bytes;
    uint32_t entry_seen, terminate_seen, parent_return_seen, out_of_order_seen;
    struct bx_ntvdm_generic_ud_event_v1 entry_event;
    struct bx_ntvdm_generic_ud_outcome_v1 entry_outcome;
    struct bx_ntvdm_generic_ud_event_v1 terminate_event;
    struct bx_ntvdm_generic_ud_outcome_v1 terminate_outcome;
    struct bx_ntvdm_generic_ud_event_v1 parent_return_event;
    struct bx_ntvdm_generic_ud_outcome_v1 parent_return_outcome;
};
#ifdef __cplusplus
extern "C" {
#endif
void bx_ntvdm_guest_exec_lifecycle_observation_v1_enable(uint32_t enabled);
int bx_ntvdm_guest_exec_lifecycle_observation_v1_copy(struct bx_ntvdm_guest_exec_lifecycle_observation_v1 *value);
void bx_ntvdm_guest_exec_lifecycle_observation_v1_consider(const struct bx_ntvdm_generic_ud_event_v1 *event, const struct bx_ntvdm_generic_ud_outcome_v1 *outcome);
#ifdef __cplusplus
}
#endif
#endif