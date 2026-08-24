#include <stdint.h>
#include <string.h>

#include "opennt-bop/observation/bx_ntvdm_ntdos_exec_observation_v1.h"

static void put_u16(uint8_t *value, uint16_t word)
{
    value[0] = (uint8_t)word;
    value[1] = (uint8_t)(word >> 8);
}

static void put_u32(uint8_t *value, uint32_t dword)
{
    put_u16(value, (uint16_t)dword);
    put_u16(value + 2u, (uint16_t)(dword >> 16));
}

int main(void)
{
    uint8_t prefix[RUNTIME_NTDOS_EXEC_PDB_PREFIX_BYTES];
    uint32_t physical = 0u, bytes = 0u;
    struct runtime_ntdos_exec_observation_v1 observation;
    memset(prefix, 0, sizeof(prefix));
    put_u32(prefix + 10u, UINT32_C(0x12345678));
    put_u32(prefix + 14u, UINT32_C(0x90abcdef));
    put_u32(prefix + 18u, UINT32_C(0x0badf00d));
    put_u16(prefix + RUNTIME_NTDOS_EXEC_PDB_PARENT_OFFSET, UINT16_C(0x1234));
    put_u16(prefix + RUNTIME_NTDOS_EXEC_PDB_ENVIRONMENT_OFFSET, UINT16_C(0x4321));
    if (!runtime_ntdos_exec_observation_v1_pdb_range(UINT16_C(0x1234),
            &physical, &bytes) || physical != UINT32_C(0x12340) ||
        bytes != RUNTIME_NTDOS_EXEC_PDB_PREFIX_BYTES) return 1;
    if (runtime_ntdos_exec_observation_v1_pdb_range(UINT16_C(0xffff),
            &physical, &bytes)) return 2;
    if (runtime_ntdos_exec_observation_v1_decode(UINT16_C(0x1234), prefix,
            RUNTIME_NTDOS_EXEC_PDB_PREFIX_BYTES - 1u, &observation) ||
        runtime_ntdos_exec_observation_v1_decode(UINT16_C(0xffff), prefix,
            sizeof(prefix), &observation)) return 3;
    if (!runtime_ntdos_exec_observation_v1_decode(UINT16_C(0x1234), prefix,
            sizeof(prefix), &observation) ||
        observation.magic != RUNTIME_NTDOS_EXEC_OBSERVATION_V1_MAGIC ||
        observation.abi_version != RUNTIME_NTDOS_EXEC_OBSERVATION_V1_VERSION ||
        observation.struct_bytes != sizeof(observation) ||
        observation.pdb_physical_address != UINT32_C(0x12340) ||
        observation.exit_vector != UINT32_C(0x12345678) ||
        observation.ctrl_c_vector != UINT32_C(0x90abcdef) ||
        observation.fatal_vector != UINT32_C(0x0badf00d) ||
        observation.parent_pdb_segment != UINT16_C(0x1234) ||
        observation.environment_segment != UINT16_C(0x4321)) return 4;
    return 0;
}
