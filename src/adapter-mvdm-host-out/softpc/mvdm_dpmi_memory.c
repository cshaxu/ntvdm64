#include <nt.h>
#include <memapi.h>

#include <stdlib.h>
#include <string.h>

#include "adapter-bochs/machine_facade.h"
#include "session/session.h"

/* Same-shaped replacement for the reached Vdm* virtual-memory calls used by
 * original dpmi32/dpmimemr.c.  Intel addresses remain 32-bit linear values;
 * this records allocation visibility only and never turns them into host
 * pointers.  The sole mapping-manager implementation remains session's. */
typedef struct mvdm_dpmi_memory_block {
    session *owner;
    uint32_t base;
    uint32_t size;
    struct mvdm_dpmi_memory_block *next;
} mvdm_dpmi_memory_block;

static mvdm_dpmi_memory_block *mvdm_dpmi_memory_blocks;

static uint32_t align_64k(uint32_t value)
{
    return value > UINT32_MAX - 0xffffu ? 0u :
        (value + 0xffffu) & ~UINT32_C(0xffff);
}

static int range_valid(uint32_t base, uint32_t size)
{
    return size != 0u && size <= UINT32_MAX - base &&
        machine_facade_memory_readable(base, size) &&
        machine_facade_memory_writable(base, size);
}

static mvdm_dpmi_memory_block *find_block(session *owner, uint32_t base)
{
    mvdm_dpmi_memory_block *block;
    for (block = mvdm_dpmi_memory_blocks; block != NULL; block = block->next) {
        if (block->owner == owner && block->base == base) return block;
    }
    return NULL;
}

static int overlaps(session *owner, uint32_t base, uint32_t size)
{
    mvdm_dpmi_memory_block *block;
    uint64_t end = (uint64_t)base + size;
    for (block = mvdm_dpmi_memory_blocks; block != NULL; block = block->next) {
        uint64_t block_end;
        if (block->owner != owner) continue;
        block_end = (uint64_t)block->base + block->size;
        if ((uint64_t)base < block_end && (uint64_t)block->base < end) return 1;
    }
    return 0;
}

static void mvdm_dpmi_memory_cleanup(void *context)
{
    session *owner = (session *)context;
    mvdm_dpmi_memory_block **cursor = &mvdm_dpmi_memory_blocks;
    while (*cursor != NULL) {
        mvdm_dpmi_memory_block *block = *cursor;
        if (block->owner == owner) {
            *cursor = block->next;
            free(block);
        } else {
            cursor = &block->next;
        }
    }
}

static int ensure_cleanup(session *owner)
{
    mvdm_dpmi_memory_block *block;
    for (block = mvdm_dpmi_memory_blocks; block != NULL; block = block->next) {
        if (block->owner == owner) return 1;
    }
    return session_register_teardown(owner, mvdm_dpmi_memory_cleanup, owner);
}

static int allocate_block(session *owner, uint32_t size, uint32_t *base_out)
{
    uint32_t base;
    mvdm_dpmi_memory_block *block;

    if (base_out != NULL) *base_out = 0u;
    if (owner == NULL || base_out == NULL || size == 0u ||
        (size = align_64k(size)) == 0u || !ensure_cleanup(owner)) return 0;
    for (base = UINT32_C(0x00100000); base <= UINT32_MAX - size;
        base += UINT32_C(0x00010000)) {
        if (!range_valid(base, size)) break;
        if (!overlaps(owner, base, size)) {
            block = (mvdm_dpmi_memory_block *)calloc(1u, sizeof(*block));
            if (block == NULL) return 0;
            block->owner = owner;
            block->base = base;
            block->size = size;
            block->next = mvdm_dpmi_memory_blocks;
            mvdm_dpmi_memory_blocks = block;
            *base_out = base;
            return 1;
        }
    }
    return 0;
}

NTSTATUS VdmAllocateVirtualMemory(PULONG address, ULONG size, BOOLEAN commit)
{
    session *owner = session_thread_current();
    uint32_t base;
    (void)commit;
    if (address == NULL || !allocate_block(owner, size, &base))
        return STATUS_NO_MEMORY;
    *address = base;
    return STATUS_SUCCESS;
}

NTSTATUS VdmFreeVirtualMemory(ULONG address)
{
    session *owner = session_thread_current();
    mvdm_dpmi_memory_block **cursor = &mvdm_dpmi_memory_blocks;
    while (*cursor != NULL) {
        mvdm_dpmi_memory_block *block = *cursor;
        if (block->owner == owner && block->base == address) {
            *cursor = block->next;
            free(block);
            return STATUS_SUCCESS;
        }
        cursor = &block->next;
    }
    return STATUS_INVALID_PARAMETER;
}

NTSTATUS VdmCommitVirtualMemory(ULONG address, ULONG size)
{
    return range_valid(address, size) ? STATUS_SUCCESS : STATUS_INVALID_PARAMETER;
}

NTSTATUS VdmDeCommitVirtualMemory(ULONG address, ULONG size)
{
    return range_valid(address, size) ? STATUS_SUCCESS : STATUS_INVALID_PARAMETER;
}

NTSTATUS VdmQueryFreeVirtualMemory(PULONG free_bytes, PULONG largest_block)
{
    uint32_t base;
    uint32_t run = 0u;
    uint32_t largest = 0u;
    uint64_t total = 0u;
    session *owner = session_thread_current();
    if (owner == NULL || free_bytes == NULL || largest_block == NULL)
        return STATUS_INVALID_PARAMETER;
    for (base = UINT32_C(0x00100000); range_valid(base, UINT32_C(0x00010000));
        base += UINT32_C(0x00010000)) {
        if (!overlaps(owner, base, UINT32_C(0x00010000))) {
            run += UINT32_C(0x00010000);
            total += UINT32_C(0x00010000);
            if (run > largest) largest = run;
        } else {
            run = 0u;
        }
    }
    *free_bytes = total > UINT32_MAX ? UINT32_MAX : (ULONG)total;
    *largest_block = largest;
    return STATUS_SUCCESS;
}

NTSTATUS VdmReallocateVirtualMemory(ULONG old_address, PULONG new_address,
    ULONG new_size)
{
    session *owner = session_thread_current();
    mvdm_dpmi_memory_block *old_block = find_block(owner, old_address);
    uint32_t base;
    uint32_t copy_bytes;
    guest_memory_lease *source = NULL;
    guest_memory_lease *destination = NULL;
    uint8_t *source_bytes = NULL;
    uint8_t *destination_bytes = NULL;
    if (new_address == NULL || old_block == NULL ||
        !allocate_block(owner, new_size, &base)) return STATUS_NO_MEMORY;
    copy_bytes = old_block->size < new_size ? old_block->size : new_size;
    if (copy_bytes != 0u && (!session_guest_memory_acquire(owner, old_address,
        copy_bytes, GUEST_MEMORY_ACCESS_READ, &source, &source_bytes) ||
        !session_guest_memory_acquire(owner, base, copy_bytes,
            GUEST_MEMORY_ACCESS_WRITE, &destination, &destination_bytes))) {
        if (source != NULL) (void)session_guest_memory_release(owner, source, 0);
        (void)VdmFreeVirtualMemory(base);
        return STATUS_NO_MEMORY;
    }
    if (copy_bytes != 0u) {
        memcpy(destination_bytes, source_bytes, copy_bytes);
        if (!session_guest_memory_release(owner, destination, 1) ||
            !session_guest_memory_release(owner, source, 0)) {
            (void)VdmFreeVirtualMemory(base);
            return STATUS_NO_MEMORY;
        }
    }
    (void)VdmFreeVirtualMemory(old_address);
    *new_address = base;
    return STATUS_SUCCESS;
}

NTSTATUS VdmAddVirtualMemory(ULONG host_address, ULONG size, PULONG intel_address)
{
    (void)host_address;
    (void)size;
    if (intel_address != NULL) *intel_address = 0u;
    return STATUS_NOT_IMPLEMENTED;
}

NTSTATUS VdmRemoveVirtualMemory(ULONG intel_address)
{
    (void)intel_address;
    return STATUS_NOT_IMPLEMENTED;
}
