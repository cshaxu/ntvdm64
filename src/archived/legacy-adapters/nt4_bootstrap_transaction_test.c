#include "nt4_bootstrap_transaction.h"

#include <stdio.h>

static int expect(nt4_bootstrap_transaction_result actual,
    nt4_bootstrap_transaction_result expected)
{
    return actual == expected ? 0 : 1;
}

int main(void)
{
    uint8_t ntio[0x8400u] = { 0u };
    uint8_t ntdos[] = { 0xe9u, 0xfeu };
    const uint8_t virtual_interrupt_state[4] = { 0u, 0u, 0u, 0u };
    nt4_ntio_bootstrap_plan ntio_plan = { NULL, 0u, 0u, 0u, 0u };
    nt4_ntdos_load_plan ntdos_plan = { NULL, 0u, 0u, 0u };
    nt4_ntdos_load_plan overlap_plan = { NULL, 0u, 0u, 0u };
    nt4_bootstrap_transaction transaction = { { { 0u, NULL, 0u }, { 0u, NULL, 0u } }, 0u, 0u, 0u };
    int failed = 0;

    failed |= nt4_ntio_bootstrap_plan_build(ntio, sizeof(ntio), virtual_interrupt_state,
        0x100000u, &ntio_plan) != NT4_NTIO_BOOTSTRAP_PLAN_OK;
    failed |= nt4_ntdos_load_plan_build(ntdos, sizeof(ntdos), 0x08b0u,
        0x100000u, &ntdos_plan) != NT4_NTDOS_LOAD_PLAN_OK;
    failed |= expect(nt4_bootstrap_transaction_build(&ntio_plan, &ntdos_plan,
        0x100000u, &transaction), NT4_BOOTSTRAP_TRANSACTION_OK);
    failed |= transaction.preload_count != 2u || transaction.preloads[0].physical != 0x700u ||
        transaction.preloads[1].physical != 0x8b00u || transaction.preloads[0].bytes != ntio_plan.preload_bytes ||
        transaction.preloads[1].bytes != ntdos_plan.preload_bytes || transaction.entry_cs != 0x70u;

    failed |= nt4_ntdos_load_plan_build(ntdos, sizeof(ntdos), 0x0070u,
        0x100000u, &overlap_plan) != NT4_NTDOS_LOAD_PLAN_OK;
    transaction.preload_count = 0u;
    failed |= expect(nt4_bootstrap_transaction_build(&ntio_plan, &overlap_plan,
        0x100000u, &transaction), NT4_BOOTSTRAP_TRANSACTION_OVERLAP_REJECTED);
    failed |= transaction.preload_count != 0u;
    failed |= expect(nt4_bootstrap_transaction_build(&ntio_plan, &ntdos_plan,
        0x8ae1u, &transaction), NT4_BOOTSTRAP_TRANSACTION_RAM_RANGE_REJECTED);
    failed |= transaction.preload_count != 0u;

    nt4_ntdos_load_plan_release(&overlap_plan);
    nt4_ntdos_load_plan_release(&ntdos_plan);
    nt4_ntio_bootstrap_plan_release(&ntio_plan);
    if (failed) return 1;
    puts("nt4-bootstrap-transaction-test: adjacent private preload transaction verified");
    return 0;
}
