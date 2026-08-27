#include "mvdm_guest_location.h"

#include <string.h>

#include "session/session.h"

static int mvdm_guest_location_linear_address(
    mvdm_guest_location const *location, uint32_t *address_out)
{
    uint32_t address;

    if (address_out != NULL) *address_out = 0u;
    if (location == NULL || address_out == NULL || location->valid != 1u)
        return 0;

    address = ((uint32_t)location->segment << 4) + location->offset;
    *address_out = address;
    return 1;
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

int mvdm_guest_location_from_far_value(mvdm_guest_location *location,
    uint32_t far_value)
{
    return mvdm_guest_location_set_real_mode(location,
        (uint16_t)(far_value >> 16), (uint16_t)far_value);
}

int mvdm_guest_location_read_far(mvdm_guest_location const *location,
    mvdm_guest_location *far_location_out)
{
    mvdm_guest_location_lease lease;
    mvdm_guest_location resolved;
    uint32_t far_value;

    if (far_location_out == NULL || !mvdm_guest_location_acquire(location,
        4u, GUEST_MEMORY_ACCESS_READ, &lease)) return 0;
    far_value = (uint32_t)lease.bytes[0] |
        ((uint32_t)lease.bytes[1] << 8) |
        ((uint32_t)lease.bytes[2] << 16) |
        ((uint32_t)lease.bytes[3] << 24);
    if (!mvdm_guest_location_release(&lease, 0) ||
        !mvdm_guest_location_from_far_value(&resolved, far_value)) return 0;
    *far_location_out = resolved;
    return 1;
}

int mvdm_guest_location_acquire_far(mvdm_guest_location const *location,
    uint32_t byte_count, uint32_t access,
    mvdm_guest_location_lease *lease_out)
{
    mvdm_guest_location far_location;

    return mvdm_guest_location_read_far(location, &far_location) &&
        mvdm_guest_location_acquire(&far_location, byte_count, access,
            lease_out);
}

int mvdm_guest_location_read_u16(mvdm_guest_location const *location,
    uint16_t *value_out)
{
    mvdm_guest_location_lease lease;
    uint16_t value;

    if (value_out != NULL) *value_out = 0u;
    if (value_out == NULL || !mvdm_guest_location_acquire(location, 2u,
        GUEST_MEMORY_ACCESS_READ, &lease)) return 0;
    value = (uint16_t)((uint16_t)lease.bytes[0] |
        ((uint16_t)lease.bytes[1] << 8));
    if (!mvdm_guest_location_release(&lease, 0)) return 0;
    *value_out = value;
    return 1;
}

int mvdm_guest_location_acquire(mvdm_guest_location const *location,
    uint32_t byte_count, uint32_t access,
    mvdm_guest_location_lease *lease_out)
{
    session *instance = session_thread_current();
    uint32_t address;

    if (lease_out != NULL) memset(lease_out, 0, sizeof(*lease_out));
    if (lease_out == NULL || instance == NULL ||
        !mvdm_guest_location_linear_address(location, &address)) return 0;

    return session_guest_memory_acquire(instance, address, byte_count, access,
        &lease_out->lease, &lease_out->bytes);
}

int mvdm_guest_location_release(mvdm_guest_location_lease *lease,
    int commit)
{
    session *instance = session_thread_current();
    int result;

    if (lease == NULL || instance == NULL || lease->lease == NULL) return 0;
    result = session_guest_memory_release(instance, lease->lease, commit);
    memset(lease, 0, sizeof(*lease));
    return result;
}
