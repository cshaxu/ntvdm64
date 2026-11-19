#include "compat/include/guest_location.h"
#include <stddef.h>

int mvdm_guest_location_from_far_value(mvdm_guest_location *location,
    uint32_t far_value)
{
    return mvdm_guest_location_set_real_mode(location,
        (uint16_t)(far_value >> 16), (uint16_t)far_value);
}

int mvdm_guest_location_set_real_mode(mvdm_guest_location *location,
    uint16_t segment, uint16_t offset)
{
    if (location == NULL) return 0;
    location->segment = segment;
    location->offset = offset;
    location->valid = 1u;
    return 1;
}
