#include "compat/include/guest_location.h"
#include <windows.h>

static const compat_guest_location_services *binding;

int compat_guest_location_install(const compat_guest_location_services *services)
{
    const compat_guest_location_services *previous;
    if (!services || !services->copy_to_guest || !services->acquire ||
            !services->release) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    previous = InterlockedCompareExchangePointer((PVOID volatile *)&binding,
        (PVOID)services, NULL);
    if (previous && previous != services) {
        SetLastError(ERROR_ALREADY_INITIALIZED);
        return 0;
    }
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

int mvdm_guest_location_copy_to_guest(const mvdm_guest_location *location,
    const uint8_t *bytes, uint32_t byte_count)
{
    if (!binding) RaiseFailFastException(NULL, NULL, 0);
    return binding->copy_to_guest(location, bytes, byte_count);
}

int mvdm_guest_location_acquire(const mvdm_guest_location *location,
    uint32_t byte_count, uint32_t access, mvdm_guest_location_lease *lease_out)
{
    if (!binding) RaiseFailFastException(NULL, NULL, 0);
    return binding->acquire(location, byte_count, access, lease_out);
}

int mvdm_guest_location_release(mvdm_guest_location_lease *lease, int commit)
{
    if (!binding) RaiseFailFastException(NULL, NULL, 0);
    return binding->release(lease, commit);
}
