#include "session/session.h"
#include "mvdm_xms_memory.h"
#include "memapi.h"
#include "suballoc.h"
#include "umb.h"
#include "xms.h"

#include <stdlib.h>
#include <string.h>

#define FIXTURE_MEMORY_BYTES (3u * 1024u * 1024u)
#define XMS_BASE_ADDRESS (1024u * 1024u + 64u * 1024u)

typedef struct fixture_memory {
    unsigned char bytes[FIXTURE_MEMORY_BYTES];
} fixture_memory;

extern PVOID ExtMemSA;
extern ULONG xmsMemorySize;
extern BOOL XMSInit(int argc, char *argv[]);

static fixture_memory active_memory;
static NTSTATUS allocation_status;
static ULONG allocation_address;
static ULONG allocation_calls;
static ULONG commit_calls;
static ULONG decommit_calls;

static int fixture_read(void *context, uint32_t address, uint8_t *bytes,
    uint32_t byte_count)
{
    fixture_memory *memory = (fixture_memory *)context;

    if (address > FIXTURE_MEMORY_BYTES ||
        byte_count > FIXTURE_MEMORY_BYTES - address) return 0;
    memcpy(bytes, memory->bytes + address, byte_count);
    return 1;
}

static int fixture_write(void *context, uint32_t address, const uint8_t *bytes,
    uint32_t byte_count)
{
    fixture_memory *memory = (fixture_memory *)context;

    if (address > FIXTURE_MEMORY_BYTES ||
        byte_count > FIXTURE_MEMORY_BYTES - address) return 0;
    memcpy(memory->bytes + address, bytes, byte_count);
    return 1;
}

/* The original XMS startup only asks this path to commit any currently-free
 * UMB.  This focused source test has no UMB inventory, so preserve the
 * original successful-empty result without introducing a UMB provider. */
BOOL ReserveUMB(WORD owner, PVOID *address, DWORD *size)
{
    (void)owner;
    *address = NULL;
    *size = 0u;
    return TRUE;
}

NTSTATUS VdmAllocateVirtualMemory(PULONG address, ULONG size, BOOLEAN commit)
{
    (void)size;
    (void)commit;
    allocation_calls += 1u;
    if (allocation_status == 0) *address = allocation_address;
    return allocation_status;
}

NTSTATUS VdmCommitVirtualMemory(ULONG address, ULONG size)
{
    commit_calls += 1u;
    return xmsCommitBlock(address, size);
}

NTSTATUS VdmDeCommitVirtualMemory(ULONG address, ULONG size)
{
    decommit_calls += 1u;
    return xmsDecommitBlock(address, size);
}

static int fixture_run_branch(fixture_memory *memory, NTSTATUS selected_status,
    int expect_vdm_callbacks)
{
    ULONG address;
    ULONG before_commit;
    ULONG before_decommit;

    memset(memory->bytes, 0xff, sizeof(memory->bytes));
    allocation_status = selected_status;
    allocation_address = XMS_BASE_ADDRESS;
    allocation_calls = 0u;
    commit_calls = 0u;
    decommit_calls = 0u;
    ExtMemSA = NULL;
    xmsMemorySize = 2048u;

    if (!XMSInit(0, NULL) || ExtMemSA == NULL || allocation_calls != 1u)
        return 0;
    before_commit = commit_calls;
    before_decommit = decommit_calls;
    if (!SAAllocate(ExtMemSA, 1024u, &address) || address != XMS_BASE_ADDRESS ||
        memory->bytes[address] != 0u) return 0;
    if (expect_vdm_callbacks) {
        if (commit_calls != before_commit + 1u) return 0;
    } else if (commit_calls != before_commit) {
        return 0;
    }
    memory->bytes[address] = 0x5au;
    if (!SAFree(ExtMemSA, 1024u, address)) return 0;
    if (expect_vdm_callbacks) {
        if (decommit_calls != before_decommit + 1u) return 0;
    } else if (decommit_calls != before_decommit) {
        return 0;
    }
    if (!SAAllocate(ExtMemSA, 1024u, &address) || memory->bytes[address] != 0u)
        return 0;
    free(ExtMemSA);
    ExtMemSA = NULL;
    return 1;
}

int main(void)
{
    session instance;

    session_initialize(&instance, 371u);
    if (!session_activate(&instance) || !session_thread_bind(&instance) ||
        !session_guest_memory_begin(&instance, &active_memory, fixture_read,
            fixture_write)) return 1;

    if (!fixture_run_branch(&active_memory, 0, 1)) return 2;
    if (!fixture_run_branch(&active_memory,
        (NTSTATUS)0xc0000002L, 0)) return 3;

    session_guest_memory_end(&instance);
    if (!session_thread_unbind(&instance) || !session_dispose(&instance))
        return 4;
    return 0;
}
