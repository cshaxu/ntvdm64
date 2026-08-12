#include "nt4_ntdos_load_plan.h"

#include <stdio.h>
#include <string.h>

static int expect(nt4_ntdos_load_plan_result actual, nt4_ntdos_load_plan_result expected)
{
    return actual == expected ? 0 : 1;
}

int main(void)
{
    uint8_t source[] = { 0xe9u, 0x01u, 0x02u, 0x03u, 0xfeu };
    uint8_t original[sizeof(source)];
    nt4_ntdos_load_plan plan = { NULL, 0u, 0u, 0u };
    int failed = 0;

    memcpy(original, source, sizeof(source));
    failed |= expect(nt4_ntdos_load_plan_build(source, sizeof(source), 0x1234u,
        0x100000u, &plan), NT4_NTDOS_LOAD_PLAN_OK);
    failed |= plan.preload_bytes == NULL || plan.preload_byte_count != sizeof(source) ||
        plan.preload_physical != 0x12340u || plan.load_segment != 0x1234u;
    failed |= memcmp(source, original, sizeof(source)) != 0 ||
        memcmp(plan.preload_bytes, original, sizeof(source)) != 0;
    nt4_ntdos_load_plan_release(&plan);

    failed |= expect(nt4_ntdos_load_plan_build(source, 0u, 0x1234u,
        0x100000u, &plan), NT4_NTDOS_LOAD_PLAN_INVALID_ARGUMENT);
    failed |= plan.preload_bytes != NULL;
    failed |= expect(nt4_ntdos_load_plan_build(source, sizeof(source), 0xffffu,
        0xffff4u, &plan), NT4_NTDOS_LOAD_PLAN_RAM_RANGE_REJECTED);
    failed |= plan.preload_bytes != NULL;
    failed |= expect(nt4_ntdos_load_plan_build(source, sizeof(source), 0x1234u,
        0x12340u + sizeof(source) - 1u, &plan), NT4_NTDOS_LOAD_PLAN_RAM_RANGE_REJECTED);
    failed |= plan.preload_bytes != NULL;
    if (failed) return 1;
    puts("nt4-ntdos-load-plan-test: complete DI:0000 range preflight verified");
    return 0;
}
