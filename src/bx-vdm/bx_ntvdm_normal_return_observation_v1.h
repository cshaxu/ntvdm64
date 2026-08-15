#ifndef BX_NTVDM_NORMAL_RETURN_OBSERVATION_V1_H
#define BX_NTVDM_NORMAL_RETURN_OBSERVATION_V1_H

#include <stdint.h>

#include "bx_ntvdm_generic_ud_bridge.h"

#define BX_NTVDM_NORMAL_RETURN_OBSERVATION_V1_MAGIC UINT32_C(0x42584e52)
#define BX_NTVDM_NORMAL_RETURN_OBSERVATION_V1_VERSION UINT32_C(1)

/* Test-only copied evidence of an already accepted COMMAND return-exit-code
 * resume.  It neither owns COMMAND nor supplies an engine result. */
struct bx_ntvdm_normal_return_observation_v1 {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t captured;
    uint16_t input_dx;
    uint16_t returned_ax;
    struct bx_ntvdm_generic_ud_event_v1 event;
    struct bx_ntvdm_generic_ud_outcome_v1 outcome;
};

#ifdef __cplusplus
extern "C" {
#endif

void bx_ntvdm_normal_return_observation_v1_enable(uint32_t enabled);
int bx_ntvdm_normal_return_observation_v1_copy(
    struct bx_ntvdm_normal_return_observation_v1 *value);
void bx_ntvdm_normal_return_observation_v1_consider(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    const struct bx_ntvdm_generic_ud_outcome_v1 *outcome);

#ifdef __cplusplus
}
#endif

#endif
