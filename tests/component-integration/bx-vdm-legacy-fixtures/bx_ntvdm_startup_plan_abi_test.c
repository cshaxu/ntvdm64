#include <stdint.h>

#include "bx_ntvdm_cpu_state_abi.h"
#include "bx_ntvdm_guest_write_abi.h"
#include "bx_ntvdm_startup_plan_abi.h"

int main(void)
{
    bx_ntvdm_guest_write_v1 write;
    bx_ntvdm_cpu_state_v1 entry;
    bx_ntvdm_startup_plan_v1 plan;

    bx_ntvdm_guest_write_v1_initialize(&write, 0x700u, 0x8400u, 0u);
    bx_ntvdm_cpu_state_v1_initialize(&entry, BX_NTVDM_CPU_EXECUTION_REAL);
    entry.cs = 0x70u;
    bx_ntvdm_startup_plan_v1_initialize(&plan, &write, &entry, 0x714u, 4u);
    if (!bx_ntvdm_startup_plan_v1_preflight(&plan, 0x100000u, 0x8400u)) return 1;
    plan.entry_cpu.edx = 0x303u;
    if (bx_ntvdm_startup_plan_v1_preflight(&plan, 0x100000u, 0x8400u)) return 2;
    plan.entry_cpu.edx = 0u;
    plan.entry_cpu.ds = 0x40u;
    if (bx_ntvdm_startup_plan_v1_preflight(&plan, 0x100000u, 0x8400u)) return 3;
    plan.entry_cpu.ds = 0u;
    plan.preserved_state_address = 0x100000u;
    if (bx_ntvdm_startup_plan_v1_preflight(&plan, 0x100000u, 0x8400u)) return 4;
    plan.preserved_state_address = 0x714u;
    plan.preserved_state_bytes = 0u;
    if (bx_ntvdm_startup_plan_v1_preflight(&plan, 0x100000u, 0x8400u)) return 5;
    plan.preserved_state_bytes = BX_NTVDM_STARTUP_PLAN_MAX_PRESERVED_BYTES + 1u;
    if (bx_ntvdm_startup_plan_v1_preflight(&plan, 0x100000u, 0x8400u)) return 6;
    return 0;
}
