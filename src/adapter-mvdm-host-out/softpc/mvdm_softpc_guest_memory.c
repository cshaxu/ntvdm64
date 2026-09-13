#include "mvdm_softpc_guest_memory.h"

#include <string.h>

#define MVDM_SOFTPC_GUEST_MEMORY_COPY_CHUNK 4096u

/* CPU40 `sas4gen.h` publishes these exact CCPU exports after its historical
 * `insignia.h` macro layer.  This adapter deliberately declares only the
 * three fixed-width entries it consumes: importing `sas.h` here would make a
 * non-mirror translation unit depend on that source-era include ordering. */
extern uint32_t c_sas_memory_size(void);
extern void c_sas_loads(uint32_t address, uint8_t *bytes,
    uint32_t byte_count);
extern void c_sas_stores(uint32_t address, uint8_t *bytes,
    uint32_t byte_count);

static int mvdm_softpc_guest_memory_range_valid(uint32_t address,
    uint32_t byte_count)
{
    uint32_t memory_size = c_sas_memory_size();

    return address <= memory_size && byte_count <= memory_size - address;
}

static int mvdm_softpc_guest_memory_read(void *context, uint32_t address,
    uint8_t *bytes, uint32_t byte_count)
{
    (void)context;
    if (bytes == NULL || !mvdm_softpc_guest_memory_range_valid(address,
        byte_count)) return 0;
    if (byte_count != 0u) c_sas_loads(address, bytes, byte_count);
    return 1;
}

static int mvdm_softpc_guest_memory_write(void *context, uint32_t address,
    const uint8_t *bytes, uint32_t byte_count)
{
    (void)context;
    if (bytes == NULL || !mvdm_softpc_guest_memory_range_valid(address,
        byte_count)) return 0;
    if (byte_count != 0u) c_sas_stores(address, (uint8_t *)bytes, byte_count);
    return 1;
}

int mvdm_softpc_guest_memory_begin(session *owner)
{
    if (owner == NULL || !session_valid(owner) ||
        owner->guest_memory_lease.active != 0u) return 0;
    return session_guest_memory_begin(owner, owner, mvdm_softpc_guest_memory_read,
        mvdm_softpc_guest_memory_write);
}

void mvdm_softpc_guest_memory_end(session *owner)
{
    if (owner != NULL) session_guest_memory_end(owner);
}

int mvdm_softpc_guest_memory_acquire(uint32_t address, uint32_t byte_count,
    uint32_t access, guest_memory_lease **lease_out, uint8_t **bytes_out)
{
    session *owner = session_thread_current();

    if (owner == NULL) return 0;
    return session_guest_memory_acquire(owner, address, byte_count, access,
        lease_out, bytes_out);
}

int mvdm_softpc_guest_memory_release(guest_memory_lease *lease, int commit)
{
    session *owner = session_thread_current();

    if (owner == NULL) return 0;
    return session_guest_memory_release(owner, lease, commit);
}

int mvdm_softpc_guest_memory_copy_from(uint32_t address, uint8_t *bytes,
    uint32_t byte_count)
{
    session *owner = session_thread_current();
    uint32_t offset = 0u;

    if (owner == NULL || (byte_count != 0u && bytes == NULL) ||
        byte_count > UINT32_MAX - address) return 0;
    while (offset < byte_count) {
        guest_memory_lease *lease;
        uint8_t *guest_bytes;
        uint32_t chunk = byte_count - offset;

        if (chunk > MVDM_SOFTPC_GUEST_MEMORY_COPY_CHUNK)
            chunk = MVDM_SOFTPC_GUEST_MEMORY_COPY_CHUNK;
        if (!session_guest_memory_acquire(owner, address + offset, chunk,
            GUEST_MEMORY_ACCESS_READ, &lease, &guest_bytes)) return 0;
        memcpy(bytes + offset, guest_bytes, chunk);
        if (!session_guest_memory_release(owner, lease, 0)) return 0;
        offset += chunk;
    }
    return 1;
}

int mvdm_softpc_guest_memory_copy_to(uint32_t address, uint8_t const *bytes,
    uint32_t byte_count)
{
    session *owner = session_thread_current();
    uint32_t offset = 0u;

    if (owner == NULL || (byte_count != 0u && bytes == NULL) ||
        byte_count > UINT32_MAX - address) return 0;
    while (offset < byte_count) {
        guest_memory_lease *lease;
        uint8_t *guest_bytes;
        uint32_t chunk = byte_count - offset;

        if (chunk > MVDM_SOFTPC_GUEST_MEMORY_COPY_CHUNK)
            chunk = MVDM_SOFTPC_GUEST_MEMORY_COPY_CHUNK;
        if (!session_guest_memory_acquire(owner, address + offset, chunk,
            GUEST_MEMORY_ACCESS_WRITE, &lease, &guest_bytes)) return 0;
        memcpy(guest_bytes, bytes + offset, chunk);
        if (!session_guest_memory_release(owner, lease, 1)) return 0;
        offset += chunk;
    }
    return 1;
}

int mvdm_softpc_guest_memory_move(uint32_t destination, uint32_t source,
    uint32_t byte_count)
{
    uint8_t bytes[MVDM_SOFTPC_GUEST_MEMORY_COPY_CHUNK];
    uint32_t remaining = byte_count;

    if (byte_count > UINT32_MAX - source ||
        byte_count > UINT32_MAX - destination) return 0;
    while (remaining != 0u) {
        uint32_t chunk = remaining;
        uint32_t offset;

        if (chunk > (uint32_t)sizeof(bytes)) chunk = (uint32_t)sizeof(bytes);
        offset = destination > source && destination - source < byte_count ?
            remaining - chunk : byte_count - remaining;
        if (!mvdm_softpc_guest_memory_copy_from(source + offset, bytes, chunk) ||
            !mvdm_softpc_guest_memory_copy_to(destination + offset, bytes, chunk))
            return 0;
        remaining -= chunk;
    }
    return 1;
}
