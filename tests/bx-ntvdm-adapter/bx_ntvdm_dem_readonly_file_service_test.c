#include "bx_ntvdm_dem_readonly_file_service.h"

#include <stdio.h>
#include <string.h>

static void service_window(bx_ntvdm_instruction_window_v1 *window, uint8_t service)
{
    const uint8_t bytes[] = { 0xc4u, 0xc4u, 0x50u, service };
    bx_ntvdm_instruction_window_v1_capture(window, bytes, sizeof(bytes));
}

int main(void)
{
    uint8_t command[] = { 1u, 2u, 3u }, path[128] = "C:\\COMMAND.COM", payload[8];
    byob_image image = { command, sizeof(command) };
    byob_profile_selection selection = { 0 };
    bx_ntvdm_readonly_namespace_v1 space;
    bx_ntvdm_exception_event_v1 event = { BX_NTVDM_EXCEPTION_ABI_MAGIC,
        BX_NTVDM_EXCEPTION_ABI_VERSION, sizeof(event), BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION,
        0u, 6u, 0u, 0u, 0x100u };
    bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_guest_read_action_v1 action;
    bx_ntvdm_cpu_result_v2 result;
    bx_ntvdm_bulk_result_transaction_v1 transaction;
    uint32_t token;

    wcscpy(selection.command_placement.path, L"\\COMMAND.COM");
    selection.command_placement.drive_index = 2u; selection.has_command_placement = 1u;
    wcscpy(selection.config_file.path, L"\\CONFIG.SYS");
    selection.config_file.materialization = BYOB_GUEST_BOOT_FILE_MINIMAL_COMMENT_V1;
    wcscpy(selection.autoexec_file.path, L"\\AUTOEXEC.BAT");
    selection.autoexec_file.materialization = BYOB_GUEST_BOOT_FILE_EMPTY_V1;
    selection.has_guest_boot_files = 1u;
    if (!bx_ntvdm_readonly_namespace_v1_initialize(&space, &image, &selection)) return 1;

    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.ds = 0x1000u; cpu.esi = 0x20u; cpu.eip = 0x100u;
    service_window(&window, 0x12u);
    if (!bx_ntvdm_dem_readonly_file_v1_prepare_open(&event, &cpu, &window, &action) ||
        action.guest_read.address != 0x10020u || action.guest_read.length != sizeof(path) ||
        !bx_ntvdm_dem_readonly_file_v1_complete_open(&space, &event, &cpu, &action,
            path, sizeof(path), &result) || result.eflags_values != 0u ||
        result.cpu_delta.gpr16_values[0] == 0u) return 2;
    token = ((uint32_t)result.cpu_delta.gpr16_values[5] << 16) |
        result.cpu_delta.gpr16_values[0];

    cpu.eax = token & 0xffffu; cpu.ebp = token >> 16; cpu.ebx = 2u;
    cpu.ecx = 0u; cpu.edx = 0u; service_window(&window, 0x00u);
    if (!bx_ntvdm_dem_readonly_file_v1_seek(&space, &event, &cpu, &window, &result) ||
        result.cpu_delta.gpr16_values[0] != sizeof(command)) return 3;

    /* AX:BP is the source-defined DEM handle input.  BX/CX/DX are deliberately
       nonzero so a result that writes anything beyond AX is observable through
       the exact write mask below. */
    cpu.ecx = 3u; cpu.edx = 0x40u; cpu.ebx = 0x1234u; cpu.esi = 0u;
    cpu.ebp = token >> 16; cpu.eflags = 0x40u;
    service_window(&window, 0x16u);
    if (!bx_ntvdm_dem_readonly_file_v1_read(&space, &event, &cpu, &window, payload,
            sizeof(payload), &transaction, &result) || transaction.payload_bytes != 0u ||
        transaction.guest_physical_address != 0u || result.cpu_delta.gpr16_write_mask != 1u ||
        result.cpu_delta.gpr16_values[0] != 0u || result.eflags_values != 0u) return 4;

    cpu.eflags = 0u; cpu.ebx = 0u; cpu.esi = 0u;
    if (!bx_ntvdm_dem_readonly_file_v1_read(&space, &event, &cpu, &window, payload,
            sizeof(payload), &transaction, &result) || transaction.payload_bytes != sizeof(command) ||
        memcmp(payload, command, sizeof(command)) != 0 ||
        result.cpu_delta.gpr16_write_mask != 1u ||
        result.cpu_delta.gpr16_values[0] != sizeof(command)) return 5;
    cpu.eax = token & 0xffffu; cpu.ebp = token >> 16; cpu.ebx = 0x1111u;
    cpu.ecx = 0xffffu; cpu.edx = 0xffffu; cpu.esi = 0x2222u;
    service_window(&window, 0x02u);
    if (!bx_ntvdm_dem_readonly_file_v1_close(&space, &event, &cpu, &window, &result) ||
        result.eflags_values != 0u || result.cpu_delta.gpr16_write_mask != 0u ||
        !bx_ntvdm_dem_readonly_file_v1_close(&space, &event, &cpu, &window, &result) ||
        (result.eflags_values & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) == 0u ||
        result.cpu_delta.gpr16_write_mask != 1u || result.cpu_delta.gpr16_values[0] != 6u) return 6;
    service_window(&window, 0x16u); cpu.ecx = 1u; cpu.edx = 0x40u; cpu.eflags = 0u;
    if (!bx_ntvdm_dem_readonly_file_v1_read(&space, &event, &cpu, &window, payload,
            sizeof(payload), &transaction, &result) ||
        transaction.magic != 0u ||
        (result.eflags_values & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) == 0u ||
        result.cpu_delta.gpr16_write_mask != 1u ||
        result.cpu_delta.gpr16_values[0] != 6u) return 7;
    puts("bx-ntvdm DEM readonly file service: bounded O/S/R/C lifecycle verified");
    return 0;
}
