#include "compat/include/guest_memory_lease.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
#include <string.h>

static int access_valid(uint32_t access)
{
    return access != 0u && (access & ~(GUEST_MEMORY_ACCESS_READ |
        GUEST_MEMORY_ACCESS_WRITE)) == 0u;
}

int guest_memory_lease_context_valid(const guest_memory_lease_context *context)
{
    return context != NULL && context->active <= 1u;
}

static int lease_begin(guest_memory_lease_context *lease_context)
{
    if (!guest_memory_lease_context_valid(lease_context) ||
        lease_context->active != 0u) return 0;
    if (++lease_context->epoch == 0u) ++lease_context->epoch;
    lease_context->active = 1u;
    return 1;
}

static void lease_end(guest_memory_lease_context *context)
{
    uint32_t index;
    if (!guest_memory_lease_context_valid(context)) return;
    for (index = 0u; index < GUEST_MEMORY_LEASE_MAXIMUM; ++index) {
        free(context->leases[index].bounce);
        memset(&context->leases[index], 0, sizeof(context->leases[index]));
    }
    context->active = 0u;
}

int guest_memory_lease_prepare(guest_memory_lease_context *context,
    uint32_t address, uint32_t byte_count, uint32_t access,
    guest_memory_lease **lease_out, uint8_t **bytes_out)
{
    guest_memory_lease *lease = NULL;
    uint32_t index;
    if (lease_out != NULL) *lease_out = NULL;
    if (bytes_out != NULL) *bytes_out = NULL;
    if (!guest_memory_lease_context_valid(context) || context->active == 0u ||
        lease_out == NULL || bytes_out == NULL || !access_valid(access) ||
        byte_count > UINT32_MAX - address) return 0;
    for (index = 0u; index < GUEST_MEMORY_LEASE_MAXIMUM; ++index) {
        if (context->leases[index].active == 0u) {
            lease = &context->leases[index];
            break;
        }
    }
    if (lease == NULL) return 0;
    lease->bounce = (uint8_t *)malloc(byte_count == 0u ? 1u : byte_count);
    if (lease->bounce == NULL) return 0;
    *lease_out = lease;
    *bytes_out = lease->bounce;
    return 1;
}

void guest_memory_lease_publish(guest_memory_lease_context *context,
    guest_memory_lease *lease, uint32_t address, uint32_t byte_count, uint32_t access)
{
    lease->epoch = context->epoch;
    lease->address = address;
    lease->byte_count = byte_count;
    lease->access = access;
    lease->active = 1u;
}
int guest_memory_lease_validate(guest_memory_lease_context *context, guest_memory_lease *lease)
{
    uint32_t index;
    if (!guest_memory_lease_context_valid(context) || context->active == 0u ||
        lease == NULL)
        return 0;
    /* Validate slot ownership before dereferencing an untrusted lease pointer. */
    for (index = 0u; index < GUEST_MEMORY_LEASE_MAXIMUM; ++index) {
        if (lease == &context->leases[index]) break;
    }
    if (index == GUEST_MEMORY_LEASE_MAXIMUM || lease->active == 0u ||
        lease->epoch != context->epoch)
        return 0;
    return 1;
}
void guest_memory_lease_discard(guest_memory_lease *lease)
{
    free(lease->bounce);
    memset(lease, 0, sizeof(*lease));
}

/* Copy-only clients (including original FDC helper threads) never contend for
 * the CPU thread's borrowed slots. Retirement waits only for this bounded
 * provider call, not for hardware work or a new device cancellation protocol. */
int guest_memory_lease_begin(guest_memory_lease_context *context)
{
    int result;
    if (context == NULL) return 0;
    AcquireSRWLockExclusive((PSRWLOCK)&context->copy_lock);
    result = lease_begin(context);
    ReleaseSRWLockExclusive((PSRWLOCK)&context->copy_lock);
    return result;
}

void guest_memory_lease_end(guest_memory_lease_context *context)
{
    if (context == NULL) return;
    AcquireSRWLockExclusive((PSRWLOCK)&context->copy_lock);
    lease_end(context);
    ReleaseSRWLockExclusive((PSRWLOCK)&context->copy_lock);
}

int guest_memory_lease_copy_enter(guest_memory_lease_context *context, uint32_t epoch)
{
    if (context == NULL) return 0;
    AcquireSRWLockShared((PSRWLOCK)&context->copy_lock);
    if (guest_memory_lease_context_valid(context) && context->active && context->epoch == epoch) return 1;
    ReleaseSRWLockShared((PSRWLOCK)&context->copy_lock);
    return 0;
}
void guest_memory_lease_copy_leave(guest_memory_lease_context *context)
{
    ReleaseSRWLockShared((PSRWLOCK)&context->copy_lock);
}
