#include "cpu_delta_abi.h"

#include <string.h>

void runtime_cpu_delta_initialize(runtime_cpu_delta *delta)
{
    if (delta == 0) return;
    memset(delta, 0, sizeof(*delta));
    delta->magic = RUNTIME_CPU_DELTA_ABI_MAGIC;
    delta->abi_version = RUNTIME_CPU_DELTA_ABI_VERSION;
    delta->struct_bytes = sizeof(*delta);
}

int runtime_cpu_delta_valid(const runtime_cpu_delta *delta)
{
    return delta != 0 && delta->magic == RUNTIME_CPU_DELTA_ABI_MAGIC &&
        delta->abi_version == RUNTIME_CPU_DELTA_ABI_VERSION &&
        delta->struct_bytes == sizeof(*delta) &&
        (delta->gpr16_write_mask & ~RUNTIME_CPU_DELTA_GPR16_MASK) == 0u &&
        (delta->segment_write_mask & ~((1u << RUNTIME_CPU_DELTA_SEGMENT_COUNT) - 1u)) == 0u &&
        delta->reserved0 == 0u;
}

int runtime_cpu_delta_set_segment(runtime_cpu_delta *delta,
    uint32_t segment_index, uint16_t value)
{
    if (!runtime_cpu_delta_valid(delta) ||
        segment_index >= RUNTIME_CPU_DELTA_SEGMENT_COUNT) return 0;
    delta->segment_values[segment_index] = value;
    delta->segment_write_mask |= 1u << segment_index;
    return 1;
}

int runtime_cpu_delta_set_gpr16(runtime_cpu_delta *delta,
    uint32_t register_index, uint16_t value)
{
    if (!runtime_cpu_delta_valid(delta) ||
        register_index >= RUNTIME_CPU_DELTA_GPR16_COUNT) return 0;
    delta->gpr16_values[register_index] = value;
    delta->gpr16_write_mask |= 1u << register_index;
    return 1;
}
