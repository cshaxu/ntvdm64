#include <stdio.h>

#include "bx_ntvdm_dem_dta_service.h"

int main(void)
{
    bx_ntvdm_exception_event_v1 event;
    bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_guest_read_action_v1 action;
    bx_ntvdm_dem_dta_registration_v1 registration;
    bx_ntvdm_cpu_result_v2 result;
    const uint8_t sft_offset[2] = { 0x34u, 0x12u };

    const uint8_t service_bytes[4] = { 0xc4u, 0xc4u, 0x50u, 0x1bu };
    event.magic = BX_NTVDM_EXCEPTION_ABI_MAGIC;
    event.abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION;
    event.struct_bytes = sizeof(event);
    event.kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION;
    event.cpu_id = 0u; event.vector = 6u; event.error_code = 0u;
    event.reserved0 = 0u; event.fault_rip = 0x7439u;
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.ds = 0x1000u; cpu.eax = 0x0080u; cpu.edx = 0x0090u;
    cpu.ecx = 0x00a0u; cpu.esi = 0x0020u;
    bx_ntvdm_instruction_window_v1_capture(&window, service_bytes,
        sizeof(service_bytes));
    if (!bx_ntvdm_dem_dta_service_v1_dispatch(&event, &cpu, &window, &action) ||
        action.disposition != BX_NTVDM_GUEST_READ_ACTION_V1_NEED_READ ||
        action.guest_read.address != 0x10040u || action.guest_read.length != 2u ||
        !bx_ntvdm_dem_dta_service_v1_complete(&event, &cpu, &action,
            sft_offset, sizeof(sft_offset), &registration, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        result.resume_rip != 0x743du || registration.dta_location != 0x10080u ||
        registration.current_pdb != 0x10090u ||
        registration.extended_error != 0x100a0u || registration.sft_head != 0x11234u)
        return 1;
    window.bytes[3] = 0x0fu;
    if (bx_ntvdm_dem_dta_service_v1_dispatch(&event, &cpu, &window, &action)) return 2;
    window.bytes[3] = 0x1bu;
    cpu.esi = 0xffe1u;
    if (bx_ntvdm_dem_dta_service_v1_dispatch(&event, &cpu, &window, &action)) return 3;
    puts("bx-ntvdm DemSetDTALocation: bounded source-derived registration verified");
    return 0;
}
