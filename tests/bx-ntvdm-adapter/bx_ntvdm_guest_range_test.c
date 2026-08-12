#include "bx_ntvdm_guest_range.h"

int main(void)
{
    bx_ntvdm_guest_range range;
    range.address = 0;
    range.length = 0;
    if (!bx_ntvdm_guest_range_within(0x1000u, &range)) return 1;
    range.address = 0x0ff0u;
    range.length = 0x10u;
    if (!bx_ntvdm_guest_range_within(0x1000u, &range)) return 2;
    range.length = 0x11u;
    if (bx_ntvdm_guest_range_within(0x1000u, &range)) return 3;
    range.address = UINT64_MAX;
    range.length = 1u;
    if (bx_ntvdm_guest_range_within(0x1000u, &range)) return 4;
    if (bx_ntvdm_guest_range_within(0x1000u, 0)) return 5;
    return 0;
}
