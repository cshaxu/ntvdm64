#ifndef RUNTIME_NTDOS_EXEC_ENTRY_OBSERVATION_H
#define RUNTIME_NTDOS_EXEC_ENTRY_OBSERVATION_H

#include <stdint.h>

#include "adapter-softpc/cpu_state_abi.h"
#include "ntdos_exec_observation.h"
#include "adapter-bop/generic_ud_bridge.h"

#define RUNTIME_NTDOS_EXEC_ENTRY_OBSERVATION_MAGIC UINT32_C(0x42584545)
#define RUNTIME_NTDOS_EXEC_ENTRY_OBSERVATION_VERSION UINT32_C(1)

/* `$Exec:Xfer_To_User` emits 50:36 only after `$Dup_PDB` has constructed the
 * child PDB and stored its return vectors.  This is diagnostic-only state: it
 * owns no DOS process data and changes neither the guest nor BOP result. */
struct runtime_ntdos_exec_entry_observation {
    uint32_t magic, abi_version, struct_bytes;
    uint32_t enabled, observed, read_failed;
    uint16_t cs, pdb_segment;
    uint32_t eip, physical_address;
    struct runtime_ntdos_exec_observation pdb;
};

typedef int (*runtime_ntdos_exec_entry_read)(void *state,
    uint64_t physical_address, uint8_t *bytes, uint32_t byte_count);

#ifdef __cplusplus
extern "C" {
#endif

void runtime_ntdos_exec_entry_observation_enable(uint32_t enabled);
int runtime_ntdos_exec_entry_observation_copy(
    struct runtime_ntdos_exec_entry_observation *value);
void runtime_ntdos_exec_entry_observation_consider(
    const struct runtime_generic_ud_event *event,
    const struct runtime_generic_ud_outcome *outcome,
    runtime_ntdos_exec_entry_read read, void *read_state);

#ifdef __cplusplus
}
#endif

#endif
