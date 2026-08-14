#include "bx_ntvdm_initial_state_abi.h"

int main(void)
{
    bx_ntvdm_initial_state_v1 value;
    bx_ntvdm_initial_state_v1_clear(&value);
    if (!bx_ntvdm_initial_state_v1_valid(&value) ||
        bx_ntvdm_initial_state_v1_admitted(&value, 0x100000u)) return 1;
    value.disposition = BX_NTVDM_INITIAL_STATE_V1_PRESENT;
    value.range_count = 2u;
    value.payload_bytes = 5u;
    value.evidence_sha256[0] = 1u;
    bx_ntvdm_guest_write_v1_initialize(&value.ranges[0], 0x40u, 4u, 0u);
    bx_ntvdm_guest_write_v1_initialize(&value.ranges[1], 0x496u, 1u, 4u);
    if (!bx_ntvdm_initial_state_v1_valid(&value) ||
        !bx_ntvdm_initial_state_v1_admitted(&value, 0x100000u)) return 2;
    value.ranges[1].guest_physical_address = 0x42u;
    if (bx_ntvdm_initial_state_v1_admitted(&value, 0x100000u)) return 3;
    value.ranges[1].guest_physical_address = 0x496u;
    value.evidence_sha256[0] = 0u;
    if (bx_ntvdm_initial_state_v1_valid(&value)) return 4;
    return 0;
}
