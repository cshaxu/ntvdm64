#include "initial_state_abi.h"

int main(void)
{
    runtime_initial_state value;
    runtime_initial_state_clear(&value);
    if (!runtime_initial_state_valid(&value) ||
        runtime_initial_state_admitted(&value, 0x100000u)) return 1;
    value.disposition = RUNTIME_INITIAL_STATE_PRESENT;
    value.range_count = 2u;
    value.payload_bytes = 5u;
    value.evidence_sha256[0] = 1u;
    runtime_guest_write_initialize(&value.ranges[0], 0x40u, 4u, 0u);
    runtime_guest_write_initialize(&value.ranges[1], 0x496u, 1u, 4u);
    if (!runtime_initial_state_valid(&value) ||
        !runtime_initial_state_admitted(&value, 0x100000u)) return 2;
    value.ranges[1].guest_physical_address = 0x42u;
    if (runtime_initial_state_admitted(&value, 0x100000u)) return 3;
    value.ranges[1].guest_physical_address = 0x496u;
    value.evidence_sha256[0] = 0u;
    if (runtime_initial_state_valid(&value)) return 4;
    return 0;
}
