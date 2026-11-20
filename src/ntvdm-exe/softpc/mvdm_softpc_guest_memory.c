#include "mvdm_softpc_guest_memory.h"

/* F selects the lifetime of B resources; C owns machine access. */
int mvdm_softpc_guest_memory_begin(session *owner)
{
    if (owner == NULL || !session_valid(owner) ||
        owner->guest_memory_lease.active != 0u) return 0;
    return session_guest_memory_begin(owner);
}

void mvdm_softpc_guest_memory_end(session *owner)
{
    if (owner != NULL) session_guest_memory_end(owner);
}

int session_guest_memory_acquire(session *instance, uint32_t address,
    uint32_t byte_count, uint32_t access, guest_memory_lease **lease_out,
    uint8_t **bytes_out)
{
    if (!session_valid(instance) || instance->host_context.state != SESSION_STATE_ACTIVE)
        return 0;
    return ccpu_guest_memory_acquire(&instance->guest_memory_lease, address,
        byte_count, access, lease_out, bytes_out);
}

int session_guest_memory_release(session *instance, guest_memory_lease *lease,
    int commit)
{
    if (!session_valid(instance) || instance->host_context.state != SESSION_STATE_ACTIVE)
        return 0;
    return ccpu_guest_memory_release(&instance->guest_memory_lease, lease,
        commit);
}
