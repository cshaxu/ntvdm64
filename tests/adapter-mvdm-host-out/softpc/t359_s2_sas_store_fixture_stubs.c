#include "mvdm_guest_location.h"
#include "session/session.h"

/*
 * The SAS-store observer fixture calls no session or guest-location operation.
 * These strict failing stand-ins isolate its default-off/reporting contract from
 * the unrelated observers co-located in mvdm_softpc_termination.c.
 */
int session_terminate_current(uint32_t completion_code)
{
    (void)completion_code;
    return 0;
}

int mvdm_guest_location_set_real_mode(mvdm_guest_location *location,
    uint16_t segment, uint16_t offset)
{
    (void)location;
    (void)segment;
    (void)offset;
    return 0;
}

int mvdm_guest_location_copy_c_string(mvdm_guest_location const *location,
    uint8_t *bytes_out, uint32_t capacity, uint32_t *byte_count_out)
{
    (void)location;
    (void)bytes_out;
    (void)capacity;
    (void)byte_count_out;
    return 0;
}

int mvdm_guest_location_acquire(mvdm_guest_location const *location,
    uint32_t byte_count, uint32_t access,
    mvdm_guest_location_lease *lease_out)
{
    (void)location;
    (void)byte_count;
    (void)access;
    (void)lease_out;
    return 0;
}

int mvdm_guest_location_release(mvdm_guest_location_lease *lease,
    int commit)
{
    (void)lease;
    (void)commit;
    return 0;
}
