#include "bx_ntvdm_generic_ud_bridge.h"
#include "bop-v1/bx_ntvdm_bop_sequence_observation_v1.h"

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
    uint32_t index;

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
    event.eax = 0x11112222u; event.ebx = 0x33334444u;
    event.ecx = 0x55556666u; event.edx = 0x77778888u;
    event.esi = 0x9999aaaau; event.edi = 0xbbbbccccu;
    event.eflags = 0x202u; event.ds = 0x1234u; event.es = 0x5678u; event.ss = 0x9abcu;
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
        sequence.records[0].disposition != BX_NTVDM_GENERIC_UD_PASS_THROUGH ||
        sequence.records[0].eax != event.eax || sequence.records[0].ebx != event.ebx ||
        sequence.records[0].ecx != event.ecx || sequence.records[0].edx != event.edx ||
        sequence.records[0].esi != event.esi || sequence.records[0].edi != event.edi ||
        sequence.records[0].eflags != event.eflags || sequence.records[0].ds != event.ds ||
        sequence.records[0].es != event.es || sequence.records[0].ss != event.ss)
        return 1;

    /* Fixed capacity is observational: the excess event cannot alter outcome. */
    bx_ntvdm_bop_sequence_observation_v1_enable(1u);
    for (index = 0u;
         index < BX_NTVDM_BOP_SEQUENCE_OBSERVATION_V1_MAX_RECORDS + 1u;
         ++index) {
        event.eip = 0x2000u + index;
        event.eax = 0x11112222u + index;
        bx_ntvdm_bop_sequence_observation_v1_consider(&event, &outcome);
    }
    if (!bx_ntvdm_bop_sequence_observation_v1_copy(&sequence) ||
        sequence.record_count != BX_NTVDM_BOP_SEQUENCE_OBSERVATION_V1_MAX_RECORDS ||
        sequence.overflowed != 1u || outcome.disposition != 99u ||
        sequence.records[BX_NTVDM_BOP_SEQUENCE_OBSERVATION_V1_MAX_RECORDS - 1u].eip !=
            0x2000u + BX_NTVDM_BOP_SEQUENCE_OBSERVATION_V1_MAX_RECORDS - 1u ||
        sequence.records[BX_NTVDM_BOP_SEQUENCE_OBSERVATION_V1_MAX_RECORDS - 1u].eax !=
            0x11112222u + BX_NTVDM_BOP_SEQUENCE_OBSERVATION_V1_MAX_RECORDS - 1u)
        return 2;
    bx_ntvdm_bop_sequence_observation_v1_enable(0u);
    return bx_ntvdm_bop_sequence_observation_v1_copy(&sequence) ? 3 : 0;
}
