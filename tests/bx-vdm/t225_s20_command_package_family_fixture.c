#include "bx_ntvdm_boot_namespace_composition_v1.h"
#include "bx-mantle/bx_ntvdm_mechanical_action_v1.h"
#include <string.h>
#include <stdio.h>

static uint8_t ram[0x100000];
static unsigned backend_calls;

int bx_ntvdm_mantle_execute_mechanical_action_v1(
    struct bx_ntvdm_mechanical_action_v1 *action)
{
    uint32_t i;
    if (!action || !bx_ntvdm_mechanical_action_v1_valid(action)) return 0;
    for (i = 0u; i < action->range_count; ++i) {
        const struct bx_ntvdm_mechanical_action_v1_range *range = &action->ranges[i];
        if (range->physical_address > sizeof(ram) - range->byte_count) return 0;
        if (action->kind == BX_NTVDM_MECHANICAL_ACTION_V1_READ)
            memcpy(action->payload + range->payload_offset,
                ram + range->physical_address, range->byte_count);
        else if (action->kind == BX_NTVDM_MECHANICAL_ACTION_V1_WRITE)
            memcpy(ram + range->physical_address,
                action->payload + range->payload_offset, range->byte_count);
        else return 0;
    }
    return 1;
}

static int fake_backend(const bx_ntvdm_command_child_backend_v1_request *request,
    bx_ntvdm_command_child_backend_v1_result *result)
{
    if (!bx_ntvdm_command_child_backend_v1_request_valid(request) || !result) return 0;
    ++backend_calls;
    bx_ntvdm_command_child_backend_v1_result_clear(result);
    result->completed = 1u;
    result->exit_code = request->launch_service == 8u ? 31u : 32u;
    return bx_ntvdm_command_child_backend_v1_result_valid(result);
}

static void selection_initialize(byob_profile_selection *selection)
{
    memset(selection, 0, sizeof(*selection));
    memcpy(selection->command_placement.path, L"\\COMMAND.COM", sizeof(L"\\COMMAND.COM"));
    selection->command_placement.drive_index = 2u; selection->has_command_placement = 1u;
    memcpy(selection->target_placement.path, L"\\TARGET.COM", sizeof(L"\\TARGET.COM"));
    selection->target_placement.drive_index = 2u; selection->has_target_placement = 1u;
    memcpy(selection->target.file_name, L"TARGET.COM", sizeof(L"TARGET.COM"));
    selection->declared_target_count = 1u;
    memcpy(selection->declared_targets[0].component.file_name, L"TARGET.COM", sizeof(L"TARGET.COM"));
    memcpy(selection->declared_targets[0].placement.path, L"\\TARGET.COM", sizeof(L"\\TARGET.COM"));
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
        bx_ntvdm_command_profile_consumer_v1_register_class(profile,
        BX_NTVDM_MUTATION_CLASS_V1_SESSION_CONTEXT, 0x0fu) &&
        bx_ntvdm_command_profile_consumer_v1_register_class(profile,
        BX_NTVDM_MUTATION_CLASS_V1_HOST_GLOBAL, 0x03u);
}

static void event_initialize(struct bx_ntvdm_generic_ud_event_v1 *event, uint8_t service)
{
    memset(event, 0, sizeof(*event));
    event->magic = BX_NTVDM_GENERIC_UD_EVENT_V1_MAGIC;
    event->abi_version = BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
    event->struct_bytes = sizeof(*event); event->cpu_id = 1u; event->vector = 6u;
    event->execution_mode = BX_NTVDM_CPU_EXECUTION_REAL;
    event->fault_rip = event->eip = 0x100u; event->window_bytes = 4u;
    event->window[0] = event->window[1] = 0xc4u; event->window[2] = 0x54u;
    event->window[3] = service;
}

static int invoke(bx_ntvdm_boot_namespace_composition_v1 *composition,
    uint8_t service, struct bx_ntvdm_generic_ud_event_v1 *event,
    struct bx_ntvdm_generic_ud_outcome_v1 *outcome)
{
    (void)composition;
    event->window[3] = service;
    return bx_ntvdm_boot_namespace_composition_v1_handle(event, outcome);
}

static int is_resume(const struct bx_ntvdm_generic_ud_outcome_v1 *outcome)
{ return outcome->disposition == BX_NTVDM_GENERIC_UD_RESUME && outcome->resume_rip == 0x104u; }

static uint32_t token_from(const struct bx_ntvdm_generic_ud_outcome_v1 *outcome)
{ return ((uint32_t)outcome->gpr16_values[3] << 16u) | outcome->gpr16_values[1]; }

static int run_mode(uint32_t mode)
{
    uint8_t ntdos_bytes[] = { 0xf4u }, command_bytes[] = { 0xf4u }, target_bytes[] = { 0xf4u };
    const byob_image ntdos = { ntdos_bytes, sizeof(ntdos_bytes) };
    const byob_image command = { command_bytes, sizeof(command_bytes) };
    const byob_image target = { target_bytes, sizeof(target_bytes) };
    bx_ntvdm_boot_namespace_composition_v1 composition;
    bx_ntvdm_host_drive_snapshot_v1 drives;
    bx_ntvdm_host_volume_snapshot_v1 volumes;
    bx_ntvdm_host_namespace_v1 host_namespace;
    bx_ntvdm_mutation_profile_v1 profile;
    bx_ntvdm_command_host_context_v1 context;
    byob_profile_selection selection;
    byob_launch_plan_v2 launch = { 2u, 1u, { 1u, BYOB_LAUNCH_TARGET_KIND_V1_COM, 0u, { 0 } } };
    struct bx_ntvdm_generic_ud_event_v1 event;
    struct bx_ntvdm_generic_ud_outcome_v1 outcome;
    uint8_t drive_types[26] = { 0u }, handles[12];
    bx_ntvdm_host_volume_record_v1 volume_records[26] = { 0 };
    uint32_t tokens[3], before;
    static const uint8_t environment[] = "A=1\0";
    static const uint8_t processor[] = "COMMAND.COM";
    int failure = 0;

    memset(ram, 0, sizeof(ram)); memset(&composition, 0, sizeof(composition));
    selection_initialize(&selection); drive_types[2] = 3u;
    volume_records[2].available = 1u; volume_records[2].bytes_per_sector = 512u;
    volume_records[2].sectors_per_cluster = 1u; volume_records[2].total_clusters = 1u;
    memset(&host_namespace, 0, sizeof(host_namespace));
    if (!profile_initialize(&profile, mode)) return 101;
    if (!bx_ntvdm_boot_namespace_composition_v1_initialize(&composition,
            &ntdos, &command, &target, 0, &selection)) return 102;
    if (!bx_ntvdm_host_drive_snapshot_v1_apply(UINT32_C(4), drive_types, 0u, 0u, &drives)) return 103;
    if (!bx_ntvdm_boot_namespace_composition_v1_set_drive_snapshot(&composition, &drives)) return 104;
    if (!bx_ntvdm_host_volume_snapshot_v1_apply(&drives, volume_records, &volumes)) return 105;
    if (!bx_ntvdm_boot_namespace_composition_v1_set_volume_snapshot(&composition, &volumes)) return 106;
    if (!bx_ntvdm_host_namespace_v1_initialize(&host_namespace, &drives)) return 107;
    if (!bx_ntvdm_boot_namespace_composition_v1_set_dem_mutation_profile(&composition, &profile)) return 108;
    if (!bx_ntvdm_boot_namespace_composition_v1_set_dem_host_namespace(&composition, &host_namespace)) return 109;
    if (!bx_ntvdm_boot_namespace_composition_v1_set_command_mutation_profile(&composition, &profile)) return 110;
    if (!bx_ntvdm_command_host_context_v1_initialize(&context, 2u, (const uint8_t *)"C:\\", 3u)) return 111;
    if (!bx_ntvdm_command_host_context_v1_set_environment(&context, environment, sizeof(environment))) return 112;
    if (!bx_ntvdm_command_host_context_v1_set_processor(&context, processor, sizeof(processor))) return 113;
    if (!bx_ntvdm_boot_namespace_composition_v1_set_command_host_context(&composition, &context)) return 114;
    if (!bx_ntvdm_boot_namespace_composition_v1_set_launch_plan(&composition, &launch)) return 115;    composition.command.launch_execution_provider.child_redirection.execute = fake_backend;
    composition.command.launch_execution_provider.stream_session.available_mask = 7u;
    composition.command.launch_execution_provider.stream_session.private_handle_values[0] = 1u;
    composition.command.launch_execution_provider.stream_session.private_handle_values[1] = 2u;
    composition.command.launch_execution_provider.stream_session.private_handle_values[2] = 3u;
    if (!bx_ntvdm_boot_namespace_composition_v1_bind(&composition)) return 2;
    event_initialize(&event, 0u);

    if (!invoke(&composition, 0u, &event, &outcome) || outcome.disposition != BX_NTVDM_GENERIC_UD_STOP) failure = 10;
    event.ds = 0x100u; event.edx = 0x80u;
    ram[0x1080u] = 0u; ram[0x1081u] = 2u; ram[0x1082u] = 48u;
    ram[0x1088u] = 0x30u; ram[0x108au] = 0x40u; ram[0x108cu] = 128u;
    ram[0x109cu] = 0x50u; ram[0x109eu] = 0x60u; ram[0x10a0u] = 17u; ram[0x10a1u] = 1u;
    if (!failure && (!invoke(&composition, 1u, &event, &outcome) || !is_resume(&outcome))) failure = 11;
    event.ds = 0x101u; event.edx = 0x20u; event.eax = 0xaa00u;
    memcpy(ram + 0x1030u, "C:\\COMMAND.COM", 15u);
    if (!failure && (!invoke(&composition, 2u, &event, &outcome) || !is_resume(&outcome))) failure = 12;
    if (!failure && (!invoke(&composition, 3u, &event, &outcome) || !is_resume(&outcome))) failure = 13;
    event.eax = 2u; event.ds = 0x100u; event.esi = 0x70u;
    if (!failure && (!invoke(&composition, 4u, &event, &outcome) || !is_resume(&outcome))) failure = 14;
    event.ds = 0x1000u; event.edx = 0x20u; event.ebx = 0x40u; event.ecx = 0x60u;
    if (!failure && (!invoke(&composition, 5u, &event, &outcome) || !is_resume(&outcome))) failure = 15;
    event.ecx = 0u;
    if (!failure && (!invoke(&composition, 6u, &event, &outcome) || !is_resume(&outcome))) failure = 16;
    tokens[0] = token_from(&outcome); event.ecx = 1u;
    if (!failure && (!invoke(&composition, 6u, &event, &outcome) || !is_resume(&outcome))) failure = 17;
    tokens[1] = token_from(&outcome); event.ecx = 2u;
    if (!failure && (!invoke(&composition, 6u, &event, &outcome) || !is_resume(&outcome))) failure = 18;
    tokens[2] = token_from(&outcome);
    if (!failure && mode == BX_NTVDM_MUTATION_MODE_V1_DIRECT && (!tokens[0] || !tokens[1] || !tokens[2])) failure = 19;
    if (!failure && mode == BX_NTVDM_MUTATION_MODE_V1_READONLY &&
        (outcome.eflags_values != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF || token_from(&outcome) != 0u)) failure = 20;
    if (!failure && (!invoke(&composition, 7u, &event, &outcome) || !is_resume(&outcome))) failure = 21;
    event.ds = 0x300u; event.es = 0x400u; event.ss = 0x500u; event.esi = 0x10u; event.ebp = 0x20u;
    memcpy(ram + 0x3010u, "COMMAND.COM /C EXIT\r", 20u); memcpy(ram + 0x4000u, environment, sizeof(environment));
    memcpy(handles + 0u, &tokens[2], 4u); memcpy(handles + 4u, &tokens[1], 4u); memcpy(handles + 8u, &tokens[0], 4u);
    memcpy(ram + 0x5020u, handles, sizeof(handles)); before = backend_calls;
    if (!failure && (!invoke(&composition, 8u, &event, &outcome) || !is_resume(&outcome))) failure = 22;
    if (!failure && mode == BX_NTVDM_MUTATION_MODE_V1_DIRECT && backend_calls != before + 1u) failure = 23;
    if (!failure && mode == BX_NTVDM_MUTATION_MODE_V1_READONLY && backend_calls != before) failure = 24;
    if (!failure && (!invoke(&composition, 9u, &event, &outcome) || !is_resume(&outcome))) failure = 25;
    event.es = 0x400u; before = backend_calls;
    if (!failure && (!invoke(&composition, 10u, &event, &outcome) || !is_resume(&outcome))) failure = 26;
    if (!failure && mode == BX_NTVDM_MUTATION_MODE_V1_DIRECT && backend_calls != before + 1u) failure = 27;
    if (!failure && mode == BX_NTVDM_MUTATION_MODE_V1_READONLY && backend_calls != before) failure = 28;
    if (!failure && (!invoke(&composition, 11u, &event, &outcome) || !is_resume(&outcome))) failure = 29;
    event.ds = 0x1000u; event.edx = 0x20u;
    if (!failure && (!invoke(&composition, 12u, &event, &outcome) || !is_resume(&outcome))) failure = 30;
    event.edx = 0x40u;
    if (!failure && (!invoke(&composition, 13u, &event, &outcome) || !is_resume(&outcome))) failure = 31;
    if (!failure && (!invoke(&composition, 14u, &event, &outcome) || !is_resume(&outcome))) failure = 32;
    event.es = 0x200u; event.ebx = 0x400u;
    if (!failure && (!invoke(&composition, 15u, &event, &outcome) || !is_resume(&outcome))) failure = 33;
    if (!failure && (!invoke(&composition, 16u, &event, &outcome) || outcome.disposition != BX_NTVDM_GENERIC_UD_STOP)) failure = 34;

    bx_ntvdm_boot_namespace_composition_v1_unbind(&composition);
    return failure;
}

int main(void)
{
    int failure;
    backend_calls = 0u;
    failure = run_mode(BX_NTVDM_MUTATION_MODE_V1_DIRECT);
    if (failure != 0) { fprintf(stderr, "direct failure=%d\n", failure); return 1; }
    failure = run_mode(BX_NTVDM_MUTATION_MODE_V1_READONLY);
    if (failure != 0) { fprintf(stderr, "readonly failure=%d\n", failure); return 2; }
    return 0;
}
