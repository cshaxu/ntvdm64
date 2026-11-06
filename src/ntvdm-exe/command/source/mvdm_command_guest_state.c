#include "mvdm_command_guest_state.h"

#include <stddef.h>
#include <string.h>

#include "ntvdm-exe/session/session.h"
#include "mvdm-host/dos/command/cmd.h"

/* DIVERGENCE(ADAPTER-SOFTPC-037): OpenNT retained these COMMAND/DEM/SoftPC
 * DOSDATA scalars as process-address aliases. Preserve one-bound-session use,
 * but retain only session-owned numeric guest locations and revalidate
 * the session and guest-memory epochs before every short lease. */
typedef struct mvdm_command_guest_state {
    session *owner;
    uint32_t session_epoch;
    uint32_t guest_memory_epoch;
    mvdm_guest_location scs;
    mvdm_guest_location to_sync;
    mvdm_guest_location is_dos_binary;
    mvdm_guest_location fd_access;
    uint32_t scs_bytes;
} mvdm_command_guest_state;

typedef struct command_registration {
    SRWLOCK lock;
    mvdm_command_guest_state state;
} command_registration;

static void registration_teardown(void *context)
{
    command_registration *registration = context;
    registration->state.owner->mvdm_command_guest_state = NULL;
    free(registration);
}

/* COMMAND registers before launching original FDC work. The session's existing
 * bound-thread disposal precondition owns this allocation's lifetime. */
static int registration_store(session *owner,
    const mvdm_command_guest_state *state)
{
    command_registration *registration = owner->mvdm_command_guest_state;
    if (registration == NULL) {
        registration = calloc(1, sizeof(*registration));
        if (registration == NULL) return 0;
        if (!session_register_teardown(owner, registration_teardown, registration)) {
            free(registration);
            return 0;
        }
        owner->mvdm_command_guest_state = registration;
    }
    AcquireSRWLockExclusive(&registration->lock);
    registration->state = *state;
    ReleaseSRWLockExclusive(&registration->lock);
    return 1;
}

/* DIV-194: original cmdGetNextCmd exposes COMMAND's completed environment
 * while cmdCheckForPIF expands startup fields. Copy only that bounded guest
 * input, release the lease before host/UI calls, then discard the snapshot. */
void mvdm_command_check_pif(PVDMINFO info, uint16_t segment, uint16_t bytes)
{
    mvdm_guest_location location;
    mvdm_guest_location_lease lease;
    char *snapshot;
    uint32_t i;
    if (cmdVDMEnvBlk.lpszzEnv || !IsFirstVDM) { cmdCheckForPIF(info); return; }
    if (bytes<2 || !mvdm_guest_location_set_real_mode(&location,segment,0)) {
        info->ErrorCode=ERROR_INVALID_ADDRESS;
        return;
    }
    snapshot=malloc(bytes);
    if (!snapshot) { info->ErrorCode=ERROR_NOT_ENOUGH_MEMORY; return; }
    if (!mvdm_guest_location_acquire(&location,bytes,GUEST_MEMORY_ACCESS_READ,&lease))
        info->ErrorCode=ERROR_INVALID_ADDRESS;
    else {
        memcpy(snapshot,lease.bytes,bytes);
        if (!mvdm_guest_location_release(&lease,0)) {
            free(snapshot);
            info->ErrorCode=ERROR_INVALID_ADDRESS;
            return;
        }
        for (i=0;i+1<bytes;++i) if (!snapshot[i] && !snapshot[i+1]) break;
        if (i+1==bytes) info->ErrorCode=ERROR_INVALID_ADDRESS;
        else {
            cmdVDMEnvBlk.lpszzEnv=snapshot;
            cmdCheckForPIF(info);
            cmdVDMEnvBlk.lpszzEnv=NULL;
        }
    }
    free(snapshot);
}

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

static int current_state_load(mvdm_command_guest_state *state)
{
    session *owner = session_thread_current();
    command_registration *registration;
    if (owner == NULL || !session_valid(owner) ||
        owner->state != SESSION_STATE_ACTIVE) return 0;
    registration = owner->mvdm_command_guest_state;
    if (registration == NULL) return 0;
    AcquireSRWLockShared(&registration->lock);
    *state = registration->state;
    ReleaseSRWLockShared(&registration->lock);
    return state->owner == owner && state->session_epoch == owner->epoch &&
        state->scs.valid == 1u &&
        state->guest_memory_epoch == owner->guest_memory_lease.epoch &&
        owner->guest_memory_lease.active != 0u;
}

int mvdm_command_guest_state_set_scs(uint16_t segment, uint16_t offset,
    uint32_t scs_bytes, uint32_t to_sync_offset)
{
    mvdm_command_guest_state current_state;
    if (!mvdm_command_guest_state_set_scs_scalars(segment, offset, scs_bytes,
            to_sync_offset, 0u, 0u, 0u, 0u)) return 0;
    if (!current_state_load(&current_state)) return 0;
    current_state.is_dos_binary.valid = 0u;
    current_state.fd_access.valid = 0u;
    return registration_store(current_state.owner, &current_state);
}

int mvdm_command_guest_state_set_scs_scalars(uint16_t scs_segment,
    uint16_t scs_offset, uint32_t scs_bytes, uint32_t to_sync_offset,
    uint16_t binary_segment, uint16_t binary_offset,
    uint16_t fd_access_segment, uint16_t fd_access_offset)
{
    session *owner = session_thread_current();
    mvdm_command_guest_state next;

    if (owner == NULL || !session_valid(owner) || scs_bytes == 0u ||
        to_sync_offset >= scs_bytes ||
        owner->state != SESSION_STATE_ACTIVE ||
        owner->guest_memory_lease.active == 0u) return 0;
    memset(&next, 0, sizeof(next));
    next.owner = owner;
    next.session_epoch = owner->epoch;
    next.guest_memory_epoch = owner->guest_memory_lease.epoch;
    next.scs_bytes = scs_bytes;
    if (!mvdm_guest_location_set_real_mode(&next.scs, scs_segment, scs_offset) ||
        !advance_real_mode(&next.scs, to_sync_offset, &next.to_sync))
        return 0;
    if (!mvdm_guest_location_set_real_mode(&next.is_dos_binary,
            binary_segment, binary_offset) ||
        !mvdm_guest_location_set_real_mode(&next.fd_access,
            fd_access_segment, fd_access_offset)) return 0;
    return registration_store(owner, &next);
}

int mvdm_command_guest_state_acquire_scs(uint32_t access,
    mvdm_guest_location_lease *lease_out)
{
    mvdm_command_guest_state current_state;
    if (!current_state_load(&current_state)) return 0;
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
    mvdm_command_guest_state current_state;
    mvdm_guest_location location;

    if (segment_out != NULL) *segment_out = 0u;
    if (offset_out != NULL) *offset_out = 0u;
    if (segment_out == NULL || offset_out == NULL || !current_state_load(&current_state) ||
        !advance_real_mode(&current_state.scs, byte_offset, &location)) return 0;
    *segment_out = location.segment;
    *offset_out = location.offset;
    return 1;
}

int mvdm_command_guest_state_set_to_sync(uint8_t value)
{
    mvdm_command_guest_state current_state;
    mvdm_guest_location_lease lease;

    if (!current_state_load(&current_state) || !mvdm_guest_location_acquire(
        &current_state.to_sync, 1u, GUEST_MEMORY_ACCESS_WRITE, &lease)) return 0;
    lease.bytes[0] = value;
    return mvdm_guest_location_release(&lease, 1);
}

int mvdm_command_guest_state_write_is_dos_binary(uint8_t value)
{
    mvdm_command_guest_state current_state;
    mvdm_guest_location_lease lease;

    if (!current_state_load(&current_state) || current_state.is_dos_binary.valid != 1u ||
        !mvdm_guest_location_acquire(&current_state.is_dos_binary, 1u,
            GUEST_MEMORY_ACCESS_WRITE, &lease)) return 0;
    lease.bytes[0] = value;
    return mvdm_guest_location_release(&lease, 1);
}

static int fd_access_copy(const mvdm_command_guest_state *state,
    uint8_t bytes[2], int write)
{
    return state->fd_access.valid == 1u && guest_memory_lease_copy(
        &state->owner->guest_memory_lease, state->guest_memory_epoch,
        ((uint32_t)state->fd_access.segment << 4) + state->fd_access.offset,
        bytes, 2u, write);
}

int mvdm_command_guest_state_read_fd_access(uint16_t *value_out)
{
    mvdm_command_guest_state state;
    uint8_t bytes[2];
    if (value_out != NULL) *value_out = 0u;
    if (value_out == NULL || !current_state_load(&state) ||
        !fd_access_copy(&state, bytes, 0)) return 0;
    *value_out = (uint16_t)(bytes[0] | ((uint16_t)bytes[1] << 8));
    return 1;
}

int mvdm_command_guest_state_write_fd_access(uint16_t value)
{
    mvdm_command_guest_state state;
    uint8_t bytes[2];
    if (!current_state_load(&state)) return 0;
    bytes[0] = (uint8_t)value;
    bytes[1] = (uint8_t)(value >> 8);
    return fd_access_copy(&state, bytes, 1);
}

int mvdm_command_guest_state_add_fd_access(uint16_t modular_delta)
{
    mvdm_command_guest_state state;
    uint8_t bytes[2];
    uint16_t value;
    /* Both copies must refer to the same registration/epoch. This retains
     * original modular arithmetic, not atomicity against guest execution. */
    if (!current_state_load(&state) || !fd_access_copy(&state, bytes, 0)) return 0;
    value = (uint16_t)((bytes[0] | ((uint16_t)bytes[1] << 8)) + modular_delta);
    bytes[0] = (uint8_t)value;
    bytes[1] = (uint8_t)(value >> 8);
    return fd_access_copy(&state, bytes, 1);
}
