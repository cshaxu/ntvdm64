#include "bop/observation/bx_ntvdm_command_current_dir_observation_v1.h"

#include <string.h>

static uint32_t enabled;
static struct bx_ntvdm_command_current_dir_observation_v1 record;

static void clear(void)
{
    memset(&record, 0, sizeof(record));
    record.magic = BX_NTVDM_COMMAND_CURRENT_DIR_OBSERVATION_V1_MAGIC;
    record.abi_version = BX_NTVDM_COMMAND_CURRENT_DIR_OBSERVATION_V1_VERSION;
    record.struct_bytes = sizeof(record); record.enabled = enabled;
}
void bx_ntvdm_command_current_dir_observation_v1_enable(uint32_t value)
{ enabled = value == 1u; clear(); }
int bx_ntvdm_command_current_dir_observation_v1_copy(
    struct bx_ntvdm_command_current_dir_observation_v1 *value)
{ if (value == NULL || !enabled) return 0; *value = record; return 1; }
void bx_ntvdm_command_current_dir_observation_v1_consider(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    const struct bx_ntvdm_generic_ud_outcome_v1 *outcome,
    bx_ntvdm_command_current_dir_read_v1 read, void *read_state)
{
    uint64_t physical;
    if (!enabled || record.observed != 0u || record.read_failed != 0u ||
        event == NULL || outcome == NULL || read == NULL ||
        event->execution_mode != BX_NTVDM_CPU_EXECUTION_REAL ||
        event->window_bytes < 4u || event->window[0] != 0xc4u ||
        event->window[1] != 0xc4u || event->window[2] != 0x54u ||
        event->window[3] != 0x04u ||
        outcome->disposition != BX_NTVDM_GENERIC_UD_RESUME) return;
    physical = ((uint64_t)event->ds << 4) + (uint16_t)event->esi;
    if (physical > UINT32_MAX - BX_NTVDM_COMMAND_CURRENT_DIR_OBSERVATION_V1_BYTES ||
        !read(read_state, physical, record.directory,
            BX_NTVDM_COMMAND_CURRENT_DIR_OBSERVATION_V1_BYTES)) {
        record.read_failed = 1u; return;
    }
    record.observed = 1u; record.ds = event->ds; record.esi = event->esi;
    record.physical_address = (uint32_t)physical;
}
