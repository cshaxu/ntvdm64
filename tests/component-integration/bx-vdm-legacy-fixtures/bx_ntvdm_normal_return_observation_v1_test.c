#include "bx_ntvdm_normal_return_observation_v1.h"

#include <string.h>

static void event_set(struct bx_ntvdm_generic_ud_event_v1 *event)
{
    memset(event, 0, sizeof(*event));
    event->magic = BX_NTVDM_GENERIC_UD_EVENT_V1_MAGIC;
    event->abi_version = BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
    event->struct_bytes = sizeof(*event);
    event->vector = 6u;
    event->fault_rip = 0x1234u;
    event->eip = 0x1234u;
    event->edx = 0x45u;
    event->window_bytes = 4u;
    event->window[0] = 0xc4u;
    event->window[1] = 0xc4u;
    event->window[2] = 0x54u;
    event->window[3] = 0x0bu;
}

static void outcome_set(struct bx_ntvdm_generic_ud_outcome_v1 *outcome)
{
    memset(outcome, 0, sizeof(*outcome));
    outcome->abi_version = BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
    outcome->disposition = BX_NTVDM_GENERIC_UD_RESUME;
    outcome->resume_rip = 0x1238u;
    outcome->gpr16_write_mask = 1u;
    outcome->gpr16_values[0] = 0x45u;
}

int main(void)
{
    struct bx_ntvdm_generic_ud_event_v1 event;
    struct bx_ntvdm_generic_ud_outcome_v1 outcome;
    struct bx_ntvdm_normal_return_observation_v1 copy;
    event_set(&event);
    outcome_set(&outcome);
    bx_ntvdm_normal_return_observation_v1_enable(0u);
    bx_ntvdm_normal_return_observation_v1_consider(&event, &outcome);
    if (bx_ntvdm_normal_return_observation_v1_copy(&copy)) return 1;
    bx_ntvdm_normal_return_observation_v1_enable(1u);
    event.window[3] = 0x0au;
    bx_ntvdm_normal_return_observation_v1_consider(&event, &outcome);
    if (bx_ntvdm_normal_return_observation_v1_copy(&copy)) return 2;
    event.window[3] = 0x0bu;
    outcome.resume_rip = 0x1239u;
    bx_ntvdm_normal_return_observation_v1_consider(&event, &outcome);
    if (bx_ntvdm_normal_return_observation_v1_copy(&copy)) return 3;
    outcome.resume_rip = 0x1238u;
    bx_ntvdm_normal_return_observation_v1_consider(&event, &outcome);
    if (!bx_ntvdm_normal_return_observation_v1_copy(&copy) ||
        copy.captured != 1u || copy.input_dx != 0x45u ||
        copy.returned_ax != 0x45u || copy.event.window[3] != 0x0bu ||
        copy.outcome.resume_rip != 0x1238u) return 4;
    bx_ntvdm_normal_return_observation_v1_enable(0u);
    return bx_ntvdm_normal_return_observation_v1_copy(&copy) ? 5 : 0;
}
