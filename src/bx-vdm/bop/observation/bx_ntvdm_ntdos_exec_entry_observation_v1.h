#ifndef BX_NTVDM_NTDOS_EXEC_ENTRY_OBSERVATION_V1_H
#define BX_NTVDM_NTDOS_EXEC_ENTRY_OBSERVATION_V1_H

#include <stdint.h>

#include "bx_ntvdm_cpu_state_abi.h"
#include "bx_ntvdm_ntdos_exec_observation_v1.h"
#include "bx-mantle/bx_ntvdm_generic_ud_bridge.h"

#define BX_NTVDM_NTDOS_EXEC_ENTRY_OBSERVATION_V1_MAGIC UINT32_C(0x42584545)
#define BX_NTVDM_NTDOS_EXEC_ENTRY_OBSERVATION_V1_VERSION UINT32_C(1)

/* `$Exec:Xfer_To_User` emits 50:36 only after `$Dup_PDB` has constructed the
 * child PDB and stored its return vectors.  This is diagnostic-only state: it
 * owns no DOS process data and changes neither the guest nor BOP result. */
struct bx_ntvdm_ntdos_exec_entry_observation_v1 {
    uint32_t magic, abi_version, struct_bytes;
    uint32_t enabled, observed, read_failed;
    uint16_t cs, pdb_segment;
    uint32_t eip, physical_address;
    struct bx_ntvdm_ntdos_exec_observation_v1 pdb;
};

typedef int (*bx_ntvdm_ntdos_exec_entry_read_v1)(void *state,
    uint64_t physical_address, uint8_t *bytes, uint32_t byte_count);

#ifdef __cplusplus
extern "C" {
#endif

void bx_ntvdm_ntdos_exec_entry_observation_v1_enable(uint32_t enabled);
int bx_ntvdm_ntdos_exec_entry_observation_v1_copy(
    struct bx_ntvdm_ntdos_exec_entry_observation_v1 *value);
void bx_ntvdm_ntdos_exec_entry_observation_v1_consider(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    const struct bx_ntvdm_generic_ud_outcome_v1 *outcome,
    bx_ntvdm_ntdos_exec_entry_read_v1 read, void *read_state);

#ifdef __cplusplus
}
#endif

#endif
