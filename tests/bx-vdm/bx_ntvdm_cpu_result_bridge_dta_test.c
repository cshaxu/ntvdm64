#include "bop-v1/bx_ntvdm_cpu_result_bridge_v1.h"
#include "bop-v1/bx_ntvdm_dem_dta_service.h"
#include <stdio.h>

static bx_ntvdm_dem_dta_registration_v1 stored;
static int stored_once;

int bx_ntvdm_startup_transaction_bridge_v1_copy_host_drive_inventory(
    uint8_t out_types[26])
{
    (void)out_types;
    return 0;
}

int bx_ntvdm_startup_transaction_bridge_v1_store_dta_registration(
    const bx_ntvdm_dem_dta_registration_v1 *registration)
{
    if (registration == 0 || stored_once) return 0;
    stored = *registration;
    stored_once = 1;
    return 1;
}

int main(void)
{
    bx_ntvdm_exception_event_v1 event = { BX_NTVDM_EXCEPTION_ABI_MAGIC,
        BX_NTVDM_EXCEPTION_ABI_VERSION, sizeof(event),
        BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION, 0u, 6u, 0u, 0u, 0x7484u };
    bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_guest_read_action_v1 action;
    bx_ntvdm_cpu_result_v2 result;
    const uint8_t bytes[2] = { 0x34u, 0x12u };
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.ds = 0x1000u; cpu.eax = 0x0080u; cpu.edx = 0x0090u;
    cpu.ecx = 0x00a0u; cpu.esi = 0x0020u;
    bx_ntvdm_instruction_window_v1_capture(&window,
        (const uint8_t[]){0xc4u, 0xc4u, 0x50u, 0x1bu}, 4u);
    if (!bx_ntvdm_cpu_result_bridge_v1_dispatch_guest_read(&event, &cpu,
            &window, &action) || action.disposition !=
            BX_NTVDM_GUEST_READ_ACTION_V1_NEED_READ ||
        action.guest_read.address != 0x10040u || action.guest_read.length != 2u ||
        !bx_ntvdm_cpu_result_bridge_v1_complete_guest_read(&event, &cpu,
            &action, bytes, sizeof(bytes), &result) || !stored_once ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        result.resume_rip != 0x7488u || stored.dta_location != 0x10080u ||
        stored.current_pdb != 0x10090u || stored.extended_error != 0x100a0u ||
        stored.sft_head != 0x11234u) return 1;
    if (bx_ntvdm_cpu_result_bridge_v1_complete_guest_read(&event, &cpu,
            &action, bytes, sizeof(bytes), &result)) return 2;
    window.bytes[3] = 0x0fu;
    if (!bx_ntvdm_cpu_result_bridge_v1_dispatch_guest_read(&event, &cpu,
            &window, &action) || action.disposition !=
            BX_NTVDM_GUEST_READ_ACTION_V1_PASS_THROUGH) return 3;
    puts("bx-ntvdm CPU-result bridge: narrow DTA read/result verified");
    return 0;
}
