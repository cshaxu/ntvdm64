#ifndef MVDM_GUEST_LOCATION_H
#define MVDM_GUEST_LOCATION_H

#include <stdint.h>

#include "session/guest_memory_lease.h"

/*
 * A retained MVDM guest location.  It deliberately preserves the original
 * real-mode numeric form; it is neither a native pointer nor an opaque
 * mapping-manager identity.
 */
typedef struct mvdm_guest_location {
    uint16_t segment;
    uint16_t offset;
    uint32_t valid;
} mvdm_guest_location;

typedef struct mvdm_guest_location_lease {
    guest_memory_lease *lease;
    uint8_t *bytes;
} mvdm_guest_location_lease;

int mvdm_guest_location_set_real_mode(mvdm_guest_location *location,
    uint16_t segment, uint16_t offset);
int mvdm_guest_location_from_far_value(mvdm_guest_location *location,
    uint32_t far_value);
int mvdm_guest_location_read_far(mvdm_guest_location const *location,
    mvdm_guest_location *far_location_out);
int mvdm_guest_location_acquire_far(mvdm_guest_location const *location,
    uint32_t byte_count, uint32_t access,
    mvdm_guest_location_lease *lease_out);
int mvdm_guest_location_acquire(mvdm_guest_location const *location,
    uint32_t byte_count, uint32_t access,
    mvdm_guest_location_lease *lease_out);
int mvdm_guest_location_release(mvdm_guest_location_lease *lease,
    int commit);

#endif
