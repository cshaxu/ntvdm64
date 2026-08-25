#include <stdint.h>

#include "cpu_state_abi.h"
#include "guest_write_abi.h"
#include "startup_plan_abi.h"

int main(void)
{
    runtime_guest_write write;
    runtime_cpu_state entry;
    runtime_startup_plan plan;

    runtime_guest_write_initialize(&write, 0x700u, 0x8400u, 0u);
    runtime_cpu_state_initialize(&entry, RUNTIME_CPU_EXECUTION_REAL);
    entry.cs = 0x70u;
    runtime_startup_plan_initialize(&plan, &write, &entry, 0x714u, 4u);
    if (!runtime_startup_plan_preflight(&plan, 0x100000u, 0x8400u)) return 1;
    plan.entry_cpu.edx = 0x303u;
    if (runtime_startup_plan_preflight(&plan, 0x100000u, 0x8400u)) return 2;
    plan.entry_cpu.edx = 0u;
    plan.entry_cpu.ds = 0x40u;
    if (runtime_startup_plan_preflight(&plan, 0x100000u, 0x8400u)) return 3;
    plan.entry_cpu.ds = 0u;
    plan.preserved_state_address = 0x100000u;
    if (runtime_startup_plan_preflight(&plan, 0x100000u, 0x8400u)) return 4;
    plan.preserved_state_address = 0x714u;
    plan.preserved_state_bytes = 0u;
    if (runtime_startup_plan_preflight(&plan, 0x100000u, 0x8400u)) return 5;
    plan.preserved_state_bytes = RUNTIME_STARTUP_PLAN_MAX_PRESERVED_BYTES + 1u;
    if (runtime_startup_plan_preflight(&plan, 0x100000u, 0x8400u)) return 6;
    return 0;
}
