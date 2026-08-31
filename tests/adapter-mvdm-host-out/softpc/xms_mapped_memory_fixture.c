#include "session/session.h"
#include "mvdm_xms_memory.h"
#include "xms.h"

#include <stdlib.h>
#include <string.h>

#define FIXTURE_MEMORY_BYTES 8192u

typedef struct fixture_memory {
    uint8_t bytes[FIXTURE_MEMORY_BYTES];
} fixture_memory;

static int fixture_read(void *context, uint32_t address, uint8_t *bytes,
    uint32_t byte_count)
{
    fixture_memory *memory = (fixture_memory *)context;
    if (address > FIXTURE_MEMORY_BYTES || byte_count > FIXTURE_MEMORY_BYTES - address)
        return 0;
    memcpy(bytes, memory->bytes + address, byte_count);
    return 1;
}

static int fixture_write(void *context, uint32_t address, const uint8_t *bytes,
    uint32_t byte_count)
{
    fixture_memory *memory = (fixture_memory *)context;
    if (address > FIXTURE_MEMORY_BYTES || byte_count > FIXTURE_MEMORY_BYTES - address)
        return 0;
    memcpy(memory->bytes + address, bytes, byte_count);
    return 1;
}

static void fixture_write_u32(uint8_t *bytes, uint32_t value)
{
    bytes[0] = (uint8_t)value;
    bytes[1] = (uint8_t)(value >> 8);
    bytes[2] = (uint8_t)(value >> 16);
    bytes[3] = (uint8_t)(value >> 24);
}

int main(void)
{
    session instance;
    fixture_memory memory;
    uint8_t expected[FIXTURE_MEMORY_BYTES];
    uint32_t index;
    uint32_t descriptor_address = ((uint32_t)0x0010u << 4) + 0x0010u - 12u;

    memset(&memory, 0xff, sizeof(memory));
    session_initialize(&instance, 322u);
    if (!session_activate(&instance) || !session_thread_bind(&instance) ||
        !session_guest_memory_begin(&instance, &memory, fixture_read, fixture_write))
        return 1;

    if (xmsCommitBlock(64u, 16u) != 0 ||
        memcmp(memory.bytes + 64u, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 16u) != 0 ||
        xmsCommitBlock(UINT32_MAX, 2u) == 0)
        return 2;

    for (index = 0u; index < 5000u; ++index) memory.bytes[100u + index] = (uint8_t)(index * 17u);
    memcpy(expected, memory.bytes, sizeof(expected));
    memmove(expected + 200u, expected + 100u, 5000u);
    xmsMoveMemory(200u, 100u, 5000u);
    if (memcmp(memory.bytes, expected, sizeof(expected)) != 0 ||
        instance.state != SESSION_STATE_ACTIVE)
        return 3;

    fixture_write_u32(memory.bytes + descriptor_address, 700u);
    fixture_write_u32(memory.bytes + descriptor_address + 4u, 600u);
    fixture_write_u32(memory.bytes + descriptor_address + 8u, 2u);
    memcpy(memory.bytes + 600u, "XMS!", 4u);
    if (!mvdm_xms_move_block(0x0010u, 0x0010u) ||
        memcmp(memory.bytes + 700u, "XMS!", 4u) != 0)
        return 4;

    if (xmsDecommitBlock(700u, 4u) != 0 ||
        memcmp(memory.bytes + 700u, "\0\0\0\0", 4u) != 0)
        return 5;

    if (mvdm_xms_move_block(0u, 0u) != 0 ||
        instance.state != SESSION_STATE_CANCELLED)
        return 6;
    session_guest_memory_end(&instance);
    if (!session_thread_unbind(&instance) || !session_dispose(&instance)) return 7;
    return 0;
}
