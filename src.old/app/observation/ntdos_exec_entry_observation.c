#include "ntdos_exec_entry_observation.h"

#include <string.h>

static uint32_t enabled;
static struct runtime_ntdos_exec_entry_observation record;

static void clear(void)
{
    memset(&record, 0, sizeof(record));
    record.magic = RUNTIME_NTDOS_EXEC_ENTRY_OBSERVATION_MAGIC;
    record.abi_version = RUNTIME_NTDOS_EXEC_ENTRY_OBSERVATION_VERSION;
    record.struct_bytes = sizeof(record);
    record.enabled = enabled;
}

void runtime_ntdos_exec_entry_observation_enable(uint32_t value)
{
    enabled = value == 1u;
    clear();
}

int runtime_ntdos_exec_entry_observation_copy(
    struct runtime_ntdos_exec_entry_observation *value)
{
    if (value == NULL || !enabled) return 0;
    *value = record;
    return 1;
}

void runtime_ntdos_exec_entry_observation_consider(
    const struct runtime_generic_ud_event *event,
    const struct runtime_generic_ud_outcome *outcome,
    runtime_ntdos_exec_entry_read read, void *read_state)
{
    uint8_t prefix[RUNTIME_NTDOS_EXEC_PDB_PREFIX_BYTES];
    uint32_t physical, bytes;
    uint16_t pdb_segment;

    if (!enabled || record.observed != 0u || record.read_failed != 0u ||
        event == NULL || outcome == NULL || read == NULL ||
        event->execution_mode != RUNTIME_CPU_EXECUTION_REAL ||
        event->window_bytes < 4u || event->window[0] != 0xc4u ||
        event->window[1] != 0xc4u || event->window[2] != 0x50u ||
        event->window[3] != 0x36u ||
        outcome->disposition != RUNTIME_GENERIC_UD_RESUME) return;

    pdb_segment = (uint16_t)event->edx;
    if (!runtime_ntdos_exec_observation_pdb_range(pdb_segment,
            &physical, &bytes) ||
        !read(read_state, physical, prefix, bytes) ||
        !runtime_ntdos_exec_observation_decode(pdb_segment, prefix, bytes,
            &record.pdb)) {
        record.read_failed = 1u;
        return;
    }

    record.observed = 1u;
    record.cs = event->cs;
    record.eip = event->eip;
    record.pdb_segment = pdb_segment;
    record.physical_address = physical;
}
