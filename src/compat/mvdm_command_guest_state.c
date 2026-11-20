#include <windows.h>

#include "include/mvdm_command_guest_directory.h"
#include "compat/include/guest_location.h"

#include <stdlib.h>
#include <string.h>

#include "include/host_context.h"
#include "include/guest_memory_lease.h"
#include "mvdm-host/inc/doswow.h"
#include "mvdm-host/inc/curdir.h"

typedef struct command_registration {
    SRWLOCK lock;
    compat_host_context *owner;
    mvdm_guest_location doswow;
    uint32_t doswow_session_epoch, doswow_memory_epoch;
} command_registration;

static void registration_teardown(void *context)
{
    command_registration *registration = context;
    registration->owner->mvdm_command_guest_state = NULL;
    free(registration);
}

/* COMMAND registers before launching original FDC work. The host context's existing
 * bound-thread disposal precondition owns this allocation's lifetime. */
static command_registration *registration_ensure(compat_host_context *owner)
{
    command_registration *registration = owner->mvdm_command_guest_state;
    if (registration != NULL) return registration;
    registration = calloc(1, sizeof(*registration));
    if (registration == NULL) return NULL;
    registration->owner = owner;
    if (!compat_host_context_register(owner, registration_teardown, registration)) {
        free(registration);
        return NULL;
    }
    owner->mvdm_command_guest_state = registration;
    return registration;
}

/* DOS owns DOSWOWDATA; retain only its numeric location and resolve the
 * current lpCDSBuffer under the existing session-bound memory lease. */
void mvdm_command_guest_state_clear_doswow(void)
{
    compat_host_context *owner = compat_host_context_current();
    command_registration *registration;
    if (owner == NULL || !compat_host_context_valid(owner)) return;
    registration = owner->mvdm_command_guest_state;
    if (registration == NULL) return;
    AcquireSRWLockExclusive(&registration->lock);
    memset(&registration->doswow, 0, sizeof(registration->doswow));
    registration->doswow_session_epoch = registration->doswow_memory_epoch = 0;
    ReleaseSRWLockExclusive(&registration->lock);
}

int mvdm_command_guest_state_register_doswow(uint16_t segment, uint16_t offset)
{
    compat_host_context *owner = compat_host_context_current();
    command_registration *registration;
    if (owner == NULL || !compat_host_context_valid(owner) ||
        owner->state != COMPAT_HOST_CONTEXT_ACTIVE ||
        owner->guest_memory == NULL || !owner->guest_memory->active)
        return 0;
    registration = registration_ensure(owner);
    if (registration == NULL) return 0;
    AcquireSRWLockExclusive(&registration->lock);
    mvdm_guest_location_set_real_mode(&registration->doswow, segment, offset);
    registration->doswow_session_epoch = owner->epoch;
    registration->doswow_memory_epoch = owner->guest_memory->epoch;
    ReleaseSRWLockExclusive(&registration->lock);
    return 1;
}

int mvdm_command_guest_state_directory_instance(uint32_t *linear_out,
    uint32_t *bytes_out)
{
    compat_host_context *owner = compat_host_context_current();
    command_registration *registration;
    mvdm_guest_location location;
    DOSWOWDATA doswow;
    uint32_t session_epoch, memory_epoch, far_value, segment, linear;
    uint8_t check[sizeof(CDS)];
    if (linear_out == NULL || bytes_out == NULL) return 0;
    *linear_out = *bytes_out = 0;
    if (owner == NULL || !compat_host_context_valid(owner) ||
        owner->state != COMPAT_HOST_CONTEXT_ACTIVE ||
        owner->guest_memory == NULL || !owner->guest_memory->active)
        return 0;
    registration = owner->mvdm_command_guest_state;
    if (registration == NULL) return 0;
    AcquireSRWLockShared(&registration->lock);
    location = registration->doswow;
    session_epoch = registration->doswow_session_epoch;
    memory_epoch = registration->doswow_memory_epoch;
    ReleaseSRWLockShared(&registration->lock);
    if (!location.valid || session_epoch != owner->epoch ||
        memory_epoch != owner->guest_memory->epoch ||
        !ccpu_guest_memory_copy(owner->guest_memory, memory_epoch,
            ((uint32_t)location.segment << 4) + location.offset,
            (uint8_t *)&doswow, sizeof(doswow), 0)) return 0;
    far_value = doswow.lpCDSBuffer;
    segment = far_value >> 16;
    if (!segment) segment = location.segment; /* before DOS patches far segments */
    linear = (segment << 4) + (far_value & 0xffffu);
    if (!ccpu_guest_memory_copy(owner->guest_memory, memory_epoch,
            linear, check, sizeof(check), 0)) return 0;
    *linear_out = linear;
    *bytes_out = sizeof(CDS);
    return 1;
}
