#include "bx_ntvdm_generic_ud_bridge.h"
#include "bx_ntvdm_bop_sequence_observation_v1.h"

#include <string.h>

/* The focused link substitutes only two declining composition handles. */
int bx_ntvdm_boot_namespace_composition_v1_handle(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    struct bx_ntvdm_generic_ud_outcome_v1 *outcome)
{
    (void) event; (void) outcome;
    return 0;
}

int bx_ntvdm_native_bop_composition_v1_handle(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    struct bx_ntvdm_generic_ud_outcome_v1 *outcome)
{
    (void) event; (void) outcome;
    return 0;
}

void bx_ntvdm_dem_open_observation_v1_consider(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    const struct bx_ntvdm_generic_ud_outcome_v1 *outcome, int accepted)
{
    (void) event; (void) outcome; (void) accepted;
}

void bx_ntvdm_terminal_observation_v1_consider(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    const struct bx_ntvdm_generic_ud_outcome_v1 *outcome)
{
    (void) event; (void) outcome;
}

void bx_ntvdm_normal_return_observation_v1_consider(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    const struct bx_ntvdm_generic_ud_outcome_v1 *outcome)
{
    (void) event; (void) outcome;
}

void bx_ntvdm_normal_terminal_sequence_observation_v1_consider(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    const struct bx_ntvdm_generic_ud_outcome_v1 *outcome)
{
    (void) event; (void) outcome;
}

int main(void)
{
    struct bx_ntvdm_generic_ud_event_v1 event;
    struct bx_ntvdm_generic_ud_outcome_v1 outcome;
    struct bx_ntvdm_bop_sequence_observation_v1 sequence;

    memset(&event, 0, sizeof(event));
    memset(&outcome, 0, sizeof(outcome));
    event.magic = BX_NTVDM_GENERIC_UD_EVENT_V1_MAGIC;
    event.abi_version = BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
    event.struct_bytes = sizeof(event);
    event.vector = 6u;
    event.cs = 0x70u;
    event.eip = 0x1234u;
    event.window_bytes = 4u;
    event.window[0] = 0xc4u;
    event.window[1] = 0xc4u;
    event.window[2] = 0x50u;
    event.window[3] = 0x11u;
    outcome.abi_version = BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
    outcome.disposition = 99u;

    bx_ntvdm_bop_sequence_observation_v1_enable(1u);
    if (bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) != 0 ||
        outcome.disposition != 99u ||
        !bx_ntvdm_bop_sequence_observation_v1_copy(&sequence) ||
        sequence.record_count != 1u || sequence.overflowed != 0u ||
        sequence.records[0].cs != 0x70u ||
        sequence.records[0].eip != 0x1234u ||
        sequence.records[0].selector != 0x50u ||
        sequence.records[0].service != 0x11u ||
        sequence.records[0].has_service != 1u ||
        sequence.records[0].disposition != BX_NTVDM_GENERIC_UD_PASS_THROUGH)
        return 1;
    bx_ntvdm_bop_sequence_observation_v1_enable(0u);
    return bx_ntvdm_bop_sequence_observation_v1_copy(&sequence) ? 2 : 0;
}
