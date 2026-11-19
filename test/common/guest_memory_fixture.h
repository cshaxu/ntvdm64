#ifndef TEST_GUEST_MEMORY_FIXTURE_H
#define TEST_GUEST_MEMORY_FIXTURE_H

#include "../../src/ntvdm-exe/softpc/include/mvdm_softpc_guest_memory.h"

/* Caller unit tests substitute only the address-space provider. Production B
 * has no provider registry. Real translation is checked by the CCPU fixture. */
typedef int (*fixture_read_memory_fn)(void *, uint32_t, uint8_t *, uint32_t);
typedef int (*fixture_write_memory_fn)(void *, uint32_t, const uint8_t *, uint32_t);
static struct fixture_memory_binding {
    guest_memory_lease_context *owner;
    void *memory;
    fixture_read_memory_fn read;
    fixture_write_memory_fn write;
} fixture_memory_bindings[2];

static __inline int fixture_memory_bind(guest_memory_lease_context *owner, void *memory,
    fixture_read_memory_fn read, fixture_write_memory_fn write)
{
    unsigned i;
    for (i = 0; i < 2; ++i) {
        if (!fixture_memory_bindings[i].owner || fixture_memory_bindings[i].owner == owner) {
            fixture_memory_bindings[i].owner = owner;
            fixture_memory_bindings[i].memory = memory;
            fixture_memory_bindings[i].read = read;
            fixture_memory_bindings[i].write = write;
            return 1;
        }
    }
    return 0;
}
static __inline int fixture_session_memory_begin(session *owner, void *memory,
    fixture_read_memory_fn read, fixture_write_memory_fn write)
{
    return read && write && session_guest_memory_begin(owner) &&
        fixture_memory_bind(&owner->guest_memory_lease, memory, read, write);
}
static __inline int fixture_lease_memory_begin(guest_memory_lease_context *owner, void *memory,
    fixture_read_memory_fn read, fixture_write_memory_fn write)
{
    return read && write && guest_memory_lease_begin(owner) &&
        fixture_memory_bind(owner, memory, read, write);
}
int fixture_guest_read(void *owner, uint32_t address, uint8_t *bytes, uint32_t count)
{
    unsigned i;
    for (i = 0; i < 2; ++i)
        if (fixture_memory_bindings[i].owner == owner)
            return fixture_memory_bindings[i].read(fixture_memory_bindings[i].memory, address, bytes, count);
    return 0;
}
int fixture_guest_write(void *owner, uint32_t address, const uint8_t *bytes, uint32_t count)
{
    unsigned i;
    for (i = 0; i < 2; ++i)
        if (fixture_memory_bindings[i].owner == owner)
            return fixture_memory_bindings[i].write(fixture_memory_bindings[i].memory, address, bytes, count);
    return 0;
}
#endif
