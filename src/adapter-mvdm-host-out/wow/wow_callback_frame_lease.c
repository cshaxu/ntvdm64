#include "wow_callback_frame_lease.h"

#include <string.h>

#include "session/session.h"

int wow_callback_frame_acquire(uint32_t guest_address, uint32_t byte_count,
    uint32_t access, wow_callback_frame_lease *view_out)
{
    session *owner = session_thread_current();
    guest_memory_lease *lease;
    uint8_t *bytes;

    if (view_out == NULL || owner == NULL || byte_count == 0u ||
        (access != GUEST_MEMORY_ACCESS_READ &&
         access != GUEST_MEMORY_ACCESS_WRITE)) return 0;
    memset(view_out, 0, sizeof(*view_out));
    if (!session_guest_memory_acquire(owner, guest_address, byte_count,
        access, &lease, &bytes)) return 0;
    view_out->lease = lease;
    view_out->bytes = bytes;
    view_out->guest_address = guest_address;
    view_out->byte_count = byte_count;
    view_out->access = access;
    return 1;
}

int wow_callback_frame_release(wow_callback_frame_lease *view, int commit)
{
    session *owner = session_thread_current();
    int result;

    if (view == NULL || owner == NULL || view->lease == NULL ||
        view->bytes == NULL || view->byte_count == 0u ||
        (view->access != GUEST_MEMORY_ACCESS_READ &&
         view->access != GUEST_MEMORY_ACCESS_WRITE) ||
        (commit != 0 && commit != 1) ||
        (commit != 0 && view->access != GUEST_MEMORY_ACCESS_WRITE)) return 0;
    result = session_guest_memory_release(owner, view->lease, commit);
    memset(view, 0, sizeof(*view));
    return result;
}
