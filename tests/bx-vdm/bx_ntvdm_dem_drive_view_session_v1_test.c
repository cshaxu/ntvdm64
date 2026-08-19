#include "bop-v1/bx_ntvdm_boot_namespace_plane_v1.h"
#include "bop-v1/bx_ntvdm_bop_ingress_v1.h"
#include "bop-v1/bx_ntvdm_bop_provider_registry_v1.h"
#include "bop-v1/bx_ntvdm_dem_package_session_v1.h"
#include "bx_ntvdm_host_namespace.h"
#include "bop-v1/bx_ntvdm_host_volume_snapshot_v1.h"

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
    memcpy(profile->command_placement.path, L"\\COMMAND.COM", sizeof(L"\\COMMAND.COM"));
    profile->command_placement.drive_index = 2u;
    profile->has_command_placement = 1u;
    memcpy(profile->target_placement.path, L"\\TARGET.COM", sizeof(L"\\TARGET.COM"));
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
    profile->guest_display_state = BYOB_GUEST_DISPLAY_STATE_STREAM_IO_V1;
    profile->ntdos.bytes = 1u;
    memcpy(profile->ntdos.file_name, L"NTDOS.SYS", sizeof(L"NTDOS.SYS"));
}

static int dispatch(bx_ntvdm_dem_package_session_v1 *session, uint8_t service,
    bx_ntvdm_cpu_state_v1 *cpu, bx_ntvdm_cpu_result_v2 *result)
{
    uint8_t bytes[4] = { 0xc4u, 0xc4u, 0x50u, service };
    bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_bop_ingress_v1 ingress;
    bx_ntvdm_bop_provider_selection_v1 selection;
    bx_ntvdm_exception_event_v1 event;
    memset(&event, 0, sizeof(event));
    event.magic = BX_NTVDM_EXCEPTION_ABI_MAGIC;
    event.abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION;
    event.struct_bytes = sizeof(event);
    event.kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION;
    event.cpu_id = 1u;
    event.vector = 6u;
    event.fault_rip = 0x100u;
    bx_ntvdm_instruction_window_v1_capture(&window, bytes, sizeof(bytes));
    return bx_ntvdm_bop_ingress_v1_classify(&window, &ingress) &&
        bx_ntvdm_bop_provider_registry_v1_select(&ingress, &selection) &&
        bx_ntvdm_dem_package_session_v1_dispatch(session, &ingress, &selection,
            &event, cpu, &window, result);
}

static int success(const bx_ntvdm_cpu_result_v2 *result)
{
    return result->disposition == BX_NTVDM_CPU_RESULT_V2_RESUME &&
        result->resume_rip == 0x104u &&
        result->eflags_write_mask == BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF &&
        result->eflags_values == 0u;
}

static int access_denied(const bx_ntvdm_cpu_result_v2 *result)
{
    return result->disposition == BX_NTVDM_CPU_RESULT_V2_RESUME &&
        result->resume_rip == 0x104u &&
        result->cpu_delta.gpr16_write_mask == 1u &&
        result->cpu_delta.gpr16_values[0] == 5u &&
        result->eflags_write_mask == BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF &&
        result->eflags_values == BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF;
}

static int initialize_session(uint32_t mode,
    bx_ntvdm_boot_namespace_plane_v1 *plane,
    bx_ntvdm_dem_package_session_v1 *session,
    const bx_ntvdm_host_drive_snapshot_v1 *drives,
    const bx_ntvdm_host_volume_snapshot_v1 *volumes,
    const bx_ntvdm_host_namespace_v1 *host,
    const byob_image *ntdos, const byob_image *command,
    const byob_image *target, const byob_profile_selection *profile)
{
    bx_ntvdm_mutation_profile_v1 mutation;
    bx_ntvdm_mutation_profile_v1_initialize(&mutation, mode);
    return bx_ntvdm_dem_profile_consumer_v1_register_class(&mutation,
        BX_NTVDM_MUTATION_CLASS_V1_SESSION_CONTEXT, 0x0fu) &&
        bx_ntvdm_dem_profile_consumer_v1_register_class(&mutation,
        BX_NTVDM_MUTATION_CLASS_V1_HOST_GLOBAL, 0x03u) &&
        bx_ntvdm_boot_namespace_plane_v1_initialize(plane, ntdos, command,
            target, 0, profile) &&
        bx_ntvdm_dem_package_session_v1_initialize(session, plane) &&
        bx_ntvdm_dem_package_session_v1_set_mutation_profile(session, &mutation) &&
        bx_ntvdm_dem_package_session_v1_set_drive_snapshot(session, drives) &&
        bx_ntvdm_dem_package_session_v1_set_volume_snapshot(session, volumes) &&
        bx_ntvdm_dem_package_session_v1_set_boot_drive(session, 2u) &&
        bx_ntvdm_dem_package_session_v1_set_drive_view_host_namespace(session, host) &&
        session->has_whole_provider == 0u && session->drive_view_host_namespace == host;
}

static int exercise(uint32_t mode, const bx_ntvdm_host_drive_snapshot_v1 *drives,
    const bx_ntvdm_host_volume_snapshot_v1 *volumes,
    const bx_ntvdm_host_namespace_v1 *host, const byob_image *ntdos,
    const byob_image *command, const byob_image *target,
    const byob_profile_selection *profile)
{
    bx_ntvdm_boot_namespace_plane_v1 plane;
    bx_ntvdm_dem_package_session_v1 session;
    bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_cpu_result_v2 result;
    int readonly = mode == BX_NTVDM_MUTATION_MODE_V1_READONLY;
    memset(ram, 0, sizeof(ram));
    if (!initialize_session(mode, &plane, &session, drives, volumes, host, ntdos, command,
            target, profile)) return 1;

    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    if (!dispatch(&session, 0x0du, &cpu, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        result.resume_rip != 0x104u ||
        (result.cpu_delta.gpr16_write_mask & 1u) == 0u ||
        (result.cpu_delta.gpr16_values[0] & 0xffu) != 3u) {
        bx_ntvdm_dem_package_session_v1_teardown(&session); return 2;
    }
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.eax = 2u;
    if (!dispatch(&session, 0x0eu, &cpu, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        result.resume_rip != 0x104u || result.cpu_delta.gpr16_write_mask != 0x1fu ||
        result.eflags_write_mask != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
        result.eflags_values != 0u) {
        bx_ntvdm_dem_package_session_v1_teardown(&session); return 6;
    }
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    if (!dispatch(&session, 0x0fu, &cpu, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        result.resume_rip != 0x104u || result.cpu_delta.gpr16_write_mask != 1u ||
        result.cpu_delta.gpr16_values[0] != 3u ||
        result.eflags_write_mask != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
        result.eflags_values != 0u) {
        bx_ntvdm_dem_package_session_v1_teardown(&session); return 7;
    }
    /* demGSetMediaID get retains its original VOLINFO layout: two caller
     * bytes stay untouched, then the provider writes serial, label and FS
     * through checked guest-RAM ranges.  Set remains the original CF-only
     * unsupported volume mutation under both Direct and Readonly. */
    memset(ram + 0x180u, 0xa5, 25u);
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.eax = 0u; cpu.ebx = 2u; cpu.ds = 0u; cpu.edx = 0x180u;
    if (!dispatch(&session, 0x10u, &cpu, &result) || !success(&result) ||
        ram[0x180u] != 0xa5u || ram[0x181u] != 0xa5u ||
        ram[0x182u] != (uint8_t)volumes->volumes[2u].serial_number ||
        ram[0x183u] != (uint8_t)(volumes->volumes[2u].serial_number >> 8) ||
        ram[0x184u] != (uint8_t)(volumes->volumes[2u].serial_number >> 16) ||
        ram[0x185u] != (uint8_t)(volumes->volumes[2u].serial_number >> 24) ||
        ram[0x198u] == 0xa5u) {
        bx_ntvdm_dem_package_session_v1_teardown(&session); return 8;
    }
    memset(ram + 0x180u, 0xa5, 25u);
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.eax = 1u; cpu.ebx = 2u; cpu.ds = 0u; cpu.edx = 0x180u;
    if (!dispatch(&session, 0x10u, &cpu, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME || result.resume_rip != 0x104u ||
        result.cpu_delta.gpr16_write_mask != 0u ||
        result.eflags_write_mask != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
        result.eflags_values != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
        ram[0x180u] != 0xa5u || ram[0x198u] != 0xa5u) {
        bx_ntvdm_dem_package_session_v1_teardown(&session); return 9;
    }
    /* Date/time queries are profile-neutral observations. Setters use invalid
     * input so no host clock changes; Direct retains either original AL=00
     * privilege fallback or AL=ff validation failure, while Readonly is ff. */
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.eax = 0xa500u;
    if (!dispatch(&session, 0x14u, &cpu, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME || result.resume_rip != 0x104u ||
        result.cpu_delta.gpr16_write_mask != ((1u << 0) | (1u << 2) | (1u << 3)) ||
        (result.cpu_delta.gpr16_values[0] & 0xff00u) != 0xa500u ||
        (result.cpu_delta.gpr16_values[0] & 0xffu) > 6u ||
        result.cpu_delta.gpr16_values[2] < 1980u ||
        (result.cpu_delta.gpr16_values[3] & 0xff00u) == 0u ||
        (result.cpu_delta.gpr16_values[3] & 0xff00u) > 0x0c00u ||
        (result.cpu_delta.gpr16_values[3] & 0xffu) == 0u ||
        result.eflags_write_mask != 0u) {
        bx_ntvdm_dem_package_session_v1_teardown(&session); return 10;
    }
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.eax = 0x5a00u;
    if (!dispatch(&session, 0x15u, &cpu, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME || result.resume_rip != 0x104u ||
        result.cpu_delta.gpr16_write_mask != ((1u << 0) | (1u << 2) | (1u << 3)) ||
        result.cpu_delta.gpr16_values[0] != 0x5a00u ||
        (result.cpu_delta.gpr16_values[2] & 0xff00u) > 0x1700u ||
        (result.cpu_delta.gpr16_values[2] & 0xffu) > 59u ||
        (result.cpu_delta.gpr16_values[3] & 0xff00u) > 0x3b00u ||
        (result.cpu_delta.gpr16_values[3] & 0xffu) != 0u ||
        result.eflags_write_mask != 0u) {
        bx_ntvdm_dem_package_session_v1_teardown(&session); return 11;
    }
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.eax = 0xa500u; cpu.ecx = cpu.edx = 0u;
    if (!dispatch(&session, 0x19u, &cpu, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME || result.resume_rip != 0x104u ||
        result.cpu_delta.gpr16_write_mask != 1u ||
        (readonly ? result.cpu_delta.gpr16_values[0] != 0xa5ffu :
         (result.cpu_delta.gpr16_values[0] != 0xa500u && result.cpu_delta.gpr16_values[0] != 0xa5ffu)) ||
        result.eflags_write_mask != 0u) {
        bx_ntvdm_dem_package_session_v1_teardown(&session); return 12;
    }
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.eax = 0x5a00u; cpu.ecx = cpu.edx = 0u;
    if (!dispatch(&session, 0x1cu, &cpu, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME || result.resume_rip != 0x104u ||
        result.cpu_delta.gpr16_write_mask != 1u ||
        (readonly ? result.cpu_delta.gpr16_values[0] != 0x5affu :
         (result.cpu_delta.gpr16_values[0] != 0x5a00u && result.cpu_delta.gpr16_values[0] != 0x5affu)) ||
        result.eflags_write_mask != 0u) {
        bx_ntvdm_dem_package_session_v1_teardown(&session); return 13;
    }    /* The DPB owner consumes the same admitted immutable volume inventory.
     * Validate its full 35-byte output, its excluded-drive terminal, and the
     * DPB-list chain as one package family rather than independent leaves. */
    memset(ram + 0x280u, 0xa5, 35u);
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.eax = 2u; cpu.ds = 0u; cpu.edi = 0x280u;
    if (!dispatch(&session, 0x25u, &cpu, &result) || !success(&result) ||
        result.cpu_delta.gpr16_write_mask != 1u || result.cpu_delta.gpr16_values[0] != 0u ||
        ram[0x280u] != 2u || ram[0x281u] != 2u ||
        ram[0x282u] == 0xa5u || ram[0x283u] == 0xa5u ||
        ram[0x28au] != 2u || ram[0x28du] != 0xa5u || ram[0x28eu] != 0xa5u ||
        ram[0x299u] != 0xf8u || ram[0x29au] != 10u ||
        ram[0x29fu] != 0xa5u || ram[0x2a0u] != 0xa5u) {
        bx_ntvdm_dem_package_session_v1_teardown(&session); return 14;
    }
    memset(ram + 0x280u, 0xa5, 35u);
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.eax = 3u; cpu.ds = 0u; cpu.edi = 0x280u;
    if (!dispatch(&session, 0x25u, &cpu, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME || result.resume_rip != 0x104u ||
        result.cpu_delta.gpr16_write_mask != 1u || result.cpu_delta.gpr16_values[0] != 15u ||
        result.eflags_write_mask != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
        result.eflags_values != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF || ram[0x280u] != 0xa5u ||
        ram[0x2a2u] != 0xa5u) {
        bx_ntvdm_dem_package_session_v1_teardown(&session); return 15;
    }
    memset(ram + 0x340u, 0xa5, 71u);
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.es = 0u; cpu.ebp = 0x340u;
    if (!dispatch(&session, 0x46u, &cpu, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME || result.resume_rip != 0x104u ||
        result.cpu_delta.gpr16_write_mask != (1u << 5) ||
        result.cpu_delta.gpr16_values[5] != 0x363u || result.eflags_write_mask != 0u ||
        ram[0x340u] != 2u || ram[0x341u] != 2u ||
        ram[0x35bu] != 0xffu || ram[0x35cu] != 0xffu ||
        ram[0x35du] != 0xffu || ram[0x35eu] != 0xffu || ram[0x363u] != 0xa5u) {
        bx_ntvdm_dem_package_session_v1_teardown(&session); return 16;
    }
    /* OpenNT's OEM host-name capability always writes a fixed 16-byte guest
     * field on success and marks CX=01FF; profile mode does not alter identity
     * observation. */
    memset(ram + 0x3c0u, 0xa5, 17u);
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.ds = 0u; cpu.edx = 0x3c0u; cpu.ecx = 0xa5b7u;
    if (!dispatch(&session, 0x41u, &cpu, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME || result.resume_rip != 0x104u ||
        result.cpu_delta.gpr16_write_mask != (1u << 2) ||
        result.cpu_delta.gpr16_values[2] != 0x01ffu || result.eflags_write_mask != 0u ||
        ram[0x3c0u] == 0u || ram[0x3cfu] != 0u || ram[0x3d0u] != 0xa5u) {
        bx_ntvdm_dem_package_session_v1_teardown(&session); return 18;
    }
    memset(ram + 0x120u, 0xa5, 71u);
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.ds = 0u; cpu.esi = 0x120u; cpu.eax = 2u;
    if (!dispatch(&session, 0x13u, &cpu, &result) || !success(&result) ||
        memcmp(ram + 0x120u, "C:\\", 3u) != 0 || ram[0x120u + 69u] != 2u ||
        ram[0x120u + 70u] != 0u) { bx_ntvdm_dem_package_session_v1_teardown(&session); return 2; }

    memcpy(ram + 0x120u, "C:\\", 4u);
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.ds = 0u; cpu.edx = 0x120u;
    if (!dispatch(&session, 0x18u, &cpu, &result) ||
        !(readonly ? access_denied(&result) : success(&result))) { bx_ntvdm_dem_package_session_v1_teardown(&session); return 3; }

    memcpy(ram + 0x120u, "C:\\", 4u);
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.ds = 0u; cpu.esi = 0x120u; cpu.edx = 2u;
    if (!dispatch(&session, 0x1au, &cpu, &result) ||
        !(readonly ? access_denied(&result) : success(&result))) { bx_ntvdm_dem_package_session_v1_teardown(&session); return 4; }
    ram[0x220u] = 0x00u; ram[0x221u] = 0x06u;
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.ds = 0u; cpu.esi = 0x200u; cpu.eax = 0x300u; cpu.edx = 0x400u;
    cpu.ecx = 0x500u;
    if (!dispatch(&session, 0x1bu, &cpu, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME || result.resume_rip != 0x104u ||
        !plane.has_dta || plane.dta.dta_location != 0x300u ||
        plane.dta.current_pdb != 0x400u || plane.dta.extended_error != 0x500u ||
        plane.dta.sft_head != 0x600u) { bx_ntvdm_dem_package_session_v1_teardown(&session); return 5; }    bx_ntvdm_dem_package_session_v1_teardown(&session);
    return 0;
}

int main(void)
{
    uint8_t ntdos_bytes[] = { 0xf4u }, command_bytes[] = { 0xf4u }, target_bytes[] = { 0xf4u };
    uint8_t drive_types[26] = { 0 };
    byob_image ntdos = { ntdos_bytes, sizeof(ntdos_bytes) };
    byob_image command = { command_bytes, sizeof(command_bytes) };
    byob_image target = { target_bytes, sizeof(target_bytes) };
    byob_profile_selection profile;
    bx_ntvdm_host_drive_snapshot_v1 drives;
    bx_ntvdm_host_volume_snapshot_v1 volumes;
    bx_ntvdm_host_namespace_v1 host;
    wchar_t original[MAX_PATH];
    DWORD original_bytes, type;
    int exercise_result;
    profile_initialize(&profile);
    type = GetDriveTypeW(L"C:\\");
    original_bytes = GetCurrentDirectoryW(MAX_PATH, original);
    if (type == DRIVE_NO_ROOT_DIR || type == DRIVE_UNKNOWN ||
        original_bytes == 0u || original_bytes >= MAX_PATH) return 1;
    drive_types[2] = (uint8_t)type;
    if (!bx_ntvdm_host_drive_snapshot_v1_apply(UINT32_C(1) << 2u, drive_types,
            0u, 0u, &drives) || !bx_ntvdm_host_volume_snapshot_v1_capture(&drives, &volumes) ||
        !bx_ntvdm_host_namespace_v1_initialize(&host, &drives)) return 2;
    exercise_result = exercise(BX_NTVDM_MUTATION_MODE_V1_DIRECT, &drives, &volumes, &host,
        &ntdos, &command, &target, &profile);
    if (exercise_result == 0 && !SetCurrentDirectoryW(original)) exercise_result = 17;
    if (exercise_result == 0) exercise_result = exercise(BX_NTVDM_MUTATION_MODE_V1_READONLY,
        &drives, &volumes, &host, &ntdos, &command, &target, &profile);
    if (exercise_result != 0) {
        SetCurrentDirectoryW(original);
        bx_ntvdm_host_namespace_v1_release(&host);
        return exercise_result;
    }
    bx_ntvdm_host_namespace_v1_release(&host);
    return 0;
}
