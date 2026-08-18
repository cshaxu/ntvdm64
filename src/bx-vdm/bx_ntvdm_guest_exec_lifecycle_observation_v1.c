#include "bx_ntvdm_guest_exec_lifecycle_observation_v1.h"

#include <string.h>

static uint32_t enabled;
static struct bx_ntvdm_guest_exec_lifecycle_observation_v1 record;

static void clear(void)
{
    memset(&record, 0, sizeof(record));
    record.magic = BX_NTVDM_GUEST_EXEC_LIFECYCLE_OBSERVATION_V1_MAGIC;
    record.abi_version = BX_NTVDM_GUEST_EXEC_LIFECYCLE_OBSERVATION_V1_VERSION;
    record.struct_bytes = (uint32_t)sizeof(record);
}

static int accepted(const struct bx_ntvdm_generic_ud_event_v1 *event,
    const struct bx_ntvdm_generic_ud_outcome_v1 *outcome, uint8_t family,
    uint8_t service)
{
    return event && outcome &&
        event->magic == BX_NTVDM_GENERIC_UD_EVENT_V1_MAGIC &&
        event->abi_version == BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION &&
        event->struct_bytes == sizeof(*event) && event->vector == 6u &&
        event->fault_rip <= UINT64_MAX - 4u && event->window_bytes >= 4u &&
        event->window[0] == 0xc4u && event->window[1] == 0xc4u &&
        event->window[2] == family && event->window[3] == service &&
        outcome->abi_version == BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION &&
        outcome->disposition == BX_NTVDM_GENERIC_UD_RESUME &&
        outcome->resume_rip == event->fault_rip + 4u;
}

void bx_ntvdm_guest_exec_lifecycle_observation_v1_enable(uint32_t value)
{
    enabled = value == 1u;
    clear();
}

int bx_ntvdm_guest_exec_lifecycle_observation_v1_copy(
    struct bx_ntvdm_guest_exec_lifecycle_observation_v1 *value)
{
    if (!value || !enabled || (!record.entry_seen && !record.terminate_seen &&
            !record.parent_return_seen && !record.out_of_order_seen)) return 0;
    *value = record;
    return 1;
}

void bx_ntvdm_guest_exec_lifecycle_observation_v1_consider(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    const struct bx_ntvdm_generic_ud_outcome_v1 *outcome)
{
    if (!enabled) return;
    if (accepted(event, outcome, 0x50u, 0x36u)) {
        if (!record.entry_seen) {
            record.entry_event = *event;
            record.entry_outcome = *outcome;
            record.entry_seen = 1u;
        }
        return;
    }
    if (accepted(event, outcome, 0x50u, 0x3cu)) {
        if (!record.entry_seen) record.out_of_order_seen = 1u;
        else if (!record.terminate_seen) {
            record.terminate_event = *event;
            record.terminate_outcome = *outcome;
            record.terminate_seen = 1u;
        }
        return;
    }
    if (accepted(event, outcome, 0x54u, 0x0bu)) {
        if (!record.entry_seen || !record.terminate_seen)
            record.out_of_order_seen = 1u;
        else if (!record.parent_return_seen) {
            record.parent_return_event = *event;
            record.parent_return_outcome = *outcome;
            record.parent_return_seen = 1u;
        }
    }
}