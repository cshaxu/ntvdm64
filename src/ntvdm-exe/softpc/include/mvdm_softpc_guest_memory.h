#ifndef MVDM_SOFTPC_GUEST_MEMORY_H
#define MVDM_SOFTPC_GUEST_MEMORY_H

#include "ntvdm-exe/session/session.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Bind the current CPU40 SAS address space as the active session's bounded
 * guest-memory lease provider.  The values remain Intel linear addresses;
 * this interface never publishes a SoftPC backing pointer. */
int mvdm_softpc_guest_memory_begin(session *owner);
void mvdm_softpc_guest_memory_end(session *owner);

/* EMS uses these only after its original native-pointer fast path finds an
 * alias-backed span.  The copies own no EMS state and retain no guest alias. */
int mvdm_softpc_guest_memory_copy_from(uint32_t address, uint8_t *bytes,
    uint32_t byte_count);
int mvdm_softpc_guest_memory_copy_to(uint32_t address, uint8_t const *bytes,
    uint32_t byte_count);
/* Source-shaped RtlCopyMemory transport for callers whose original contract
 * guarantees only forward (destination-before-source) overlap.  Full-span
 * overwrite uses numeric CCPU addresses, never a native alias or a generic
 * read-modify-write lease. */
int mvdm_softpc_guest_memory_copy_forward(uint32_t destination,
    uint32_t source, uint32_t byte_count);
int mvdm_softpc_guest_memory_move(uint32_t destination, uint32_t source,
    uint32_t byte_count);

#ifdef __cplusplus
}
#endif

#endif
