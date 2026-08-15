#include "nt4_ntio_bootstrap_plan.h"

#include <stdio.h>
#include <string.h>

static int expect(nt4_ntio_bootstrap_plan_result actual,
    nt4_ntio_bootstrap_plan_result expected)
{
    return actual == expected ? 0 : 1;
}

int main(void)
{
    uint8_t source[32];
    uint8_t original[32];
    const uint8_t virtual_interrupt_state[NT4_NTIO_VIRTUAL_INTERRUPT_SIZE] =
        { 0x91u, 0x82u, 0x73u, 0x64u };
    nt4_ntio_bootstrap_plan plan = { NULL, 0u, 0u, 0u, 0u };
    size_t index;
    int failed = 0;

    for (index = 0u; index < sizeof(source); ++index) source[index] = (uint8_t)index;
    memcpy(original, source, sizeof(source));

    failed |= expect(nt4_ntio_bootstrap_plan_build(source, sizeof(source),
        virtual_interrupt_state, 0x10000u, &plan), NT4_NTIO_BOOTSTRAP_PLAN_OK);
    failed |= plan.preload_bytes == NULL || plan.preload_byte_count != sizeof(source) ||
        plan.preload_physical != NT4_NTIO_LOAD_PHYSICAL || plan.entry_cs != NT4_NTIO_LOAD_CS ||
        plan.entry_ip != NT4_NTIO_LOAD_IP;
    failed |= memcmp(source, original, sizeof(source)) != 0;
    failed |= memcmp(plan.preload_bytes + 0x14u, virtual_interrupt_state,
        NT4_NTIO_VIRTUAL_INTERRUPT_SIZE) != 0;
    failed |= memcmp(plan.preload_bytes, original, 0x14u) != 0;
    failed |= memcmp(plan.preload_bytes + 0x18u, original + 0x18u,
        sizeof(source) - 0x18u) != 0;
    nt4_ntio_bootstrap_plan_release(&plan);

    failed |= expect(nt4_ntio_bootstrap_plan_build(source, 0x17u,
        virtual_interrupt_state, 0x10000u, &plan), NT4_NTIO_BOOTSTRAP_PLAN_IMAGE_TOO_SHORT);
    failed |= plan.preload_bytes != NULL;
    failed |= expect(nt4_ntio_bootstrap_plan_build(source, sizeof(source),
        virtual_interrupt_state, NT4_NTIO_LOAD_PHYSICAL + sizeof(source) - 1u,
        &plan), NT4_NTIO_BOOTSTRAP_PLAN_RAM_RANGE_REJECTED);
    failed |= plan.preload_bytes != NULL;
    failed |= expect(nt4_ntio_bootstrap_plan_build(source, sizeof(source),
        virtual_interrupt_state, 0x10000u, NULL), NT4_NTIO_BOOTSTRAP_PLAN_INVALID_ARGUMENT);
    failed |= plan.preload_bytes != NULL;

    if (failed) return 1;
    puts("nt4-ntio-bootstrap-plan-test: private NTIO preload preservation verified");
    return 0;
}
