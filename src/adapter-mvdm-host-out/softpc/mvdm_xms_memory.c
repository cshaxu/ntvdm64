#include "mvdm_xms_memory.h"

#include <string.h>

#include "mvdm_softpc_guest_memory.h"
#include "session/session.h"
#include "xms.h"

#define MVDM_XMS_ZERO_CHUNK_BYTES 4096u

static int mvdm_xms_zero_range(session *owner, uint32_t address,
    uint32_t byte_count)
{
    uint32_t offset = 0u;

    while (offset < byte_count) {
        guest_memory_lease *lease;
        uint8_t *bytes;
        uint32_t chunk = byte_count - offset;

        if (chunk > MVDM_XMS_ZERO_CHUNK_BYTES) chunk = MVDM_XMS_ZERO_CHUNK_BYTES;
        if (!session_guest_memory_acquire(owner, address + offset, chunk,
            GUEST_MEMORY_ACCESS_WRITE, &lease, &bytes)) return 0;
        memset(bytes, 0, chunk);
        if (!session_guest_memory_release(owner, lease, 1)) return 0;
        offset += chunk;
    }
    return 1;
}

NTSTATUS xmsCommitBlock(ULONG base_address, ULONG byte_count)
{
    session *owner = session_thread_current();

    if (owner == NULL || byte_count > UINT32_MAX - base_address ||
        !mvdm_xms_zero_range(owner, base_address, byte_count)) {
        return (NTSTATUS)UINT32_C(0xc0000001);
    }
    return 0;
}

NTSTATUS xmsDecommitBlock(ULONG base_address, ULONG byte_count)
{
    /* A released XMS range must not retain a previous guest-byte value.
     * The selected SoftPC backing is already reserved, so decommit is its
     * observable zero-on-next-commit effect rather than a host VirtualFree. */
    return xmsCommitBlock(base_address, byte_count);
}

VOID xmsMoveMemory(ULONG destination, ULONG source, ULONG byte_count)
{
    session *owner = session_thread_current();

    if (owner == NULL || byte_count > UINT32_MAX - source ||
        byte_count > UINT32_MAX - destination ||
        !mvdm_softpc_guest_memory_move(destination, source, byte_count)) {
        if (owner != NULL) (void)session_request_cancellation(owner,
            SESSION_CANCELLATION_REQUESTED);
    }
}

void mvdm_xms_cancel_current_operation(void)
{
    session *owner = session_thread_current();

    if (owner != NULL) (void)session_request_cancellation(owner,
        SESSION_CANCELLATION_REQUESTED);
}
