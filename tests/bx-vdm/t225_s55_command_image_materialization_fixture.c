#include "bx_ntvdm_bop_ingress_v1.h"
#include "bx_ntvdm_bop_provider_registry_v1.h"
#include "bx_ntvdm_dem_package_session_v1.h"
#include "bx_ntvdm_host_namespace.h"

#include <string.h>

#define T225_S55_COMMAND_BYTES 50384u
#define T225_S55_RAM_BYTES 0x100000u

static uint8_t ram[T225_S55_RAM_BYTES];

int bx_ntvdm_mantle_execute_mechanical_action_v1(
    struct bx_ntvdm_mechanical_action_v1 *action)
{
    uint32_t index;
    if (!action || !bx_ntvdm_mechanical_action_v1_valid(action)) return 0;
    for (index = 0u; index < action->range_count; ++index) {
        const struct bx_ntvdm_mechanical_action_v1_range *range = &action->ranges[index];
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
    memcpy(profile->target.file_name, L"TARGET.COM", sizeof(L"TARGET.COM"));
    memcpy(profile->config_file.path, L"\\CONFIG.SYS", sizeof(L"\\CONFIG.SYS"));
    profile->config_file.materialization = BYOB_GUEST_BOOT_FILE_MINIMAL_COMMENT_V1;
    memcpy(profile->autoexec_file.path, L"\\AUTOEXEC.BAT", sizeof(L"\\AUTOEXEC.BAT"));
    profile->autoexec_file.materialization = BYOB_GUEST_BOOT_FILE_EMPTY_V1;
    profile->has_guest_boot_files = profile->has_guest_search_metadata = 1u;
    profile->command_metadata.attributes = profile->target_metadata.attributes =
        profile->config_metadata.attributes = profile->autoexec_metadata.attributes = 0x20u;
    profile->command_metadata.dos_date = profile->target_metadata.dos_date =
        profile->config_metadata.dos_date = profile->autoexec_metadata.dos_date = 1u;
    profile->declared_target_count = 1u;
    memcpy(profile->declared_targets[0].component.file_name, L"TARGET.COM",
        sizeof(L"TARGET.COM"));
    memcpy(profile->declared_targets[0].placement.path, L"\\TARGET.COM",
        sizeof(L"\\TARGET.COM"));
    profile->declared_targets[0].placement.drive_index = 2u;
    profile->declared_targets[0].metadata = profile->target_metadata;
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
        ((result->eflags_write_mask & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) == 0u ||
         (result->eflags_values & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) == 0u);
}

static uint32_t token_from(const bx_ntvdm_cpu_result_v2 *result)
{
    return ((uint32_t)result->cpu_delta.gpr16_values[0] << 16) |
        result->cpu_delta.gpr16_values[5];
}

int main(void)
{
    static const uint32_t modes[] = {
        BX_NTVDM_MUTATION_MODE_V1_DIRECT,
        BX_NTVDM_MUTATION_MODE_V1_READONLY
    };
    static uint8_t command_bytes[T225_S55_COMMAND_BYTES];
    uint8_t ntdos_bytes[] = { 0xf4u }, target_bytes[] = { 0xf4u };
    byob_image ntdos = { ntdos_bytes, sizeof(ntdos_bytes) };
    byob_image command = { command_bytes, sizeof(command_bytes) };
    byob_image target = { target_bytes, sizeof(target_bytes) };
    byob_profile_selection profile;
    bx_ntvdm_host_drive_snapshot_v1 drives;
    bx_ntvdm_host_namespace_v1 host;
    uint8_t drive_types[26] = { 0u };
    uint32_t index, byte_index;
    DWORD type = GetDriveTypeW(L"C:\\");

    if (type == DRIVE_NO_ROOT_DIR || type == DRIVE_UNKNOWN) return 1;
    for (byte_index = 0u; byte_index < sizeof(command_bytes); ++byte_index)
        command_bytes[byte_index] = (uint8_t)((byte_index * 37u + 11u) & 0xffu);
    command_bytes[0] = 0xf4u;
    profile_initialize(&profile);
    drive_types[2] = (uint8_t)type;
    if (!bx_ntvdm_host_drive_snapshot_v1_apply(1u << 2u, drive_types, 0u, 0u, &drives) ||
        !bx_ntvdm_host_namespace_v1_initialize(&host, &drives)) return 2;

    for (index = 0u; index < sizeof(modes) / sizeof(modes[0]); ++index) {
        bx_ntvdm_boot_namespace_plane_v1 plane;
        bx_ntvdm_dem_package_session_v1 session;
        bx_ntvdm_mutation_profile_v1 mutation;
        bx_ntvdm_cpu_state_v1 cpu;
        bx_ntvdm_cpu_result_v2 result;
        uint32_t token, image_offset, chunk;

        bx_ntvdm_mutation_profile_v1_initialize(&mutation, modes[index]);
        if (!bx_ntvdm_dem_profile_consumer_v1_register_class(&mutation,
                BX_NTVDM_MUTATION_CLASS_V1_SESSION_CONTEXT, 0x0fu) ||
            !bx_ntvdm_dem_profile_consumer_v1_register_class(&mutation,
                BX_NTVDM_MUTATION_CLASS_V1_NAMESPACE_CONTENT, 0x0fu) ||
            !bx_ntvdm_dem_profile_consumer_v1_register_class(&mutation,
                BX_NTVDM_MUTATION_CLASS_V1_FILE_METADATA, 0x0fu) ||
            !bx_ntvdm_boot_namespace_plane_v1_initialize(&plane, &ntdos, &command,
                &target, 0, &profile) ||
            !bx_ntvdm_dem_package_session_v1_initialize(&session, &plane) ||
            !bx_ntvdm_dem_package_session_v1_set_mutation_profile(&session, &mutation) ||
            !bx_ntvdm_dem_package_session_v1_set_drive_snapshot(&session, &drives) ||
            !bx_ntvdm_dem_package_session_v1_set_drive_view_host_namespace(&session, &host) ||
            !bx_ntvdm_dem_package_session_v1_set_host_namespace(&session, &host)) {
            bx_ntvdm_host_namespace_v1_release(&host);
            return 10 + (int)index;
        }

        memset(ram, 0, sizeof(ram));
        ram[0x520u] = 0x00u; ram[0x521u] = 0x06u;
        ram[0x450u] = 0x34u; ram[0x451u] = 0x12u;
        bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
        cpu.ds = 0u; cpu.esi = 0x500u; cpu.eax = 0x400u;
        cpu.edx = 0x450u; cpu.ecx = 0x460u;
        if (!dispatch(&session, 0x1bu, &cpu, &result) || !success(&result)) {
            bx_ntvdm_dem_package_session_v1_teardown(&session);
            bx_ntvdm_host_namespace_v1_release(&host);
            return 20 + (int)index;
        }

        memcpy(ram + 0x200u, "C:\\COMMAND.COM", sizeof("C:\\COMMAND.COM"));
        bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
        cpu.ds = 0u; cpu.esi = 0x200u; cpu.ebx = 0u;
        if (!dispatch(&session, 0x12u, &cpu, &result) || !success(&result) ||
            token_from(&result) == 0u) {
            bx_ntvdm_dem_package_session_v1_teardown(&session);
            bx_ntvdm_host_namespace_v1_release(&host);
            return 30 + (int)index;
        }
        token = token_from(&result);
        for (image_offset = 0u; image_offset < sizeof(command_bytes);
             image_offset += chunk) {
            chunk = (uint32_t)(sizeof(command_bytes) - image_offset);
            if (chunk > 1024u) chunk = 1024u;
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            cpu.ds = (uint16_t)((0x20000u + image_offset) >> 4u);
            cpu.edx = 0u; cpu.ecx = chunk; cpu.eflags = 0x40u;
            cpu.eax = token >> 16; cpu.ebp = token & 0xffffu;
            if (!dispatch(&session, 0x16u, &cpu, &result) || !success(&result) ||
                result.cpu_delta.gpr16_values[0] != chunk ||
                memcmp(ram + 0x20000u + image_offset,
                    command_bytes + image_offset, chunk) != 0) {
                bx_ntvdm_dem_package_session_v1_teardown(&session);
                bx_ntvdm_host_namespace_v1_release(&host);
                return 40 + (int)index;
            }
        }
        bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
        cpu.eax = token >> 16; cpu.ebp = token & 0xffffu;
        if (!dispatch(&session, 0x02u, &cpu, &result) || !success(&result)) {
            bx_ntvdm_dem_package_session_v1_teardown(&session);
            bx_ntvdm_host_namespace_v1_release(&host);
            return 50 + (int)index;
        }
        bx_ntvdm_dem_package_session_v1_teardown(&session);
    }
    bx_ntvdm_host_namespace_v1_release(&host);
    return 0;
}
