#include <stdint.h>

#include "cpu_state_abi.h"

int main(void)
{
    runtime_cpu_state_v1 state;

    runtime_cpu_state_v1_initialize(&state, RUNTIME_CPU_EXECUTION_REAL);
    state.eax = UINT32_C(0x12345678);
    state.edi = UINT32_C(0x000008ae);
    state.eip = UINT32_C(0x00000475);
    state.cs = UINT16_C(0x0070);
    state.ss = UINT16_C(0x0000);
    if (sizeof(state) != 72u || !runtime_cpu_state_v1_valid(&state) ||
        state.eax != UINT32_C(0x12345678) || state.edi != UINT32_C(0x000008ae) ||
        state.eip != UINT32_C(0x00000475) || state.cs != UINT16_C(0x0070)) {
        return 1;
    }
    state.execution_mode = 0u;
    if (runtime_cpu_state_v1_valid(&state)) { return 2; }
    state.execution_mode = RUNTIME_CPU_EXECUTION_PROTECTED;
    state.reserved0 = 1u;
    if (runtime_cpu_state_v1_valid(&state)) { return 3; }
    runtime_cpu_state_v1_initialize(0, RUNTIME_CPU_EXECUTION_REAL);
    return runtime_cpu_state_v1_valid(0) ? 4 : 0;
}
