#include "mvdm_wow_pointer_scope.h"

#include <string.h>

#include "mvdm_command_registers.h"
#include "mvdm_protected_span.h"
#include "session/session.h"

#define MVDM_WOW_POINTER_SCOPE_MAXIMUM 8u

typedef struct mvdm_wow_pointer_entry {
    guest_memory_lease *lease;
    void *bytes;
} mvdm_wow_pointer_entry;

typedef struct mvdm_wow_pointer_scope_state {
    session *owner;
    uint32_t active;
    uint32_t count;
    mvdm_wow_pointer_entry entries[MVDM_WOW_POINTER_SCOPE_MAXIMUM];
} mvdm_wow_pointer_scope_state;

static __declspec(thread) mvdm_wow_pointer_scope_state current_scope;

static int mvdm_wow_pointer_linear(uint32_t vp, uint32_t byte_count,
    uint32_t access, uint32_t *linear_out)
{
    uint16_t selector = (uint16_t)(vp >> 16);
    uint16_t offset = (uint16_t)vp;
    uint32_t linear;

    if (linear_out == NULL || byte_count == 0u ||
        (access & ~(MVDM_WOW_POINTER_ACCESS_READ |
            MVDM_WOW_POINTER_ACCESS_WRITE)) != 0u || access == 0u) return 0;
    if (getPE() != 0u) {
        return mvdm_protected_span_resolve(
            (access & MVDM_WOW_POINTER_ACCESS_WRITE) != 0u ?
                MVDM_PROTECTED_SPAN_WRITE : MVDM_PROTECTED_SPAN_READ,
            selector, offset, byte_count, linear_out);
    }
    linear = ((uint32_t)selector << 4) + offset;
    if (linear > UINT32_MAX - byte_count) return 0;
    *linear_out = linear;
    return 1;
}

int mvdm_wow_pointer_scope_begin(void)
{
    session *owner = session_thread_current();
    if (owner == NULL || current_scope.active != 0u) return 0;
    memset(&current_scope, 0, sizeof(current_scope));
    current_scope.owner = owner;
    current_scope.active = 1u;
    return 1;
}

void *mvdm_wow_pointer_scope_acquire(uint32_t vp, uint32_t byte_count,
    uint32_t access)
{
    guest_memory_lease *lease;
    uint8_t *bytes;
    uint32_t linear;
    uint32_t index;

    if (current_scope.active != 1u ||
        current_scope.owner != session_thread_current() ||
        !mvdm_wow_pointer_linear(vp, byte_count, access, &linear) ||
        !session_guest_memory_acquire(current_scope.owner, linear, byte_count,
            access, &lease, &bytes)) return NULL;
    for (index = 0u; index != MVDM_WOW_POINTER_SCOPE_MAXIMUM; ++index) {
        if (current_scope.entries[index].lease == NULL) break;
    }
    if (index == MVDM_WOW_POINTER_SCOPE_MAXIMUM) {
        (void)session_guest_memory_release(current_scope.owner, lease, 0);
        return NULL;
    }
    current_scope.entries[index].lease = lease;
    current_scope.entries[index].bytes = bytes;
    ++current_scope.count;
    return bytes;
}

int mvdm_wow_pointer_scope_release(void *bytes, int flush)
{
    uint32_t index;
    int result;

    if (current_scope.active != 1u || bytes == NULL ||
        current_scope.owner != session_thread_current() ||
        (flush != 0 && flush != 1)) return 0;
    for (index = 0u; index != MVDM_WOW_POINTER_SCOPE_MAXIMUM; ++index) {
        mvdm_wow_pointer_entry *entry = &current_scope.entries[index];
        if (entry->bytes != bytes) continue;
        result = session_guest_memory_release(current_scope.owner, entry->lease,
            flush);
        entry->lease = NULL;
        entry->bytes = NULL;
        --current_scope.count;
        return result;
    }
    return 0;
}

int mvdm_wow_pointer_scope_end(void)
{
    uint32_t index;
    int result = 1;

    if (current_scope.active != 1u ||
        current_scope.owner != session_thread_current()) return 0;
    for (index = 0u; index != MVDM_WOW_POINTER_SCOPE_MAXIMUM; ++index) {
        mvdm_wow_pointer_entry *entry = &current_scope.entries[index];
        if (entry->lease != NULL) {
            (void)session_guest_memory_release(current_scope.owner, entry->lease,
                0);
            result = 0;
        }
    }
    memset(&current_scope, 0, sizeof(current_scope));
    return result;
}
