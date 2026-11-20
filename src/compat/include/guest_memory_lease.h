#ifndef GUEST_MEMORY_LEASE_H
#define GUEST_MEMORY_LEASE_H

#include "guest_memory_types.h"

#define GUEST_MEMORY_LEASE_MAXIMUM 8u

struct guest_memory_lease {
    uint32_t epoch;
    uint32_t address;
    uint32_t byte_count;
    uint32_t access;
    uint32_t active;
    uint8_t *bounce;
};

typedef struct guest_memory_lease_context {
    /* Zero-initialized Win32 SRW lock: copied access versus provider retirement. */
    void *copy_lock;
    uint32_t epoch;
    uint32_t active;
    guest_memory_lease leases[GUEST_MEMORY_LEASE_MAXIMUM];
} guest_memory_lease_context;

/* Slot operations belong to the owning CPU thread. prepare/publish is a
 * synchronous pair; discard rolls back an unpublished allocation as well. */
int guest_memory_lease_context_valid(const guest_memory_lease_context *context);
int guest_memory_lease_begin(guest_memory_lease_context *context);
void guest_memory_lease_end(guest_memory_lease_context *context);
int guest_memory_lease_prepare(guest_memory_lease_context *context,
    uint32_t address, uint32_t byte_count, uint32_t access,
    guest_memory_lease **lease_out, uint8_t **bytes_out);
void guest_memory_lease_publish(guest_memory_lease_context *context,
    guest_memory_lease *lease, uint32_t address, uint32_t byte_count, uint32_t access);
int guest_memory_lease_validate(guest_memory_lease_context *context,
    guest_memory_lease *lease);
void guest_memory_lease_discard(guest_memory_lease *lease);
/* copy_enter holds a shared lifetime lock until copy_leave. */
int guest_memory_lease_copy_enter(guest_memory_lease_context *context, uint32_t epoch);
void guest_memory_lease_copy_leave(guest_memory_lease_context *context);

/* CCPU owns the actual copy through the admitted SAS address space. */
int ccpu_guest_memory_copy(guest_memory_lease_context *context, uint32_t epoch,
    uint32_t address, uint8_t *bytes, uint32_t byte_count, int write);
#endif
