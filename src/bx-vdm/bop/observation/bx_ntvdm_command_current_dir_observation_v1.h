#ifndef BX_NTVDM_COMMAND_CURRENT_DIR_OBSERVATION_V1_H
#define BX_NTVDM_COMMAND_CURRENT_DIR_OBSERVATION_V1_H

#include <stdint.h>

#include "bx_ntvdm_cpu_state_abi.h"
#include "adapter-softpc/bx_ntvdm_generic_ud_bridge.h"

#define BX_NTVDM_COMMAND_CURRENT_DIR_OBSERVATION_V1_MAGIC UINT32_C(0x42584344)
#define BX_NTVDM_COMMAND_CURRENT_DIR_OBSERVATION_V1_VERSION UINT32_C(1)
#define BX_NTVDM_COMMAND_CURRENT_DIR_OBSERVATION_V1_BYTES 64u

/* Original cmdmisc.c:cmdGetCurrentDir receives DS:SI as its destination.
 * This adapter observation copies its completed source-owned result only. */
struct bx_ntvdm_command_current_dir_observation_v1 {
    uint32_t magic, abi_version, struct_bytes;
    uint32_t enabled, observed, read_failed;
    uint16_t ds, reserved0;
    uint32_t esi, physical_address;
    uint8_t directory[BX_NTVDM_COMMAND_CURRENT_DIR_OBSERVATION_V1_BYTES];
};

typedef int (*bx_ntvdm_command_current_dir_read_v1)(void *state,
    uint64_t physical_address, uint8_t *bytes, uint32_t byte_count);

#ifdef __cplusplus
extern "C" {
#endif
void bx_ntvdm_command_current_dir_observation_v1_enable(uint32_t enabled);
int bx_ntvdm_command_current_dir_observation_v1_copy(
    struct bx_ntvdm_command_current_dir_observation_v1 *value);
void bx_ntvdm_command_current_dir_observation_v1_consider(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    const struct bx_ntvdm_generic_ud_outcome_v1 *outcome,
    bx_ntvdm_command_current_dir_read_v1 read, void *read_state);
#ifdef __cplusplus
}
#endif
#endif
