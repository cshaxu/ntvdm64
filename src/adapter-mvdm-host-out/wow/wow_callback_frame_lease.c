#include "wow_callback_frame_lease.h"

#include <string.h>

#include "adapter-mvdm-host-out/softpc/include/mvdm_softpc_guest_memory.h"

/* This is the fixed-width source contract exported by the SoftPC adapter.
 * Do not include the full historical SoftPC macro environment here: that
 * environment is owned by original mirror translation units, while this
 * narrow WOW binding only consumes this one numeric conversion. */
extern int mvdm_softpc_effective_address(uint16_t selector, uint32_t offset,
    uint32_t *address_out);

int wow_callback_frame_acquire_linear(uint32_t guest_address, uint32_t byte_count,
    uint32_t access, wow_callback_frame_lease *view_out)
{
    guest_memory_lease *lease;
    uint8_t *bytes;

    if (view_out == NULL || byte_count == 0u ||
        (access != GUEST_MEMORY_ACCESS_READ &&
         access != GUEST_MEMORY_ACCESS_WRITE)) return 0;
    memset(view_out, 0, sizeof(*view_out));
    if (!mvdm_softpc_guest_memory_acquire(guest_address, byte_count,
        access, &lease, &bytes)) return 0;
    view_out->lease = lease;
    view_out->bytes = bytes;
    view_out->guest_address = guest_address;
    view_out->byte_count = byte_count;
    view_out->access = access;
    return 1;
}

int wow_callback_frame_acquire_vp(uint32_t vp, uint32_t byte_count,
    uint32_t access, wow_callback_frame_lease *view_out)
{
    uint32_t linear;

    if (!mvdm_softpc_effective_address((uint16_t)(vp >> 16),
        (uint16_t)vp, &linear)) return 0;
    return wow_callback_frame_acquire_linear(linear, byte_count, access,
        view_out);
}

int wow_callback_frame_release(wow_callback_frame_lease *view, int commit)
{
    int result;

    if (view == NULL || view->lease == NULL ||
        view->bytes == NULL || view->byte_count == 0u ||
        (view->access != GUEST_MEMORY_ACCESS_READ &&
         view->access != GUEST_MEMORY_ACCESS_WRITE) ||
        (commit != 0 && commit != 1) ||
        (commit != 0 && view->access != GUEST_MEMORY_ACCESS_WRITE)) return 0;
    result = mvdm_softpc_guest_memory_release(view->lease, commit);
    memset(view, 0, sizeof(*view));
    return result;
}
