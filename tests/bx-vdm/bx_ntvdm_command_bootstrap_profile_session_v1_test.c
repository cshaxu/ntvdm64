#include "bx_ntvdm_boot_namespace_composition_v1.h"

#include <string.h>

static uint8_t ram[0x100000];

int bx_ntvdm_mantle_execute_mechanical_action_v1(
    struct bx_ntvdm_mechanical_action_v1 *action)
{
    uint32_t index;
    if (!bx_ntvdm_mechanical_action_v1_valid(action)) return 0;
    for (index = 0u; index < action->range_count; ++index) {
        const struct bx_ntvdm_mechanical_action_v1_range *range = &action->ranges[index];
        if (range->physical_address > sizeof(ram) - range->byte_count) return 0;
        if (action->kind == BX_NTVDM_MECHANICAL_ACTION_V1_READ)
            memcpy(action->payload + range->payload_offset, ram + range->physical_address,
                range->byte_count);
        else if (action->kind == BX_NTVDM_MECHANICAL_ACTION_V1_WRITE)
            memcpy(ram + range->physical_address, action->payload + range->payload_offset,
                range->byte_count);
        else return 0;
    }
    return 1;
}

static void event_initialize(struct bx_ntvdm_generic_ud_event_v1 *event,
    uint8_t service)
{
    memset(event, 0, sizeof(*event));
    event->magic = BX_NTVDM_GENERIC_UD_EVENT_V1_MAGIC;
    event->abi_version = BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
    event->struct_bytes = sizeof(*event);
    event->cpu_id = 1u;
    event->vector = 6u;
    event->execution_mode = BX_NTVDM_CPU_EXECUTION_REAL;
    event->fault_rip = event->eip = 0x100u;
    event->window_bytes = 4u;
    event->window[0] = event->window[1] = 0xc4u;
    event->window[2] = 0x54u;
    event->window[3] = service;
}

static void selection_initialize(byob_profile_selection *selection)
{
    memset(selection, 0, sizeof(*selection));
    memcpy(selection->command_placement.path, L"\\COMMAND.COM", sizeof(L"\\COMMAND.COM"));
    selection->command_placement.drive_index = 2u;
    selection->has_command_placement = 1u;
    memcpy(selection->target_placement.path, L"\\TARGET.COM", sizeof(L"\\TARGET.COM"));
    selection->target_placement.drive_index = 2u;
    selection->has_target_placement = 1u;
    memcpy(selection->target.file_name, L"TARGET.COM", sizeof(L"TARGET.COM"));
    selection->declared_target_count = 1u;
    memcpy(selection->declared_targets[0].component.file_name, L"TARGET.COM",
        sizeof(L"TARGET.COM"));
    memcpy(selection->declared_targets[0].placement.path, L"\\TARGET.COM",
        sizeof(L"\\TARGET.COM"));
    selection->declared_targets[0].placement.drive_index = 2u;
    memcpy(selection->config_file.path, L"\\CONFIG.SYS", sizeof(L"\\CONFIG.SYS"));
    selection->config_file.materialization = BYOB_GUEST_BOOT_FILE_MINIMAL_COMMENT_V1;
    memcpy(selection->autoexec_file.path, L"\\AUTOEXEC.BAT", sizeof(L"\\AUTOEXEC.BAT"));
    selection->autoexec_file.materialization = BYOB_GUEST_BOOT_FILE_EMPTY_V1;
    selection->has_guest_boot_files = selection->has_guest_search_metadata = 1u;
    selection->command_metadata.attributes = selection->target_metadata.attributes =
        selection->config_metadata.attributes = selection->autoexec_metadata.attributes = 0x20u;
    selection->command_metadata.dos_date = selection->target_metadata.dos_date =
        selection->config_metadata.dos_date = selection->autoexec_metadata.dos_date = 1u;
    selection->declared_targets[0].metadata = selection->target_metadata;
}

static int profile_initialize(bx_ntvdm_mutation_profile_v1 *profile, uint32_t mode)
{
    bx_ntvdm_mutation_profile_v1_initialize(profile, mode);
    return bx_ntvdm_dem_profile_consumer_v1_register_class(profile,
        BX_NTVDM_MUTATION_CLASS_V1_SESSION_CONTEXT, 0x0fu) &&
        bx_ntvdm_dem_profile_consumer_v1_register_class(profile,
        BX_NTVDM_MUTATION_CLASS_V1_NAMESPACE_CONTENT, 0x0fu) &&
        bx_ntvdm_dem_profile_consumer_v1_register_class(profile,
        BX_NTVDM_MUTATION_CLASS_V1_FILE_METADATA, 0x0fu) &&
        bx_ntvdm_command_profile_consumer_v1_register_class(profile,
        BX_NTVDM_MUTATION_CLASS_V1_SESSION_CONTEXT, 0x0fu) &&
        bx_ntvdm_command_profile_consumer_v1_register_class(profile,
        BX_NTVDM_MUTATION_CLASS_V1_HOST_GLOBAL, 0x03u);
}

static int run_session(uint32_t mode)
{
    uint8_t ntdos_bytes[] = { 0xfa, 0xfc, 0xf4 };
    uint8_t command_bytes[] = { 0x90, 0xc3 };
    uint8_t target_bytes[] = { 0xf4 };
    const byob_image ntdos = { ntdos_bytes, sizeof(ntdos_bytes) };
    const byob_image command = { command_bytes, sizeof(command_bytes) };
    const byob_image target = { target_bytes, sizeof(target_bytes) };
    static const uint8_t environment[] = "PATH=C:\\DOS\0PROMPT=$P$G\0";
    bx_ntvdm_boot_namespace_composition_v1 composition;
    bx_ntvdm_host_drive_snapshot_v1 drives;
    bx_ntvdm_host_volume_snapshot_v1 volumes;
    bx_ntvdm_host_namespace_v1 host_namespace;
    bx_ntvdm_mutation_profile_v1 mutation_profile;
    bx_ntvdm_command_host_context_v1 context;
    byob_profile_selection selection;
    byob_launch_plan_v2 launch = { 2u, 1u,
        { 1u, BYOB_LAUNCH_TARGET_KIND_V1_COM, 0u, { 0 } } };
    uint8_t drive_types[26] = { 0u };
    bx_ntvdm_host_volume_record_v1 volume_records[26] = { 0 };
    struct bx_ntvdm_generic_ud_event_v1 event;
    struct bx_ntvdm_generic_ud_outcome_v1 outcome;
    uint32_t policy = 0u;
    uint32_t expected_policy = mode == BX_NTVDM_MUTATION_MODE_V1_READONLY ?
        BX_NTVDM_MUTATION_POLICY_V1_REJECT_READONLY : BX_NTVDM_MUTATION_POLICY_V1_DIRECT_HOST;
    int failure = 0;

    selection_initialize(&selection);
    drive_types[2] = 3u;
    volume_records[2].available = 1u;
    volume_records[2].bytes_per_sector = 512u;
    volume_records[2].sectors_per_cluster = 1u;
    volume_records[2].total_clusters = 1u;
    memset(&composition, 0, sizeof(composition));
    memset(&host_namespace, 0, sizeof(host_namespace));
    if (!profile_initialize(&mutation_profile, mode)) failure = 10;
    else if (!bx_ntvdm_boot_namespace_composition_v1_initialize(&composition,
            &ntdos, &command, &target, 0, &selection)) failure = 11;
    else if (!bx_ntvdm_host_drive_snapshot_v1_apply(UINT32_C(4), drive_types, 0u, 0u,
            &drives)) failure = 12;
    else if (!bx_ntvdm_boot_namespace_composition_v1_set_drive_snapshot(&composition,
            &drives)) failure = 13;
    else if (!bx_ntvdm_host_volume_snapshot_v1_apply(&drives, volume_records,
            &volumes)) failure = 14;
    else if (!bx_ntvdm_boot_namespace_composition_v1_set_volume_snapshot(&composition,
            &volumes)) failure = 15;
    else if (!bx_ntvdm_host_namespace_v1_initialize(&host_namespace, &drives)) failure = 16;
    else if (!bx_ntvdm_boot_namespace_composition_v1_set_dem_mutation_profile(
            &composition, &mutation_profile)) failure = 17;
    else if (!bx_ntvdm_command_package_session_v1_valid(&composition.command)) failure = 18;
    else if (composition.command.has_mutation_profile != 0u) failure = 19;
    else if (!bx_ntvdm_boot_namespace_composition_v1_set_command_mutation_profile(
            &composition, &mutation_profile)) failure = 20;
    else if (!bx_ntvdm_command_host_context_v1_initialize(&context, 2u,
            (const uint8_t *)"C:\\NTDOS64", 10u)) failure = 19;
    else if (!bx_ntvdm_command_host_context_v1_set_environment(&context,
            environment, (uint32_t)sizeof(environment))) failure = 20;
    else if (!bx_ntvdm_boot_namespace_composition_v1_set_command_host_context(
            &composition, &context)) failure = 21;
    else if (!bx_ntvdm_boot_namespace_composition_v1_set_dem_host_namespace(
            &composition, &host_namespace)) failure = 22;
    else if (!bx_ntvdm_boot_namespace_composition_v1_set_launch_plan(&composition,
            &launch)) failure = 23;
    else if (!bx_ntvdm_boot_namespace_composition_v1_bind(&composition)) failure = 24;
    if (!failure && (!bx_ntvdm_command_package_session_v1_resolve_mutation_class(
            &composition.command, BX_NTVDM_MUTATION_CLASS_V1_SESSION_CONTEXT, &policy) ||
        policy != expected_policy ||
        !bx_ntvdm_command_package_session_v1_resolve_mutation_class(
            &composition.command, BX_NTVDM_MUTATION_CLASS_V1_HOST_GLOBAL, &policy) ||
        policy != expected_policy)) failure = 2;
    event_initialize(&event, 0x02u);
    event.ds = 0x101u; event.edx = 0x20u; event.eax = 0xaa00u;
    memcpy(ram + 0x1030u, "C:\\COMMAND.COM", 15u);
    if (!failure && (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
        outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME || outcome.gpr16_write_mask != 1u ||
        outcome.gpr16_values[0] != 0xaa01u)) failure = 3;
    event_initialize(&event, 0x0fu); event.es = 0x200u; event.ebx = 1u;
    if (!failure && (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
        outcome.gpr16_write_mask != (1u << 3) || outcome.gpr16_values[3] != 3u)) failure = 4;
    event.ebx = 3u;
    if (!failure && (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
        outcome.gpr16_write_mask != (1u << 3) || outcome.gpr16_values[3] != 3u ||
        memcmp(ram + 0x2000u, "COMSPEC=C:\\COMMAND.COM", 23u) != 0)) failure = 5;
    event_initialize(&event, 0x05u);
    event.ds = 0x1000u; event.edx = 0x20u; event.ebx = 0x40u; event.ecx = 0x60u;
    if (!failure && (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
        !composition.command.bootstrap_provider.set_info.valid)) failure = 6;
    event_initialize(&event, 0x01u);
    event.ds = 0x100u; event.edx = 0x80u;
    ram[0x1080u] = 0u; ram[0x1081u] = 2u; ram[0x1082u] = 48u;
    ram[0x1088u] = 0x30u; ram[0x108au] = 0x40u; ram[0x108cu] = 128u;
    ram[0x109cu] = 0x50u; ram[0x109eu] = 0x60u; ram[0x10a0u] = 17u; ram[0x10a1u] = 1u;
    if (!failure && (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
        outcome.eflags_write_mask != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
        outcome.eflags_values != 0u || memcmp(ram + 0x560u, "C:\\TARGET.COM", 14u) != 0)) failure = 7;
    event_initialize(&event, 0x04u); event.eax = 2u; event.ds = 0x100u; event.esi = 0x70u;
    if (!failure && (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
        outcome.eflags_values != 0u || memcmp(ram + 0x1070u, "C:\\NTDOS64", 10u) != 0)) failure = 8;
    event.eax = 3u;
    if (!failure && (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
        outcome.gpr16_write_mask != 1u || outcome.gpr16_values[0] != 1u ||
        outcome.eflags_values != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF)) failure = 9;
    event_initialize(&event, 0x0cu); event.ds = 0x1000u; event.edx = 0x20u;
    if (!failure && (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
        memcmp(ram + 0x10020u, "C:\\CONFIG.SYS", 14u) != 0)) failure = 10;
    event_initialize(&event, 0x0du); event.ds = 0x1000u; event.edx = 0x40u;
    if (!failure && (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
        memcmp(ram + 0x10040u, "C:\\AUTOEXEC.BAT", 15u) != 0)) failure = 11;
    if (composition.bound) bx_ntvdm_boot_namespace_composition_v1_unbind(&composition);
    bx_ntvdm_host_namespace_v1_release(&host_namespace);
    return failure;
}

int main(void)
{
    int failure = run_session(BX_NTVDM_MUTATION_MODE_V1_DIRECT);
    if (failure != 0) return failure;
    return run_session(BX_NTVDM_MUTATION_MODE_V1_READONLY);
}
