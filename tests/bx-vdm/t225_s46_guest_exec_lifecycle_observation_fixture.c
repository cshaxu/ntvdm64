#include "bx_ntvdm_guest_exec_lifecycle_observation_v1.h"

#include <string.h>

static void event_for(struct bx_ntvdm_generic_ud_event_v1 *event,
    uint8_t selector, uint8_t service, uint64_t rip)
{
    memset(event, 0, sizeof(*event));
    event->magic = BX_NTVDM_GENERIC_UD_EVENT_V1_MAGIC;
    event->abi_version = BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
    event->struct_bytes = sizeof(*event);
    event->vector = 6u;
    event->fault_rip = rip;
    event->window_bytes = 4u;
    event->window[0] = 0xc4u;
    event->window[1] = 0xc4u;
    event->window[2] = selector;
    event->window[3] = service;
}

static void resume_for(struct bx_ntvdm_generic_ud_outcome_v1 *outcome,
    uint64_t rip)
{
    memset(outcome, 0, sizeof(*outcome));
    outcome->abi_version = BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
    outcome->disposition = BX_NTVDM_GENERIC_UD_RESUME;
    outcome->resume_rip = rip + 4u;
}

int main(void)
{
    struct bx_ntvdm_generic_ud_event_v1 event;
    struct bx_ntvdm_generic_ud_outcome_v1 outcome;
    struct bx_ntvdm_guest_exec_lifecycle_observation_v1 observation;

    event_for(&event, 0x50u, 0x36u, 0x1000u);
    resume_for(&outcome, event.fault_rip);
    bx_ntvdm_guest_exec_lifecycle_observation_v1_enable(0u);
    bx_ntvdm_guest_exec_lifecycle_observation_v1_consider(&event, &outcome);
    if (bx_ntvdm_guest_exec_lifecycle_observation_v1_copy(&observation)) return 1;

    bx_ntvdm_guest_exec_lifecycle_observation_v1_enable(1u);
    event_for(&event, 0x50u, 0x3cu, 0x2000u);
    resume_for(&outcome, event.fault_rip);
    bx_ntvdm_guest_exec_lifecycle_observation_v1_consider(&event, &outcome);
    if (!bx_ntvdm_guest_exec_lifecycle_observation_v1_copy(&observation) ||
        observation.entry_seen || observation.terminate_seen ||
        observation.parent_return_seen || !observation.out_of_order_seen) return 2;

    bx_ntvdm_guest_exec_lifecycle_observation_v1_enable(1u);
    event_for(&event, 0x50u, 0x36u, 0x3000u);
    resume_for(&outcome, event.fault_rip);
    bx_ntvdm_guest_exec_lifecycle_observation_v1_consider(&event, &outcome);
    event_for(&event, 0x50u, 0x3cu, 0x4000u);
    resume_for(&outcome, event.fault_rip);
    bx_ntvdm_guest_exec_lifecycle_observation_v1_consider(&event, &outcome);
    event_for(&event, 0x54u, 0x0bu, 0x5000u);
    resume_for(&outcome, event.fault_rip);
    bx_ntvdm_guest_exec_lifecycle_observation_v1_consider(&event, &outcome);
    if (!bx_ntvdm_guest_exec_lifecycle_observation_v1_copy(&observation) ||
        !observation.entry_seen || !observation.terminate_seen ||
        !observation.parent_return_seen || observation.out_of_order_seen ||
        observation.entry_event.fault_rip != 0x3000u ||
        observation.terminate_event.fault_rip != 0x4000u ||
        observation.parent_return_event.fault_rip != 0x5000u ||
        observation.parent_return_outcome.resume_rip != 0x5004u) return 3;

    event_for(&event, 0x50u, 0x36u, 0x6000u);
    resume_for(&outcome, event.fault_rip);
    outcome.disposition = BX_NTVDM_GENERIC_UD_PASS_THROUGH;
    bx_ntvdm_guest_exec_lifecycle_observation_v1_consider(&event, &outcome);
    if (!bx_ntvdm_guest_exec_lifecycle_observation_v1_copy(&observation) ||
        observation.entry_event.fault_rip != 0x3000u) return 4;
    bx_ntvdm_guest_exec_lifecycle_observation_v1_enable(0u);
    return bx_ntvdm_guest_exec_lifecycle_observation_v1_copy(&observation) ? 5 : 0;
}