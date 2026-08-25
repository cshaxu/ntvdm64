#ifndef RUNTIME_COMMAND_BOOTSTRAP_OBSERVATION_H
#define RUNTIME_COMMAND_BOOTSTRAP_OBSERVATION_H

#include <stdint.h>

#include "cpu_state_abi.h"
#include "adapter-bop/generic_ud_bridge.h"

#define RUNTIME_COMMAND_BOOTSTRAP_OBSERVATION_MAGIC UINT32_C(0x42584342)
#define RUNTIME_COMMAND_BOOTSTRAP_OBSERVATION_VERSION UINT32_C(1)
#define RUNTIME_COMMAND_BOOTSTRAP_OBSERVATION_BYTES 64u

/* Source-built NTIO map: `commnd` is 009d:3466.  A native run reaches BOP
 * 5E at 009d:08af after sysconf.asm has written `commnd`; this gives one
 * exact, passive point at
 * which the guest-owned SHELL token can be copied. */
struct runtime_command_bootstrap_observation {
    uint32_t magic, abi_version, struct_bytes;
    uint32_t enabled, observed, read_failed;
    uint16_t cs, reserved0;
    uint32_t eip, physical_address;
    uint8_t command[RUNTIME_COMMAND_BOOTSTRAP_OBSERVATION_BYTES];
};

typedef int (*runtime_command_bootstrap_read)(void *state,
    uint64_t physical_address, uint8_t *bytes, uint32_t byte_count);

#ifdef __cplusplus
extern "C" {
#endif

void runtime_command_bootstrap_observation_enable(uint32_t enabled);
int runtime_command_bootstrap_observation_copy(
    struct runtime_command_bootstrap_observation *value);
void runtime_command_bootstrap_observation_consider(
    const struct runtime_generic_ud_event *event,
    const struct runtime_generic_ud_outcome *outcome,
    runtime_command_bootstrap_read read, void *read_state);

#ifdef __cplusplus
}
#endif

#endif
