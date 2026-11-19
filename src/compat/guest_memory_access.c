#include "compat/include/guest_memory_access.h"
#include "compat/include/guest_memory_lease.h"
#include "compat/include/host_context.h"

#include <string.h>

#define MVDM_SOFTPC_GUEST_MEMORY_COPY_CHUNK 4096u

static int acquire(compat_host_context *owner, uint32_t address,
    uint32_t byte_count, uint32_t access, guest_memory_lease **lease_out,
    uint8_t **bytes_out)
{
    if (!compat_host_context_valid(owner) || !owner->registration_open) return 0;
    return guest_memory_lease_acquire(owner->guest_memory, address, byte_count,
        access, lease_out, bytes_out);
}

static int release(compat_host_context *owner, guest_memory_lease *lease,
    int commit)
{
    if (!compat_host_context_valid(owner) || !owner->registration_open) return 0;
    return guest_memory_lease_release(owner->guest_memory, lease, commit);
}

int mvdm_softpc_guest_memory_acquire(uint32_t address, uint32_t byte_count,
    uint32_t access, guest_memory_lease **lease_out, uint8_t **bytes_out)
{
    compat_host_context *owner = compat_host_context_current();

    if (owner == NULL) return 0;
    return acquire(owner, address, byte_count, access,
        lease_out, bytes_out);
}

int mvdm_softpc_guest_memory_release(guest_memory_lease *lease, int commit)
{
    compat_host_context *owner = compat_host_context_current();

    if (owner == NULL) return 0;
    return release(owner, lease, commit);
}

int mvdm_softpc_guest_memory_copy_from(uint32_t address, uint8_t *bytes,
    uint32_t byte_count)
{
    compat_host_context *owner = compat_host_context_current();
    uint32_t offset = 0u;

    if (owner == NULL || (byte_count != 0u && bytes == NULL) ||
        byte_count > UINT32_MAX - address) return 0;
    while (offset < byte_count) {
        guest_memory_lease *lease;
        uint8_t *guest_bytes;
        uint32_t chunk = byte_count - offset;

        if (chunk > MVDM_SOFTPC_GUEST_MEMORY_COPY_CHUNK)
            chunk = MVDM_SOFTPC_GUEST_MEMORY_COPY_CHUNK;
        if (!acquire(owner, address + offset, chunk,
            GUEST_MEMORY_ACCESS_READ, &lease, &guest_bytes)) return 0;
        memcpy(bytes + offset, guest_bytes, chunk);
        if (!release(owner, lease, 0)) return 0;
        offset += chunk;
    }
    return 1;
}

int mvdm_softpc_guest_memory_copy_to(uint32_t address, uint8_t const *bytes,
    uint32_t byte_count)
{
    compat_host_context *owner = compat_host_context_current();
    uint32_t offset = 0u;

    if (owner == NULL || (byte_count != 0u && bytes == NULL) ||
        byte_count > UINT32_MAX - address) return 0;
    while (offset < byte_count) {
        guest_memory_lease *lease;
        uint8_t *guest_bytes;
        uint32_t chunk = byte_count - offset;

        if (chunk > MVDM_SOFTPC_GUEST_MEMORY_COPY_CHUNK)
            chunk = MVDM_SOFTPC_GUEST_MEMORY_COPY_CHUNK;
        if (!acquire(owner, address + offset, chunk,
            GUEST_MEMORY_ACCESS_WRITE, &lease, &guest_bytes)) return 0;
        memcpy(guest_bytes, bytes + offset, chunk);
        if (!release(owner, lease, 1)) return 0;
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
