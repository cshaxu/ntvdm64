#include "bx_ntvdm_cpu_state_abi.h"

#include <string.h>

static int bx_ntvdm_cpu_execution_mode_valid(uint32_t execution_mode)
{
    return execution_mode == BX_NTVDM_CPU_EXECUTION_REAL ||
        execution_mode == BX_NTVDM_CPU_EXECUTION_PROTECTED ||
        execution_mode == BX_NTVDM_CPU_EXECUTION_V8086;
}

void bx_ntvdm_cpu_state_v1_initialize(bx_ntvdm_cpu_state_v1 *state,
    uint32_t execution_mode)
{
    if (state == 0) return;
    memset(state, 0, sizeof(*state));
    state->magic = BX_NTVDM_CPU_STATE_ABI_MAGIC;
    state->abi_version = BX_NTVDM_CPU_STATE_ABI_VERSION;
    state->struct_bytes = sizeof(*state);
    state->execution_mode = execution_mode;
}

int bx_ntvdm_cpu_state_v1_valid(const bx_ntvdm_cpu_state_v1 *state)
{
    return state != 0 && state->magic == BX_NTVDM_CPU_STATE_ABI_MAGIC &&
        state->abi_version == BX_NTVDM_CPU_STATE_ABI_VERSION &&
        state->struct_bytes == sizeof(*state) &&
        bx_ntvdm_cpu_execution_mode_valid(state->execution_mode) &&
        state->reserved0 == 0u;
}
