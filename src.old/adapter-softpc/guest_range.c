#include "guest_range.h"

int runtime_guest_range_within(uint64_t aperture_bytes,
    const runtime_guest_range *range)
{
    if (range == 0 || range->address > aperture_bytes) {
        return 0;
    }
    return range->length <= aperture_bytes - range->address;
}
