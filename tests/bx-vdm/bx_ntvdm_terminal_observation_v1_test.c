#include "bx_ntvdm_terminal_observation_v1.h"

#include <string.h>

static void event_set(struct bx_ntvdm_generic_ud_event_v1 *event)
{
    memset(event, 0, sizeof(*event));
    event->magic = BX_NTVDM_GENERIC_UD_EVENT_V1_MAGIC;
    event->abi_version = BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
    event->struct_bytes = sizeof(*event);
}

static void outcome_set(struct bx_ntvdm_generic_ud_outcome_v1 *outcome,
    uint32_t disposition)
{
    memset(outcome, 0, sizeof(*outcome));
    outcome->abi_version = BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
    outcome->disposition = disposition;
}

int main(void)
{
    struct bx_ntvdm_generic_ud_event_v1 event;
    struct bx_ntvdm_generic_ud_outcome_v1 outcome;
    struct bx_ntvdm_terminal_observation_v1 copy;
    event_set(&event);
    outcome_set(&outcome, BX_NTVDM_GENERIC_UD_RESUME);
    bx_ntvdm_terminal_observation_v1_enable(0u);
    bx_ntvdm_terminal_observation_v1_consider(&event, &outcome);
    if (bx_ntvdm_terminal_observation_v1_considered_count() != 0u ||
        bx_ntvdm_terminal_observation_v1_stop_count() != 0u) return 1;
    bx_ntvdm_terminal_observation_v1_enable(1u);
    bx_ntvdm_terminal_observation_v1_consider(&event, &outcome);
    if (bx_ntvdm_terminal_observation_v1_considered_count() != 1u ||
        bx_ntvdm_terminal_observation_v1_stop_count() != 0u ||
        bx_ntvdm_terminal_observation_v1_copy(&copy)) return 2;
    outcome.disposition = BX_NTVDM_GENERIC_UD_STOP;
    bx_ntvdm_terminal_observation_v1_consider(&event, &outcome);
    if (bx_ntvdm_terminal_observation_v1_considered_count() != 2u ||
        bx_ntvdm_terminal_observation_v1_stop_count() != 1u ||
        !bx_ntvdm_terminal_observation_v1_copy(&copy) ||
        copy.captured != 1u || copy.outcome.disposition != BX_NTVDM_GENERIC_UD_STOP)
        return 3;
    bx_ntvdm_terminal_observation_v1_enable(0u);
    return bx_ntvdm_terminal_observation_v1_considered_count() != 0u ||
        bx_ntvdm_terminal_observation_v1_stop_count() != 0u;
}
