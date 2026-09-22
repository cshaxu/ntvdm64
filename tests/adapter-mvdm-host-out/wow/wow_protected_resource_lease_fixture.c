#include <stdio.h>
#include <string.h>
#include "ntvdm-exe/wow/include/wow_callback_frame_lease.h"

/* Actual WOW binding and bounce-lease implementation; only descriptor lookup
 * and backing memory are stand-ins. This is not guest/provider acceptance. */
static guest_memory_lease_context memory;
static unsigned char backing[0x24000];
static unsigned errors, protected_calls, current_calls, writes;
#define CHECK(x) do { if (!(x)) { ++errors; printf("FAIL line=%u\n", __LINE__); } } while (0)

static int read_bytes(void *context, uint32_t address, uint8_t *bytes,
    uint32_t count)
{
    (void)context;
    if (address > sizeof(backing) || count > sizeof(backing) - address) return 0;
    memcpy(bytes, backing + address, count);
    return 1;
}

static int write_bytes(void *context, uint32_t address, const uint8_t *bytes,
    uint32_t count)
{
    (void)context; (void)address; (void)bytes; (void)count;
    ++writes;
    return 0;
}

int __cdecl mvdm_softpc_effective_address(uint16_t selector, uint32_t offset,
    uint32_t *out)
{
    ++current_calls;
    *out = ((uint32_t)selector << 4) + offset;
    return 1;
}

int __cdecl mvdm_softpc_protected_address(uint16_t selector, uint32_t offset,
    uint32_t *out)
{
    ++protected_calls;
    if (selector == 0x1234) *out = 0x1000 + offset;
    else if (selector == 0x5678) *out = UINT32_MAX - 15;
    else return 0;
    return 1;
}

int __cdecl mvdm_softpc_guest_memory_acquire(uint32_t address, uint32_t count,
    uint32_t access, guest_memory_lease **lease, uint8_t **bytes)
{
    return guest_memory_lease_acquire(&memory, address, count, access, lease, bytes);
}

int __cdecl mvdm_softpc_guest_memory_release(guest_memory_lease *lease, int commit)
{
    return guest_memory_lease_release(&memory, lease, commit);
}

static void rejected(uint32_t vp, uint32_t count, uint32_t access)
{
    wow_callback_frame_lease view;
    unsigned i;
    memset(&view, 0xCC, sizeof(view));
    CHECK(!wow_callback_frame_acquire_protected_vp(vp, count, access, &view));
    CHECK(!view.lease && !view.bytes && !view.byte_count);
    for (i = 0; i < GUEST_MEMORY_LEASE_MAXIMUM; ++i)
        CHECK(!memory.leases[i].active && !memory.leases[i].bounce);
}

int __cdecl main(void)
{
    wow_callback_frame_lease view;
    unsigned i, before, after;
    CHECK(guest_memory_lease_begin(&memory, NULL, read_bytes, write_bytes));
    for (i = 0; i < sizeof(backing); ++i) backing[i] = (unsigned char)(i ^ (i >> 8));
    __asm mov before, esp
    CHECK(wow_callback_frame_acquire_protected_vp(0x12340020, 0x18000,
        GUEST_MEMORY_ACCESS_READ, &view));
    CHECK(view.guest_address == 0x1020 && view.byte_count == 0x18000);
    CHECK(view.bytes && memcmp(view.bytes, backing + 0x1020, 0x18000) == 0);
    CHECK(wow_callback_frame_release(&view, 0));
    CHECK(!view.lease && !view.bytes);
    rejected(0x43210020, 8, GUEST_MEMORY_ACCESS_READ);
    rejected(0x56780000, 32, GUEST_MEMORY_ACCESS_READ);
    rejected(0x12340000, sizeof(backing), GUEST_MEMORY_ACCESS_READ);
    rejected(0x12340000, 0, GUEST_MEMORY_ACCESS_READ);
    rejected(0x12340000, 8, 0);
    CHECK(wow_callback_frame_acquire_protected_vp(0x12340000,
        sizeof(backing) - 0x1000, GUEST_MEMORY_ACCESS_READ, &view));
    CHECK(wow_callback_frame_release(&view, 0));
    __asm mov after, esp
    __asm mov esp, before
    CHECK(before == after && current_calls == 0 && protected_calls == 7 && writes == 0);
    guest_memory_lease_end(&memory);
    printf("errors=%u protected=%u current=%u stack_delta=%d\n",
        errors, protected_calls, current_calls, (int)(after - before));
    if (errors) return 3;
    puts("WOW_PROTECTED_RESOURCE_LEASE_OK huge=98304 failures=5 retry=1");
    return 0;
}
