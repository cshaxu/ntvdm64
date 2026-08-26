#include "generic_ud_sequence_observation.h"

#include <string.h>

static uint32_t enabled;
static struct runtime_generic_ud_sequence_observation record;

static void clear(void)
{
    memset(&record, 0, sizeof(record));
    record.magic = RUNTIME_GENERIC_UD_SEQUENCE_OBSERVATION_MAGIC;
    record.abi_version = RUNTIME_GENERIC_UD_SEQUENCE_OBSERVATION_VERSION;
    record.struct_bytes = sizeof(record);
}

void runtime_generic_ud_sequence_observation_enable(uint32_t value)
{
    enabled = value == 1u;
    clear();
}

int runtime_generic_ud_sequence_observation_copy(
    struct runtime_generic_ud_sequence_observation *value)
{
    if (value == 0 || !enabled) return 0;
    *value = record;
    return 1;
}

void runtime_generic_ud_sequence_observation_consider(
    const struct runtime_generic_ud_event *event,
    const struct runtime_generic_ud_outcome *outcome)
{
    struct runtime_generic_ud_sequence_observation_record *slot;
    if (!enabled || event == 0 || outcome == 0 ||
        event->window_bytes > RUNTIME_GENERIC_UD_WINDOW_BYTES) return;
    if (record.record_count >=
        RUNTIME_GENERIC_UD_SEQUENCE_OBSERVATION_MAX_RECORDS) {
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
