#include "mvdm_realmode_stack.h"

#include <string.h>

#include "adapter-bochs/machine_facade.h"
#include "mvdm_guest_location.h"

static uint16_t read_u16(const uint8_t *bytes)
{
    return (uint16_t)((uint16_t)bytes[0] | ((uint16_t)bytes[1] << 8));
}

static void write_u16(uint8_t *bytes, uint16_t value)
{
    bytes[0] = (uint8_t)value;
    bytes[1] = (uint8_t)(value >> 8);
}

int mvdm_realmode_push_interrupt(uint16_t interrupt_number,
    uint32_t return_bop_far)
{
    struct machine_facade_real_mode_frame expected;
    struct machine_facade_real_mode_frame candidate;
    mvdm_guest_location ivt;
    mvdm_guest_location stack;
    mvdm_guest_location_lease ivt_lease;
    mvdm_guest_location_lease stack_lease;
    uint8_t original_bytes[6];
    uint16_t target_ip;
    uint16_t target_cs;

    if (interrupt_number > 255u ||
        machine_facade_copy_real_mode_frame(&expected) !=
            MACHINE_FACADE_REAL_MODE_FRAME_OK || expected.sp <= 6u ||
        !mvdm_guest_location_set_real_mode(&ivt, 0u,
            (uint16_t)(interrupt_number * 4u)) ||
        !mvdm_guest_location_acquire(&ivt, 4u, GUEST_MEMORY_ACCESS_READ,
            &ivt_lease)) return 0;
    target_ip = read_u16(ivt_lease.bytes);
    target_cs = read_u16(ivt_lease.bytes + 2u);
    if (!mvdm_guest_location_release(&ivt_lease, 0) ||
        !mvdm_guest_location_set_real_mode(&stack, expected.ss,
            (uint16_t)(expected.sp - 6u)) ||
        !mvdm_guest_location_acquire(&stack, 6u, GUEST_MEMORY_ACCESS_WRITE,
            &stack_lease)) return 0;

    memcpy(original_bytes, stack_lease.bytes, sizeof(original_bytes));
    write_u16(stack_lease.bytes, (uint16_t)return_bop_far);
    write_u16(stack_lease.bytes + 2u, (uint16_t)(return_bop_far >> 16));
    write_u16(stack_lease.bytes + 4u, (uint16_t)expected.eflags);
    candidate = expected;
    candidate.sp = (uint16_t)(expected.sp - 6u);
    candidate.cs = target_cs;
    candidate.ip = target_ip;

    /* The session lease first gives the exact bounded source stack span.
     * The paused CPU cannot become stale during this synchronous operation;
     * if its guarded commit nevertheless rejects, restore the copied bytes. */
    if (!mvdm_guest_location_release(&stack_lease, 1)) return 0;
    if (machine_facade_commit_real_mode_frame(&expected, &candidate) ==
        MACHINE_FACADE_REAL_MODE_FRAME_OK) return 1;
    (void)mvdm_guest_location_copy_to_guest(&stack, original_bytes,
        sizeof(original_bytes));
    return 0;
}

int mvdm_realmode_simulate_iret_cf(void)
{
    struct machine_facade_real_mode_frame expected;
    struct machine_facade_real_mode_frame candidate;
    mvdm_guest_location stack;
    mvdm_guest_location_lease lease;
    uint16_t saved_flags;

    if (machine_facade_copy_real_mode_frame(&expected) !=
            MACHINE_FACADE_REAL_MODE_FRAME_OK || expected.sp > 0xfff9u ||
        !mvdm_guest_location_set_real_mode(&stack, expected.ss, expected.sp) ||
        !mvdm_guest_location_acquire(&stack, 6u, GUEST_MEMORY_ACCESS_READ,
            &lease)) return 0;
    candidate = expected;
    candidate.ip = read_u16(lease.bytes);
    candidate.cs = read_u16(lease.bytes + 2u);
    saved_flags = read_u16(lease.bytes + 4u);
    candidate.eflags = (expected.eflags & 0xffff0000u) |
        ((uint32_t)(saved_flags & UINT16_C(0xfffe)) |
         (expected.eflags & 1u));
    candidate.sp = (uint16_t)(expected.sp + 6u);
    if (!mvdm_guest_location_release(&lease, 0)) return 0;
    return machine_facade_commit_real_mode_frame(&expected, &candidate) ==
        MACHINE_FACADE_REAL_MODE_FRAME_OK;
}
