#ifndef BX_NTVDM_DEM_OPEN_OBSERVATION_V1_H
#define BX_NTVDM_DEM_OPEN_OBSERVATION_V1_H

#include <stdint.h>

#include "bx_ntvdm_cpu_state_abi.h"
#include "bx-mantle/bx_ntvdm_generic_ud_bridge.h"

#define BX_NTVDM_DEM_OPEN_OBSERVATION_V1_MAGIC UINT32_C(0x4258444f)
#define BX_NTVDM_DEM_OPEN_OBSERVATION_V1_VERSION UINT32_C(1)
#define BX_NTVDM_DEM_OPEN_OBSERVATION_V1_PATH_BYTES 261u

/* Original demfile.c:demOpen receives DS:SI full path and BL open mode. */
struct bx_ntvdm_dem_open_observation_v1 {
    uint32_t magic, abi_version, struct_bytes;
    uint32_t enabled, observed, read_failed;
    uint16_t ds, mode;
    uint32_t esi, physical_address;
    uint32_t disposition, gpr16_write_mask, eflags_write_mask, eflags_values;
    uint16_t gpr16_values[BX_NTVDM_GENERIC_UD_GPR16_COUNT];
    uint8_t path[BX_NTVDM_DEM_OPEN_OBSERVATION_V1_PATH_BYTES];
};
typedef int (*bx_ntvdm_dem_open_read_v1)(void *state, uint64_t address,
    uint8_t *bytes, uint32_t byte_count);
#ifdef __cplusplus
extern "C" {
#endif
void bx_ntvdm_dem_open_observation_v1_enable(uint32_t enabled);
int bx_ntvdm_dem_open_observation_v1_copy(struct bx_ntvdm_dem_open_observation_v1 *value);
void bx_ntvdm_dem_open_observation_v1_consider(const struct bx_ntvdm_generic_ud_event_v1 *event,
    const struct bx_ntvdm_generic_ud_outcome_v1 *outcome,
    bx_ntvdm_dem_open_read_v1 read, void *read_state);
#ifdef __cplusplus
}
#endif
#endif
