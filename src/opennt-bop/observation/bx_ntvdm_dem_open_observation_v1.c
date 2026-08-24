#include "bx_ntvdm_dem_open_observation_v1.h"

#include <string.h>
static uint32_t enabled;
static struct runtime_dem_open_observation_v1 record;
static void clear(void) { memset(&record, 0, sizeof(record)); record.magic = RUNTIME_DEM_OPEN_OBSERVATION_V1_MAGIC; record.abi_version = RUNTIME_DEM_OPEN_OBSERVATION_V1_VERSION; record.struct_bytes = sizeof(record); record.enabled = enabled; }
void runtime_dem_open_observation_v1_enable(uint32_t value) { enabled = value == 1u; clear(); }
int runtime_dem_open_observation_v1_copy(struct runtime_dem_open_observation_v1 *value) { if (!value || !enabled) return 0; *value = record; return 1; }
void runtime_dem_open_observation_v1_consider(const struct runtime_generic_ud_event_v1 *event,
    const struct runtime_generic_ud_outcome_v1 *outcome,
    runtime_dem_open_read_v1 read, void *read_state)
{
    uint64_t physical;
    if (!enabled || record.read_failed || !event || !outcome || !read ||
        event->execution_mode != RUNTIME_CPU_EXECUTION_REAL || event->window_bytes < 4u ||
        event->window[0] != 0xc4u || event->window[1] != 0xc4u || event->window[2] != 0x50u ||
        event->window[3] != 0x12u || outcome->disposition != RUNTIME_GENERIC_UD_RESUME) return;
    physical = ((uint64_t)event->ds << 4) + (uint16_t)event->esi;
    if (physical > UINT32_MAX - RUNTIME_DEM_OPEN_OBSERVATION_V1_PATH_BYTES ||
        !read(read_state, physical, record.path, RUNTIME_DEM_OPEN_OBSERVATION_V1_PATH_BYTES)) { record.read_failed = 1u; return; }
    /* S18 retained the first successful observation.  The permanent COMMAND
     * open follows the temporary CONFIG open, so retain the latest accepted
     * source-owned demOpen record and its bounded count instead.  This is
     * diagnostic state only: it neither changes DEM dispatch nor guest RAM. */
    ++record.observed_count;
    record.observed = 1u; record.ds = event->ds; record.mode = event->ebx & 0xffu;
    record.esi = event->esi; record.physical_address = (uint32_t)physical;
    record.disposition = outcome->disposition; record.gpr16_write_mask = outcome->gpr16_write_mask;
    record.eflags_write_mask = outcome->eflags_write_mask; record.eflags_values = outcome->eflags_values;
    memcpy(record.gpr16_values, outcome->gpr16_values, sizeof(record.gpr16_values));
}
