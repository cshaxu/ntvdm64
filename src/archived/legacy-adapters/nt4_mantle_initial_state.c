#include "nt4_mantle_initial_state.h"

static int evidence_is_locked(const nt4_mantle_evidence *evidence)
{
    return evidence != NULL && evidence->record_id != 0u &&
        (evidence->evidence_class == NT4_MANTLE_EVIDENCE_SOURCE_LOCKED ||
         evidence->evidence_class == NT4_MANTLE_EVIDENCE_TRACE_LOCKED);
}

nt4_mantle_initial_state_result nt4_mantle_initial_state_validate(
    const nt4_mantle_initial_state *candidate,
    nt4_mantle_initial_state *out_accepted)
{
    if (candidate == NULL || out_accepted == NULL) {
        return NT4_MANTLE_INITIAL_STATE_INVALID_ARGUMENT;
    }
    if (!evidence_is_locked(&candidate->incoming_int10.evidence)) {
        return NT4_MANTLE_INITIAL_STATE_INT10_UNPROVEN;
    }
    if (!evidence_is_locked(&candidate->incoming_int15.evidence)) {
        return NT4_MANTLE_INITIAL_STATE_INT15_UNPROVEN;
    }
    if (!evidence_is_locked(&candidate->incoming_int19.evidence)) {
        return NT4_MANTLE_INITIAL_STATE_INT19_UNPROVEN;
    }
    if (!evidence_is_locked(&candidate->incoming_int1b.evidence)) {
        return NT4_MANTLE_INITIAL_STATE_INT1B_UNPROVEN;
    }
    if (!evidence_is_locked(&candidate->keyboard_flags_0096.evidence)) {
        return NT4_MANTLE_INITIAL_STATE_BDA_0096_UNPROVEN;
    }
    *out_accepted = *candidate;
    return NT4_MANTLE_INITIAL_STATE_OK;
}
