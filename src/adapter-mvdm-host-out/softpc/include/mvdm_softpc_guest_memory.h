#ifndef MVDM_SOFTPC_GUEST_MEMORY_H
#define MVDM_SOFTPC_GUEST_MEMORY_H

#include "session/session.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Bind the current CPU40 SAS address space as the active session's bounded
 * guest-memory lease provider.  The values remain Intel linear addresses;
 * this interface never publishes a SoftPC backing pointer. */
int mvdm_softpc_guest_memory_begin(session *owner);
void mvdm_softpc_guest_memory_end(session *owner);

/* The only historical guest-pointer seam.  Callers provide original numeric
 * guest addresses and a bounded synchronous span; the SoftPC adapter binds
 * the current session's one guest-memory mapping instance. */
int mvdm_softpc_guest_memory_acquire(uint32_t address, uint32_t byte_count,
    uint32_t access, guest_memory_lease **lease_out, uint8_t **bytes_out);
int mvdm_softpc_guest_memory_release(guest_memory_lease *lease, int commit);

#ifdef __cplusplus
}
#endif

#endif
