#include "mvdm_redirector_pointer_scope.h"

#include <string.h>

#include "session/session.h"

#define MVDM_REDIRECTOR_POINTER_SCOPE_MAXIMUM 8u

typedef struct mvdm_redirector_pointer_scope_entry {
    guest_memory_lease *lease;
} mvdm_redirector_pointer_scope_entry;

typedef struct mvdm_redirector_pointer_scope_state {
    session *owner;
    uint32_t active;
    uint32_t count;
    mvdm_redirector_pointer_scope_entry entries[MVDM_REDIRECTOR_POINTER_SCOPE_MAXIMUM];
} mvdm_redirector_pointer_scope_state;

static __declspec(thread) mvdm_redirector_pointer_scope_state mvdm_redirector_pointer_scope;

int mvdm_redirector_pointer_scope_begin(void)
{
    session *owner = session_thread_current();
    if (owner == NULL || mvdm_redirector_pointer_scope.active != 0u) return 0;
    memset(&mvdm_redirector_pointer_scope, 0, sizeof(mvdm_redirector_pointer_scope));
    mvdm_redirector_pointer_scope.owner = owner;
    mvdm_redirector_pointer_scope.active = 1u;
    return 1;
}

int mvdm_redirector_pointer_scope_end(int commit)
{
    uint32_t index;
    int result = 1;
    if (mvdm_redirector_pointer_scope.active != 1u ||
        mvdm_redirector_pointer_scope.owner != session_thread_current()) return 0;
    for (index = mvdm_redirector_pointer_scope.count; index != 0u; --index) {
        guest_memory_lease *lease = mvdm_redirector_pointer_scope.entries[index - 1u].lease;
        if (lease != NULL && !session_guest_memory_release(mvdm_redirector_pointer_scope.owner,
            lease, commit)) result = 0;
    }
    memset(&mvdm_redirector_pointer_scope, 0, sizeof(mvdm_redirector_pointer_scope));
    return result;
}

void *mvdm_redirector_pointer_from_real_mode(uint16_t segment, uint16_t offset)
{
    guest_memory_lease *lease;
    uint8_t *bytes;
    uint32_t address;
    uint32_t byte_count;
    if (mvdm_redirector_pointer_scope.active != 1u ||
        mvdm_redirector_pointer_scope.owner != session_thread_current() ||
        mvdm_redirector_pointer_scope.count == MVDM_REDIRECTOR_POINTER_SCOPE_MAXIMUM) return NULL;
    address = ((uint32_t)segment << 4) + offset;
    byte_count = UINT16_MAX - (uint32_t)offset + 1u;
    if (!session_guest_memory_acquire(mvdm_redirector_pointer_scope.owner, address,
        byte_count, GUEST_MEMORY_ACCESS_WRITE, &lease, &bytes)) return NULL;
    mvdm_redirector_pointer_scope.entries[mvdm_redirector_pointer_scope.count++].lease = lease;
    return bytes;
}

void *mvdm_redirector_pointer_from_source_address(uint32_t address, uint32_t protected_mode)
{
    if (protected_mode != 0u) return NULL;
    return mvdm_redirector_pointer_from_real_mode((uint16_t)(address >> 16),
        (uint16_t)address);
}
