#include "ntvdm-exe/session/session.h"
#include "mvdm_softpc_guest_memory.h"
#include "xms.h"
#include "suballoc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEMORY_SIZE 131072u
static unsigned char memory[MEMORY_SIZE], expected[MEMORY_SIZE];
static unsigned reads, writes, commits, decommits;
static int reject_io;

/* Test endpoint for original CPU40 SAS management. Production uses stubs.c. */
BOOL sas_manage_xms(PVOID address, ULONG size, int action)
{
    if ((ULONG)address > MEMORY_SIZE || size > MEMORY_SIZE - (ULONG)address) return FALSE;
    if (action == 1) ++commits;
    else if (action == 2) ++decommits;
    else return FALSE;
    return TRUE;
}
uint32_t c_sas_memory_size(void) { return MEMORY_SIZE; }
void c_sas_loads(uint32_t a, uint8_t *b, uint32_t n) { memcpy(b, memory+a, n); }
void c_sas_stores(uint32_t a, uint8_t *b, uint32_t n) { memcpy(memory+a, b, n); }
static int read_memory(void *ctx, uint32_t a, uint8_t *b, uint32_t n)
{
    (void)ctx; ++reads;
    if (reject_io || a > MEMORY_SIZE || n > MEMORY_SIZE-a) return 0;
    memcpy(b, memory+a, n); return 1;
}
static int write_memory(void *ctx, uint32_t a, const uint8_t *b, uint32_t n)
{
    (void)ctx; ++writes;
    if (reject_io || a > MEMORY_SIZE || n > MEMORY_SIZE-a) return 0;
    memcpy(memory+a, b, n); return 1;
}
#define CHECK(x) do { if (!(x)) { printf("FAIL line %d\n", __LINE__); return 1; } } while (0)
int main(void)
{
    session owner;
    PVOID pool;
    ULONG a, available, largest;
    unsigned i;
    guest_memory_lease *held[GUEST_MEMORY_LEASE_MAXIMUM];
    uint8_t *bytes;
    session_initialize(&owner, 420u);
    CHECK(session_activate(&owner) && session_thread_bind(&owner));
    CHECK(session_guest_memory_begin(&owner, NULL, read_memory, write_memory));
    memset(memory, 0x5a, sizeof(memory));
    pool = SAInitialize(0, MEMORY_SIZE, xmsCommitBlock, xmsDecommitBlock, xmsMoveMemory);
    CHECK(pool && SAAllocate(pool, MEMORY_SIZE, &a) && a == 0);
    for (i=0; i<GUEST_MEMORY_LEASE_MAXIMUM; ++i)
        CHECK(session_guest_memory_acquire(&owner, 0, 1, GUEST_MEMORY_ACCESS_READ, &held[i], &bytes));
    reads = writes = 0; reject_io = 1;
    CHECK(SAFree(pool, 8192, 65536));
    CHECK(SAQueryFree(pool, &available, &largest) && available == 8192 && largest == 8192);
    CHECK(SAAllocate(pool, 8192, &a) && a == 65536);
    CHECK(reads == 0 && writes == 0 && memory[65536] == 0x5a && memory[69632] == 0x5a);
    CHECK(commits == 34 && decommits == 2);
    for (i=0; i<GUEST_MEMORY_LEASE_MAXIMUM; ++i)
        CHECK(session_guest_memory_release(&owner, held[i], 0));
    reject_io = 0;
    CHECK(SAFree(pool, MEMORY_SIZE, 0));
    CHECK(SAQueryFree(pool, &available, &largest) && available == MEMORY_SIZE);
    free(pool);
    puts("S36_ORIGINAL_XMS_MANAGE_NO_BOUNCE_NO_ZERO_REUSE_OK");
    for (i=0; i<5000; ++i) memory[100+i]=(unsigned char)(i*17);
    memcpy(expected, memory, sizeof(memory)); memmove(expected+200, expected+100, 5000);
    xmsMoveMemory(200, 100, 5000);
    CHECK(!memcmp(memory, expected, sizeof(memory)) && owner.state == SESSION_STATE_ACTIVE);
    reject_io = 1; xmsMoveMemory(4096, 0, 4096);
    CHECK(owner.state == SESSION_STATE_CANCELLED);
    session_guest_memory_end(&owner);
    CHECK(session_thread_unbind(&owner) && session_dispose(&owner));
    puts("S36_ORIGINAL_XMS_BOUNDED_MOVE_CANCEL_OK");
    return 0;
}
