#include "session/session.h"

#include <string.h>

typedef struct test_memory {
    uint8_t bytes[64];
    uint32_t read_count;
    uint32_t write_count;
} test_memory;

static int read_memory(void *context, uint32_t address, uint8_t *bytes,
    uint32_t byte_count)
{
    test_memory *memory = (test_memory *)context;
    if (address > sizeof(memory->bytes) || byte_count > sizeof(memory->bytes) - address)
        return 0;
    memcpy(bytes, memory->bytes + address, byte_count);
    ++memory->read_count;
    return 1;
}

static int write_memory(void *context, uint32_t address, const uint8_t *bytes,
    uint32_t byte_count)
{
    test_memory *memory = (test_memory *)context;
    if (address > sizeof(memory->bytes) || byte_count > sizeof(memory->bytes) - address)
        return 0;
    memcpy(memory->bytes + address, bytes, byte_count);
    ++memory->write_count;
    return 1;
}

int main(void)
{
    session instance;
    test_memory memory;
    guest_memory_lease *lease;
    uint8_t *bytes;

    memset(&memory, 0, sizeof(memory));
    memory.bytes[8] = 0x11u;
    session_initialize(&instance, 1u);
    if (session_activate(&instance) == 0 ||
        session_guest_memory_begin(&instance, &memory, read_memory, write_memory) == 0 ||
        session_guest_memory_acquire(&instance, 8u, 2u,
            GUEST_MEMORY_ACCESS_WRITE, &lease, &bytes) == 0 || bytes[0] != 0x11u)
        return 1;
    bytes[1] = 0x22u;
    if (session_guest_memory_release(&instance, lease, 1) == 0 ||
        memory.bytes[9] != 0x22u || memory.read_count != 1u || memory.write_count != 1u)
        return 2;
    if (session_guest_memory_acquire(&instance, UINT32_MAX, 2u,
            GUEST_MEMORY_ACCESS_READ, &lease, &bytes) != 0 ||
        session_guest_memory_acquire(&instance, 8u, 1u, 0u, &lease, &bytes) != 0)
        return 3;
    if (session_guest_memory_acquire(&instance, 8u, 1u,
            GUEST_MEMORY_ACCESS_READ, &lease, &bytes) == 0)
        return 4;
    session_guest_memory_end(&instance);
    if (session_guest_memory_release(&instance, lease, 0) != 0 ||
        session_dispose(&instance) == 0)
        return 5;
    return 0;
}
