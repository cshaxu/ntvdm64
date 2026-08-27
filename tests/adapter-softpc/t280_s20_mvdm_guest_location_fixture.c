#include "mvdm_guest_location.h"

#include <string.h>

#include "session/session.h"

typedef struct fixture_memory {
    uint8_t bytes[0x20000];
} fixture_memory;

static int read_memory(void *context, uint32_t address, uint8_t *bytes,
    uint32_t byte_count)
{
    fixture_memory *memory = (fixture_memory *)context;
    if (memory == NULL || bytes == NULL || address > sizeof(memory->bytes) ||
        byte_count > sizeof(memory->bytes) - address) return 0;
    memcpy(bytes, memory->bytes + address, byte_count);
    return 1;
}

static int write_memory(void *context, uint32_t address, uint8_t const *bytes,
    uint32_t byte_count)
{
    fixture_memory *memory = (fixture_memory *)context;
    if (memory == NULL || bytes == NULL || address > sizeof(memory->bytes) ||
        byte_count > sizeof(memory->bytes) - address) return 0;
    memcpy(memory->bytes + address, bytes, byte_count);
    return 1;
}

int main(void)
{
    fixture_memory memory;
    session instance;
    mvdm_guest_location location;
    mvdm_guest_location_lease lease;
    uint32_t address = 0x12396u;

    memset(&memory, 0, sizeof(memory));
    session_initialize(&instance, 1u);
    if (!session_activate(&instance) ||
        !session_guest_memory_begin(&instance, &memory, read_memory,
            write_memory) || !session_thread_bind(&instance) ||
        !mvdm_guest_location_set_real_mode(&location, 0x1234u, 0x0056u) ||
        !mvdm_guest_location_acquire(&location, 9u,
            GUEST_MEMORY_ACCESS_READ | GUEST_MEMORY_ACCESS_WRITE, &lease))
        return 1;
    if (lease.bytes == NULL || lease.lease == NULL || lease.lease->address != address)
        return 2;
    memset(lease.bytes, 0xa5, 9u);
    if (!mvdm_guest_location_release(&lease, 1) ||
        memory.bytes[address] != 0xa5 || memory.bytes[address + 8u] != 0xa5)
        return 3;
    if (!mvdm_guest_location_from_far_value(&location, 0x12340056u) ||
        !mvdm_guest_location_acquire(&location, 1u, GUEST_MEMORY_ACCESS_READ,
            &lease) || !mvdm_guest_location_release(&lease, 0)) return 4;
    memory.bytes[0x88u] = 0x56u;
    memory.bytes[0x89u] = 0x00u;
    memory.bytes[0x8au] = 0x34u;
    memory.bytes[0x8bu] = 0x12u;
    if (!mvdm_guest_location_set_real_mode(&location, 0u, 0x88u) ||
        !mvdm_guest_location_acquire_far(&location, 1u,
            GUEST_MEMORY_ACCESS_READ, &lease) || lease.lease->address != address ||
        !mvdm_guest_location_release(&lease, 0)) return 5;
    if (!mvdm_guest_location_set_real_mode(&location, 0xffffu, 0xffffu) ||
        mvdm_guest_location_acquire(&location, 1u, GUEST_MEMORY_ACCESS_READ,
            &lease)) return 6;
    if (!session_thread_unbind(&instance)) return 7;
    session_guest_memory_end(&instance);
    return session_dispose(&instance) ? 0 : 8;
}
