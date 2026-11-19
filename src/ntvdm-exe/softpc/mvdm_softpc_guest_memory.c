#include "mvdm_softpc_guest_memory.h"

/* CCPU-owned checked SAS provider; F owns provider registration/lifetime. */
extern int mvdm_softpc_guest_memory_range_valid(uint32_t address,
    uint32_t byte_count, int write);
extern int mvdm_softpc_guest_memory_read(void *context, uint32_t address,
    uint8_t *bytes, uint32_t byte_count);
extern int mvdm_softpc_guest_memory_write(void *context, uint32_t address,
    const uint8_t *bytes, uint32_t byte_count);
extern void c_sas_move_bytes_forward(uint32_t source, uint32_t destination,
    uint32_t byte_count);

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

int mvdm_softpc_guest_memory_copy_forward(uint32_t destination,
    uint32_t source, uint32_t byte_count)
{
    session *owner = session_thread_current();

    if (byte_count > UINT32_MAX - source ||
        byte_count > UINT32_MAX - destination) return 0;
    if (byte_count == 0u) return 1;
    if (!session_valid(owner) || owner->state != SESSION_STATE_ACTIVE ||
        owner->guest_memory_lease.active != 1u ||
        !mvdm_softpc_guest_memory_range_valid(source, byte_count, 0) ||
        !mvdm_softpc_guest_memory_range_valid(destination, byte_count, 1)) return 0;

    /* XMS's original MoveBlock contract overwrites every destination byte in
     * forward order.  Keep the transfer as numeric CCPU addresses: unlike a
     * generic WRITE lease, it neither exposes a host alias nor hydrates a
     * destination bounce buffer that this operation cannot read. */
    c_sas_move_bytes_forward(source, destination, byte_count);
    return 1;
}
