#ifndef MVDM_SOFTPC_GUEST_MEMORY_H
#define MVDM_SOFTPC_GUEST_MEMORY_H

#include "ntvdm-exe/session/session.h"
#include "compat/include/guest_memory_access.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Session admission for bounded access to the current CPU40 SAS address space.
 * Addresses are Intel linear values, never native backing pointers. */
/* C-owned SAS coordination. F may use an explicitly admitted resource owner. */
int ccpu_guest_memory_acquire(guest_memory_lease_context *context,
    uint32_t address, uint32_t byte_count, uint32_t access,
    guest_memory_lease **lease_out, uint8_t **bytes_out);
int ccpu_guest_memory_release(guest_memory_lease_context *context,
    guest_memory_lease *lease, int commit);

int mvdm_softpc_guest_memory_begin(session *owner);
void mvdm_softpc_guest_memory_end(session *owner);

#ifdef __cplusplus
}
#endif

#endif
