#include "bx_ntvdm_boot_namespace_composition_v1.h"
#include "bx_ntvdm_dem_package_facade_v1.h"
#include "bx_ntvdm_command_package_facade_v1.h"
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
    memcpy(profile->command_placement.path, L"\\COMMAND.COM",
        sizeof(L"\\COMMAND.COM"));
    profile->command_placement.drive_index = 2; profile->has_command_placement = 1;
    memcpy(profile->target_placement.path, L"\\TARGET.COM",
        sizeof(L"\\TARGET.COM"));
    profile->target_placement.drive_index = 2; profile->has_target_placement = 1;
    memcpy(profile->target.file_name, L"TARGET.COM", sizeof(L"TARGET.COM"));
    memcpy(profile->config_file.path, L"\\CONFIG.SYS", sizeof(L"\\CONFIG.SYS"));
    profile->config_file.materialization = BYOB_GUEST_BOOT_FILE_MINIMAL_COMMENT_V1;
    memcpy(profile->autoexec_file.path, L"\\AUTOEXEC.BAT", sizeof(L"\\AUTOEXEC.BAT"));
    profile->autoexec_file.materialization = BYOB_GUEST_BOOT_FILE_EMPTY_V1;
    profile->has_guest_boot_files = profile->has_guest_search_metadata = 1;
    profile->command_metadata.attributes = profile->target_metadata.attributes =
        profile->config_metadata.attributes = profile->autoexec_metadata.attributes = 0x20;
    profile->command_metadata.dos_date = profile->target_metadata.dos_date =
        profile->config_metadata.dos_date = profile->autoexec_metadata.dos_date = 1;
}

static int facade_existing_provider(uint32_t service)
{
    switch (service) {
    case 0x00u: case 0x02u: case 0x09u: case 0x0bu: case 0x0du:
    case 0x11u: case 0x12u: case 0x16u: case 0x18u: case 0x1bu:
    case 0x32u: case 0x3bu: case 0x3cu: case 0x45u: case 0x46u:
        return 1;
    default:
        return 0;
    }
}

static int facade_original_noop(uint32_t service)
{
    switch (service) {
    case 0x1fu: case 0x24u: case 0x26u: case 0x28u:
    case 0x2bu: case 0x40u: case 0x43u:
        return 1;
    default:
        return 0;
    }
}

static int facade_regression(void)
{
    uint32_t service;
    for (service = 0u; service < 73u; ++service) {
        uint8_t bytes[4] = { 0xc4u, 0xc4u, 0x50u, (uint8_t)service };
        bx_ntvdm_instruction_window_v1 window;
        bx_ntvdm_bop_ingress_v1 ingress;
        bx_ntvdm_bop_provider_selection_v1 selection;
        bx_ntvdm_dem_package_route_v1 route;
        uint32_t expected = facade_original_noop(service) ?
            BX_NTVDM_DEM_PACKAGE_ORIGINAL_NOOP : service == 0x42u ?
            BX_NTVDM_DEM_PACKAGE_FASTREAD_COMPATIBILITY :
            facade_existing_provider(service) ? BX_NTVDM_DEM_PACKAGE_EXISTING_PROVIDER :
            BX_NTVDM_DEM_PACKAGE_DEFERRED;
        bx_ntvdm_instruction_window_v1_capture(&window, bytes, 4u);
        if (!bx_ntvdm_bop_ingress_v1_classify(&window, &ingress) ||
            !bx_ntvdm_bop_provider_registry_v1_select(&ingress, &selection) ||
            !bx_ntvdm_dem_package_facade_v1_classify(&ingress, &selection, &route) ||
            route.plane.service != service || route.disposition != expected) return 0;
        if (expected == BX_NTVDM_DEM_PACKAGE_ORIGINAL_NOOP) {
            bx_ntvdm_exception_event_v1 event;
            bx_ntvdm_cpu_state_v1 cpu;
            bx_ntvdm_cpu_result_v2 result;
            memset(&event, 0, sizeof(event));
            event.magic = BX_NTVDM_EXCEPTION_ABI_MAGIC;
            event.abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION;
            event.struct_bytes = sizeof(event);
            event.kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION;
            event.vector = 6u; event.fault_rip = 0x100u;
            bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
            if (!bx_ntvdm_dem_package_facade_v1_dispatch(&ingress, &selection,
                    &route, &event, &cpu, &result) ||
                result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
                result.resume_rip != 0x104u ||
                result.eflags_write_mask != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
                result.eflags_values != 0u) return 0;
        }
    }
    {
        uint8_t bytes[4] = { 0xc4u, 0xc4u, 0x50u, 73u };
        bx_ntvdm_instruction_window_v1 window;
        bx_ntvdm_bop_ingress_v1 ingress;
        bx_ntvdm_bop_provider_selection_v1 selection;
        bx_ntvdm_dem_package_route_v1 route;
        bx_ntvdm_instruction_window_v1_capture(&window, bytes, 4u);
        if (!bx_ntvdm_bop_ingress_v1_classify(&window, &ingress) ||
            !bx_ntvdm_bop_provider_registry_v1_select(&ingress, &selection) ||
            bx_ntvdm_dem_package_facade_v1_classify(&ingress, &selection, &route)) return 0;
    }
    return 1;
}

static int command_facade_regression(void)
{
    uint32_t service;
    for (service=0u; service<17u; ++service) {
        uint8_t bytes[4]={0xc4u,0xc4u,0x54u,(uint8_t)service};
        bx_ntvdm_instruction_window_v1 window; bx_ntvdm_bop_ingress_v1 ingress;
        bx_ntvdm_bop_provider_selection_v1 selection; bx_ntvdm_command_package_route_v1 route;
        uint32_t expected=(service==3u)?BX_NTVDM_COMMAND_PACKAGE_ORIGINAL_NOOP:
            ((service==8u||service==10u)?BX_NTVDM_COMMAND_PACKAGE_EXPLICIT_UNAVAILABLE:
            ((service==1u||service==2u||service==4u||service==5u||service==12u||service==13u||service==14u)?BX_NTVDM_COMMAND_PACKAGE_EXISTING_PROVIDER:BX_NTVDM_COMMAND_PACKAGE_DEFERRED));
        bx_ntvdm_instruction_window_v1_capture(&window,bytes,4u);
        if(!bx_ntvdm_bop_ingress_v1_classify(&window,&ingress)||!bx_ntvdm_bop_provider_registry_v1_select(&ingress,&selection)||!bx_ntvdm_command_package_facade_v1_classify(&ingress,&selection,&route)||route.disposition!=expected) return 0;
        if(expected==BX_NTVDM_COMMAND_PACKAGE_ORIGINAL_NOOP||expected==BX_NTVDM_COMMAND_PACKAGE_EXPLICIT_UNAVAILABLE){bx_ntvdm_exception_event_v1 event;bx_ntvdm_cpu_state_v1 cpu;bx_ntvdm_cpu_result_v2 result;memset(&event,0,sizeof(event));event.magic=BX_NTVDM_EXCEPTION_ABI_MAGIC;event.abi_version=BX_NTVDM_EXCEPTION_ABI_VERSION;event.struct_bytes=sizeof(event);event.kind=BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION;event.vector=6u;event.fault_rip=0x100u;bx_ntvdm_cpu_state_v1_initialize(&cpu,BX_NTVDM_CPU_EXECUTION_REAL);if(!bx_ntvdm_command_package_facade_v1_dispatch(&ingress,&selection,&route,&event,&cpu,&result)||result.disposition!=(expected==BX_NTVDM_COMMAND_PACKAGE_ORIGINAL_NOOP?(uint32_t)BX_NTVDM_CPU_RESULT_V2_RESUME:(uint32_t)BX_NTVDM_CPU_RESULT_V2_STOP))return 0;}
    }
    return 1;
}

int main(void)
{
    uint8_t command_bytes[] = { 0x90, 0xc3 };
    uint8_t target_bytes[] = { 0xf4 };
    uint8_t ntdos_bytes[] = { 0xfa, 0xfc, 0xf4 };
    byob_image ntdos = { ntdos_bytes, sizeof(ntdos_bytes) };
    byob_image command = { command_bytes, sizeof(command_bytes) };
    byob_image target = { target_bytes, sizeof(target_bytes) };
    byob_profile_selection profile;
    bx_ntvdm_boot_namespace_composition_v1 composition;
    bx_ntvdm_host_drive_snapshot_v1 drives;
    uint8_t drive_types[26] = { 0 };
    byob_launch_plan_v2 launch_plan = { 2u, 1u,
        { 1u, BYOB_LAUNCH_TARGET_KIND_V1_COM, 0u, { 0 } } };
    struct bx_ntvdm_generic_ud_event_v1 event;
    struct bx_ntvdm_generic_ud_outcome_v1 outcome;
    uint32_t token;

    profile_initialize(&profile);
    if (!facade_regression()) return 45;
    if (!command_facade_regression()) return 46;
    drive_types[2] = 3u;
    profile.guest_display_state = BYOB_GUEST_DISPLAY_STATE_STREAM_IO_V1;
    profile.ntdos.bytes = sizeof(ntdos_bytes);
    memcpy(profile.ntdos.file_name, L"NTDOS.SYS", sizeof(L"NTDOS.SYS"));
    event_initialize(&event, 0x54, 0x0c);
    event.ds = 0x1000; event.edx = 0x20;
    if (bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome)) return 1;
    if (!bx_ntvdm_boot_namespace_composition_v1_initialize(&composition,
            &ntdos, &command, &target, 0, &profile) ||
        !bx_ntvdm_host_drive_snapshot_v1_apply(UINT32_C(4), drive_types, 0u, 0u,
            &drives) || !bx_ntvdm_boot_namespace_composition_v1_set_drive_snapshot(
            &composition, &drives) ||
        !bx_ntvdm_boot_namespace_composition_v1_set_launch_plan(&composition,
            &launch_plan) ||
        !bx_ntvdm_boot_namespace_composition_v1_bind(&composition)) return 2;
    if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome)) return 3;
    if (outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME) return 4;
    if (outcome.resume_rip != 0x104) return 5;
    if (ram[0x10020] == 0) return 6;

    /* COMMAND bootstrap is one classified pair: COMSPEC capture, environment
     * size retry and publication, followed by original-compatible repeats. */
    event_initialize(&event, 0x54, 0x02);
    event.ds = 0x101u; event.edx = 0x20u; event.eax = 0xaa00u;
    memcpy(ram + 0x1030, "C:\\COMMAND.COM", 15u);
    if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
        outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME ||
        outcome.resume_rip != 0x104u || outcome.gpr16_write_mask != 1u ||
        outcome.gpr16_values[0] != 0xaa01u ||
        composition.command_bootstrap.stage !=
            BX_NTVDM_CMD_COMSPEC_BOOTSTRAP_ENVIRONMENT_READY) return 23;
    event_initialize(&event, 0x54, 0x0f);
    event.es = 0x200u; event.ebx = 1u;
    if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
        outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME ||
        outcome.gpr16_write_mask != (1u << 3) || outcome.gpr16_values[3] != 2u ||
        composition.command_bootstrap.stage !=
            BX_NTVDM_CMD_COMSPEC_BOOTSTRAP_ENVIRONMENT_READY) return 24;
    event.ebx = 2u;
    if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
        outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME ||
        outcome.gpr16_write_mask != (1u << 3) || outcome.gpr16_values[3] != 0u ||
        memcmp(ram + 0x2000, "COMSPEC=C:\\COMMAND.COM", 23u) != 0 ||
        composition.command_bootstrap.stage !=
            BX_NTVDM_CMD_COMSPEC_BOOTSTRAP_ENVIRONMENT_CONSUMED) return 25;
    event_initialize(&event, 0x54, 0x02);
    event.eax = 0xbb00u;
    if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
        outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME ||
        outcome.resume_rip != 0x104u || outcome.gpr16_write_mask != 0u) return 26;
    event_initialize(&event, 0x54, 0x0f);
    event.es = 0x200u; event.ebx = 0xffffu;
    if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
        outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME ||
        outcome.gpr16_write_mask != (1u << 3) || outcome.gpr16_values[3] != 0u) return 27;

    event_initialize(&event, 0x5f, 0);
    event.eax = 0xbeefu; event.cs = event.ds = 0x1000u; event.esi = 0x40u;
    ram[0x10062] = 0x70u; ram[0x10063] = 0u;
    if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
        outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME ||
        outcome.resume_rip != 0x103u ||
        outcome.eflags_write_mask != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
        outcome.eflags_values != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
        ram[0x10070] != BYOB_GUEST_DISPLAY_STATE_STREAM_IO_V1) return 21;
    event_initialize(&event, 0x5f, 0);
    event.eax = 0xbeefu; event.cs = 0xffffu; event.ds = 0xfffcu; event.esi = 0u;
    if (bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome)) return 22;

    event_initialize(&event, 0x50, 0x11);
    event.edi = 0x0800;
    if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome)) return 11;
    if (outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME) return 12;
    if (outcome.resume_rip != 0x104) return 13;
    if (memcmp(ram + 0x8000, ntdos_bytes, sizeof(ntdos_bytes)) != 0) return 14;
    event_initialize(&event, 0x12, 0);
    if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
        outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME ||
        outcome.resume_rip != 0x103 || outcome.gpr16_write_mask != 1u ||
        outcome.gpr16_values[0] != 0x027fu) return 16;
    /* The admitted bare-machine profile reports its fixed extended-memory
     * value through the existing BIOS-memory provider. */
    event_initialize(&event, 0x15, 0);
    event.eax = 0x8800;
    if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
        outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME ||
        outcome.resume_rip != 0x103 || outcome.gpr16_write_mask != 1u ||
        outcome.gpr16_values[0] != 0x0c00u) return 17;
    event_initialize(&event, 0x54, 0x05);
    event.ds = 0x1000; event.edx = 0x20; event.ebx = 0x40; event.ecx = 0x60;
    if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
        outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME || outcome.resume_rip != 0x104 ||
        !composition.launch.valid || composition.launch.registration.scs_info != 0x10020u ||
        composition.launch.registration.scs_to_sync != 0x100eeu ||
        composition.launch.registration.is_dos_binary != 0x10040u ||
        composition.launch.registration.fd_access != 0x10060u) return 18;
    /* The complete source-derived CMDGETNEXT lifecycle consumes the fixed
       CMDINFO gather, writes the declared target response and commits exactly
       one immutable launch slot. */
    event_initialize(&event, 0x54, 0x01);
    event.ds = 0x100u; event.edx = 0x80u;
    ram[0x1088] = 0x30u; ram[0x108a] = 0x40u; ram[0x108c] = 128u;
    ram[0x109c] = 0x50u; ram[0x109e] = 0x60u; ram[0x10a0] = 17u;
    ram[0x10a1] = 1u;
    if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome)) return 31;
    if (outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME || outcome.resume_rip != 0x104u) return 32;
    if (outcome.gpr16_write_mask != 0u || outcome.eflags_write_mask !=
        BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF || outcome.eflags_values != 0u) return 33;
    if (composition.cmd_get_next.delivered != 1u) return 34;
    if (ram[0x340u + 1u] != 8u ||
        memcmp(ram + 0x340u + 2u, "TARGET\r\n", 8u) != 0) return 35;
    if (memcmp(ram + 0x560u, "C:\\TARGET.COM", 14u) != 0) return 36;
    if (bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
        composition.cmd_get_next.delivered != 1u) return 32;
    /* The selected COMMAND console capability is a fixed CLI no-install
       response.  Its identity was established by ingress and COMMAND-plane
       classification, rather than by the detached legacy runtime gate. */
    event_initialize(&event, 0x54, 0x0e);
    event.edx = 1u;
    if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
        outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME ||
        outcome.resume_rip != 0x104u || outcome.gpr16_write_mask != (1u << 2) ||
        outcome.gpr16_values[2] != 0u) return 37;
    event_initialize(&event, 0x54, 0x0e);
    event.execution_mode = BX_NTVDM_CPU_EXECUTION_PROTECTED;
    if (bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome)) return 38;
    event_initialize(&event, 0x54, 0x04);
    event.eax = 2u; event.ds = 0x100u; event.esi = 0x70u;
    if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
        outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME ||
        outcome.resume_rip != 0x104u || outcome.eflags_write_mask !=
            BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF || outcome.eflags_values != 0u ||
        memcmp(ram + 0x1070u, "C:\\", 4u) != 0) return 39;
    event.eax = 3u;
    if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome)) return 40;
    if (outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME ||
        outcome.resume_rip != 0x104u) return 42;
    if (outcome.gpr16_write_mask != 1u || outcome.gpr16_values[0] != 0u) return 43;
    if (outcome.eflags_write_mask != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
        outcome.eflags_values != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) return 44;
    event.execution_mode = BX_NTVDM_CPU_EXECUTION_PROTECTED;
    if (bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome)) return 41;
    event_initialize(&event, 0x50, 0x3b);
    event.eax = 0xabcd;
    if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
        outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME ||
        outcome.resume_rip != 0x104 || outcome.gpr16_write_mask != 1u ||
        outcome.gpr16_values[0] != 0xab00u) return 19;
    event_initialize(&event, 0x50, 0x3c);
    event.ebx = 0xabcd1234u;
    if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
        outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME ||
        outcome.resume_rip != 0x104 || outcome.gpr16_write_mask != 0u ||
        outcome.eflags_write_mask != 0u) return 20;
    event_initialize(&event, 0x50, 0x11);
    event.edi = 0x0900;
    composition.plane.ntdos.byte_count--;
    if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
        outcome.disposition != BX_NTVDM_GENERIC_UD_STOP ||
        ram[0x9000] != 0 || ram[0x9001] != 0 || ram[0x9002] != 0) return 15;
    composition.plane.ntdos.byte_count++;

    event_initialize(&event, 0x50, 0x12);
    event.ds = 0x1000; event.esi = 0x40;
    memcpy(ram + 0x10040, "C:\\CONFIG.SYS", 14);
    if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
        outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME) return 7;
    token = ((uint32_t)outcome.gpr16_values[0] << 16) | outcome.gpr16_values[5];

    event_initialize(&event, 0x50, 0x00);
    event.eax = token >> 16; event.ebp = token & 0xffffu;
    if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
        outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME) return 8;

    event_initialize(&event, 0x50, 0x02);
    event.eax = token >> 16; event.ebp = token & 0xffffu;
    event.ecx = event.edx = 0xffffu;
    if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
        outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME) return 28;
    event_initialize(&event, 0x50, 0x12);
    event.ds = 0x1000; event.esi = 0x40;
    memcpy(ram + 0x10040, "C:\\AUTOEXEC.BAT", 15);
    if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
        outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME) return 29;
    token = ((uint32_t)outcome.gpr16_values[0] << 16) | outcome.gpr16_values[5];
    allow_action = 0;
    event_initialize(&event, 0x50, 0x42);
    event.eax = token >> 16; event.ebp = token & 0xffffu;
    event.ecx = 0x20u; event.ds = 0x1000; event.edx = 0x40u; event.eflags = 0x40u;
    if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
        outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME || outcome.resume_rip != 0x104u ||
        outcome.gpr16_write_mask != 1u || outcome.gpr16_values[0] != 0u ||
        outcome.eflags_write_mask != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
        outcome.eflags_values != 0u) return 30;

    event_initialize(&event, 0x54, 0x0d);
    event.ds = 0x1000; event.edx = 0x20;
    if (bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome)) return 9;
    bx_ntvdm_boot_namespace_composition_v1_unbind(&composition);
    if (bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome)) return 10;
    return 0;
}
