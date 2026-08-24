#include "bx_ntvdm_normal_terminal_sequence_observation_v1.h"

#include <string.h>

static void event_set(struct bx_ntvdm_generic_ud_event_v1 *event, uint8_t service)
{
    memset(event, 0, sizeof(*event));
    event->magic = BX_NTVDM_GENERIC_UD_EVENT_V1_MAGIC;
    event->abi_version = BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
    event->struct_bytes = (uint32_t)sizeof(*event);
    event->vector = 6u; event->fault_rip = 0x1234u; event->window_bytes = 4u;
    event->window[0] = 0xc4u; event->window[1] = 0xc4u;
    event->window[2] = 0x54u; event->window[3] = service;
}

static void outcome_set(struct bx_ntvdm_generic_ud_outcome_v1 *outcome, uint32_t disposition)
{
    memset(outcome, 0, sizeof(*outcome));
    outcome->abi_version = BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
    outcome->disposition = disposition; outcome->resume_rip = 0x1238u;
    outcome->gpr16_write_mask = 1u; outcome->gpr16_values[0] = 0x34u;
}

int main(void)
{
    struct bx_ntvdm_generic_ud_event_v1 event;
    struct bx_ntvdm_generic_ud_outcome_v1 outcome;
    struct bx_ntvdm_normal_terminal_sequence_observation_v1 copy;
    event_set(&event, 0x0bu); outcome_set(&outcome, BX_NTVDM_GENERIC_UD_RESUME);
    bx_ntvdm_normal_terminal_sequence_observation_v1_enable(0u);
    bx_ntvdm_normal_terminal_sequence_observation_v1_consider(&event, &outcome);
    if (bx_ntvdm_normal_terminal_sequence_observation_v1_copy(&copy)) return 1;
    bx_ntvdm_normal_terminal_sequence_observation_v1_enable(1u);
    event.window[3] = 0x0au;
    bx_ntvdm_normal_terminal_sequence_observation_v1_consider(&event, &outcome);
    if (bx_ntvdm_normal_terminal_sequence_observation_v1_copy(&copy)) return 2;
    outcome.disposition = BX_NTVDM_GENERIC_UD_STOP;
    bx_ntvdm_normal_terminal_sequence_observation_v1_consider(&event, &outcome);
    if (!bx_ntvdm_normal_terminal_sequence_observation_v1_copy(&copy) ||
        copy.normal_return_seen != 0u || copy.stop_before_normal_seen != 1u ||
        copy.stop_after_normal_seen != 0u) return 3;
    bx_ntvdm_normal_terminal_sequence_observation_v1_enable(1u);
    event_set(&event, 0x0bu); outcome_set(&outcome, BX_NTVDM_GENERIC_UD_RESUME);
    bx_ntvdm_normal_terminal_sequence_observation_v1_consider(&event, &outcome);
    outcome.disposition = BX_NTVDM_GENERIC_UD_STOP;
    bx_ntvdm_normal_terminal_sequence_observation_v1_consider(&event, &outcome);
    if (!bx_ntvdm_normal_terminal_sequence_observation_v1_copy(&copy) ||
        copy.normal_return_seen != 1u || copy.stop_before_normal_seen != 0u ||
        copy.stop_after_normal_seen != 1u || copy.normal_event.window[3] != 0x0bu ||
        copy.normal_outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME ||
        copy.terminal_outcome.disposition != BX_NTVDM_GENERIC_UD_STOP) return 4;
    bx_ntvdm_normal_terminal_sequence_observation_v1_enable(0u);
    return bx_ntvdm_normal_terminal_sequence_observation_v1_copy(&copy) ? 5 : 0;
}
