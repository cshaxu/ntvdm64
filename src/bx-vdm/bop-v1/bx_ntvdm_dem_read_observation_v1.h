#ifndef BX_NTVDM_DEM_READ_OBSERVATION_V1_H
#define BX_NTVDM_DEM_READ_OBSERVATION_V1_H

#include <stdint.h>

#include "bx_ntvdm_exception_abi.h"
#include "bx_ntvdm_cpu_state_abi.h"

/* Default-off diagnostic for the original DEM read contract.  It is not a
 * provider, does not retain guest memory, and cannot affect a CPU result. */
typedef struct bx_ntvdm_dem_read_observation_v1 {
    uint32_t seen_count;
    uint32_t captured;
    uint16_t cs;
    uint16_t ds;
    uint16_t destination_offset;
    uint16_t requested_bytes;
    uint16_t handle_ax;
    uint16_t handle_bp;
    uint32_t eip;
    uint8_t bytes[4];
} bx_ntvdm_dem_read_observation_v1;

#ifdef __cplusplus
extern "C" {
#endif

void bx_ntvdm_dem_read_observation_v1_enable(uint32_t enabled);
int bx_ntvdm_dem_read_observation_v1_copy(bx_ntvdm_dem_read_observation_v1 *out);
void bx_ntvdm_dem_read_observation_v1_consider(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    uint8_t service, const uint8_t *payload, uint32_t payload_bytes);

#ifdef __cplusplus
}
#endif

#endif
