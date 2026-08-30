#include "mvdm_xms_memory.h"

#include <string.h>

#include "session/session.h"
#include "xms.h"

#define MVDM_XMS_MOVE_DESCRIPTOR_BYTES 12u
#define MVDM_XMS_MOVE_CHUNK_BYTES 4096u

static uint32_t mvdm_xms_read_u32(uint8_t const *bytes)
{
    return (uint32_t)bytes[0] | ((uint32_t)bytes[1] << 8) |
        ((uint32_t)bytes[2] << 16) | ((uint32_t)bytes[3] << 24);
}

static int mvdm_xms_copy_forward(session *owner, uint32_t destination,
    uint32_t source, uint32_t byte_count)
{
    uint8_t bytes[MVDM_XMS_MOVE_CHUNK_BYTES];
    uint32_t offset = 0u;

    while (offset < byte_count) {
        guest_memory_lease *source_lease;
        guest_memory_lease *destination_lease;
        uint8_t *source_bytes;
        uint8_t *destination_bytes;
        uint32_t chunk = byte_count - offset;

        if (chunk > (uint32_t)sizeof(bytes)) chunk = (uint32_t)sizeof(bytes);
        if (!session_guest_memory_acquire(owner, source + offset, chunk,
            GUEST_MEMORY_ACCESS_READ, &source_lease, &source_bytes)) return 0;
        memcpy(bytes, source_bytes, chunk);
        if (!session_guest_memory_release(owner, source_lease, 0)) return 0;
        if (!session_guest_memory_acquire(owner, destination + offset, chunk,
            GUEST_MEMORY_ACCESS_WRITE, &destination_lease,
            &destination_bytes)) return 0;
        memcpy(destination_bytes, bytes, chunk);
        if (!session_guest_memory_release(owner, destination_lease, 1)) return 0;
        offset += chunk;
    }
    return 1;
}

static int mvdm_xms_zero_range(session *owner, uint32_t address,
    uint32_t byte_count)
{
    uint32_t offset = 0u;

    while (offset < byte_count) {
        guest_memory_lease *lease;
        uint8_t *bytes;
        uint32_t chunk = byte_count - offset;

        if (chunk > MVDM_XMS_MOVE_CHUNK_BYTES) chunk = MVDM_XMS_MOVE_CHUNK_BYTES;
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
        !mvdm_xms_copy_forward(owner, destination, source, byte_count)) {
        if (owner != NULL) (void)session_request_cancellation(owner,
            SESSION_CANCELLATION_REQUESTED);
    }
}

int mvdm_xms_move_block(uint16_t stack_segment, uint16_t stack_offset)
{
    guest_memory_lease *descriptor_lease;
    session *owner = session_thread_current();
    uint8_t *descriptor;
    uint32_t descriptor_end;
    uint32_t source;
    uint32_t destination;
    uint32_t word_count;
    uint32_t byte_count;

    descriptor_end = ((uint32_t)stack_segment << 4) + stack_offset;
    if (owner == NULL || descriptor_end < MVDM_XMS_MOVE_DESCRIPTOR_BYTES ||
        !session_guest_memory_acquire(owner,
            descriptor_end - MVDM_XMS_MOVE_DESCRIPTOR_BYTES,
            MVDM_XMS_MOVE_DESCRIPTOR_BYTES, GUEST_MEMORY_ACCESS_READ,
            &descriptor_lease, &descriptor)) {
        if (owner != NULL) (void)session_request_cancellation(owner,
            SESSION_CANCELLATION_REQUESTED);
        return 0;
    }

    destination = mvdm_xms_read_u32(descriptor);
    source = mvdm_xms_read_u32(descriptor + 4u);
    word_count = mvdm_xms_read_u32(descriptor + 8u);
    if (!session_guest_memory_release(owner, descriptor_lease, 0) ||
        word_count > UINT32_MAX / 2u) {
        (void)session_request_cancellation(owner, SESSION_CANCELLATION_REQUESTED);
        return 0;
    }
    byte_count = word_count * 2u;
    if (byte_count > UINT32_MAX - source ||
        byte_count > UINT32_MAX - destination ||
        !mvdm_xms_copy_forward(owner, destination, source, byte_count)) {
        (void)session_request_cancellation(owner, SESSION_CANCELLATION_REQUESTED);
        return 0;
    }
    return 1;
}
