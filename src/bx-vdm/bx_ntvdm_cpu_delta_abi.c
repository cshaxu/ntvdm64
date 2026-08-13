#include "bx_ntvdm_cpu_delta_abi.h"

#include <string.h>

void bx_ntvdm_cpu_delta_v1_initialize(bx_ntvdm_cpu_delta_v1 *delta)
{
    if (delta == 0) return;
    memset(delta, 0, sizeof(*delta));
    delta->magic = BX_NTVDM_CPU_DELTA_ABI_MAGIC;
    delta->abi_version = BX_NTVDM_CPU_DELTA_ABI_VERSION;
    delta->struct_bytes = sizeof(*delta);
}

int bx_ntvdm_cpu_delta_v1_valid(const bx_ntvdm_cpu_delta_v1 *delta)
{
    return delta != 0 && delta->magic == BX_NTVDM_CPU_DELTA_ABI_MAGIC &&
        delta->abi_version == BX_NTVDM_CPU_DELTA_ABI_VERSION &&
        delta->struct_bytes == sizeof(*delta) &&
        (delta->gpr16_write_mask & ~BX_NTVDM_CPU_DELTA_V1_GPR16_MASK) == 0u &&
        delta->reserved0 == 0u;
}

int bx_ntvdm_cpu_delta_v1_set_gpr16(bx_ntvdm_cpu_delta_v1 *delta,
    uint32_t register_index, uint16_t value)
{
    if (!bx_ntvdm_cpu_delta_v1_valid(delta) ||
        register_index >= BX_NTVDM_CPU_DELTA_V1_GPR16_COUNT) return 0;
    delta->gpr16_values[register_index] = value;
    delta->gpr16_write_mask |= 1u << register_index;
    return 1;
}
