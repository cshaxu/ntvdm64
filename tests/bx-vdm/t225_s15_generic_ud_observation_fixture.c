#include "bx_ntvdm_generic_ud_bridge.h"
#include "bop-v1/bx_ntvdm_generic_ud_sequence_observation_v1.h"

#include <string.h>

static int accept;

int bx_ntvdm_boot_namespace_composition_v1_handle(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    struct bx_ntvdm_generic_ud_outcome_v1 *outcome)
{
    (void) event;
    if (!accept) return 0;
    outcome->abi_version = BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
    outcome->disposition = BX_NTVDM_GENERIC_UD_RESUME;
    outcome->resume_rip = 0x1237u;
    return 1;
}

int bx_ntvdm_native_bop_composition_v1_handle(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    struct bx_ntvdm_generic_ud_outcome_v1 *outcome)
{
    (void) event; (void) outcome;
    return 0;
}

void bx_ntvdm_bop_sequence_observation_v1_consider(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    const struct bx_ntvdm_generic_ud_outcome_v1 *outcome)
{
    (void) event; (void) outcome;
}
void bx_ntvdm_dem_open_observation_v1_consider(const struct bx_ntvdm_generic_ud_event_v1 *e, const struct bx_ntvdm_generic_ud_outcome_v1 *o, int a) { (void)e; (void)o; (void)a; }
void bx_ntvdm_terminal_observation_v1_consider(const struct bx_ntvdm_generic_ud_event_v1 *e, const struct bx_ntvdm_generic_ud_outcome_v1 *o) { (void)e; (void)o; }
void bx_ntvdm_normal_return_observation_v1_consider(const struct bx_ntvdm_generic_ud_event_v1 *e, const struct bx_ntvdm_generic_ud_outcome_v1 *o) { (void)e; (void)o; }
void bx_ntvdm_normal_terminal_sequence_observation_v1_consider(const struct bx_ntvdm_generic_ud_event_v1 *e, const struct bx_ntvdm_generic_ud_outcome_v1 *o) { (void)e; (void)o; }

int main(void)
{
    struct bx_ntvdm_generic_ud_event_v1 event;
    struct bx_ntvdm_generic_ud_outcome_v1 outcome;
    struct bx_ntvdm_generic_ud_sequence_observation_v1 sequence;

    memset(&event, 0, sizeof(event));
    event.magic = BX_NTVDM_GENERIC_UD_EVENT_V1_MAGIC;
    event.abi_version = BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
    event.struct_bytes = sizeof(event);
    event.vector = 6u;
    event.execution_mode = 1u;
    event.cs = 0x70u;
    event.eip = 0x1234u;
    event.window_bytes = 3u;
    event.window[0] = 0x0fu;
    event.window[1] = 0x00u;
    event.window[2] = 0x00u;

    bx_ntvdm_generic_ud_sequence_observation_v1_enable(1u);
    memset(&outcome, 0, sizeof(outcome));
    outcome.abi_version = BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
    outcome.disposition = 99u;
    accept = 0;
    if (bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) != 0 ||
        outcome.disposition != 99u) return 1;
    accept = 1;
    memset(&outcome, 0, sizeof(outcome));
    if (bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) != 1 ||
        outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME ||
        !bx_ntvdm_generic_ud_sequence_observation_v1_copy(&sequence) ||
        sequence.record_count != 2u || sequence.overflowed != 0u ||
        sequence.records[0].cs != 0x70u || sequence.records[0].eip != 0x1234u ||
        sequence.records[0].execution_mode != 1u ||
        sequence.records[0].window_bytes != 3u ||
        sequence.records[0].window[0] != 0x0fu ||
        sequence.records[0].window[1] != 0x00u ||
        sequence.records[0].window[2] != 0x00u ||
        sequence.records[0].disposition != BX_NTVDM_GENERIC_UD_PASS_THROUGH ||
        sequence.records[1].disposition != BX_NTVDM_GENERIC_UD_RESUME)
        return 2;
    bx_ntvdm_generic_ud_sequence_observation_v1_enable(0u);
    return bx_ntvdm_generic_ud_sequence_observation_v1_copy(&sequence) ? 3 : 0;
}