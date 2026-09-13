#include "session/session.h"
#include "mvdm_softpc_guest_memory.h"
#include "mvdm_xms_memory.h"
#include "xms.h"

#include <stdlib.h>
#include <string.h>

#define FIXTURE_MEMORY_BYTES 8192u

typedef struct fixture_memory {
    uint8_t bytes[FIXTURE_MEMORY_BYTES];
} fixture_memory;

/* The focused lease fixture binds its own memory callbacks rather than the
 * production CPU40 SAS provider.  Keep these three exports only as dormant
 * link witnesses for the shared SoftPC adapter translation unit. */
uint32_t c_sas_memory_size(void) { return FIXTURE_MEMORY_BYTES; }
void c_sas_loads(uint32_t address, uint8_t *bytes, uint32_t byte_count)
{
    (void)address;
    (void)bytes;
    (void)byte_count;
}
void c_sas_stores(uint32_t address, uint8_t *bytes, uint32_t byte_count)
{
    (void)address;
    (void)bytes;
    (void)byte_count;
}

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

int main(void)
{
    session instance;
    fixture_memory memory;
    uint8_t expected[FIXTURE_MEMORY_BYTES];
    uint32_t index;

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

    memcpy(memory.bytes + 600u, "XMS!", 4u);
    if (!mvdm_softpc_guest_memory_copy_forward(700u, 600u, 4u) ||
        memcmp(memory.bytes + 700u, "XMS!", 4u) != 0)
        return 4;

    if (xmsDecommitBlock(700u, 4u) != 0 ||
        memcmp(memory.bytes + 700u, "\0\0\0\0", 4u) != 0)
        return 5;

    if (mvdm_softpc_guest_memory_copy_forward(UINT32_MAX, 0u, 2u) != 0 ||
        instance.state != SESSION_STATE_ACTIVE)
        return 6;
    session_guest_memory_end(&instance);
    if (!session_thread_unbind(&instance) || !session_dispose(&instance)) return 7;
    return 0;
}
