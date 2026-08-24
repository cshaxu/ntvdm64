#include <stdint.h>
#include <string.h>

#include "bx_ntvdm_exception_abi.h"

int main(void)
{
    bx_ntvdm_exception_event_v1 event;
    bx_ntvdm_exception_result_v1 result;
    bx_ntvdm_cpu_state_v1 state;
    bx_ntvdm_instruction_window_v1 window;

    memset(&event, 0, sizeof(event));
    memset(&result, 0, sizeof(result));
    event.magic = BX_NTVDM_EXCEPTION_ABI_MAGIC;
    event.abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION;
    event.struct_bytes = (uint32_t)sizeof(event);
    event.kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION;
    event.cpu_id = 0u;
    event.vector = 6u;
    event.error_code = 0u;
    event.fault_rip = UINT64_C(0x00000000000ffff0);

    if (sizeof(event) != 40u || event.struct_bytes != 40u ||
        event.magic != BX_NTVDM_EXCEPTION_ABI_MAGIC || event.vector != 6u ||
        event.fault_rip != UINT64_C(0x00000000000ffff0)) {
        return 1;
    }

    if (!bx_ntvdm_exception_event_v1_valid(&event) ||
        bx_ntvdm_exception_event_v1_valid(0)) {
        return 2;
    }
    if (!bx_ntvdm_exception_result_v1_resume(&result, event.fault_rip + 2u) ||
        bx_ntvdm_exception_result_v1_resume(0, event.fault_rip)) {
        return 3;
    }
    if (sizeof(result) != 32u || result.struct_bytes != 32u ||
        result.disposition != BX_NTVDM_EXCEPTION_RESULT_RESUME ||
        result.resume_rip != UINT64_C(0x00000000000ffff2)) {
        return 4;
    }
    event.kind = 0u;
    if (bx_ntvdm_exception_event_v1_valid(&event)) { return 5; }
    event.kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION;
    bx_ntvdm_exception_result_v1_pass_through(&result);
    if (result.disposition != BX_NTVDM_EXCEPTION_RESULT_PASS_THROUGH ||
        result.resume_rip != 0u) { return 6; }
    if (!bx_ntvdm_exception_dispatch_v1(&event, &result) ||
        result.disposition != BX_NTVDM_EXCEPTION_RESULT_PASS_THROUGH ||
        bx_ntvdm_exception_dispatch_v1(0, &result)) { return 7; }
    bx_ntvdm_cpu_state_v1_initialize(&state, BX_NTVDM_CPU_EXECUTION_REAL);
    if (!bx_ntvdm_exception_dispatch_state_v1(&event, &state, &result) ||
        result.disposition != BX_NTVDM_EXCEPTION_RESULT_PASS_THROUGH) { return 8; }
    state.execution_mode = 0u;
    if (bx_ntvdm_exception_dispatch_state_v1(&event, &state, &result)) { return 9; }
    bx_ntvdm_cpu_state_v1_initialize(&state, BX_NTVDM_CPU_EXECUTION_REAL);
    bx_ntvdm_instruction_window_v1_capture(&window, 0, 0u);
    if (!bx_ntvdm_exception_dispatch_state_window_v1(&event, &state, &window,
        &result) || result.disposition != BX_NTVDM_EXCEPTION_RESULT_PASS_THROUGH)
        return 10;
    window.reserved0 = 1u;
    if (bx_ntvdm_exception_dispatch_state_window_v1(&event, &state, &window,
        &result)) return 11;
    return 0;
}
