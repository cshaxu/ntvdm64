#include "startup_plan_abi.h"

#include <string.h>

#include "bx_ntvdm_guest_range.h"

static int bx_ntvdm_startup_entry_delta_v1_valid(
    const bx_ntvdm_cpu_state_v1 *entry)
{
    return entry != 0 && bx_ntvdm_cpu_state_v1_valid(entry) &&
        entry->execution_mode == BX_NTVDM_CPU_EXECUTION_REAL &&
        entry->eip <= 0xffffu && entry->eax == 0u && entry->ebx == 0u &&
        entry->ecx == 0u && entry->edx == 0u && entry->esi == 0u &&
        entry->edi == 0u && entry->ebp == 0u && entry->esp == 0u &&
        entry->eflags == 0u && entry->ds == 0u && entry->es == 0u &&
        entry->ss == 0u && entry->fs == 0u && entry->gs == 0u &&
        entry->reserved0 == 0u;
}

void bx_ntvdm_startup_plan_v1_initialize(bx_ntvdm_startup_plan_v1 *plan,
    const bx_ntvdm_guest_write_v1 *payload_write,
    const bx_ntvdm_cpu_state_v1 *entry_cpu,
    uint64_t preserved_state_address, uint64_t preserved_state_bytes)
{
    if (plan == 0 || payload_write == 0 || entry_cpu == 0) return;
    memset(plan, 0, sizeof(*plan));
    plan->magic = BX_NTVDM_STARTUP_PLAN_ABI_MAGIC;
    plan->abi_version = BX_NTVDM_STARTUP_PLAN_ABI_VERSION;
    plan->struct_bytes = (uint32_t)sizeof(*plan);
    plan->payload_write = *payload_write;
    plan->entry_cpu = *entry_cpu;
    plan->preserved_state_address = preserved_state_address;
    plan->preserved_state_bytes = preserved_state_bytes;
}

int bx_ntvdm_startup_plan_v1_preflight(const bx_ntvdm_startup_plan_v1 *plan,
    uint64_t aperture_bytes, uint64_t payload_bytes)
{
    bx_ntvdm_guest_range preserved;
    if (plan == 0 || plan->magic != BX_NTVDM_STARTUP_PLAN_ABI_MAGIC ||
        plan->abi_version != BX_NTVDM_STARTUP_PLAN_ABI_VERSION ||
        plan->struct_bytes != sizeof(*plan) || plan->flags != 0u ||
        !bx_ntvdm_guest_write_v1_preflight(&plan->payload_write,
            aperture_bytes, payload_bytes) ||
        !bx_ntvdm_startup_entry_delta_v1_valid(&plan->entry_cpu) ||
        plan->preserved_state_bytes == 0u ||
        plan->preserved_state_bytes > BX_NTVDM_STARTUP_PLAN_MAX_PRESERVED_BYTES)
        return 0;
    preserved.address = plan->preserved_state_address;
    preserved.length = plan->preserved_state_bytes;
    return bx_ntvdm_guest_range_within(aperture_bytes, &preserved);
}
