#include "bx_ntvdm_boot_namespace_composition_v1.h"
#include <string.h>

static uint8_t ram[0x100000];
static int allow_action = 1;

int bx_ntvdm_mantle_execute_mechanical_action_v1(
    struct bx_ntvdm_mechanical_action_v1 *action)
{
    uint32_t index;
    if (!allow_action || !bx_ntvdm_mechanical_action_v1_valid(action)) return 0;
    for (index = 0; index < action->range_count; ++index) {
        struct bx_ntvdm_mechanical_action_v1_range *range = &action->ranges[index];
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
    uint8_t selector, uint8_t service)
{
    memset(event, 0, sizeof(*event));
    event->magic = BX_NTVDM_GENERIC_UD_EVENT_V1_MAGIC;
    event->abi_version = BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
    event->struct_bytes = sizeof(*event); event->cpu_id = 1;
    event->vector = 6; event->execution_mode = BX_NTVDM_CPU_EXECUTION_REAL;
    event->fault_rip = event->eip = 0x100; event->window_bytes = 4;
    event->window[0] = 0xc4; event->window[1] = 0xc4;
    event->window[2] = selector; event->window[3] = service;
}

static void profile_initialize(byob_profile_selection *profile)
{
    memset(profile, 0, sizeof(*profile));
    wcscpy(profile->command_placement.path, L"\\COMMAND.COM");
    profile->command_placement.drive_index = 2; profile->has_command_placement = 1;
    wcscpy(profile->target_placement.path, L"\\TARGET.COM");
    profile->target_placement.drive_index = 2; profile->has_target_placement = 1;
    wcscpy(profile->target.file_name, L"TARGET.COM");
    wcscpy(profile->config_file.path, L"\\CONFIG.SYS");
    profile->config_file.materialization = BYOB_GUEST_BOOT_FILE_MINIMAL_COMMENT_V1;
    wcscpy(profile->autoexec_file.path, L"\\AUTOEXEC.BAT");
    profile->autoexec_file.materialization = BYOB_GUEST_BOOT_FILE_EMPTY_V1;
    profile->has_guest_boot_files = profile->has_guest_search_metadata = 1;
    profile->command_metadata.attributes = profile->target_metadata.attributes =
        profile->config_metadata.attributes = profile->autoexec_metadata.attributes = 0x20;
    profile->command_metadata.dos_date = profile->target_metadata.dos_date =
        profile->config_metadata.dos_date = profile->autoexec_metadata.dos_date = 1;
}

int main(void)
{
    uint8_t command_bytes[] = { 0x90, 0xc3 };
    uint8_t target_bytes[] = { 0xf4 };
    byob_image command = { command_bytes, sizeof(command_bytes) };
    byob_image target = { target_bytes, sizeof(target_bytes) };
    byob_profile_selection profile;
    bx_ntvdm_boot_namespace_composition_v1 composition;
    struct bx_ntvdm_generic_ud_event_v1 event;
    struct bx_ntvdm_generic_ud_outcome_v1 outcome;
    uint32_t token;

    profile_initialize(&profile);
    event_initialize(&event, 0x54, 0x0c);
    event.ds = 0x1000; event.edx = 0x20;
    if (bx_ntvdm_boot_namespace_composition_v1_handle(&event, &outcome)) return 1;
    if (!bx_ntvdm_boot_namespace_composition_v1_initialize(&composition,
            &command, &target, 0, &profile) ||
        !bx_ntvdm_boot_namespace_composition_v1_bind(&composition)) return 2;
    if (!bx_ntvdm_boot_namespace_composition_v1_handle(&event, &outcome)) return 3;
    if (outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME) return 4;
    if (outcome.resume_rip != 0x104) return 5;
    if (ram[0x10020] == 0) return 6;

    event_initialize(&event, 0x50, 0x12);
    event.ds = 0x1000; event.esi = 0x40;
    memcpy(ram + 0x10040, "C:\\CONFIG.SYS", 14);
    if (!bx_ntvdm_boot_namespace_composition_v1_handle(&event, &outcome) ||
        outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME) return 7;
    token = ((uint32_t)outcome.gpr16_values[5] << 16) | outcome.gpr16_values[0];

    event_initialize(&event, 0x50, 0x00);
    event.eax = token & 0xffffu; event.ebp = token >> 16;
    if (!bx_ntvdm_boot_namespace_composition_v1_handle(&event, &outcome) ||
        outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME) return 8;

    allow_action = 0;
    event_initialize(&event, 0x54, 0x0d);
    event.ds = 0x1000; event.edx = 0x20;
    if (bx_ntvdm_boot_namespace_composition_v1_handle(&event, &outcome)) return 9;
    bx_ntvdm_boot_namespace_composition_v1_unbind(&composition);
    if (bx_ntvdm_boot_namespace_composition_v1_handle(&event, &outcome)) return 10;
    return 0;
}
