#ifndef RUNTIME_NTDOS_EXEC_OBSERVATION_V1_H
#define RUNTIME_NTDOS_EXEC_OBSERVATION_V1_H

#include <stdint.h>

/*
 * Fixture-only copied view of the documented prefix of an OpenNT DOS Process
 * Data Block (PDB).  The offsets below come directly from
 * base/mvdm/dos/v86/inc/pdb.inc.  This type does not allocate, alter, or
 * otherwise interpret a guest PDB: the NTDOS $Exec/$Exit source remains its
 * sole owner.
 */
#define RUNTIME_NTDOS_EXEC_OBSERVATION_V1_MAGIC UINT32_C(0x4258454f)
#define RUNTIME_NTDOS_EXEC_OBSERVATION_V1_VERSION UINT32_C(1)
#define RUNTIME_NTDOS_EXEC_PDB_PREFIX_BYTES 50u
#define RUNTIME_NTDOS_EXEC_PDB_PARENT_OFFSET 22u
#define RUNTIME_NTDOS_EXEC_PDB_ENVIRONMENT_OFFSET 44u

struct runtime_ntdos_exec_observation_v1 {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t pdb_physical_address;
    uint32_t prefix_bytes;
    uint32_t exit_vector;
    uint32_t ctrl_c_vector;
    uint32_t fatal_vector;
    uint16_t parent_pdb_segment;
    uint16_t environment_segment;
};

#ifdef __cplusplus
extern "C" {
#endif

/* A segment is copied as a 16-bit guest fact.  This validates the bounded
 * one-MiB fixture machine address before any caller requests a RAM copy. */
int runtime_ntdos_exec_observation_v1_pdb_range(
    uint16_t pdb_segment, uint32_t *physical_address, uint32_t *byte_count);

/* Decode exactly the fixed prefix after a checked caller-owned RAM copy.
 * There is intentionally no guest-memory pointer or mutable result here. */
int runtime_ntdos_exec_observation_v1_decode(
    uint16_t pdb_segment, const uint8_t *prefix, uint32_t prefix_bytes,
    struct runtime_ntdos_exec_observation_v1 *observation);

#ifdef __cplusplus
}
#endif

#endif
