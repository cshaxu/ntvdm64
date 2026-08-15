#ifndef BX_NTVDM_NORMAL_TERMINAL_SEQUENCE_OBSERVATION_V1_H
#define BX_NTVDM_NORMAL_TERMINAL_SEQUENCE_OBSERVATION_V1_H

#include <stdint.h>

#include "bx_ntvdm_generic_ud_bridge.h"

#define BX_NTVDM_NORMAL_TERMINAL_SEQUENCE_OBSERVATION_V1_MAGIC UINT32_C(0x42584e53)
#define BX_NTVDM_NORMAL_TERMINAL_SEQUENCE_OBSERVATION_V1_VERSION UINT32_C(1)

/* Test-only, adapter-owned sequence evidence. It is never an engine result. */
struct bx_ntvdm_normal_terminal_sequence_observation_v1 {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t normal_return_seen;
    uint32_t stop_before_normal_seen;
    uint32_t stop_after_normal_seen;
    uint32_t reserved0;
    struct bx_ntvdm_generic_ud_event_v1 normal_event;
    struct bx_ntvdm_generic_ud_outcome_v1 normal_outcome;
    struct bx_ntvdm_generic_ud_event_v1 terminal_event;
    struct bx_ntvdm_generic_ud_outcome_v1 terminal_outcome;
};

#ifdef __cplusplus
extern "C" {
#endif

void bx_ntvdm_normal_terminal_sequence_observation_v1_enable(uint32_t enabled);
int bx_ntvdm_normal_terminal_sequence_observation_v1_copy(
    struct bx_ntvdm_normal_terminal_sequence_observation_v1 *value);
void bx_ntvdm_normal_terminal_sequence_observation_v1_consider(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    const struct bx_ntvdm_generic_ud_outcome_v1 *outcome);

#ifdef __cplusplus
}
#endif

#endif
