#include "bx_ntvdm_cmd_current_dir_service.h"

#include <stdio.h>
#include <string.h>

int main(void)
{
    bx_ntvdm_exception_event_v1 event = { BX_NTVDM_EXCEPTION_ABI_MAGIC,
        BX_NTVDM_EXCEPTION_ABI_VERSION, sizeof(event),
        BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION, 0u, 6u, 0u, 0u, 0x6774u };
    bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_multi_write_transaction_v1 transaction;
    bx_ntvdm_command_host_context_v1 context;
    uint8_t payload[BX_NTVDM_MULTI_WRITE_MAX_PAYLOAD] = {0};

    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.eax = 2u; cpu.ds = 0x1234u; cpu.esi = 0x5678u;
    bx_ntvdm_instruction_window_v1_capture(&window,
        (const uint8_t[]){0xc4u, 0xc4u, 0x54u, 0x04u}, 4u);
    if (!bx_ntvdm_command_host_context_v1_initialize(&context, 2u,
            (const uint8_t *)"C:\\WORK\\NTDOS64", 15u) ||
        !bx_ntvdm_cmd_current_dir_service_v1_prepare(UINT32_C(1) << 2u,
            &context, &event, &cpu, &window, &transaction, payload) ||
        transaction.writes.write_count != 1u ||
        transaction.writes.writes[0].guest_physical_address != 0x179b8u ||
        transaction.writes.writes[0].byte_count != 16u ||
        memcmp(payload, "C:\\WORK\\NTDOS64", 16u) != 0 || transaction.result.eflags_values != 0u ||
        transaction.result.resume_rip != 0x6778u) return 1;
    cpu.eax = 3u;
    if (!bx_ntvdm_cmd_current_dir_service_v1_prepare((UINT32_C(1) << 2u) |
            (UINT32_C(1) << 3u), &context, &event, &cpu, &window,
            &transaction, payload) || transaction.writes.write_count != 0u ||
        transaction.result.cpu_delta.gpr16_write_mask != 1u ||
        transaction.result.cpu_delta.gpr16_values[0] != 0u ||
        transaction.result.eflags_values != 1u) return 2;
    cpu.eax = 2u;
    if (!bx_ntvdm_cmd_current_dir_service_v1_prepare(0u, &context, &event, &cpu,
            &window, &transaction, payload) || transaction.writes.write_count != 0u ||
        transaction.result.cpu_delta.gpr16_write_mask != 1u ||
        transaction.result.cpu_delta.gpr16_values[0] != 0u ||
        transaction.result.eflags_values != 1u) return 3;
    puts("bx-ntvdm CMD current-directory service: root/error atomic contracts verified");
    return 0;
}
