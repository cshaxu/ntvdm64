#include <stdint.h>
#include <string.h>

#include "adapter-softpc/bx_ntvdm_ntdos_exec_entry_observation_v1.h"

static uint8_t ram[UINT32_C(0x100000)];

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

static int read_ram(void *state, uint64_t physical, uint8_t *bytes,
    uint32_t byte_count)
{
    (void)state;
    if (physical > sizeof(ram) || byte_count > sizeof(ram) - (uint32_t)physical)
        return 0;
    memcpy(bytes, ram + (uint32_t)physical, byte_count);
    return 1;
}

static void event_5036(struct bx_ntvdm_generic_ud_event_v1 *event,
    uint16_t pdb_segment)
{
    memset(event, 0, sizeof(*event));
    event->magic = BX_NTVDM_GENERIC_UD_EVENT_V1_MAGIC;
    event->abi_version = BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
    event->struct_bytes = sizeof(*event);
    event->vector = 6u;
    event->execution_mode = BX_NTVDM_CPU_EXECUTION_REAL;
    event->cs = UINT16_C(0x1111); event->eip = UINT32_C(0x2222);
    event->edx = pdb_segment;
    event->window_bytes = 4u;
    event->window[0] = 0xc4u; event->window[1] = 0xc4u;
    event->window[2] = 0x50u; event->window[3] = 0x36u;
}

int main(void)
{
    struct bx_ntvdm_generic_ud_event_v1 event;
    struct bx_ntvdm_generic_ud_outcome_v1 outcome;
    struct bx_ntvdm_ntdos_exec_entry_observation_v1 observation;
    uint8_t before[BX_NTVDM_NTDOS_EXEC_PDB_PREFIX_BYTES];
    uint32_t physical = UINT32_C(0x12340);

    memset(ram, 0, sizeof(ram));
    put_u32(ram + physical + 10u, UINT32_C(0x12345678));
    put_u32(ram + physical + 14u, UINT32_C(0x90abcdef));
    put_u32(ram + physical + 18u, UINT32_C(0x0badf00d));
    put_u16(ram + physical + 22u, UINT16_C(0x4567));
    put_u16(ram + physical + 44u, UINT16_C(0x89ab));
    memcpy(before, ram + physical, sizeof(before));
    memset(&outcome, 0, sizeof(outcome));
    outcome.disposition = BX_NTVDM_GENERIC_UD_RESUME;
    event_5036(&event, UINT16_C(0x1234));

    bx_ntvdm_ntdos_exec_entry_observation_v1_enable(1u);
    bx_ntvdm_ntdos_exec_entry_observation_v1_consider(&event, &outcome,
        read_ram, NULL);
    if (!bx_ntvdm_ntdos_exec_entry_observation_v1_copy(&observation) ||
        observation.observed != 1u || observation.read_failed != 0u ||
        observation.cs != UINT16_C(0x1111) || observation.eip != UINT32_C(0x2222) ||
        observation.pdb_segment != UINT16_C(0x1234) ||
        observation.physical_address != physical ||
        observation.pdb.exit_vector != UINT32_C(0x12345678) ||
        observation.pdb.ctrl_c_vector != UINT32_C(0x90abcdef) ||
        observation.pdb.fatal_vector != UINT32_C(0x0badf00d) ||
        observation.pdb.parent_pdb_segment != UINT16_C(0x4567) ||
        observation.pdb.environment_segment != UINT16_C(0x89ab) ||
        memcmp(before, ram + physical, sizeof(before)) != 0) return 1;

    /* A non-resume result and a malformed PDB each leave the guest bytes
     * untouched; this seam must only observe a completed original handoff. */
    outcome.disposition = BX_NTVDM_GENERIC_UD_STOP;
    bx_ntvdm_ntdos_exec_entry_observation_v1_enable(1u);
    bx_ntvdm_ntdos_exec_entry_observation_v1_consider(&event, &outcome,
        read_ram, NULL);
    if (!bx_ntvdm_ntdos_exec_entry_observation_v1_copy(&observation) ||
        observation.observed != 0u || observation.read_failed != 0u) return 2;
    outcome.disposition = BX_NTVDM_GENERIC_UD_RESUME;
    event.edx = UINT32_C(0xffff);
    bx_ntvdm_ntdos_exec_entry_observation_v1_enable(1u);
    bx_ntvdm_ntdos_exec_entry_observation_v1_consider(&event, &outcome,
        read_ram, NULL);
    if (!bx_ntvdm_ntdos_exec_entry_observation_v1_copy(&observation) ||
        observation.observed != 0u || observation.read_failed != 1u) return 3;
    return 0;
}
