#include "bx_ntvdm_boot_namespace_provider_v1.h"

#include <stdio.h>
#include <string.h>

static void window_for(bx_ntvdm_instruction_window_v1 *window, uint8_t selector, uint8_t service)
{
    const uint8_t bytes[] = { 0xc4u, 0xc4u, selector, service };
    bx_ntvdm_instruction_window_v1_capture(window, bytes, sizeof(bytes));
}

int main(void)
{
    uint8_t command[] = { 0x90u, 0xc3u }, target[] = { 0xf4u }, path[128] = "C:\\CONFIG.SYS";
    uint8_t payload[BX_NTVDM_MULTI_WRITE_MAX_PAYLOAD], read_payload[32];
    byob_image command_image = { command, sizeof(command) }, target_image = { target, sizeof(target) };
    byob_profile_selection selection = { 0 };
    bx_ntvdm_boot_namespace_provider_v1 provider;
    bx_ntvdm_exception_event_v1 event = { BX_NTVDM_EXCEPTION_ABI_MAGIC,
        BX_NTVDM_EXCEPTION_ABI_VERSION, sizeof(event), BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION,
        0u, 6u, 0u, 0u, 0x100u };
    bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_multi_write_transaction_v1 writes;
    bx_ntvdm_guest_read_action_v1 action;
    bx_ntvdm_bulk_result_transaction_v1 bulk;
    bx_ntvdm_cpu_result_v2 result;
    uint32_t token;

    wcscpy(selection.command_placement.path, L"\\COMMAND.COM");
    selection.command_placement.drive_index = 2u; selection.has_command_placement = 1u;
    wcscpy(selection.target_placement.path, L"\\TARGET.COM");
    selection.target_placement.drive_index = 2u; selection.has_target_placement = 1u;
    wcscpy(selection.target.file_name, L"TARGET.COM");
    wcscpy(selection.config_file.path, L"\\CONFIG.SYS");
    selection.config_file.materialization = BYOB_GUEST_BOOT_FILE_MINIMAL_COMMENT_V1;
    wcscpy(selection.autoexec_file.path, L"\\AUTOEXEC.BAT");
    selection.autoexec_file.materialization = BYOB_GUEST_BOOT_FILE_EMPTY_V1;
    selection.has_guest_boot_files = 1u; selection.has_guest_search_metadata = 1u;
    selection.command_metadata.attributes = selection.target_metadata.attributes =
        selection.config_metadata.attributes = selection.autoexec_metadata.attributes = 0x20u;
    selection.command_metadata.dos_date = selection.target_metadata.dos_date =
        selection.config_metadata.dos_date = selection.autoexec_metadata.dos_date = 1u;
    if (!bx_ntvdm_boot_namespace_provider_v1_initialize(&provider, &command_image,
        &target_image, &selection)) return 1;

    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.ds = 0x1000u; cpu.edx = 0x20u;
    window_for(&window, 0x54u, 0x0cu);
    if (!bx_ntvdm_boot_namespace_provider_v1_prepare_boot_file(&provider, &event, &cpu,
        &window, &writes, payload) || writes.writes.write_count != 1u ||
        writes.writes.writes[0].byte_count != sizeof("C:\\CONFIG.SYS")) return 2;

    cpu.esi = 0x40u; window_for(&window, 0x50u, 0x12u);
    if (!bx_ntvdm_boot_namespace_provider_v1_prepare_open(&provider, &event, &cpu,
        &window, &action) || !bx_ntvdm_boot_namespace_provider_v1_complete_open(&provider,
        &event, &cpu, &action, path, sizeof(path), &result) || result.eflags_values != 0u)
        return 3;
    token = ((uint32_t)result.cpu_delta.gpr16_values[5] << 16) | result.cpu_delta.gpr16_values[0];
    cpu.eax = token & 0xffffu; cpu.ebp = token >> 16; cpu.ecx = 20u; cpu.edx = 0x80u;
    cpu.eflags = 0x40u; window_for(&window, 0x50u, 0x16u);
    if (!bx_ntvdm_boot_namespace_provider_v1_read(&provider, &event, &cpu, &window,
        read_payload, sizeof(read_payload), &bulk, &result) || bulk.payload_bytes == 0u ||
        memcmp(read_payload, "REM NTVDM64\r\n", bulk.payload_bytes) != 0) return 4;
    window_for(&window, 0x50u, 0x02u);
    if (!bx_ntvdm_boot_namespace_provider_v1_close(&provider, &event, &cpu, &window,
        &result) || result.eflags_values != 0u) return 5;
    window_for(&window, 0x50u, 0x0au);
    if (bx_ntvdm_boot_namespace_provider_v1_prepare_open(&provider, &event, &cpu, &window,
        &action)) return 6; /* FCB remains outside this provider. */
    puts("bx-ntvdm boot namespace provider: paired config and normal read-only lifecycle verified");
    return 0;
}
