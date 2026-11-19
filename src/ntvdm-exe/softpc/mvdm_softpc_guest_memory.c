#include "mvdm_softpc_guest_memory.h"

/* CCPU-owned checked SAS provider; F owns provider registration/lifetime. */
extern int mvdm_softpc_guest_memory_read(void *context, uint32_t address,
    uint8_t *bytes, uint32_t byte_count);
extern int mvdm_softpc_guest_memory_write(void *context, uint32_t address,
    const uint8_t *bytes, uint32_t byte_count);

int mvdm_softpc_guest_memory_begin(session *owner)
{
    if (owner == NULL || !session_valid(owner) ||
        owner->guest_memory_lease.active != 0u) return 0;
    return session_guest_memory_begin(owner, NULL, mvdm_softpc_guest_memory_read,
        mvdm_softpc_guest_memory_write);
}

void mvdm_softpc_guest_memory_end(session *owner)
{
    if (owner != NULL) session_guest_memory_end(owner);
}
