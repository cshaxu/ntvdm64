#include <windows.h>

#include <stdlib.h>

#include "suballoc.h"

static ULONG commit_calls;
static ULONG decommit_calls;
static ULONG move_calls;

static NTSTATUS
fixture_commit(
    ULONG address,
    ULONG size
    )
{
    if (size != COMMIT_GRANULARITY || address < 0x120000u) return (NTSTATUS)1;
    commit_calls += 1u;
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
    return 0;
}

static VOID
fixture_move(
    ULONG destination,
    ULONG source,
    ULONG size
    )
{
    if (destination != source && size != 0u) move_calls += 1u;
}

int
main(void)
{
    PVOID allocation;
    ULONG initial_free;
    ULONG initial_largest;
    ULONG final_free;
    ULONG final_largest;
    ULONG first;
    ULONG second;
    ULONG grown;

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
    return 0;
}
