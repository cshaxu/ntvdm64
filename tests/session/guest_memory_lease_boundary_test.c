#include "ntvdm-exe/session/guest_memory_lease.h"
#include <stdio.h>
#include <string.h>

typedef struct memory {
    uint8_t bytes[64];
    unsigned reads, writes;
    int fail_read, fail_write;
} memory;

static int read_bytes(void *opaque, uint32_t address, uint8_t *bytes, uint32_t count)
{
    memory *m = (memory *)opaque;
    ++m->reads;
    if (m->fail_read || address > 64u || count > 64u - address) return 0;
    memcpy(bytes, m->bytes + address, count);
    return 1;
}

static int write_bytes(void *opaque, uint32_t address, const uint8_t *bytes, uint32_t count)
{
    memory *m = (memory *)opaque;
    ++m->writes;
    if (m->fail_write || address > 64u || count > 64u - address) return 0;
    memcpy(m->bytes + address, bytes, count);
    return 1;
}

#define CHECK(condition) do { ++checks; if (!(condition)) { \
    fprintf(stderr, "LEASE_BOUNDARY_FAIL line=%d checks=%u\n", __LINE__, checks); \
    return 1; } } while (0)

int main(void)
{
    guest_memory_lease_context a = {0}, b = {0};
    memory ma = {0}, mb = {0};
    guest_memory_lease *lease, *slots[GUEST_MEMORY_LEASE_MAXIMUM];
    guest_memory_lease forged = {0};
    uint8_t *bytes, *saved;
    unsigned checks = 0, i;

    CHECK(guest_memory_lease_begin(&a, &ma, read_bytes, write_bytes));
    CHECK(guest_memory_lease_begin(&b, &mb, read_bytes, write_bytes));
    CHECK(a.epoch == b.epoch);
    CHECK(guest_memory_lease_acquire(&b, 8u, 2u, GUEST_MEMORY_ACCESS_WRITE, &lease, &bytes));
    bytes[0] = 0x42u;
    saved = bytes;
    /* This must fail before accessing or releasing another session's resource. */
    CHECK(!guest_memory_lease_release(&a, lease, 1));
    CHECK(ma.writes == 0u && mb.writes == 0u && ma.bytes[8] == 0u && mb.bytes[8] == 0u);
    CHECK(lease->active && lease->bounce == saved && bytes[0] == 0x42u);
    CHECK(guest_memory_lease_release(&b, lease, 1));
    CHECK(mb.bytes[8] == 0x42u && mb.writes == 1u);
    forged.active = 1u;
    forged.epoch = a.epoch;
    CHECK(!guest_memory_lease_release(&a, &forged, 0));
    CHECK(forged.active == 1u);
    CHECK(!guest_memory_lease_release(&a, (guest_memory_lease *)(uintptr_t)1u, 0));
    CHECK(!guest_memory_lease_acquire(&a, UINT32_MAX, 2u, GUEST_MEMORY_ACCESS_READ, &lease, &bytes));
    CHECK(lease == NULL && bytes == NULL);
    CHECK(!guest_memory_lease_acquire(&a, 0u, 1u, 0u, &lease, &bytes));
    ma.fail_read = 1;
    CHECK(!guest_memory_lease_acquire(&a, 0u, 1u, GUEST_MEMORY_ACCESS_READ, &lease, &bytes));
    CHECK(a.leases[0].active == 0u && a.leases[0].bounce == NULL);
    ma.fail_read = 0;
    CHECK(guest_memory_lease_acquire(&a, 0u, 1u, GUEST_MEMORY_ACCESS_WRITE, &lease, &bytes));
    bytes[0] = 0x99u;
    ma.fail_write = 1;
    CHECK(!guest_memory_lease_release(&a, lease, 1));
    CHECK(ma.bytes[0] == 0u && lease->active == 0u && lease->bounce == NULL);
    ma.fail_write = 0;
    CHECK(guest_memory_lease_acquire(&a, 0u, 1u, GUEST_MEMORY_ACCESS_READ, &lease, &bytes));
    bytes[0] = 0x99u;
    CHECK(guest_memory_lease_release(&a, lease, 1));
    CHECK(ma.writes == 1u && ma.bytes[0] == 0u);
    for (i = 0; i < GUEST_MEMORY_LEASE_MAXIMUM; ++i) {
        CHECK(guest_memory_lease_acquire(&a, i, 1u, GUEST_MEMORY_ACCESS_WRITE, &slots[i], &bytes));
        bytes[0] = 0x77u;
    }
    CHECK(!guest_memory_lease_acquire(&a, 0u, 1u, GUEST_MEMORY_ACCESS_READ, &lease, &bytes));
    CHECK(lease == NULL && bytes == NULL);
    CHECK(guest_memory_lease_release(&a, slots[3], 0));
    CHECK(guest_memory_lease_acquire(&a, 3u, 1u, GUEST_MEMORY_ACCESS_WRITE, &lease, &bytes));
    guest_memory_lease_end(&a);
    for (i = 0; i < GUEST_MEMORY_LEASE_MAXIMUM; ++i)
        CHECK(a.leases[i].active == 0u && a.leases[i].bounce == NULL && ma.bytes[i] == 0u);
    CHECK(ma.writes == 1u && !guest_memory_lease_release(&a, lease, 1));
    guest_memory_lease_end(&a);
    a.epoch = UINT32_MAX;
    CHECK(guest_memory_lease_begin(&a, &ma, read_bytes, write_bytes) && a.epoch == 1u);
    guest_memory_lease_end(&a);
    guest_memory_lease_end(&b);
    printf("LEASE_BOUNDARY_PASS checks=%u\n", checks);
    return 0;
}
