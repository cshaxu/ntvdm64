#include "bx_ntvdm_normal_return_observation_v1.h"

#include <string.h>

static uint32_t enabled;
static struct bx_ntvdm_normal_return_observation_v1 record;

static void clear(void)
{
    memset(&record, 0, sizeof(record));
    record.magic = BX_NTVDM_NORMAL_RETURN_OBSERVATION_V1_MAGIC;
    record.abi_version = BX_NTVDM_NORMAL_RETURN_OBSERVATION_V1_VERSION;
    record.struct_bytes = sizeof(record);
}

void bx_ntvdm_normal_return_observation_v1_enable(uint32_t value)
{
    enabled = value == 1u;
    clear();
}

int bx_ntvdm_normal_return_observation_v1_copy(
    struct bx_ntvdm_normal_return_observation_v1 *value)
{
    if (value == 0 || !enabled || record.captured == 0u) return 0;
    *value = record;
    return 1;
}

void bx_ntvdm_normal_return_observation_v1_consider(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    const struct bx_ntvdm_generic_ud_outcome_v1 *outcome)
{
    if (!enabled || record.captured != 0u || event == 0 || outcome == 0 ||
        event->magic != BX_NTVDM_GENERIC_UD_EVENT_V1_MAGIC ||
        event->abi_version != BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION ||
        event->struct_bytes != sizeof(*event) || event->vector != 6u ||
        event->fault_rip > UINT64_MAX - 4u || event->window_bytes < 4u ||
        event->window[0] != 0xc4u || event->window[1] != 0xc4u ||
        event->window[2] != 0x54u || event->window[3] != 0x0bu ||
        outcome->abi_version != BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION ||
        outcome->disposition != BX_NTVDM_GENERIC_UD_RESUME ||
        outcome->resume_rip != event->fault_rip + 4u ||
        (outcome->gpr16_write_mask & 1u) == 0u) return;
    record.event = *event;
    record.outcome = *outcome;
    record.input_dx = (uint16_t)event->edx;
    record.returned_ax = outcome->gpr16_values[0];
    record.captured = 1u;
}
