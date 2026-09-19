#include <windows.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "suballoc.h"

static ULONG commit_calls;
static ULONG decommit_calls;
static ULONG move_calls;

#define TEST_BASE 0x120000u
#define TEST_SIZE 0x20000u
static unsigned char backing[TEST_SIZE];
static ULONG fail_commit_call;
static ULONG fail_decommit_call;
static ULONG move_source, move_destination, move_size;

#define CHECK(expression) do { if (!(expression)) { \
    fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #expression); \
    return 1; } } while (0)

static NTSTATUS
fixture_commit(
    ULONG address,
    ULONG size
    )
{
    if (size != COMMIT_GRANULARITY || address < 0x120000u) return (NTSTATUS)1;
    commit_calls += 1u;
    if (commit_calls == fail_commit_call) return (NTSTATUS)1;
    return 0;
}

static NTSTATUS
fixture_decommit(
    ULONG address,
    ULONG size
    )
{
    if (size != COMMIT_GRANULARITY || address < 0x120000u) return (NTSTATUS)1;
    decommit_calls += 1u;
    if (decommit_calls == fail_decommit_call) return (NTSTATUS)1;
    return 0;
}

static VOID
fixture_move(
    ULONG destination,
    ULONG source,
    ULONG size
    )
{
    if (destination < TEST_BASE || source < TEST_BASE ||
        size > TEST_SIZE || destination - TEST_BASE > TEST_SIZE - size ||
        source - TEST_BASE > TEST_SIZE - size) abort();
    move_source = source;
    move_destination = destination;
    move_size = size;
    if (destination != source && size != 0u) move_calls += 1u;
    memmove(backing + destination - TEST_BASE,
        backing + source - TEST_BASE, size);
}

static PVOID fresh_pool(void)
{
    commit_calls = decommit_calls = move_calls = 0;
    fail_commit_call = fail_decommit_call = 0;
    move_source = move_destination = move_size = 0;
    memset(backing, 0, sizeof(backing));
    return SAInitialize(TEST_BASE, TEST_SIZE, fixture_commit,
        fixture_decommit, fixture_move);
}

static int capacity(PVOID pool, ULONG expected_free, ULONG expected_largest)
{
    ULONG available, largest;
    return SAQueryFree(pool, &available, &largest) &&
        available == expected_free && largest == expected_largest;
}

static void fill_pattern(ULONG address, ULONG size)
{
    ULONG i;
    for (i = 0; i < size; ++i)
        backing[address - TEST_BASE + i] = (unsigned char)(i * 37u + i / 251u);
}

static int pattern_intact(ULONG address, ULONG size)
{
    ULONG i;
    for (i = 0; i < size; ++i)
        if (backing[address - TEST_BASE + i] !=
            (unsigned char)(i * 37u + i / 251u)) return 0;
    return 1;
}

static int branch_tests(void)
{
    PVOID pool;
    ULONG a, b, c, moved, i, blocks[32];

    pool = fresh_pool();
    CHECK(pool != NULL);
    CHECK(SAAllocate(pool, 1025, &a));
    fill_pattern(a, 1025);
    CHECK(SAReallocate(pool, 1025, a, 2048, &moved) && moved == a);
    CHECK(SAReallocate(pool, 2048, a, 8192, &moved) && moved == a);
    CHECK(pattern_intact(a, 1025) && move_calls == 0);
    CHECK(SAReallocate(pool, 8192, a, 1024, &moved) && moved == a);
    CHECK(pattern_intact(a, 1024));
    CHECK(SAFree(pool, 1024, a) && capacity(pool, TEST_SIZE, TEST_SIZE));
    free(pool);
    puts("SUBALLOC_ROUND_TAIL_GROW_SHRINK_OK");

    pool = fresh_pool();
    CHECK(pool != NULL);
    CHECK(SAAllocate(pool, 1024, &a) && SAAllocate(pool, 4096, &b) &&
        SAAllocate(pool, 1024, &c));
    CHECK(b == a + 1024 && c == b + 4096);
    fill_pattern(b, 4096);
    CHECK(SAFree(pool, 1024, a));
    CHECK(SAReallocate(pool, 4096, b, 5120, &moved) && moved == a);
    CHECK(move_calls == 1 && move_source == b && move_destination == a &&
        move_size == 4096 && pattern_intact(moved, 4096));
    CHECK(SAFree(pool, 5120, moved) && SAFree(pool, 1024, c));
    CHECK(capacity(pool, TEST_SIZE, TEST_SIZE));
    free(pool);
    puts("SUBALLOC_PRECEDING_OVERLAP_DATA_OK");

    pool = fresh_pool();
    CHECK(pool != NULL);
    CHECK(SAAllocate(pool, 4096, &a) && SAAllocate(pool, 4096, &b));
    fill_pattern(a, 4096);
    CHECK(SAReallocate(pool, 4096, a, 8192, &moved) && moved == b + 4096);
    CHECK(move_calls == 1 && move_source == a && move_destination == moved &&
        move_size == 4096 && pattern_intact(moved, 4096));
    CHECK(SAFree(pool, 4096, b) && SAFree(pool, 8192, moved));
    CHECK(capacity(pool, TEST_SIZE, TEST_SIZE));
    free(pool);
    puts("SUBALLOC_ELSEWHERE_DATA_OK");

    pool = fresh_pool();
    CHECK(pool != NULL);
    for (i = 0; i < 32; ++i) CHECK(SAAllocate(pool, 4096, &blocks[i]));
    CHECK(capacity(pool, 0, 0) && !SAAllocate(pool, 1024, &a));
    for (i = 0; i < 32; i += 2) CHECK(SAFree(pool, 4096, blocks[i]));
    CHECK(capacity(pool, TEST_SIZE / 2, 4096));
    fill_pattern(blocks[1], 4096);
    CHECK(!SAAllocate(pool, 8192, &a));
    CHECK(!SAReallocate(pool, 4096, blocks[1], 12288, &moved));
    CHECK(pattern_intact(blocks[1], 4096));
    CHECK(capacity(pool, TEST_SIZE / 2, 4096));
    for (i = 1; i < 32; i += 2) CHECK(SAFree(pool, 4096, blocks[i]));
    CHECK(capacity(pool, TEST_SIZE, TEST_SIZE));
    CHECK(SAAllocate(pool, TEST_SIZE, &a) && SAFree(pool, TEST_SIZE, a));
    CHECK(capacity(pool, TEST_SIZE, TEST_SIZE));
    free(pool);
    puts("SUBALLOC_FRAGMENT_EXHAUST_FAILGROW_COALESCE_OK");

    pool = fresh_pool();
    CHECK(pool != NULL);
    fail_commit_call = 1;
    CHECK(!SAAllocate(pool, 4096, &a));
    CHECK(capacity(pool, TEST_SIZE, TEST_SIZE));
    fail_commit_call = 0;
    CHECK(SAAllocate(pool, 4096, &a) && SAFree(pool, 4096, a));
    CHECK(capacity(pool, TEST_SIZE, TEST_SIZE));
    free(pool);
    puts("SUBALLOC_FIRST_COMMIT_FAILURE_REUSE_OK");
    return 0;
}

/* Characterize original-source limitations, not product acceptance passes. */
static int failure_audit(void)
{
    PVOID pool;
    ULONG address;

    pool = fresh_pool();
    CHECK(pool != NULL);
    fail_commit_call = 2;
    CHECK(!SAAllocate(pool, 8192, &address));
    CHECK(commit_calls == 2 && decommit_calls == 0);
    CHECK(capacity(pool, TEST_SIZE, TEST_SIZE));
    puts("LIMITATION_PARTIAL_COMMIT_NO_ROLLBACK: first page committed; bitmap free");
    free(pool);

    pool = fresh_pool();
    CHECK(pool != NULL);
    CHECK(SAAllocate(pool, TEST_SIZE, &address));
    CHECK(capacity(pool, 0, 0));
    fail_decommit_call = 1;
    CHECK(!SAFree(pool, 4096, address + 65536));
    CHECK(capacity(pool, 4096, 4096));
    puts("LIMITATION_DECOMMIT_FAILURE_BITMAP_NOT_RESTORED: failed free exposes 4096 bytes");
    free(pool);
    return 0;
}

int
main(int argc, char **argv)
{
    PVOID allocation;
    ULONG initial_free;
    ULONG initial_largest;
    ULONG final_free;
    ULONG final_largest;
    ULONG first;
    ULONG second;
    ULONG grown;

    if (argc == 2 && strcmp(argv[1], "--failure-audit") == 0)
        return failure_audit();
    if (argc != 1) return 64;

    allocation = SAInitialize(0x120000u, 0x20000u, fixture_commit,
        fixture_decommit, fixture_move);
    if (allocation == NULL ||
        !SAQueryFree(allocation, &initial_free, &initial_largest) ||
        initial_free == 0u || initial_largest == 0u) return 1;

    if (!SAAllocate(allocation, 1024u, &first) ||
        !SAAllocate(allocation, 1024u, &second) ||
        first == second || commit_calls == 0u) return 2;

    if (!SAReallocate(allocation, 1024u, first, 6144u, &grown) ||
        grown == 0u || move_calls == 0u) return 3;

    if (!SAFree(allocation, 1024u, second) ||
        !SAFree(allocation, 6144u, grown) || decommit_calls == 0u ||
        !SAQueryFree(allocation, &final_free, &final_largest) ||
        final_free != initial_free || final_largest != initial_largest) return 4;

    free(allocation);
    return branch_tests();
}
