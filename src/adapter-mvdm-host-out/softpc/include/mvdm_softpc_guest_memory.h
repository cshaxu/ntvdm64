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

#ifdef __cplusplus
}
#endif

#endif
