#ifndef RUNTIME_DEM_OPEN_OBSERVATION_H
#define RUNTIME_DEM_OPEN_OBSERVATION_H

#include <stdint.h>

#include "adapter-softpc/cpu_state_abi.h"
#include "adapter-bop/generic_ud_bridge.h"

#define RUNTIME_DEM_OPEN_OBSERVATION_MAGIC UINT32_C(0x4258444f)
#define RUNTIME_DEM_OPEN_OBSERVATION_VERSION UINT32_C(1)
#define RUNTIME_DEM_OPEN_OBSERVATION_PATH_BYTES 261u

/* Original demfile.c:demOpen receives DS:SI full path and BL open mode. */
struct runtime_dem_open_observation {
    uint32_t magic, abi_version, struct_bytes;
    uint32_t enabled, observed, read_failed, observed_count;
    uint16_t ds, mode;
    uint32_t esi, physical_address;
    uint32_t disposition, gpr16_write_mask, eflags_write_mask, eflags_values;
    uint16_t gpr16_values[RUNTIME_GENERIC_UD_GPR16_COUNT];
    uint8_t path[RUNTIME_DEM_OPEN_OBSERVATION_PATH_BYTES];
};
typedef int (*runtime_dem_open_read)(void *state, uint64_t address,
    uint8_t *bytes, uint32_t byte_count);
#ifdef __cplusplus
extern "C" {
#endif
void runtime_dem_open_observation_enable(uint32_t enabled);
int runtime_dem_open_observation_copy(struct runtime_dem_open_observation *value);
void runtime_dem_open_observation_consider(const struct runtime_generic_ud_event *event,
    const struct runtime_generic_ud_outcome *outcome,
    runtime_dem_open_read read, void *read_state);
#ifdef __cplusplus
}
#endif
#endif
