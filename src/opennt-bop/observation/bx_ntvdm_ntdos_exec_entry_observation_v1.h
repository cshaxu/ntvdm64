#ifndef RUNTIME_NTDOS_EXEC_ENTRY_OBSERVATION_V1_H
#define RUNTIME_NTDOS_EXEC_ENTRY_OBSERVATION_V1_H

#include <stdint.h>

#include "cpu_state_abi.h"
#include "bx_ntvdm_ntdos_exec_observation_v1.h"
#include "adapter-bop/generic_ud_bridge.h"

#define RUNTIME_NTDOS_EXEC_ENTRY_OBSERVATION_V1_MAGIC UINT32_C(0x42584545)
#define RUNTIME_NTDOS_EXEC_ENTRY_OBSERVATION_V1_VERSION UINT32_C(1)

/* `$Exec:Xfer_To_User` emits 50:36 only after `$Dup_PDB` has constructed the
 * child PDB and stored its return vectors.  This is diagnostic-only state: it
 * owns no DOS process data and changes neither the guest nor BOP result. */
struct runtime_ntdos_exec_entry_observation_v1 {
    uint32_t magic, abi_version, struct_bytes;
    uint32_t enabled, observed, read_failed;
    uint16_t cs, pdb_segment;
    uint32_t eip, physical_address;
    struct runtime_ntdos_exec_observation_v1 pdb;
};

typedef int (*runtime_ntdos_exec_entry_read_v1)(void *state,
    uint64_t physical_address, uint8_t *bytes, uint32_t byte_count);

#ifdef __cplusplus
extern "C" {
#endif

void runtime_ntdos_exec_entry_observation_v1_enable(uint32_t enabled);
int runtime_ntdos_exec_entry_observation_v1_copy(
    struct runtime_ntdos_exec_entry_observation_v1 *value);
void runtime_ntdos_exec_entry_observation_v1_consider(
    const struct runtime_generic_ud_event_v1 *event,
    const struct runtime_generic_ud_outcome_v1 *outcome,
    runtime_ntdos_exec_entry_read_v1 read, void *read_state);

#ifdef __cplusplus
}
#endif

#endif
