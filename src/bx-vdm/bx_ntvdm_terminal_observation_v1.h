#ifndef BX_NTVDM_TERMINAL_OBSERVATION_V1_H
#define BX_NTVDM_TERMINAL_OBSERVATION_V1_H

#include <stdint.h>

#include "bx_ntvdm_generic_ud_bridge.h"

#define BX_NTVDM_TERMINAL_OBSERVATION_V1_MAGIC UINT32_C(0x4258544f)
#define BX_NTVDM_TERMINAL_OBSERVATION_V1_VERSION UINT32_C(1)

/* Test-only copied record of an already accepted generic STOP. The bridge
 * does not parse selector/service IDs here and product composition leaves
 * this facility disabled. */
struct bx_ntvdm_terminal_observation_v1 {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t captured;
    struct bx_ntvdm_generic_ud_event_v1 event;
    struct bx_ntvdm_generic_ud_outcome_v1 outcome;
};

#ifdef __cplusplus
extern "C" {
#endif

void bx_ntvdm_terminal_observation_v1_enable(uint32_t enabled);
/* Test-only count of accepted product-bridge calls.  It contains no guest,
 * selector, provider, or outcome data and is zero whenever disabled. */
uint32_t bx_ntvdm_terminal_observation_v1_considered_count(void);
/* Test-only count of accepted STOP outcomes; it carries no event data. */
uint32_t bx_ntvdm_terminal_observation_v1_stop_count(void);
int bx_ntvdm_terminal_observation_v1_copy(
    struct bx_ntvdm_terminal_observation_v1 *value);
void bx_ntvdm_terminal_observation_v1_consider(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    const struct bx_ntvdm_generic_ud_outcome_v1 *outcome);

#ifdef __cplusplus
}
#endif

#endif
