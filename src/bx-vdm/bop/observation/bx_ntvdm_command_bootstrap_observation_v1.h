#ifndef BX_NTVDM_COMMAND_BOOTSTRAP_OBSERVATION_V1_H
#define BX_NTVDM_COMMAND_BOOTSTRAP_OBSERVATION_V1_H

#include <stdint.h>

#include "bx_ntvdm_cpu_state_abi.h"
#include "adapter-softpc/bx_ntvdm_generic_ud_bridge.h"

#define BX_NTVDM_COMMAND_BOOTSTRAP_OBSERVATION_V1_MAGIC UINT32_C(0x42584342)
#define BX_NTVDM_COMMAND_BOOTSTRAP_OBSERVATION_V1_VERSION UINT32_C(1)
#define BX_NTVDM_COMMAND_BOOTSTRAP_OBSERVATION_V1_BYTES 64u

/* Source-built NTIO map: `commnd` is 009d:3466.  A native run reaches BOP
 * 5E at 009d:08af after sysconf.asm has written `commnd`; this gives one
 * exact, passive point at
 * which the guest-owned SHELL token can be copied. */
struct bx_ntvdm_command_bootstrap_observation_v1 {
    uint32_t magic, abi_version, struct_bytes;
    uint32_t enabled, observed, read_failed;
    uint16_t cs, reserved0;
    uint32_t eip, physical_address;
    uint8_t command[BX_NTVDM_COMMAND_BOOTSTRAP_OBSERVATION_V1_BYTES];
};

typedef int (*bx_ntvdm_command_bootstrap_read_v1)(void *state,
    uint64_t physical_address, uint8_t *bytes, uint32_t byte_count);

#ifdef __cplusplus
extern "C" {
#endif

void bx_ntvdm_command_bootstrap_observation_v1_enable(uint32_t enabled);
int bx_ntvdm_command_bootstrap_observation_v1_copy(
    struct bx_ntvdm_command_bootstrap_observation_v1 *value);
void bx_ntvdm_command_bootstrap_observation_v1_consider(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    const struct bx_ntvdm_generic_ud_outcome_v1 *outcome,
    bx_ntvdm_command_bootstrap_read_v1 read, void *read_state);

#ifdef __cplusplus
}
#endif

#endif
