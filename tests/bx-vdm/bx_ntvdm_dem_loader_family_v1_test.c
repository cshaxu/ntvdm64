#include "bx_ntvdm_boot_namespace_plane_v1.h"
#include "bx_ntvdm_bop_ingress_v1.h"
#include "bx_ntvdm_bop_provider_registry_v1.h"
#include "bx_ntvdm_dem_package_session_v1.h"
#include "byob_image.h"
#include "byob_profile.h"
#include "bx-mantle/bx_ntvdm_mechanical_action_v1.h"

#include <string.h>

static uint8_t ram[0x100000];

int bx_ntvdm_mantle_execute_mechanical_action_v1(
    struct bx_ntvdm_mechanical_action_v1 *action)
{
    uint32_t index;
    if (action == 0 || !bx_ntvdm_mechanical_action_v1_valid(action)) return 0;
    for (index = 0u; index < action->range_count; ++index) {
        const struct bx_ntvdm_mechanical_action_v1_range *range =
            &action->ranges[index];
        if (range->physical_address > sizeof(ram) - range->byte_count) return 0;
        if (action->kind == BX_NTVDM_MECHANICAL_ACTION_V1_READ)
            memcpy(action->payload + range->payload_offset,
                ram + range->physical_address, range->byte_count);
        else if (action->kind == BX_NTVDM_MECHANICAL_ACTION_V1_WRITE)
            memcpy(ram + range->physical_address,
                action->payload + range->payload_offset, range->byte_count);
        else return 0;
    }
    return action->kind == BX_NTVDM_MECHANICAL_ACTION_V1_READ ||
        action->kind == BX_NTVDM_MECHANICAL_ACTION_V1_WRITE;
}

static void profile_initialize(byob_profile_selection *profile)
{
    memset(profile, 0, sizeof(*profile));
    memcpy(profile->command_placement.path, L"\\COMMAND.COM",
        sizeof(L"\\COMMAND.COM"));
    profile->command_placement.drive_index = 2u;
    profile->has_command_placement = 1u;
    memcpy(profile->target_placement.path, L"\\TARGET.COM",
        sizeof(L"\\TARGET.COM"));
    profile->target_placement.drive_index = 2u;
    profile->has_target_placement = 1u;
    profile->declared_target_count = 1u;
    memcpy(profile->declared_targets[0].component.file_name, L"TARGET.COM",
        sizeof(L"TARGET.COM"));
    memcpy(profile->declared_targets[0].placement.path, L"\\TARGET.COM",
        sizeof(L"\\TARGET.COM"));
    profile->declared_targets[0].placement.drive_index = 2u;
    memcpy(profile->target.file_name, L"TARGET.COM", sizeof(L"TARGET.COM"));
    memcpy(profile->config_file.path, L"\\CONFIG.SYS", sizeof(L"\\CONFIG.SYS"));
    profile->config_file.materialization = BYOB_GUEST_BOOT_FILE_MINIMAL_COMMENT_V1;
    memcpy(profile->autoexec_file.path, L"\\AUTOEXEC.BAT", sizeof(L"\\AUTOEXEC.BAT"));
    profile->autoexec_file.materialization = BYOB_GUEST_BOOT_FILE_EMPTY_V1;
    profile->has_guest_boot_files = 1u;
    profile->has_guest_search_metadata = 1u;
    profile->command_metadata.attributes = 0x20u;
    profile->target_metadata.attributes = 0x20u;
    profile->command_metadata.dos_date = 1u;
    profile->target_metadata.dos_date = 1u;
    profile->config_metadata.attributes = 0x20u;
    profile->autoexec_metadata.attributes = 0x20u;
    profile->config_metadata.dos_date = 1u;
    profile->autoexec_metadata.dos_date = 1u;
    profile->guest_display_state = BYOB_GUEST_DISPLAY_STATE_STREAM_IO_V1;
    profile->ntdos.bytes = 1u;
    memcpy(profile->ntdos.file_name, L"NTDOS.SYS", sizeof(L"NTDOS.SYS"));
}

static int dispatch(bx_ntvdm_dem_package_session_v1 *session,
    bx_ntvdm_exception_event_v1 *event, bx_ntvdm_cpu_state_v1 *cpu,
    uint8_t service, bx_ntvdm_cpu_result_v2 *result)
{
    uint8_t bytes[4] = { 0xc4u, 0xc4u, 0x50u, service };
    bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_bop_ingress_v1 ingress;
    bx_ntvdm_bop_provider_selection_v1 selection;
    bx_ntvdm_instruction_window_v1_capture(&window, bytes, sizeof(bytes));
    return bx_ntvdm_bop_ingress_v1_classify(&window, &ingress) &&
        bx_ntvdm_bop_provider_registry_v1_select(&ingress, &selection) &&
        bx_ntvdm_dem_package_session_v1_dispatch(session, &ingress, &selection,
            event, cpu, &window, result);
}

int main(void)
{
    uint8_t ntdos_bytes[] = { 0xf4u }, command_bytes[] = { 0xf4u, 0x90u };
    uint8_t target_bytes[] = { 0xf4u };
    byob_image ntdos = { ntdos_bytes, sizeof(ntdos_bytes) };
    byob_image command = { command_bytes, sizeof(command_bytes) };
    byob_image target = { target_bytes, sizeof(target_bytes) };
    byob_profile_selection profile;
    bx_ntvdm_boot_namespace_plane_v1 plane;
    bx_ntvdm_dem_package_session_v1 session;
    bx_ntvdm_exception_event_v1 event;
    bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_cpu_result_v2 result;
    uint32_t token;

    profile_initialize(&profile);
    if (!bx_ntvdm_boot_namespace_plane_v1_initialize(&plane, &ntdos,
            &command, &target, 0, &profile) ||
        !bx_ntvdm_dem_package_session_v1_initialize(&session, &plane)) return 2;
    memcpy(ram + 0x200u, "C:\\COMMAND.COM", sizeof("C:\\COMMAND.COM"));
    memset(&event, 0, sizeof(event));
    event.magic = BX_NTVDM_EXCEPTION_ABI_MAGIC;
    event.abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION;
    event.struct_bytes = sizeof(event);
    event.kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION;
    event.vector = 6u; event.fault_rip = 0x100u;
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.esi = 0x200u;
    if (!dispatch(&session, &event, &cpu, 0x12u, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        result.cpu_delta.gpr16_write_mask != ((1u << 0) | (1u << 1) | (1u << 2) |
            (1u << 3) | (1u << 5)) || result.eflags_values != 0u) return 3;
    token = ((uint32_t)result.cpu_delta.gpr16_values[0] << 16u) |
        result.cpu_delta.gpr16_values[5];

    cpu.eax = token >> 16u; cpu.ebp = token & 0xffffu;
    cpu.ebx = cpu.esi = cpu.ecx = cpu.edx = 0u; cpu.eflags = 0u;
    if (!dispatch(&session, &event, &cpu, 0x00u, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME || result.eflags_values != 0u)
        return 8;
    cpu.ecx = 1u; cpu.edx = 0x300u;
    /* FastOrSlow selects 42h only as an optimization.  The selected
     * source-derived compatibility path must use the same token and advance
     * the same position as the ordinary 16h provider; it may not become a
     * detached file service. */
    if (!dispatch(&session, &event, &cpu, 0x42u, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        result.cpu_delta.gpr16_write_mask != 1u || result.cpu_delta.gpr16_values[0] != 1u ||
        result.eflags_values != 0u || ram[0x300u] != 0xf4u)
        return 4;
    /* `handle.asm` issues `test sf_nt_seek` immediately before each SVC.
     * A clear flag means no requested reposition, so the second read must
     * retain ZF and continue from the provider-owned post-fast-read offset. */
    cpu.eflags = 0x40u;
    if (!dispatch(&session, &event, &cpu, 0x16u, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        result.cpu_delta.gpr16_write_mask != 1u || result.cpu_delta.gpr16_values[0] != 1u ||
        result.eflags_values != 0u || ram[0x300u] != 0x90u) return 5;
    cpu.eax = token >> 16u; cpu.ebp = token & 0xffffu;
    if (!dispatch(&session, &event, &cpu, 0x02u, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME || result.eflags_values != 0u)
        return 6;
    cpu.eax = 0xdead0001u; cpu.ebp = 1u; cpu.ecx = 1u; cpu.edx = 0x300u;
    if (!dispatch(&session, &event, &cpu, 0x42u, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        result.cpu_delta.gpr16_write_mask != 1u || result.cpu_delta.gpr16_values[0] != 6u ||
        result.eflags_values != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) return 7;
    return 0;
}
