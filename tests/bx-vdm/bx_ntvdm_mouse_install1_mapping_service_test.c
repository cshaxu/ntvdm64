#include "bx_ntvdm_mouse_install1_mapping_service.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    bx_ntvdm_exception_event_v1 event = { BX_NTVDM_EXCEPTION_ABI_MAGIC,
        BX_NTVDM_EXCEPTION_ABI_VERSION, sizeof(event),
        BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION, 0u, 6u, 0u, 0u, 0x958cbu };
    bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_guest_gather_read_action_v1 action;
    bx_ntvdm_multi_write_transaction_v1 transaction;
    uint8_t payload[4];
    const uint8_t table[4] = { 0x34u, 0x12u, 0x3bu, 0x07u };

    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.cs = 0x8dc8u;
    cpu.ebx = 0x77bfu;
    bx_ntvdm_instruction_window_v1_capture(&window,
        (const uint8_t[]){ 0xc4u, 0xc4u, 0xc8u }, 3u);
    if (!bx_ntvdm_mouse_install1_mapping_service_v1_prepare(&event, &cpu,
            &window, &action) || action.disposition !=
            BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_NEED_READ ||
        action.range_count != 1u || action.total_bytes != 4u ||
        action.ranges[0].address != 0x9543fu || action.ranges[0].length != 4u ||
        action.cpu_result.resume_rip != 0x958ceu) return 1;
    if (!bx_ntvdm_mouse_install1_mapping_service_v1_complete(&event, &cpu,
            &action, table, sizeof(table), &transaction, payload) ||
        transaction.writes.write_count != 1u ||
        transaction.writes.writes[0].guest_physical_address != 0xccu ||
        transaction.writes.writes[0].byte_count != 4u ||
        transaction.result.resume_rip != 0x958ceu || memcmp(payload, table, 4u))
        return 2;
    window.bytes[2] = 0xc9u;
    if (bx_ntvdm_mouse_install1_mapping_service_v1_prepare(&event, &cpu,
            &window, &action)) return 3;
    window.bytes[2] = 0xc8u;
    cpu.execution_mode = BX_NTVDM_CPU_EXECUTION_V8086;
    if (bx_ntvdm_mouse_install1_mapping_service_v1_prepare(&event, &cpu,
            &window, &action)) return 4;
    cpu.execution_mode = BX_NTVDM_CPU_EXECUTION_REAL;
    event.fault_rip = UINT64_MAX - 2u;
    if (bx_ntvdm_mouse_install1_mapping_service_v1_prepare(&event, &cpu,
            &window, &action)) return 5;
    puts("bx-ntvdm mouse-install1 mapping: exact C8 table-to-INT33 transaction verified");
    return 0;
}
