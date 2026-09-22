#include <stdio.h>
#include "wow32-dll/include/wow_callback_frame_lease.h"

/* Native ABI fixture: real /Gz WOW binding, cdecl worker stand-ins. No guest
 * execution or real session/memory publication is claimed by this test. */
static unsigned address_calls, acquire_calls, release_calls, errors;
static uint8_t data[8];

int __cdecl mvdm_softpc_protected_address(uint16_t selector, uint32_t offset,
    uint32_t *address_out)
{
    (void)selector;
    (void)offset;
    (void)address_out;
    ++errors; /* This existing frame fixture must keep current-mode lookup. */
    return 0;
}

int __cdecl mvdm_softpc_effective_address(uint16_t selector, uint32_t offset,
    uint32_t *address_out)
{
    ++address_calls;
    if (selector != 0x1234u) return 0;
    *address_out = 0x1000u + offset;
    return 1;
}

int __cdecl mvdm_softpc_guest_memory_acquire(uint32_t address, uint32_t count,
    uint32_t access, guest_memory_lease **lease_out, uint8_t **bytes_out)
{
    ++acquire_calls;
    if (address != 0x1020u || access != GUEST_MEMORY_ACCESS_WRITE) ++errors;
    if (count > sizeof(data)) return 0;
    *lease_out = (guest_memory_lease *)data;
    *bytes_out = data;
    return 1;
}

int __cdecl mvdm_softpc_guest_memory_release(guest_memory_lease *lease, int commit)
{
    ++release_calls;
    if (lease != (guest_memory_lease *)data || commit != 1) ++errors;
    return 1;
}

int __cdecl main(void)
{
    wow_callback_frame_lease view;
    unsigned before, after;
    int good, released, bad_selector, failed_acquire;
    __asm mov before, esp
    good = wow_callback_frame_acquire_vp(0x12340020u, 8,
        GUEST_MEMORY_ACCESS_WRITE, &view);
    if (!good || view.bytes != data || view.guest_address != 0x1020u ||
        view.byte_count != 8) ++errors;
    released = wow_callback_frame_release(&view, 1);
    if (view.lease || view.bytes || view.byte_count) ++errors;
    bad_selector = wow_callback_frame_acquire_vp(0x43210020u, 8,
        GUEST_MEMORY_ACCESS_WRITE, &view);
    failed_acquire = wow_callback_frame_acquire_vp(0x12340020u, 9,
        GUEST_MEMORY_ACCESS_WRITE, &view);
    __asm mov after, esp
    __asm mov esp, before
    printf("stack_delta=%d address=%u acquire=%u release=%u errors=%u\n",
        (int)(after - before), address_calls, acquire_calls, release_calls, errors);
    if (before != after || !good || !released || bad_selector || failed_acquire ||
        errors || address_calls != 3 || acquire_calls != 2 || release_calls != 1)
        return 3;
    puts("WOW_STDCALL_TO_CDECL_WORKER_LEASE_ABI_OK");
    return 0;
}
