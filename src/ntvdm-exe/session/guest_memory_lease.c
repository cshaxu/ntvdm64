#include "guest_memory_lease.h"

#include <stdlib.h>
#include <string.h>

static int access_valid(uint32_t access)
{
    return access != 0u && (access & ~(GUEST_MEMORY_ACCESS_READ |
        GUEST_MEMORY_ACCESS_WRITE)) == 0u;
}

int guest_memory_lease_context_valid(const guest_memory_lease_context *context)
{
    return context != NULL &&
        ((context->active == 0u && context->context == NULL &&
          context->read == NULL && context->write == NULL) ||
         (context->active == 1u && context->read != NULL &&
          context->write != NULL));
}

int guest_memory_lease_begin(guest_memory_lease_context *lease_context,
    void *context, guest_memory_read_fn read, guest_memory_write_fn write)
{
    if (!guest_memory_lease_context_valid(lease_context) ||
        lease_context->active != 0u || read == NULL || write == NULL) return 0;
    if (++lease_context->epoch == 0u) ++lease_context->epoch;
    lease_context->context = context;
    lease_context->read = read;
    lease_context->write = write;
    lease_context->active = 1u;
    return 1;
}

void guest_memory_lease_end(guest_memory_lease_context *context)
{
    uint32_t index;
    if (!guest_memory_lease_context_valid(context)) return;
    for (index = 0u; index < GUEST_MEMORY_LEASE_MAXIMUM; ++index) {
        free(context->leases[index].bounce);
        memset(&context->leases[index], 0, sizeof(context->leases[index]));
    }
    context->context = NULL;
    context->read = NULL;
    context->write = NULL;
    context->active = 0u;
}

int guest_memory_lease_acquire(guest_memory_lease_context *context,
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
    if (byte_count != 0u && !context->read(context->context, address,
        lease->bounce, byte_count)) {
        free(lease->bounce);
        memset(lease, 0, sizeof(*lease));
        return 0;
    }
    lease->epoch = context->epoch;
    lease->address = address;
    lease->byte_count = byte_count;
    lease->access = access;
    lease->active = 1u;
    *lease_out = lease;
    *bytes_out = lease->bounce;
    return 1;
}

int guest_memory_lease_release(guest_memory_lease_context *context,
    guest_memory_lease *lease, int commit)
{
    int result;
    if (!guest_memory_lease_context_valid(context) || context->active == 0u ||
        lease == NULL || lease->active == 0u || lease->epoch != context->epoch)
        return 0;
    result = commit == 0 || (lease->access & GUEST_MEMORY_ACCESS_WRITE) == 0u ||
        lease->byte_count == 0u || context->write(context->context, lease->address,
            lease->bounce, lease->byte_count);
    free(lease->bounce);
    memset(lease, 0, sizeof(*lease));
    return result;
}
