#include "bx_ntvdm_terminal_observation_v1.h"

#include <string.h>

static uint32_t enabled;
static uint32_t considered;
static uint32_t stop_considered;
static struct bx_ntvdm_terminal_observation_v1 record;

static void clear(void)
{
    memset(&record, 0, sizeof(record));
    record.magic = BX_NTVDM_TERMINAL_OBSERVATION_V1_MAGIC;
    record.abi_version = BX_NTVDM_TERMINAL_OBSERVATION_V1_VERSION;
    record.struct_bytes = sizeof(record);
}

void bx_ntvdm_terminal_observation_v1_enable(uint32_t value)
{
    enabled = value == 1u;
    considered = 0u;
    stop_considered = 0u;
    clear();
}

uint32_t bx_ntvdm_terminal_observation_v1_considered_count(void)
{
    return enabled ? considered : 0u;
}

uint32_t bx_ntvdm_terminal_observation_v1_stop_count(void)
{
    return enabled ? stop_considered : 0u;
}

int bx_ntvdm_terminal_observation_v1_copy(
    struct bx_ntvdm_terminal_observation_v1 *value)
{
    if (value == 0 || !enabled || record.captured == 0u) return 0;
    *value = record;
    return 1;
}

void bx_ntvdm_terminal_observation_v1_consider(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    const struct bx_ntvdm_generic_ud_outcome_v1 *outcome)
{
    if (!enabled) return;
    ++considered;
    if (outcome != 0 && outcome->abi_version == BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION &&
        outcome->disposition == BX_NTVDM_GENERIC_UD_STOP) ++stop_considered;
    if (record.captured != 0u || event == 0 || outcome == 0 ||
        outcome->abi_version != BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION ||
        outcome->disposition != BX_NTVDM_GENERIC_UD_STOP) return;
    record.event = *event;
    record.outcome = *outcome;
    record.captured = 1u;
}
