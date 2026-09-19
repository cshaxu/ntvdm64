#include "ntvdm-exe/session/session.h"
#include "mvdm_softpc_guest_memory.h"
#include "mvdm_xms_memory.h"
#include "xms.h"
#include "suballoc.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define FIXTURE_MEMORY_BYTES 131072u

typedef struct fixture_memory {
    uint8_t bytes[FIXTURE_MEMORY_BYTES];
    int reject_read;
    int reject_write;
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
    if (memory->reject_read) return 0;
    if (address > FIXTURE_MEMORY_BYTES || byte_count > FIXTURE_MEMORY_BYTES - address)
        return 0;
    memcpy(bytes, memory->bytes + address, byte_count);
    return 1;
}

static int fixture_write(void *context, uint32_t address, const uint8_t *bytes,
    uint32_t byte_count)
{
    fixture_memory *memory = (fixture_memory *)context;
    if (memory->reject_write) return 0;
    if (address > FIXTURE_MEMORY_BYTES || byte_count > FIXTURE_MEMORY_BYTES - address)
        return 0;
    memcpy(memory->bytes + address, bytes, byte_count);
    return 1;
}

static int allocator_failure_audit(session *instance, fixture_memory *memory)
{
    PVOID pool = SAInitialize(0u, FIXTURE_MEMORY_BYTES, xmsCommitBlock,
        xmsDecommitBlock, xmsMoveMemory);
    ULONG address, available, largest;
    if (!pool || !SAAllocate(pool, FIXTURE_MEMORY_BYTES, &address) || address)
        return 20;
    memset(memory->bytes + 65536u, 0x5a, 4096u);
    memory->reject_write = 1;
    if (SAFree(pool, 4096u, 65536u)) return 21;
    memory->reject_write = 0;
    /* This is an expected-defect reproduction, never a rollback pass. The
     * failing real callback leaves the bytes intact but FreeChunk's fixed
     * struct copy does not restore the trailing allocation bitmap. */
    if (!SAQueryFree(pool, &available, &largest) || available != 4096u ||
        largest != 4096u || memory->bytes[65536u] != 0x5a ||
        instance->state != SESSION_STATE_ACTIVE) return 22;
    /* FirstFree was restored while the trailing bitmap was not: reported
     * free capacity is not actually found by the next allocation. */
    if (SAAllocate(pool, 4096u, &address) ||
        memory->bytes[65536u] != 0x5a) return 23;
    free(pool);
    puts("S36_KNOWN_DEFECT_FAILED_FREE_PHANTOM_CAPACITY_REPRODUCED");
    return 0;
}

int main(int argc, char **argv)
{
    session instance;
    fixture_memory memory;
    uint8_t expected[FIXTURE_MEMORY_BYTES];
    uint32_t index;
    guest_memory_lease *held[GUEST_MEMORY_LEASE_MAXIMUM];
    uint8_t *bytes;

    memset(&memory, 0xff, sizeof(memory));
    memory.reject_read = memory.reject_write = 0;
    session_initialize(&instance, 322u);
    if (!session_activate(&instance) || !session_thread_bind(&instance) ||
        !session_guest_memory_begin(&instance, &memory, fixture_read, fixture_write))
        return 1;

    if (argc == 2 && strcmp(argv[1], "--allocator-failure-audit") == 0) {
        int result = allocator_failure_audit(&instance, &memory);
        session_guest_memory_end(&instance);
        if (!session_thread_unbind(&instance) || !session_dispose(&instance)) return 24;
        return result;
    }

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

    /* Exercise real worker callbacks and leases, not substitute allocator
     * callbacks. Failure is reported, and every failed lease must be released. */
    memory.reject_read = 1;
    if (xmsCommitBlock(0u, 4096u) == 0) return 8;
    memory.reject_read = 0;
    memory.reject_write = 1;
    if (xmsDecommitBlock(0u, 4096u) == 0) return 9;
    memory.reject_write = 0;
    for (index = 0; index < GUEST_MEMORY_LEASE_MAXIMUM; ++index) {
        if (instance.guest_memory_lease.leases[index].active ||
            instance.guest_memory_lease.leases[index].bounce) return 10;
        if (!session_guest_memory_acquire(&instance, 0u, 1u,
            GUEST_MEMORY_ACCESS_READ, &held[index], &bytes)) return 11;
    }
    if (xmsCommitBlock(0u, 4096u) == 0) return 12;
    for (index = 0; index < GUEST_MEMORY_LEASE_MAXIMUM; ++index)
        if (!session_guest_memory_release(&instance, held[index], 0)) return 13;
    if (xmsCommitBlock(0u, 4096u) != 0 ||
        instance.state != SESSION_STATE_ACTIVE) return 14;
    puts("S36_XMS_CALLBACK_FAILURE_LEASE_RELEASE_REUSE_OK");

    memory.reject_read = 1;
    xmsMoveMemory(4096u, 0u, 4096u);
    if (instance.state != SESSION_STATE_CANCELLED) return 15;
    puts("S36_XMS_MOVE_FAILURE_CANCEL_OK");
    session_guest_memory_end(&instance);
    if (!session_thread_unbind(&instance) || !session_dispose(&instance)) return 7;
    return 0;
}
