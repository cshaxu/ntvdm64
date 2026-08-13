#include "bx_ntvdm_guest_range.h"

int bx_ntvdm_guest_range_within(uint64_t aperture_bytes,
    const bx_ntvdm_guest_range *range)
{
    if (range == 0 || range->address > aperture_bytes) {
        return 0;
    }
    return range->length <= aperture_bytes - range->address;
}
