#include "bx_ntvdm_boot_namespace_plane_v1.h"
#include <string.h>

static void window_capture(bx_ntvdm_instruction_window_v1 *window,
    uint8_t selector, uint8_t service)
{
    uint8_t bytes[] = { 0xc4, 0xc4, selector, service };
    bx_ntvdm_instruction_window_v1_capture(window, bytes, sizeof(bytes));
}

static int classify_and_select(bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_bop_ingress_v1 *ingress,
    bx_ntvdm_bop_provider_selection_v1 *selection)
{
    return bx_ntvdm_bop_ingress_v1_classify(window, ingress) &&
        bx_ntvdm_bop_provider_registry_v1_select(ingress, selection);
}

static int replay_dta_write(const struct bx_ntvdm_mechanical_action_v1 *action,
    uint64_t dta_address, uint8_t dta[43])
{
    uint32_t index;
    uint64_t begin;
    uint64_t end;
    uint64_t range_end;

    if (!action || !dta || action->kind != BX_NTVDM_MECHANICAL_ACTION_V1_WRITE ||
        !bx_ntvdm_mechanical_action_v1_valid(action)) return 0;
    begin = dta_address;
    end = begin + 43u;
    for (index = 0; index < action->range_count; ++index) {
        range_end = action->ranges[index].physical_address + action->ranges[index].byte_count;
        if (action->ranges[index].physical_address < begin || range_end > end) continue;
        memcpy(dta + action->ranges[index].physical_address - begin,
            action->payload + action->ranges[index].payload_offset,
            action->ranges[index].byte_count);
    }
    return 1;
}

int main(void)
{
    uint8_t command_bytes[] = { 0x90, 0xc3 };
    uint8_t target_bytes[] = { 0xf4 };
    uint8_t dta_bytes[43];
    byob_image command = { command_bytes, sizeof(command_bytes) };
    byob_image target = { target_bytes, sizeof(target_bytes) };
    byob_profile_selection profile = { 0 };
    bx_ntvdm_boot_namespace_plane_v1 plane;
    bx_ntvdm_dem_dta_registration_v1 dta = { 0x200u, 0x300u, 0x400u, 0x500u };
    bx_ntvdm_exception_event_v1 event = {
        BX_NTVDM_EXCEPTION_ABI_MAGIC, 1, sizeof(event), 1, 0, 6, 0, 0, 0x100
    };
    bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_bop_ingress_v1 ingress;
    bx_ntvdm_bop_provider_selection_v1 selection;
    struct bx_ntvdm_mechanical_action_v1 action;
    struct bx_ntvdm_mechanical_action_v1 next;
    bx_ntvdm_cpu_result_v2 result;
    uint32_t token;
    uint32_t action_id;

    wcscpy(profile.command_placement.path, L"\\COMMAND.COM");
    profile.command_placement.drive_index = 2;
    profile.has_command_placement = 1;
    wcscpy(profile.target_placement.path, L"\\TARGET.COM");
    profile.target_placement.drive_index = 2;
    profile.has_target_placement = 1;
    wcscpy(profile.target.file_name, L"TARGET.COM");
    wcscpy(profile.config_file.path, L"\\CONFIG.SYS");
    profile.config_file.materialization = BYOB_GUEST_BOOT_FILE_MINIMAL_COMMENT_V1;
    wcscpy(profile.autoexec_file.path, L"\\AUTOEXEC.BAT");
    profile.autoexec_file.materialization = BYOB_GUEST_BOOT_FILE_EMPTY_V1;
    profile.has_guest_boot_files = 1;
    profile.has_guest_search_metadata = 1;
    profile.command_metadata.attributes = profile.target_metadata.attributes =
        profile.config_metadata.attributes = profile.autoexec_metadata.attributes = 0x20u;
    profile.command_metadata.dos_date = profile.target_metadata.dos_date =
        profile.config_metadata.dos_date = profile.autoexec_metadata.dos_date = 1u;

    if (!bx_ntvdm_boot_namespace_plane_v1_initialize(
            &plane, 0, &command, &target, 0, &profile)) return 1;
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.ds = 0x1000;
    cpu.edx = 0x20;
    window_capture(&window, 0x54, 0x0c);
    if (!classify_and_select(&window, &ingress, &selection) ||
        !bx_ntvdm_boot_namespace_plane_v1_dispatch(&plane, &ingress, &selection,
            &event, &cpu, &window, &action, &result) ||
        action.kind != BX_NTVDM_MECHANICAL_ACTION_V1_WRITE) return 2;
    window_capture(&window, 0x54, 0x0d);
    if (!classify_and_select(&window, &ingress, &selection) ||
        !bx_ntvdm_boot_namespace_plane_v1_dispatch(&plane, &ingress, &selection,
            &event, &cpu, &window, &action, &result) ||
        action.kind != BX_NTVDM_MECHANICAL_ACTION_V1_WRITE) return 3;

    cpu.esi = 0x40;
    window_capture(&window, 0x50, 0x12);
    if (!classify_and_select(&window, &ingress, &selection)) return 30;
    if (!bx_ntvdm_boot_namespace_plane_v1_dispatch(&plane, &ingress, &selection,
            &event, &cpu, &window, &action, &result)) return 31;
    if (action.kind != BX_NTVDM_MECHANICAL_ACTION_V1_READ) return 32;
    if (action.payload_bytes != 128) return 33;
    memcpy(action.payload, "C:\\CONFIG.SYS", 14);
    if (!bx_ntvdm_boot_namespace_plane_v1_complete_read(&plane, &action, &next,
            &result) || result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        next.kind != BX_NTVDM_MECHANICAL_ACTION_V1_NONE) return 4;
    token = ((uint32_t)result.cpu_delta.gpr16_values[0] << 16) |
        result.cpu_delta.gpr16_values[5];

    cpu.eax = token >> 16;
    cpu.ebp = token & 0xffffu;
    cpu.ecx = 20;
    cpu.edx = 0x80;
    cpu.eflags = 0x40;
    window_capture(&window, 0x50, 0x16);
    if (!classify_and_select(&window, &ingress, &selection) ||
        !bx_ntvdm_boot_namespace_plane_v1_dispatch(&plane, &ingress, &selection,
            &event, &cpu, &window, &action, &result) ||
        action.kind != BX_NTVDM_MECHANICAL_ACTION_V1_WRITE || !action.payload_bytes) return 5;

    /* 50:42 is the separately admitted fast-read contract.  ZF clear uses
     * the copied BX:SI position and must produce the existing namespace bulk
     * write, rather than falling through as raw media or opening FASTWRITE. */
    cpu.ebx = 0u;
    cpu.esi = 0u;
    cpu.ecx = 3u;
    cpu.edx = 0x90u;
    cpu.eflags = 0u;
    window_capture(&window, 0x50, 0x42);
    if (!classify_and_select(&window, &ingress, &selection) ||
        !bx_ntvdm_boot_namespace_plane_v1_dispatch(&plane, &ingress, &selection,
            &event, &cpu, &window, &action, &result) ||
        action.kind != BX_NTVDM_MECHANICAL_ACTION_V1_WRITE || action.payload_bytes != 3u ||
        memcmp(action.payload, "REM", 3u) != 0 ||
        result.cpu_delta.gpr16_write_mask != 1u || result.cpu_delta.gpr16_values[0] != 3u) return 16;
    window_capture(&window, 0x50, 0x43);
    if (!classify_and_select(&window, &ingress, &selection) ||
        !bx_ntvdm_boot_namespace_plane_v1_dispatch(&plane, &ingress, &selection,
            &event, &cpu, &window, &action, &result) ||
        action.kind != BX_NTVDM_MECHANICAL_ACTION_V1_NONE ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_PASS_THROUGH) return 17;

    cpu.ebx = 0;
    cpu.ecx = 0;
    cpu.edx = 0;
    window_capture(&window, 0x50, 0x00);
    if (!classify_and_select(&window, &ingress, &selection) ||
        !bx_ntvdm_boot_namespace_plane_v1_dispatch(&plane, &ingress, &selection,
            &event, &cpu, &window, &action, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME) return 6;
    window_capture(&window, 0x50, 0x02);
    if (!classify_and_select(&window, &ingress, &selection) ||
        !bx_ntvdm_boot_namespace_plane_v1_dispatch(&plane, &ingress, &selection,
            &event, &cpu, &window, &action, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME) return 7;

    cpu.ds = 0x1000;
    cpu.edx = 0x20;
    cpu.ecx = 0;
    window_capture(&window, 0x50, 0x09);
    if (!classify_and_select(&window, &ingress, &selection) ||
        !bx_ntvdm_boot_namespace_plane_v1_dispatch(&plane, &ingress, &selection,
            &event, &cpu, &window, &action, &result) ||
        action.kind != BX_NTVDM_MECHANICAL_ACTION_V1_NONE ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_PASS_THROUGH) return 8;
    if (!bx_ntvdm_boot_namespace_plane_v1_set_dta(&plane, &dta)) return 9;
    if (!classify_and_select(&window, &ingress, &selection) ||
        !bx_ntvdm_boot_namespace_plane_v1_dispatch(&plane, &ingress, &selection,
            &event, &cpu, &window, &action, &result) ||
        action.kind != BX_NTVDM_MECHANICAL_ACTION_V1_READ || action.payload_bytes != 134) return 10;
    memcpy(action.payload, "C:\\*.*", 6);
    action.payload[130] = 0x20;
    if (!bx_ntvdm_boot_namespace_plane_v1_complete_read(&plane, &action, &next,
            &result) || next.kind != BX_NTVDM_MECHANICAL_ACTION_V1_WRITE ||
        !next.payload_bytes) return 11;
    memset(dta_bytes, 0, sizeof(dta_bytes));
    if (!replay_dta_write(&next, dta.dta_location, dta_bytes)) return 12;

    window_capture(&window, 0x50, 0x0b);
    if (!classify_and_select(&window, &ingress, &selection) ||
        !bx_ntvdm_boot_namespace_plane_v1_dispatch(&plane, &ingress, &selection,
            &event, &cpu, &window, &action, &result) ||
        action.kind != BX_NTVDM_MECHANICAL_ACTION_V1_READ || action.payload_bytes != 45) return 13;
    action_id = action.action_id;
    action.action_id++;
    if (bx_ntvdm_boot_namespace_plane_v1_complete_read(&plane, &action, &next,
            &result)) return 14;
    action.action_id = action_id;
    memcpy(action.payload, dta_bytes, sizeof(dta_bytes));
    if (!bx_ntvdm_boot_namespace_plane_v1_complete_read(&plane, &action, &next,
            &result) || next.kind != BX_NTVDM_MECHANICAL_ACTION_V1_WRITE ||
        !next.payload_bytes) return 15;
    return 0;
}
