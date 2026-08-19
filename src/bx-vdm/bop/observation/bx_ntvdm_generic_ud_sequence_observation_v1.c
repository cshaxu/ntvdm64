#include "bop/observation/bx_ntvdm_generic_ud_sequence_observation_v1.h"

#include <string.h>

static uint32_t enabled;
static struct bx_ntvdm_generic_ud_sequence_observation_v1 record;

static void clear(void)
{
    memset(&record, 0, sizeof(record));
    record.magic = BX_NTVDM_GENERIC_UD_SEQUENCE_OBSERVATION_V1_MAGIC;
    record.abi_version = BX_NTVDM_GENERIC_UD_SEQUENCE_OBSERVATION_V1_VERSION;
    record.struct_bytes = sizeof(record);
}

void bx_ntvdm_generic_ud_sequence_observation_v1_enable(uint32_t value)
{
    enabled = value == 1u;
    clear();
}

int bx_ntvdm_generic_ud_sequence_observation_v1_copy(
    struct bx_ntvdm_generic_ud_sequence_observation_v1 *value)
{
    if (value == 0 || !enabled) return 0;
    *value = record;
    return 1;
}

void bx_ntvdm_generic_ud_sequence_observation_v1_consider(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    const struct bx_ntvdm_generic_ud_outcome_v1 *outcome)
{
    struct bx_ntvdm_generic_ud_sequence_observation_record_v1 *slot;
    if (!enabled || event == 0 || outcome == 0 ||
        event->window_bytes > BX_NTVDM_GENERIC_UD_WINDOW_BYTES) return;
    if (record.record_count >=
        BX_NTVDM_GENERIC_UD_SEQUENCE_OBSERVATION_V1_MAX_RECORDS) {
        record.overflowed = 1u;
        return;
    }
    slot = &record.records[record.record_count++];
    slot->cs = event->cs;
    slot->eip = event->eip;
    slot->execution_mode = event->execution_mode;
    slot->window_bytes = event->window_bytes;
    slot->disposition = (uint8_t)outcome->disposition;
    memcpy(slot->window, event->window, event->window_bytes);
}
