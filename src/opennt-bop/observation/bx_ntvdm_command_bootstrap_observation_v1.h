#ifndef RUNTIME_COMMAND_BOOTSTRAP_OBSERVATION_V1_H
#define RUNTIME_COMMAND_BOOTSTRAP_OBSERVATION_V1_H

#include <stdint.h>

#include "cpu_state_abi.h"
#include "adapter-bop/generic_ud_bridge.h"

#define RUNTIME_COMMAND_BOOTSTRAP_OBSERVATION_V1_MAGIC UINT32_C(0x42584342)
#define RUNTIME_COMMAND_BOOTSTRAP_OBSERVATION_V1_VERSION UINT32_C(1)
#define RUNTIME_COMMAND_BOOTSTRAP_OBSERVATION_V1_BYTES 64u

/* Source-built NTIO map: `commnd` is 009d:3466.  A native run reaches BOP
 * 5E at 009d:08af after sysconf.asm has written `commnd`; this gives one
 * exact, passive point at
 * which the guest-owned SHELL token can be copied. */
struct runtime_command_bootstrap_observation_v1 {
    uint32_t magic, abi_version, struct_bytes;
    uint32_t enabled, observed, read_failed;
    uint16_t cs, reserved0;
    uint32_t eip, physical_address;
    uint8_t command[RUNTIME_COMMAND_BOOTSTRAP_OBSERVATION_V1_BYTES];
};

typedef int (*runtime_command_bootstrap_read_v1)(void *state,
    uint64_t physical_address, uint8_t *bytes, uint32_t byte_count);

#ifdef __cplusplus
extern "C" {
#endif

void runtime_command_bootstrap_observation_v1_enable(uint32_t enabled);
int runtime_command_bootstrap_observation_v1_copy(
    struct runtime_command_bootstrap_observation_v1 *value);
void runtime_command_bootstrap_observation_v1_consider(
    const struct runtime_generic_ud_event_v1 *event,
    const struct runtime_generic_ud_outcome_v1 *outcome,
    runtime_command_bootstrap_read_v1 read, void *read_state);

#ifdef __cplusplus
}
#endif

#endif
