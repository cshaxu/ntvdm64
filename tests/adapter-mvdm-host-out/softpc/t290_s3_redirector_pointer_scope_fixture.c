#include "mvdm_redirector_pointer_scope.h"

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
    uint8_t *first;
    uint8_t *second;
    uint32_t first_address = 0x1020u;
    uint32_t second_address = 0x1030u;

    memset(&memory, 0, sizeof(memory));
    session_initialize(&instance, 1u);
    if (!session_activate(&instance) ||
        !session_guest_memory_begin(&instance, &memory, read_memory, write_memory) ||
        !session_thread_bind(&instance)) return 1;
    if (mvdm_redirector_pointer_from_real_mode(0x0100u, 0x0020u) != NULL ||
        !mvdm_redirector_pointer_scope_begin() ||
        mvdm_redirector_pointer_scope_begin() ||
        mvdm_redirector_pointer_from_source_address(0x01000020u, 1u) != NULL ||
        !mvdm_redirector_pointer_scope_prepare(0x0100u, 0x0020u, 2u,
            GUEST_MEMORY_ACCESS_WRITE) ||
        !mvdm_redirector_pointer_scope_prepare(0x0100u, 0x0030u, 1u,
            GUEST_MEMORY_ACCESS_WRITE))
        return 2;
    first = (uint8_t *)mvdm_redirector_pointer_from_real_mode(0x0100u, 0x0020u);
    second = (uint8_t *)mvdm_redirector_pointer_from_source_address(0x01000030u, 0u);
    if (first == NULL || second == NULL) return 3;
    first[0] = 0xa1u;
    first[1] = 0xb2u;
    second[0] = 0xc3u;
    if (!mvdm_redirector_pointer_scope_end(1) ||
        memory.bytes[first_address] != 0xa1u ||
        memory.bytes[first_address + 1u] != 0xb2u ||
        memory.bytes[second_address] != 0xc3u ||
        mvdm_redirector_pointer_from_real_mode(0x0100u, 0x0020u) != NULL ||
        mvdm_redirector_pointer_scope_end(1)) return 4;
    if (!session_thread_unbind(&instance)) return 5;
    session_guest_memory_end(&instance);
    return session_dispose(&instance) ? 0 : 6;
}
