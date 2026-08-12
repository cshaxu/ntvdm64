#include "nt4_mantle_initial_state.h"

#include <stdio.h>
#include <string.h>

static nt4_mantle_evidence source_evidence(uint64_t record_id)
{
    nt4_mantle_evidence evidence = { NT4_MANTLE_EVIDENCE_SOURCE_LOCKED, record_id };
    return evidence;
}

static int expect(nt4_mantle_initial_state_result actual,
    nt4_mantle_initial_state_result expected)
{
    return actual == expected ? 0 : 1;
}

int main(void)
{
    nt4_mantle_initial_state state = { 0 };
    nt4_mantle_initial_state accepted = { 0 };
    int failed = 0;

    state.incoming_int10.evidence = source_evidence(1u);
    state.incoming_int15.evidence = source_evidence(2u);
    state.incoming_int19.evidence = source_evidence(3u);
    state.incoming_int1b.evidence = source_evidence(4u);
    state.keyboard_flags_0096.evidence = source_evidence(5u);
    failed |= expect(nt4_mantle_initial_state_validate(&state, &accepted),
        NT4_MANTLE_INITIAL_STATE_OK);
    failed |= memcmp(&state, &accepted, sizeof(state)) != 0;
    state.incoming_int10.offset = 0xffffu;
    failed |= accepted.incoming_int10.offset != 0u;

    state = accepted;
    state.incoming_int10.evidence.evidence_class = NT4_MANTLE_EVIDENCE_UNPROVEN;
    failed |= expect(nt4_mantle_initial_state_validate(&state, &accepted),
        NT4_MANTLE_INITIAL_STATE_INT10_UNPROVEN);
    state = accepted;
    state.incoming_int15.evidence.record_id = 0u;
    failed |= expect(nt4_mantle_initial_state_validate(&state, &accepted),
        NT4_MANTLE_INITIAL_STATE_INT15_UNPROVEN);
    state = accepted;
    state.incoming_int19.evidence.evidence_class = NT4_MANTLE_EVIDENCE_UNPROVEN;
    failed |= expect(nt4_mantle_initial_state_validate(&state, &accepted),
        NT4_MANTLE_INITIAL_STATE_INT19_UNPROVEN);
    state = accepted;
    state.incoming_int1b.evidence.record_id = 0u;
    failed |= expect(nt4_mantle_initial_state_validate(&state, &accepted),
        NT4_MANTLE_INITIAL_STATE_INT1B_UNPROVEN);
    state = accepted;
    state.keyboard_flags_0096.evidence.evidence_class = NT4_MANTLE_EVIDENCE_UNPROVEN;
    failed |= expect(nt4_mantle_initial_state_validate(&state, &accepted),
        NT4_MANTLE_INITIAL_STATE_BDA_0096_UNPROVEN);
    if (failed) return 1;
    puts("nt4-mantle-initial-state-test: evidence-gated IVT/BDA profile verified");
    return 0;
}
