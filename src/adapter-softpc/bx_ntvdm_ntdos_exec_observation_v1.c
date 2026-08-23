#include "bx_ntvdm_ntdos_exec_observation_v1.h"

#include <string.h>

#define BX_NTVDM_NTDOS_EXEC_FIXTURE_RAM_BYTES UINT32_C(0x100000)
#define BX_NTVDM_NTDOS_EXEC_PDB_EXIT_OFFSET 10u
#define BX_NTVDM_NTDOS_EXEC_PDB_CTRL_C_OFFSET 14u
#define BX_NTVDM_NTDOS_EXEC_PDB_FATAL_OFFSET 18u

static uint16_t read_u16(const uint8_t *value)
{
    return (uint16_t)((uint16_t)value[0] | ((uint16_t)value[1] << 8));
}

static uint32_t read_u32(const uint8_t *value)
{
    return (uint32_t)read_u16(value) | ((uint32_t)read_u16(value + 2u) << 16);
}

int bx_ntvdm_ntdos_exec_observation_v1_pdb_range(
    uint16_t pdb_segment, uint32_t *physical_address, uint32_t *byte_count)
{
    uint32_t physical = (uint32_t)pdb_segment << 4;
    if (physical > BX_NTVDM_NTDOS_EXEC_FIXTURE_RAM_BYTES -
            BX_NTVDM_NTDOS_EXEC_PDB_PREFIX_BYTES)
        return 0;
    if (physical_address != 0) *physical_address = physical;
    if (byte_count != 0) *byte_count = BX_NTVDM_NTDOS_EXEC_PDB_PREFIX_BYTES;
    return 1;
}

int bx_ntvdm_ntdos_exec_observation_v1_decode(
    uint16_t pdb_segment, const uint8_t *prefix, uint32_t prefix_bytes,
    struct bx_ntvdm_ntdos_exec_observation_v1 *observation)
{
    uint32_t physical;
    if (prefix == 0 || observation == 0 ||
        prefix_bytes != BX_NTVDM_NTDOS_EXEC_PDB_PREFIX_BYTES ||
        !bx_ntvdm_ntdos_exec_observation_v1_pdb_range(pdb_segment,
            &physical, 0))
        return 0;
    memset(observation, 0, sizeof(*observation));
    observation->magic = BX_NTVDM_NTDOS_EXEC_OBSERVATION_V1_MAGIC;
    observation->abi_version = BX_NTVDM_NTDOS_EXEC_OBSERVATION_V1_VERSION;
    observation->struct_bytes = sizeof(*observation);
    observation->pdb_physical_address = physical;
    observation->prefix_bytes = BX_NTVDM_NTDOS_EXEC_PDB_PREFIX_BYTES;
    observation->exit_vector = read_u32(prefix + BX_NTVDM_NTDOS_EXEC_PDB_EXIT_OFFSET);
    observation->ctrl_c_vector = read_u32(prefix + BX_NTVDM_NTDOS_EXEC_PDB_CTRL_C_OFFSET);
    observation->fatal_vector = read_u32(prefix + BX_NTVDM_NTDOS_EXEC_PDB_FATAL_OFFSET);
    observation->parent_pdb_segment = read_u16(prefix + BX_NTVDM_NTDOS_EXEC_PDB_PARENT_OFFSET);
    observation->environment_segment = read_u16(prefix + BX_NTVDM_NTDOS_EXEC_PDB_ENVIRONMENT_OFFSET);
    return 1;
}
