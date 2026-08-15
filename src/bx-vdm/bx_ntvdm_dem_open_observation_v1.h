#ifndef BX_NTVDM_DEM_OPEN_OBSERVATION_V1_H
#define BX_NTVDM_DEM_OPEN_OBSERVATION_V1_H

#include "bx_ntvdm_generic_ud_bridge.h"

/* Fixture-only, default-off integration evidence for the existing normal DEM
 * file package. It carries copied event/outcome fields only: no pathname,
 * guest bytes, provider identity, or provider state. */
typedef struct bx_ntvdm_dem_open_observation_v1 {
    uint32_t seen_open_count, captured;
    uint16_t cs, ds;
    uint32_t eip, eax, ebx, esi;
    uint32_t accepted, disposition;
    uint64_t resume_rip;
    uint32_t gpr16_write_mask, eflags_write_mask, eflags_values;
    uint16_t gpr16_values[BX_NTVDM_GENERIC_UD_GPR16_COUNT];
} bx_ntvdm_dem_open_observation_v1;

#ifdef __cplusplus
extern "C" {
#endif
void bx_ntvdm_dem_open_observation_v1_enable(uint32_t enabled);
int bx_ntvdm_dem_open_observation_v1_copy(bx_ntvdm_dem_open_observation_v1 *out);
void bx_ntvdm_dem_open_observation_v1_consider(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    const struct bx_ntvdm_generic_ud_outcome_v1 *outcome, int accepted);
#ifdef __cplusplus
}
#endif

#endif
