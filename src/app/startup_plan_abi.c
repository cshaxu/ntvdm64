#include "startup_plan_abi.h"

#include <string.h>

#include "guest_range.h"

static int runtime_startup_entry_delta_v1_valid(
    const runtime_cpu_state_v1 *entry)
{
    return entry != 0 && runtime_cpu_state_v1_valid(entry) &&
        entry->execution_mode == RUNTIME_CPU_EXECUTION_REAL &&
        entry->eip <= 0xffffu && entry->eax == 0u && entry->ebx == 0u &&
        entry->ecx == 0u && entry->edx == 0u && entry->esi == 0u &&
        entry->edi == 0u && entry->ebp == 0u && entry->esp == 0u &&
        entry->eflags == 0u && entry->ds == 0u && entry->es == 0u &&
        entry->ss == 0u && entry->fs == 0u && entry->gs == 0u &&
        entry->reserved0 == 0u;
}

void runtime_startup_plan_v1_initialize(runtime_startup_plan_v1 *plan,
    const runtime_guest_write_v1 *payload_write,
    const runtime_cpu_state_v1 *entry_cpu,
    uint64_t preserved_state_address, uint64_t preserved_state_bytes)
{
    if (plan == 0 || payload_write == 0 || entry_cpu == 0) return;
    memset(plan, 0, sizeof(*plan));
    plan->magic = RUNTIME_STARTUP_PLAN_ABI_MAGIC;
    plan->abi_version = RUNTIME_STARTUP_PLAN_ABI_VERSION;
    plan->struct_bytes = (uint32_t)sizeof(*plan);
    plan->payload_write = *payload_write;
    plan->entry_cpu = *entry_cpu;
    plan->preserved_state_address = preserved_state_address;
    plan->preserved_state_bytes = preserved_state_bytes;
}

int runtime_startup_plan_v1_preflight(const runtime_startup_plan_v1 *plan,
    uint64_t aperture_bytes, uint64_t payload_bytes)
{
    runtime_guest_range preserved;
    if (plan == 0 || plan->magic != RUNTIME_STARTUP_PLAN_ABI_MAGIC ||
        plan->abi_version != RUNTIME_STARTUP_PLAN_ABI_VERSION ||
        plan->struct_bytes != sizeof(*plan) || plan->flags != 0u ||
        !runtime_guest_write_v1_preflight(&plan->payload_write,
            aperture_bytes, payload_bytes) ||
        !runtime_startup_entry_delta_v1_valid(&plan->entry_cpu) ||
        plan->preserved_state_bytes == 0u ||
        plan->preserved_state_bytes > RUNTIME_STARTUP_PLAN_MAX_PRESERVED_BYTES)
        return 0;
    preserved.address = plan->preserved_state_address;
    preserved.length = plan->preserved_state_bytes;
    return runtime_guest_range_within(aperture_bytes, &preserved);
}
