#ifndef WOW_CALLBACK_FRAME_LEASE_H
#define WOW_CALLBACK_FRAME_LEASE_H

#include <stdint.h>

#include "session/guest_memory_lease.h"

/*
 * A source-shaped replacement for the temporary `GETFRAMEPTR` mappings used
 * by original WOW CallBack16.  It owns no WOW task, callback, CPU or guest
 * control semantics: it makes a single numeric VDM frame address available
 * only during the original source's read/write/flush/free interval.
 */
typedef struct wow_callback_frame_lease {
    guest_memory_lease *lease;
    uint8_t *bytes;
    uint32_t guest_address;
    uint32_t byte_count;
    uint32_t access;
} wow_callback_frame_lease;

int wow_callback_frame_acquire(uint32_t guest_address, uint32_t byte_count,
    uint32_t access, wow_callback_frame_lease *view_out);
int wow_callback_frame_release(wow_callback_frame_lease *view, int commit);

#endif
