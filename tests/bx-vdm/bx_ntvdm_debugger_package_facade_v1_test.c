#include "bop-v1/bx_ntvdm_debugger_package_facade_v1.h"

#include <string.h>

static int dispatch(uint32_t window_bytes)
{
    uint8_t bytes[4] = { 0xc4u, 0xc4u, 0x56u, 0x90u };
    bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_bop_ingress_v1 ingress;
    bx_ntvdm_bop_provider_selection_v1 selection;
    bx_ntvdm_exception_event_v1 event;
    bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_cpu_result_v2 result;
    memset(&event, 0, sizeof(event));
    event.magic = BX_NTVDM_EXCEPTION_ABI_MAGIC;
    event.abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION;
    event.struct_bytes = sizeof(event);
    event.kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION;
    event.vector = 6u;
    event.fault_rip = 0x100u;
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    bx_ntvdm_instruction_window_v1_capture(&window, bytes, window_bytes);
    return bx_ntvdm_bop_ingress_v1_classify(&window, &ingress) &&
        ingress.has_service == 0u &&
        bx_ntvdm_bop_provider_registry_v1_select(&ingress, &selection) &&
        bx_ntvdm_debugger_package_facade_v1_dispatch(&ingress, &selection,
            &event, &cpu, &result) && result.disposition ==
            BX_NTVDM_CPU_RESULT_V2_STOP && result.resume_rip == 0u &&
        result.cpu_delta.gpr16_write_mask == 0u && result.eflags_write_mask == 0u;
}

int main(void)
{
    return dispatch(3u) && dispatch(4u) ? 0 : 1;
}
