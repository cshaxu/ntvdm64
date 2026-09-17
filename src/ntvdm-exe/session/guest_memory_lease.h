#ifndef GUEST_MEMORY_LEASE_H
#define GUEST_MEMORY_LEASE_H

#include <stdint.h>

#define GUEST_MEMORY_LEASE_MAXIMUM 8u

enum guest_memory_access {
    GUEST_MEMORY_ACCESS_READ = 1u,
    GUEST_MEMORY_ACCESS_WRITE = 2u
};

typedef int (*guest_memory_read_fn)(void *context, uint32_t address,
    uint8_t *bytes, uint32_t byte_count);
typedef int (*guest_memory_write_fn)(void *context, uint32_t address,
    const uint8_t *bytes, uint32_t byte_count);

typedef struct guest_memory_lease {
    uint32_t epoch;
    uint32_t address;
    uint32_t byte_count;
    uint32_t access;
    uint32_t active;
    uint8_t *bounce;
} guest_memory_lease;

typedef struct guest_memory_lease_context {
    uint32_t epoch;
    uint32_t active;
    void *context;
    guest_memory_read_fn read;
    guest_memory_write_fn write;
    guest_memory_lease leases[GUEST_MEMORY_LEASE_MAXIMUM];
} guest_memory_lease_context;

int guest_memory_lease_context_valid(const guest_memory_lease_context *context);
int guest_memory_lease_begin(guest_memory_lease_context *lease_context,
    void *context, guest_memory_read_fn read, guest_memory_write_fn write);
void guest_memory_lease_end(guest_memory_lease_context *context);
int guest_memory_lease_acquire(guest_memory_lease_context *context,
    uint32_t address, uint32_t byte_count, uint32_t access,
    guest_memory_lease **lease_out, uint8_t **bytes_out);
int guest_memory_lease_release(guest_memory_lease_context *context,
    guest_memory_lease *lease, int commit);

#endif
