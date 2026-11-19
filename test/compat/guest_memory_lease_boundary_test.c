#include "compat/include/guest_memory_lease.h"
#include <stdio.h>
#include <string.h>

#define CHECK(condition) do { ++checks; if (!(condition)) { \
    fprintf(stderr, "LEASE_RESOURCE_FAIL line=%d checks=%u\n", __LINE__, checks); \
    return 1; } } while (0)

int main(void)
{
    guest_memory_lease_context a = {0}, b = {0};
    guest_memory_lease *lease, *slots[GUEST_MEMORY_LEASE_MAXIMUM];
    guest_memory_lease foreign = {0};
    uint8_t *bytes;
    unsigned checks = 0, i;

    CHECK(!guest_memory_lease_begin(NULL));
    CHECK(guest_memory_lease_begin(&a));
    CHECK(!guest_memory_lease_begin(&a));
    CHECK(guest_memory_lease_begin(&b));
    CHECK(a.epoch == b.epoch);
    CHECK(!guest_memory_lease_prepare(&a, UINT32_MAX, 1, GUEST_MEMORY_ACCESS_READ, &lease, &bytes));
    CHECK(!lease && !bytes);
    CHECK(!guest_memory_lease_prepare(&a, 0, 1, 0, &lease, &bytes));
    CHECK(guest_memory_lease_prepare(&a, 8, 2, GUEST_MEMORY_ACCESS_WRITE, &lease, &bytes));
    /* Preparing storage must not publish a live lease before C hydration. */
    CHECK(!lease->active && lease->bounce == bytes);
    CHECK(!guest_memory_lease_validate(&a, lease));
    guest_memory_lease_discard(lease);
    CHECK(!lease->active && !lease->bounce);
    CHECK(guest_memory_lease_prepare(&a, 8, 2, GUEST_MEMORY_ACCESS_WRITE, &lease, &bytes));
    bytes[0] = 0x42;
    guest_memory_lease_publish(&a, lease, 8, 2, GUEST_MEMORY_ACCESS_WRITE);
    CHECK(guest_memory_lease_validate(&a, lease));
    CHECK(!guest_memory_lease_validate(&b, lease));
    CHECK(!guest_memory_lease_validate(&a, &foreign));
    CHECK(!guest_memory_lease_validate(&a, (guest_memory_lease *)1));
    CHECK(lease->bounce == bytes && bytes[0] == 0x42);
    guest_memory_lease_discard(lease);
    CHECK(!guest_memory_lease_validate(&a, lease));
    for (i = 0; i < GUEST_MEMORY_LEASE_MAXIMUM; ++i) {
        CHECK(guest_memory_lease_prepare(&a, i, 1, GUEST_MEMORY_ACCESS_WRITE, &slots[i], &bytes));
        guest_memory_lease_publish(&a, slots[i], i, 1, GUEST_MEMORY_ACCESS_WRITE);
    }
    CHECK(!guest_memory_lease_prepare(&a, 0, 1, GUEST_MEMORY_ACCESS_READ, &lease, &bytes));
    CHECK(!lease && !bytes);
    guest_memory_lease_discard(slots[3]);
    CHECK(guest_memory_lease_prepare(&a, UINT32_MAX, 0, GUEST_MEMORY_ACCESS_READ, &lease, &bytes));
    CHECK(lease == slots[3] && bytes);
    guest_memory_lease_publish(&a, lease, UINT32_MAX, 0, GUEST_MEMORY_ACCESS_READ);
    CHECK(!guest_memory_lease_copy_enter(&a, a.epoch + 1));
    CHECK(guest_memory_lease_copy_enter(&a, a.epoch));
    guest_memory_lease_copy_leave(&a);
    guest_memory_lease_end(&a);
    CHECK(!guest_memory_lease_validate(&a, lease));
    CHECK(!guest_memory_lease_copy_enter(&a, a.epoch));
    for (i = 0; i < GUEST_MEMORY_LEASE_MAXIMUM; ++i)
        CHECK(!a.leases[i].active && !a.leases[i].bounce);
    guest_memory_lease_end(&a);
    a.epoch = UINT32_MAX;
    CHECK(guest_memory_lease_begin(&a) && a.epoch == 1);
    guest_memory_lease_end(&a);
    guest_memory_lease_end(&b);
    printf("LEASE_RESOURCE_PASS checks=%u\n", checks);
    return 0;
}
