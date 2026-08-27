#include "mvdm_wow_pointer_scope.h"

#include <string.h>

#include "adapter-bochs/machine_facade.h"
#include "session/session.h"

typedef struct fixture_memory {
    uint8_t bytes[0x20000];
} fixture_memory;

uint32_t getPE(void)
{
    return 0u;
}

int machine_facade_copy_protected_segment(uint32_t slot,
    struct machine_facade_protected_segment *segment)
{
    (void)slot;
    (void)segment;
    return MACHINE_FACADE_PROTECTED_FRAME_REJECTED_MODE;
}

int machine_facade_resolve_protected_range(uint32_t kind, uint32_t segment,
    uint32_t offset, uint32_t byte_count, uint32_t *linear_out)
{
    (void)kind;
    (void)segment;
    (void)offset;
    (void)byte_count;
    (void)linear_out;
    return MACHINE_FACADE_PROTECTED_RANGE_REJECTED_MODE;
}

int machine_facade_protected_span_transfer(uint32_t kind, uint32_t segment,
    uint32_t offset, uint32_t byte_count, uint8_t *bytes)
{
    (void)kind;
    (void)segment;
    (void)offset;
    (void)byte_count;
    (void)bytes;
    return MACHINE_FACADE_PROTECTED_RANGE_REJECTED_MODE;
}

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
    session_initialize(&instance, 291u);
    if (!session_activate(&instance) ||
        !session_guest_memory_begin(&instance, &memory, read_memory, write_memory) ||
        !session_thread_bind(&instance)) return 1;
    if (mvdm_wow_pointer_scope_acquire(0x01000020u, 2u,
            MVDM_WOW_POINTER_ACCESS_WRITE) != NULL ||
        !mvdm_wow_pointer_scope_begin() ||
        mvdm_wow_pointer_scope_begin() ||
        mvdm_wow_pointer_scope_acquire(0x01000020u, 0u,
            MVDM_WOW_POINTER_ACCESS_WRITE) != NULL) return 2;
    first = (uint8_t *)mvdm_wow_pointer_scope_acquire(0x01000020u, 2u,
        MVDM_WOW_POINTER_ACCESS_WRITE);
    second = (uint8_t *)mvdm_wow_pointer_scope_acquire(0x01000030u, 1u,
        MVDM_WOW_POINTER_ACCESS_WRITE);
    if (first == NULL || second == NULL) return 3;
    first[0] = 0xa1u;
    first[1] = 0xb2u;
    second[0] = 0xc3u;
    if (!mvdm_wow_pointer_scope_release(first, 1) ||
        mvdm_wow_pointer_scope_release(first, 1) ||
        !mvdm_wow_pointer_scope_release(second, 0) ||
        memory.bytes[first_address] != 0xa1u ||
        memory.bytes[first_address + 1u] != 0xb2u ||
        memory.bytes[second_address] != 0u ||
        !mvdm_wow_pointer_scope_end() ||
        mvdm_wow_pointer_scope_end()) return 4;
    if (!session_thread_unbind(&instance)) return 5;
    session_guest_memory_end(&instance);
    return session_dispose(&instance) ? 0 : 6;
}
