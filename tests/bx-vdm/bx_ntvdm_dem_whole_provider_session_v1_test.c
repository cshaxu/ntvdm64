#include "bx_ntvdm_boot_namespace_plane_v1.h"
#include "bx_ntvdm_bop_ingress_v1.h"
#include "bx_ntvdm_bop_provider_registry_v1.h"
#include "bx_ntvdm_dem_package_session_v1.h"
#include "bx_ntvdm_host_namespace.h"

#include <string.h>

/* Integration seam only: proves normal BOP ingress reaches the one installed
 * whole-provider session.  It deliberately does not reuse the unrelated
 * historical 73-service/GSET fixture. */
static uint8_t ram[0x100000];

int bx_ntvdm_mantle_execute_mechanical_action_v1(
    struct bx_ntvdm_mechanical_action_v1 *action)
{
    uint32_t index;
    if (!action || !bx_ntvdm_mechanical_action_v1_valid(action)) return 0;
    for (index = 0; index < action->range_count; ++index) {
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
    memcpy(profile->command_placement.path, L"\\COMMAND.COM", sizeof(L"\\COMMAND.COM"));
    profile->command_placement.drive_index = 2u; profile->has_command_placement = 1u;
    memcpy(profile->target_placement.path, L"\\TARGET.COM", sizeof(L"\\TARGET.COM"));
    profile->target_placement.drive_index = 2u; profile->has_target_placement = 1u;
    memcpy(profile->target.file_name, L"TARGET.COM", sizeof(L"TARGET.COM"));
    memcpy(profile->config_file.path, L"\\CONFIG.SYS", sizeof(L"\\CONFIG.SYS"));
    profile->config_file.materialization = BYOB_GUEST_BOOT_FILE_MINIMAL_COMMENT_V1;
    memcpy(profile->autoexec_file.path, L"\\AUTOEXEC.BAT", sizeof(L"\\AUTOEXEC.BAT"));
    profile->autoexec_file.materialization = BYOB_GUEST_BOOT_FILE_EMPTY_V1;
    profile->has_guest_boot_files = 1u; profile->has_guest_search_metadata = 1u;
    profile->guest_display_state = BYOB_GUEST_DISPLAY_STATE_STREAM_IO_V1;
    profile->ntdos.bytes = 1u;
    memcpy(profile->ntdos.file_name, L"NTDOS.SYS", sizeof(L"NTDOS.SYS"));
}

static int dispatch(bx_ntvdm_dem_package_session_v1 *session, uint8_t service,
    bx_ntvdm_cpu_state_v1 *cpu, bx_ntvdm_cpu_result_v2 *result)
{
    uint8_t bytes[4] = { 0xc4u, 0xc4u, 0x50u, service };
    bx_ntvdm_instruction_window_v1 window; bx_ntvdm_bop_ingress_v1 ingress;
    bx_ntvdm_bop_provider_selection_v1 selection; bx_ntvdm_exception_event_v1 event;
    memset(&event, 0, sizeof(event)); event.magic = BX_NTVDM_EXCEPTION_ABI_MAGIC;
    event.abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION; event.struct_bytes = sizeof(event);
    event.kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION; event.cpu_id = 1u;
    event.vector = 6u; event.fault_rip = 0x100u;
    bx_ntvdm_instruction_window_v1_capture(&window, bytes, sizeof(bytes));
    return bx_ntvdm_bop_ingress_v1_classify(&window, &ingress) &&
        bx_ntvdm_bop_provider_registry_v1_select(&ingress, &selection) &&
        bx_ntvdm_dem_package_session_v1_dispatch(session, &ingress, &selection,
            &event, cpu, &window, result);
}

static int cf_ax(const bx_ntvdm_cpu_result_v2 *result, uint16_t ax)
{
    return result->disposition == BX_NTVDM_CPU_RESULT_V2_RESUME &&
        result->resume_rip == 0x104u && result->cpu_delta.gpr16_write_mask == 1u &&
        result->cpu_delta.gpr16_values[0] == ax &&
        result->eflags_write_mask == BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF &&
        result->eflags_values == BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF;
}

int main(void)
{
    uint8_t ntdos_bytes[] = { 0xf4u }, command_bytes[] = { 0xf4u }, target_bytes[] = { 0xf4u };
    uint8_t drive_types[26] = {0}; byob_image ntdos = { ntdos_bytes, sizeof(ntdos_bytes) };
    byob_image command = { command_bytes, sizeof(command_bytes) }, target = { target_bytes, sizeof(target_bytes) };
    byob_profile_selection profile; bx_ntvdm_boot_namespace_plane_v1 plane;
    bx_ntvdm_dem_package_session_v1 session; bx_ntvdm_mutation_profile_v1 mutation;
    bx_ntvdm_host_drive_snapshot_v1 drives; bx_ntvdm_host_namespace_v1 host;
    bx_ntvdm_cpu_state_v1 cpu; bx_ntvdm_cpu_result_v2 result; wchar_t root[4] = L"C:\\";
    DWORD type;
    profile_initialize(&profile); type = GetDriveTypeW(root);
    if (type == DRIVE_NO_ROOT_DIR || type == DRIVE_UNKNOWN) return 1;
    drive_types[2] = (uint8_t)type;
    bx_ntvdm_mutation_profile_v1_initialize(&mutation, BX_NTVDM_MUTATION_MODE_V1_OVERLAY);
    if (!bx_ntvdm_dem_profile_consumer_v1_register_class(&mutation, BX_NTVDM_MUTATION_CLASS_V1_SESSION_CONTEXT, 0x0fu) ||
        !bx_ntvdm_dem_profile_consumer_v1_register_class(&mutation, BX_NTVDM_MUTATION_CLASS_V1_NAMESPACE_CONTENT, 0x0fu) ||
        !bx_ntvdm_dem_profile_consumer_v1_register_class(&mutation, BX_NTVDM_MUTATION_CLASS_V1_FILE_METADATA, 0x0fu) ||
        !bx_ntvdm_host_drive_snapshot_v1_apply(1u << 2u, drive_types, 0u, 0u, &drives) ||
        !bx_ntvdm_host_namespace_v1_initialize(&host, &drives) ||
        !bx_ntvdm_boot_namespace_plane_v1_initialize(&plane, &ntdos, &command, &target, 0, &profile) ||
        !bx_ntvdm_dem_package_session_v1_initialize(&session, &plane) ||
        !bx_ntvdm_dem_package_session_v1_set_mutation_profile(&session, &mutation) ||
        !bx_ntvdm_dem_package_session_v1_set_drive_snapshot(&session, &drives) ||
        !bx_ntvdm_dem_package_session_v1_set_host_namespace(&session, &host) || !session.has_whole_provider) return 2;
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    /* 00's invalid opaque token is handled by the installed file provider (6),
     * differentiating it from generic CLI unavailable (5). */
    if (!dispatch(&session, 0x00u, &cpu, &result) || !cf_ax(&result, 6u)) return 3;
    /* 47/48 remain explicit Redirector deferment, not file-provider fallbacks. */
    if (!dispatch(&session, 0x47u, &cpu, &result) || !cf_ax(&result, 6u) ||
        !dispatch(&session, 0x48u, &cpu, &result) || !cf_ax(&result, 6u)) return 4;
    bx_ntvdm_dem_package_session_v1_teardown(&session); bx_ntvdm_host_namespace_v1_release(&host);
    return 0;
}
