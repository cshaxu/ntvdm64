#include "cpu_state_abi.h"

#include <string.h>

static int runtime_cpu_execution_mode_valid(uint32_t execution_mode)
{
    return execution_mode == RUNTIME_CPU_EXECUTION_REAL ||
        execution_mode == RUNTIME_CPU_EXECUTION_PROTECTED ||
        execution_mode == RUNTIME_CPU_EXECUTION_V8086;
}

void runtime_cpu_state_v1_initialize(runtime_cpu_state_v1 *state,
    uint32_t execution_mode)
{
    if (state == 0) return;
    memset(state, 0, sizeof(*state));
    state->magic = RUNTIME_CPU_STATE_ABI_MAGIC;
    state->abi_version = RUNTIME_CPU_STATE_ABI_VERSION;
    state->struct_bytes = sizeof(*state);
    state->execution_mode = execution_mode;
}

int runtime_cpu_state_v1_valid(const runtime_cpu_state_v1 *state)
{
    return state != 0 && state->magic == RUNTIME_CPU_STATE_ABI_MAGIC &&
        state->abi_version == RUNTIME_CPU_STATE_ABI_VERSION &&
        state->struct_bytes == sizeof(*state) &&
        runtime_cpu_execution_mode_valid(state->execution_mode) &&
        state->reserved0 == 0u;
}
