#include "bop-v1/bx_ntvdm_boot_namespace_composition_v1.h"
#include "bx-mantle/bx_ntvdm_mechanical_action_v1.h"
#include <string.h>
#include <stdio.h>

int bx_ntvdm_mantle_execute_mechanical_action_v1(
    struct bx_ntvdm_mechanical_action_v1 *action)
{
    (void)action;
    return 0;
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
}

static int profile_initialize(bx_ntvdm_mutation_profile_v1 *profile, uint32_t mode)
{
    bx_ntvdm_mutation_profile_v1_initialize(profile, mode);
    return bx_ntvdm_command_profile_consumer_v1_register_class(profile,
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

static int setup(bx_ntvdm_boot_namespace_composition_v1 *composition,
    uint32_t mode, uint32_t slots)
{
    static uint8_t ntdos_bytes[] = { 0xf4u }, command_bytes[] = { 0xf4u }, target_bytes[] = { 0xf4u };
    byob_image ntdos = { ntdos_bytes, sizeof(ntdos_bytes) };
    byob_image command = { command_bytes, sizeof(command_bytes) };
    byob_image target = { target_bytes, sizeof(target_bytes) };
    byob_profile_selection selection; bx_ntvdm_mutation_profile_v1 profile;
    byob_launch_plan_v2 launch = { 2u, 1u, { 1u, BYOB_LAUNCH_TARGET_KIND_V1_COM, 0u, { 0 } } };
    selection_initialize(&selection); if (!profile_initialize(&profile, mode)) return 0;
    launch.slot_count = slots;
    return bx_ntvdm_boot_namespace_composition_v1_initialize(composition,
        &ntdos, &command, &target, 0, &selection) &&
        bx_ntvdm_boot_namespace_composition_v1_set_command_mutation_profile(composition, &profile) &&
        bx_ntvdm_boot_namespace_composition_v1_set_launch_plan(composition, &launch) &&
        bx_ntvdm_boot_namespace_composition_v1_bind(composition);
}

static int parent_return_case(uint32_t mode)
{
    bx_ntvdm_boot_namespace_composition_v1 composition; struct bx_ntvdm_generic_ud_event_v1 event;
    struct bx_ntvdm_generic_ud_outcome_v1 outcome; int failure = 0;
    memset(&composition, 0, sizeof(composition));
    if (!setup(&composition, mode, 1u)) return 1;
    composition.command.bootstrap_provider.get_next.delivered = 1u;
    event_initialize(&event, 11u); event.edx = 0x1234u;
    if (!bx_ntvdm_boot_namespace_composition_v1_handle(&event, &outcome) ||
        outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME || outcome.resume_rip != 0x104u ||
        (outcome.eflags_values & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) != 0u ||
        outcome.gpr16_values[0] != 0x34u ||
        !bx_ntvdm_boot_namespace_composition_v1_copy_ordinary_terminal(&composition)) failure = 2;
    if (!failure && bx_ntvdm_boot_namespace_composition_v1_handle(&event, &outcome)) failure = 3;
    bx_ntvdm_boot_namespace_composition_v1_unbind(&composition);
    return failure;
}

static int reentry_case(uint32_t mode)
{
    bx_ntvdm_boot_namespace_composition_v1 composition; struct bx_ntvdm_generic_ud_event_v1 event;
    struct bx_ntvdm_generic_ud_outcome_v1 outcome; int failure = 0;
    memset(&composition, 0, sizeof(composition));
    if (!setup(&composition, mode, 2u)) return 1;
    composition.command.bootstrap_provider.get_next.delivered = 1u;
    event_initialize(&event, 11u); event.edx = 0x55u;
    if (!bx_ntvdm_boot_namespace_composition_v1_handle(&event, &outcome) ||
        outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME ||
        (outcome.eflags_values & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) == 0u ||
        bx_ntvdm_boot_namespace_composition_v1_copy_ordinary_terminal(&composition)) failure = 2;
    if (!failure && bx_ntvdm_boot_namespace_composition_v1_handle(&event, &outcome)) failure = 3;
    bx_ntvdm_boot_namespace_composition_v1_unbind(&composition);
    return failure;
}

static int top_level_case(uint32_t mode)
{
    bx_ntvdm_boot_namespace_composition_v1 composition; struct bx_ntvdm_generic_ud_event_v1 event;
    struct bx_ntvdm_generic_ud_outcome_v1 outcome; bx_ntvdm_command_terminal_v1 terminal;
    int failure = 0;
    memset(&composition, 0, sizeof(composition));
    if (!setup(&composition, mode, 1u)) return 1;
    event_initialize(&event, 0u);
    if (!bx_ntvdm_boot_namespace_composition_v1_handle(&event, &outcome) ||
        outcome.disposition != BX_NTVDM_GENERIC_UD_STOP ||
        !bx_ntvdm_command_lifecycle_provider_v1_copy_terminal(
            &composition.command.lifecycle_provider, &terminal) ||
        terminal.terminal_kind != BX_NTVDM_COMMAND_TERMINAL_V1_TOP_LEVEL_EXIT ||
        terminal.has_dos_exit_code != 0u ||
        !bx_ntvdm_boot_namespace_composition_v1_copy_ordinary_terminal(&composition)) failure = 2;
    bx_ntvdm_boot_namespace_composition_v1_unbind(&composition);
    return failure;
}

int main(void)
{
    uint32_t modes[] = { BX_NTVDM_MUTATION_MODE_V1_DIRECT, BX_NTVDM_MUTATION_MODE_V1_READONLY };
    uint32_t i; int result;
    for (i = 0u; i < sizeof(modes) / sizeof(modes[0]); ++i) {
        if ((result = parent_return_case(modes[i])) != 0) { fprintf(stderr, "parent mode=%u failure=%d\n", modes[i], result); return 1; }
        if ((result = reentry_case(modes[i])) != 0) { fprintf(stderr, "reentry mode=%u failure=%d\n", modes[i], result); return 2; }
        if ((result = top_level_case(modes[i])) != 0) { fprintf(stderr, "top mode=%u failure=%d\n", modes[i], result); return 3; }
    }
    puts("S27 COMMAND parent return composition direct/readonly family verified");
    return 0;
}