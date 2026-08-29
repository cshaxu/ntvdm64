#include "mvdm_command_guest_state.h"

#include <stddef.h>
#include <string.h>

#include "session/session.h"

/* The original COMMAND globals were process-address aliases.  Preserve their
 * one-bound-session use, but retain only numeric guest locations per calling
 * thread.  COMMAND workers bind their creator session before entering the
 * original source body. */
typedef struct mvdm_command_guest_state {
    session *owner;
    mvdm_guest_location scs;
    mvdm_guest_location to_sync;
    uint32_t scs_bytes;
} mvdm_command_guest_state;

static __declspec(thread) mvdm_command_guest_state current_state;

static int advance_real_mode(const mvdm_guest_location *base,
    uint32_t byte_count, mvdm_guest_location *result)
{
    uint32_t linear;

    if (base == NULL || result == NULL || base->valid != 1u) return 0;
    linear = ((uint32_t)base->segment << 4) + base->offset;
    if (byte_count > UINT32_C(0x10ffef) - linear) return 0;
    linear += byte_count;
    return mvdm_guest_location_set_real_mode(result,
        (uint16_t)(linear >> 4), (uint16_t)(linear & 0x0fu));
}

static int current_state_valid(void)
{
    session *owner = session_thread_current();

    return owner != NULL && session_valid(owner) &&
        owner->state == SESSION_STATE_ACTIVE && current_state.owner == owner &&
        current_state.scs.valid == 1u;
}

int mvdm_command_guest_state_set_scs(uint16_t segment, uint16_t offset,
    uint32_t scs_bytes, uint32_t to_sync_offset)
{
    session *owner = session_thread_current();
    mvdm_command_guest_state next;

    if (owner == NULL || !session_valid(owner) || scs_bytes == 0u ||
        to_sync_offset >= scs_bytes ||
        owner->state != SESSION_STATE_ACTIVE) return 0;
    memset(&next, 0, sizeof(next));
    next.owner = owner;
    next.scs_bytes = scs_bytes;
    if (!mvdm_guest_location_set_real_mode(&next.scs, segment, offset) ||
        !advance_real_mode(&next.scs, to_sync_offset, &next.to_sync))
        return 0;
    current_state = next;
    return 1;
}

int mvdm_command_guest_state_acquire_scs(uint32_t access,
    mvdm_guest_location_lease *lease_out)
{
    if (!current_state_valid()) return 0;
    return mvdm_guest_location_acquire(&current_state.scs,
        current_state.scs_bytes, access, lease_out);
}

int mvdm_command_guest_state_release_scs(mvdm_guest_location_lease *lease,
    int commit)
{
    return mvdm_guest_location_release(lease, commit);
}

int mvdm_command_guest_state_scs_field(uint32_t byte_offset,
    uint16_t *segment_out, uint16_t *offset_out)
{
    mvdm_guest_location location;

    if (segment_out != NULL) *segment_out = 0u;
    if (offset_out != NULL) *offset_out = 0u;
    if (segment_out == NULL || offset_out == NULL || !current_state_valid() ||
        !advance_real_mode(&current_state.scs, byte_offset, &location)) return 0;
    *segment_out = location.segment;
    *offset_out = location.offset;
    return 1;
}

int mvdm_command_guest_state_set_to_sync(uint8_t value)
{
    mvdm_guest_location_lease lease;

    if (!current_state_valid() || !mvdm_guest_location_acquire(
        &current_state.to_sync, 1u, GUEST_MEMORY_ACCESS_WRITE, &lease)) return 0;
    lease.bytes[0] = value;
    return mvdm_guest_location_release(&lease, 1);
}
