#include <stdint.h>

#include "bx_ntvdm_cpu_state_abi.h"

int main(void)
{
    bx_ntvdm_cpu_state_v1 state;

    bx_ntvdm_cpu_state_v1_initialize(&state, BX_NTVDM_CPU_EXECUTION_REAL);
    state.eax = UINT32_C(0x12345678);
    state.edi = UINT32_C(0x000008ae);
    state.eip = UINT32_C(0x00000475);
    state.cs = UINT16_C(0x0070);
    state.ss = UINT16_C(0x0000);
    if (sizeof(state) != 72u || !bx_ntvdm_cpu_state_v1_valid(&state) ||
        state.eax != UINT32_C(0x12345678) || state.edi != UINT32_C(0x000008ae) ||
        state.eip != UINT32_C(0x00000475) || state.cs != UINT16_C(0x0070)) {
        return 1;
    }
    state.execution_mode = 0u;
    if (bx_ntvdm_cpu_state_v1_valid(&state)) { return 2; }
    state.execution_mode = BX_NTVDM_CPU_EXECUTION_PROTECTED;
    state.reserved0 = 1u;
    if (bx_ntvdm_cpu_state_v1_valid(&state)) { return 3; }
    bx_ntvdm_cpu_state_v1_initialize(0, BX_NTVDM_CPU_EXECUTION_REAL);
    return bx_ntvdm_cpu_state_v1_valid(0) ? 4 : 0;
}
