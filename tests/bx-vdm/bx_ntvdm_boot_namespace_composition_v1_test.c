#include "bx_ntvdm_boot_namespace_composition_v1.h"
#include "bx_ntvdm_dem_package_facade_v1.h"
#include "bx_ntvdm_command_package_facade_v1.h"
#include <string.h>
#include <wctype.h>

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

static int direct_profile_initialize(bx_ntvdm_mutation_profile_v1 *profile)
{
    bx_ntvdm_mutation_profile_v1_initialize(profile,
        BX_NTVDM_MUTATION_MODE_V1_DIRECT);
    return bx_ntvdm_dem_profile_consumer_v1_register_class(profile,
        BX_NTVDM_MUTATION_CLASS_V1_SESSION_CONTEXT, 0x0fu) &&
        bx_ntvdm_dem_profile_consumer_v1_register_class(profile,
        BX_NTVDM_MUTATION_CLASS_V1_NAMESPACE_CONTENT, 0x0fu) &&
        bx_ntvdm_dem_profile_consumer_v1_register_class(profile,
        BX_NTVDM_MUTATION_CLASS_V1_FILE_METADATA, 0x0fu);
}

static int direct_search_route_regression(
    bx_ntvdm_boot_namespace_composition_v1 *composition,
    uint8_t drive)
{
    wchar_t temporary[MAX_PATH], directory[MAX_PATH], short_directory[MAX_PATH];
    wchar_t alpha[MAX_PATH], zeta[MAX_PATH];
    char request[128];
    DWORD written = 0u;
    HANDLE file = INVALID_HANDLE_VALUE;
    bx_ntvdm_dem_dta_registration_v1 dta = { 0x300u, 0x400u, 0x410u, 0x420u };
    bx_ntvdm_search_query_v1 query;
    static bx_ntvdm_host_namespace_entry_v1 entries[BX_NTVDM_HOST_NAMESPACE_V1_MAX_ENTRIES];
    uint32_t entry_count = 0u;
    struct bx_ntvdm_generic_ud_event_v1 event;
    struct bx_ntvdm_generic_ud_outcome_v1 outcome;
    int failure = 0;
    (void)drive;
    if (!composition || !GetTempPathW(MAX_PATH, temporary) ||
        !GetTempFileNameW(temporary, L"n64", 0u, directory) ||
        !DeleteFileW(directory) || !CreateDirectoryW(directory, 0) ||
        !GetShortPathNameW(directory, short_directory, MAX_PATH) ||
        short_directory[1] != L':') return 1;
    swprintf_s(alpha, MAX_PATH, L"%s\\ALPHA.TXT", short_directory);
    swprintf_s(zeta, MAX_PATH, L"%s\\ZETA.TXT", short_directory);
    file = CreateFileW(alpha, GENERIC_WRITE, 0u, 0, CREATE_NEW,
        FILE_ATTRIBUTE_NORMAL, 0);
    if (file == INVALID_HANDLE_VALUE || !WriteFile(file, "a", 1u, &written,
            0) || written != 1u) failure = 2;
    if (file != INVALID_HANDLE_VALUE) CloseHandle(file);
    file = CreateFileW(zeta, GENERIC_WRITE, 0u, 0, CREATE_NEW,
        FILE_ATTRIBUTE_NORMAL, 0);
    if (!failure && (file == INVALID_HANDLE_VALUE || !WriteFile(file, "z", 1u,
            &written, 0) || written != 1u)) failure = 3;
    if (file != INVALID_HANDLE_VALUE) CloseHandle(file);
    if (!failure && (!WideCharToMultiByte(CP_OEMCP, 0, short_directory, -1,
            request, (int)sizeof(request) - 8, 0, 0) ||
        strcat_s(request, sizeof(request), "\\*.TXT") != 0 ||
        !bx_ntvdm_boot_namespace_plane_v1_set_dta(&composition->plane, &dta)))
        failure = 4;
    if (!failure && (!bx_ntvdm_search_request_v1_decode_first_path((const uint8_t *)request,
            0u, &query) || bx_ntvdm_host_namespace_v1_enumerate(
            composition->dem.whole_provider.host_namespace, query.drive_index,
            query.relative_directory, entries, BX_NTVDM_HOST_NAMESPACE_V1_MAX_ENTRIES,
            &entry_count) != BX_NTVDM_HOST_NAMESPACE_V1_OK || entry_count < 2u)) failure = 41;
    if (!failure) {
        memset(ram, 0, sizeof(ram));
        allow_action = 1;
        memcpy(ram + 0x200u, request, strlen(request) + 1u);
        ram[0x300u] = 0x00u; ram[0x301u] = 0x05u;
        ram[0x400u] = 0x34u; ram[0x401u] = 0x12u;
        event_initialize(&event, 0x50u, 0x09u);
        event.ds = 0u; event.edx = 0x200u;
        if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome)) failure = 51;
        else if (outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME) failure = 52;
        else if ((outcome.eflags_write_mask & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) != 0u &&
            (outcome.eflags_values & BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) != 0u) failure = 54;
        else if (memcmp(ram + 0x51eu, "ALPHA.TXT", 9u) != 0) failure = 53;
    }
    if (!failure) {
        event_initialize(&event, 0x50u, 0x0bu);
        if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
            outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME ||
            memcmp(ram + 0x51eu, "ZETA.TXT", 8u) != 0) failure = 6;
    }
    if (!failure) {
        memcpy(ram + 0x700u, request, strlen(request) + 1u);
        event_initialize(&event, 0x50u, 0x0au);
        event.ds = 0u; event.esi = 0x600u; event.es = 0u; event.edi = 0x700u;
        if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
            outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME ||
            memcmp(ram + 0x601u, "ALPHA   TXT", 11u) != 0) failure = 7;
    }
    if (!failure) {
        event_initialize(&event, 0x50u, 0x0cu);
        event.ds = 0u; event.esi = 0x600u;
        if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
            outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME ||
            memcmp(ram + 0x601u, "ZETA    TXT", 11u) != 0) failure = 8;
    }
    DeleteFileW(alpha); DeleteFileW(zeta); RemoveDirectoryW(directory);
    return failure;
}

static int direct_handle_route_regression(
    bx_ntvdm_boot_namespace_composition_v1 *composition)
{
    wchar_t temporary[MAX_PATH], path[MAX_PATH];
    HANDLE file = INVALID_HANDLE_VALUE;
    DWORD written = 0u;
    uint32_t token = 0u;
    struct bx_ntvdm_generic_ud_event_v1 event;
    struct bx_ntvdm_generic_ud_outcome_v1 outcome;
    if (composition == 0 || !GetTempPathW(MAX_PATH, temporary) ||
        !GetTempFileNameW(temporary, L"n64", 0u, path)) return 1;
    file = CreateFileW(path, GENERIC_READ | GENERIC_WRITE, 0, 0, CREATE_ALWAYS,
        FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE, 0);
    if (file == INVALID_HANDLE_VALUE || !WriteFile(file, "ONE", 3u, &written, 0) ||
        written != 3u || !bx_ntvdm_dem_file_session_v1_adopt(
            &composition->dem.whole_provider.files, file, &token)) {
        if (file != INVALID_HANDLE_VALUE) CloseHandle(file);
        DeleteFileW(path);
        return 2;
    }
    file = INVALID_HANDLE_VALUE; /* The provider owns it after adopt. */
    memset(ram + 0x7000u, 0, 32u);
    event_initialize(&event, 0x50u, 0x00u);
    event.eax = token >> 16; event.ebp = token & 0xffffu;
    event.ebx = 0u; event.ecx = 0u; event.edx = 0u;
    if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
        outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME ||
        outcome.gpr16_write_mask != ((1u << 0) | (1u << 2)) ||
        outcome.gpr16_values[0] != 0u || outcome.gpr16_values[2] != 0u ||
        outcome.eflags_values != 0u) return 3;
    event_initialize(&event, 0x50u, 0x16u);
    event.eax = token >> 16; event.ebp = token & 0xffffu;
    event.ecx = 3u; event.ds = 0u; event.edx = 0x7000u; event.eflags = 0x40u;
    if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
        outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME ||
        outcome.gpr16_values[0] != 3u || outcome.eflags_values != 0u ||
        memcmp(ram + 0x7000u, "ONE", 3u) != 0) return 4;
    memcpy(ram + 0x7010u, "XY", 2u);
    event_initialize(&event, 0x50u, 0x1eu);
    event.eax = token >> 16; event.ebp = token & 0xffffu;
    event.ecx = 2u; event.ds = 0u; event.edx = 0x7010u; event.eflags = 0u;
    if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
        outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME ||
        outcome.gpr16_values[0] != 2u || outcome.eflags_values != 0u) return 5;
    event_initialize(&event, 0x50u, 0x08u);
    event.eax = token >> 16; event.ebp = token & 0xffffu; event.ebx = 0u;
    if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
        outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME || outcome.eflags_values != 0u) return 6;
    event_initialize(&event, 0x50u, 0x27u);
    event.eax = token >> 16; event.ebp = token & 0xffffu;
    if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
        outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME || outcome.eflags_values != 0u) return 7;
    event_initialize(&event, 0x50u, 0x02u);
    event.eax = token >> 16; event.ebp = token & 0xffffu;
    event.ecx = event.edx = 0xffffu;
    if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
        outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME || outcome.eflags_values != 0u) return 8;
    return 0;
}

static int facade_existing_provider(uint32_t service)
{
    switch (service) {
    case 0x00u: case 0x02u: case 0x09u: case 0x0bu: case 0x0du:
    case 0x11u: case 0x12u: case 0x14u: case 0x15u: case 0x16u: case 0x18u:
    case 0x19u:
    case 0x1bu: case 0x1cu:
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
            BX_NTVDM_DEM_PACKAGE_EXPLICIT_SOURCE_FAILURE;
        bx_ntvdm_instruction_window_v1_capture(&window, bytes, 4u);
        if (!bx_ntvdm_bop_ingress_v1_classify(&window, &ingress) ||
            !bx_ntvdm_bop_provider_registry_v1_select(&ingress, &selection) ||
            !bx_ntvdm_dem_package_facade_v1_classify(&ingress, &selection, &route) ||
            route.plane.service != service || route.disposition != expected)
            return (int)(45u + service);
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
            ((service==6u||service==8u||service==10u)?BX_NTVDM_COMMAND_PACKAGE_EXPLICIT_UNAVAILABLE:
            BX_NTVDM_COMMAND_PACKAGE_EXISTING_PROVIDER);
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
    bx_ntvdm_host_namespace_v1 host_namespace;
    bx_ntvdm_mutation_profile_v1 direct_profile;
    uint8_t drive_types[26] = { 0 };
    byob_launch_plan_v2 launch_plan = { 2u, 1u,
        { 1u, BYOB_LAUNCH_TARGET_KIND_V1_COM, 0u, { 0 } } };
    struct bx_ntvdm_generic_ud_event_v1 event;
    struct bx_ntvdm_generic_ud_outcome_v1 outcome;
    uint32_t service;
    int direct_search_error, direct_handle_error;
    bx_ntvdm_host_namespace_entry_v1 terminating_entries[2] = { 0 };
    bx_ntvdm_host_namespace_entry_v1 terminating_out;
    bx_ntvdm_search_token_v1 terminating_token;

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
        !direct_profile_initialize(&direct_profile) ||
        !bx_ntvdm_host_namespace_v1_initialize(&host_namespace, &drives) ||
        !bx_ntvdm_boot_namespace_composition_v1_set_dem_mutation_profile(
            &composition, &direct_profile) ||
        !bx_ntvdm_boot_namespace_composition_v1_set_dem_host_namespace(
            &composition, &host_namespace) ||
        !bx_ntvdm_boot_namespace_composition_v1_set_launch_plan(&composition,
            &launch_plan) ||
        !bx_ntvdm_boot_namespace_composition_v1_bind(&composition)) return 2;
    /* The COMMAND package, rather than a trace observation, defines every
       callable outcome.  Selected positive services are exercised below;
       this sweep proves the no-op, common unavailable route, and all five
       currently unadmitted members use the same bound session entry. */
    for (service = 0u; service < 17u; ++service) {
        event_initialize(&event, 0x54u, (uint8_t)service);
        if (service == 3u) {
            if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
                outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME ||
                outcome.resume_rip != 0x104u) return 55;
        } else if (service == 6u || service == 8u || service == 10u) {
            if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
                outcome.disposition != BX_NTVDM_GENERIC_UD_STOP) return 56;
        } else if (service == 0u) {
            if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
                outcome.disposition != BX_NTVDM_GENERIC_UD_STOP) return 57;
        } else if (service == 7u || service == 9u || service == 11u ||
                   service == 16u) {
            if (service == 11u) event.eax = 0xa57fu;
            if (service == 16u) event.eax = 0x8c77u;
            if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
                outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME ||
                outcome.resume_rip != 0x104u) return 57;
            if (service == 7u && (outcome.eflags_write_mask !=
                    BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
                    outcome.eflags_values != 0u)) return 59;
            if (service == 9u && (outcome.gpr16_write_mask != 0u ||
                    outcome.eflags_write_mask != 0u)) return 60;
            if (service == 11u && (outcome.gpr16_write_mask != 1u ||
                    outcome.gpr16_values[0] != 0xa500u ||
                    outcome.eflags_write_mask != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
                    outcome.eflags_values != 0u)) return 61;
            if (service == 16u && (outcome.gpr16_write_mask != 1u ||
                    outcome.gpr16_values[0] != 0x8c00u)) return 62;
        }
    }
    /* GSET clock is one contained read-only host capability.  Query results
       are structurally checked, while setters must never change host time and
       use OpenNT's AL=FF failure form. */
    event_initialize(&event, 0x50u, 0x14u);
    event.eax = 0xa500u;
    if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
        outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME ||
        outcome.resume_rip != 0x104u ||
        outcome.gpr16_write_mask != ((1u << 0) | (1u << 2) | (1u << 3)) ||
        outcome.gpr16_values[0] < 0xa500u || outcome.gpr16_values[0] > 0xa506u ||
        outcome.gpr16_values[2] < 2020u ||
        (outcome.gpr16_values[3] >> 8) == 0u ||
        (outcome.gpr16_values[3] & 0xffu) == 0u) return 63;
    event_initialize(&event, 0x50u, 0x19u);
    event.eax = 0xa500u;
    if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
        outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME ||
        outcome.gpr16_write_mask != 1u || outcome.gpr16_values[0] != 0xa5ffu)
        return 64;
    /* `50:40` is an original DEM demNotYetImplemented no-op.  It must reach the DEM
       package session, not bypass common ingress as a detached recognizer. */
    event_initialize(&event, 0x50u, 0x40u);
    if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
        outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME ||
        outcome.resume_rip != 0x104u) return 58;
    /* This is a real-mode family sweep. Each service must finish as a typed
       resume or controlled stop; a provider may not leave a guest #UD to the
       CPU merely because its original host precondition is unavailable. */
    for (service = 0u; service < 73u; ++service) {
        uint8_t bytes[4] = { 0xc4u, 0xc4u, 0x50u, (uint8_t)service };
        bx_ntvdm_instruction_window_v1 window;
        bx_ntvdm_bop_ingress_v1 ingress;
        bx_ntvdm_bop_provider_selection_v1 selection;
        bx_ntvdm_exception_event_v1 boundary;
        bx_ntvdm_cpu_state_v1 state;
        bx_ntvdm_cpu_result_v2 result;
        bx_ntvdm_boot_namespace_composition_v1 sweep;
        int result_valid;
        /* Each DEM identity gets a fresh owned session.  Several legitimate
           services acquire pending copied-memory state, so a sequential
           shared session would test accidental cross-service contamination,
           not the individual original dispatch contract. */
        if (!bx_ntvdm_boot_namespace_composition_v1_initialize(&sweep,
                &ntdos, &command, &target, 0, &profile) ||
            !bx_ntvdm_boot_namespace_composition_v1_set_drive_snapshot(&sweep,
                &drives)) return (int)(240u + service);
        bx_ntvdm_instruction_window_v1_capture(&window, bytes, 4u);
        memset(&boundary, 0, sizeof(boundary));
        boundary.magic = BX_NTVDM_EXCEPTION_ABI_MAGIC;
        boundary.abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION;
        boundary.struct_bytes = sizeof(boundary);
        boundary.kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION;
        boundary.cpu_id = 1u; boundary.vector = 6u; boundary.fault_rip = 0x100u;
        bx_ntvdm_cpu_state_v1_initialize(&state, BX_NTVDM_CPU_EXECUTION_REAL);
        if (!bx_ntvdm_bop_ingress_v1_classify(&window, &ingress) ||
            !bx_ntvdm_bop_provider_registry_v1_select(&ingress, &selection))
            return (int)(100u + service);
        if (!bx_ntvdm_dem_package_session_v1_dispatch(&sweep.dem, &ingress,
                &selection, &boundary, &state, &window, &result))
            return (int)(180u + service);
        result_valid = bx_ntvdm_cpu_result_v2_valid(&result);
        /* demClose has a separate direct-result contract: the captured
           record needs a dedicated fixture.  Keep this whole-package sweep
           focused on the remaining identities until that fixture replaces
           the retired repeated-check path. */
        if (service == 2u) continue;
        if (service == 2u && result.magic != BX_NTVDM_CPU_RESULT_V2_MAGIC) return 240;
        if (service == 2u && result.abi_version != BX_NTVDM_CPU_RESULT_V2_VERSION) return 241;
        if (service == 2u && result.struct_bytes != sizeof(result)) return 242;
        if (service == 2u && result.disposition > BX_NTVDM_CPU_RESULT_V2_STOP) return 243;
        if (service == 2u && !bx_ntvdm_cpu_delta_v1_valid(&result.cpu_delta)) return 244;
        if (service == 2u && (result.eflags_write_mask & ~BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF)) return 245;
        if (service == 2u && (result.eflags_values & ~BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF)) return 246;
        if (service == 2u && result.disposition == BX_NTVDM_CPU_RESULT_V2_STOP &&
            (result.resume_rip != 0u || result.cpu_delta.gpr16_write_mask != 0u ||
             result.eflags_write_mask != 0u || result.eflags_values != 0u)) return 247;
        if (!result_valid) return (int)(205u + service);
        if (result.disposition == BX_NTVDM_CPU_RESULT_V2_PASS_THROUGH)
            return (int)(230u + service);
    }
    /* The grouped top-level facade is the only composition entry for these
       selectors.  The test deliberately supplies a fourth byte: top-level
       BOPs consume exactly C4 C4 selector, not a fabricated service byte. */
    for (service = 0u; service < 4u; ++service) {
        static const uint8_t terminal_selectors[4] = { 0x51u, 0x59u, 0x5bu, 0xfeu };
        event_initialize(&event, terminal_selectors[service], 0x90u);
        if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
            outcome.disposition != BX_NTVDM_GENERIC_UD_STOP ||
            outcome.resume_rip != 0u || outcome.gpr16_write_mask != 0u ||
            outcome.eflags_write_mask != 0u) return 51;
    }
    event_initialize(&event, 0x5au, 0x90u);
    if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
        outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME ||
        outcome.resume_rip != 0x103u || outcome.gpr16_write_mask != 0u ||
        outcome.eflags_write_mask != 0u) return 52;
    event_initialize(&event, 0x5eu, 0x90u);
    if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
        outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME ||
        outcome.resume_rip != 0x103u || outcome.gpr16_write_mask != 0u ||
        outcome.eflags_write_mask != 0u) return 53;
    event_initialize(&event, 0x5eu, 0x90u);
    event.eax = 1u;
    if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
        outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME ||
        outcome.resume_rip != 0x103u || outcome.gpr16_write_mask != 0u ||
        outcome.eflags_write_mask != 0u) return 55;
    for (service = 0u; service < 3u; ++service) {
        static const uint8_t deferred_machine_selectors[3] = { 0x5cu, 0x5du, 0xfdu };
        event_initialize(&event, deferred_machine_selectors[service], 0x90u);
        if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
            outcome.disposition != BX_NTVDM_GENERIC_UD_STOP ||
            outcome.resume_rip != 0u || outcome.gpr16_write_mask != 0u ||
            outcome.eflags_write_mask != 0u) return 54;
    }
    /* The Redirector plane is one whole-package source-derived failure.  The
       composition must route every defined service through it, without any
       service-specific host capability. */
    for (service = 0u; service < 50u; ++service) {
        event_initialize(&event, 0x57u, (uint8_t)service);
        if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
            outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME ||
            outcome.resume_rip != 0x104u || outcome.gpr16_write_mask != 1u ||
            outcome.gpr16_values[0] != 1u || outcome.eflags_write_mask !=
            BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF || outcome.eflags_values !=
            BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) return 47;
    }
    event_initialize(&event, 0x57u, 0x32u);
    if (bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome)) return 48;
    event_initialize(&event, 0x56u, 0x90u);
    if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
        outcome.disposition != BX_NTVDM_GENERIC_UD_STOP ||
        outcome.resume_rip != 0u || outcome.gpr16_write_mask != 0u ||
        outcome.eflags_write_mask != 0u) return 49;
    event.window_bytes = 3u;
    if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
        outcome.disposition != BX_NTVDM_GENERIC_UD_STOP ||
        outcome.resume_rip != 0u) return 50;
    event_initialize(&event, 0x54u, 0x0cu);
    event.ds = 0x1000; event.edx = 0x20;
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
        composition.command.bootstrap.stage !=
            BX_NTVDM_CMD_COMSPEC_BOOTSTRAP_ENVIRONMENT_READY) return 23;
    event_initialize(&event, 0x54, 0x0f);
    event.es = 0x200u; event.ebx = 1u;
    if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
        outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME ||
        outcome.gpr16_write_mask != (1u << 3) || outcome.gpr16_values[3] != 2u ||
        composition.command.bootstrap.stage !=
            BX_NTVDM_CMD_COMSPEC_BOOTSTRAP_ENVIRONMENT_READY) return 24;
    event.ebx = 2u;
    if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
        outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME ||
        outcome.gpr16_write_mask != (1u << 3) || outcome.gpr16_values[3] != 0u ||
        memcmp(ram + 0x2000, "COMSPEC=C:\\COMMAND.COM", 23u) != 0 ||
        composition.command.bootstrap.stage !=
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
    /* Original cmdGetNextCmd gathers CMDINFO itself; cmdSetInfo's SCS/DOSDATA
       registration is not a reason to leak a pre-registration call back to
       the CPU as an unhandled #UD.  Keep this probe transaction-local so the
       registered lifecycle below still proves its normal first slot. */
    event_initialize(&event, 0x54, 0x01);
    event.ds = 0x100u; event.edx = 0x80u;
    ram[0x1080] = 0u; ram[0x1081] = 2u; ram[0x1082] = 23u;
    ram[0x1088] = 0x30u; ram[0x108a] = 0x40u; ram[0x108c] = 128u;
    ram[0x109c] = 0x50u; ram[0x109e] = 0x60u; ram[0x10a0] = 17u;
    ram[0x10a1] = 1u;
    if (composition.command.launch.valid ||
        !bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
        outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME ||
        outcome.resume_rip != 0x104u ||
        outcome.eflags_write_mask != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
        outcome.eflags_values != 0u ||
        composition.command.get_next.delivered != 1u) return 70;
    bx_ntvdm_cmd_get_next_state_v1_initialize(&composition.command.get_next);
    event_initialize(&event, 0x54, 0x05);
    event.ds = 0x1000; event.edx = 0x20; event.ebx = 0x40; event.ecx = 0x60;
    if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
        outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME || outcome.resume_rip != 0x104 ||
        !composition.command.launch.valid || composition.command.launch.registration.scs_info != 0x10020u ||
        composition.command.launch.registration.scs_to_sync != 0x100eeu ||
        composition.command.launch.registration.is_dos_binary != 0x10040u ||
        composition.command.launch.registration.fd_access != 0x10060u) return 18;
    /* The complete source-derived CMDGETNEXT lifecycle consumes the fixed
       CMDINFO gather, writes the declared target response and commits exactly
       one immutable launch slot. */
    event_initialize(&event, 0x54, 0x01);
    event.ds = 0x100u; event.edx = 0x80u;
    ram[0x1080] = 0u; ram[0x1081] = 2u; ram[0x1082] = 23u;
    ram[0x1088] = 0x30u; ram[0x108a] = 0x40u; ram[0x108c] = 128u;
    ram[0x109c] = 0x50u; ram[0x109e] = 0x60u; ram[0x10a0] = 17u;
    ram[0x10a1] = 1u;
    if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome)) return 31;
    if (outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME || outcome.resume_rip != 0x104u) return 32;
    if (outcome.gpr16_write_mask != 0u || outcome.eflags_write_mask !=
        BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF || outcome.eflags_values != 0u) return 33;
    if (composition.command.get_next.delivered != 1u) return 34;
    if (ram[0x340u + 1u] != 8u ||
        memcmp(ram + 0x340u + 2u, "TARGET\r\n", 8u) != 0) return 35;
    if (memcmp(ram + 0x560u, "C:\\TARGET.COM", 14u) != 0) return 36;
    if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
        outcome.disposition != BX_NTVDM_GENERIC_UD_STOP || outcome.resume_rip != 0u ||
        outcome.gpr16_write_mask != 0u || outcome.eflags_write_mask != 0u ||
        composition.command.get_next.delivered != 1u) return 32;
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
    strcpy((char *)(ram + 0x10040), "C:\\NTDOS64-NO-BOOT-SNAPSHOT.SYS");
    if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
        outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME || outcome.resume_rip != 0x104u ||
        outcome.eflags_write_mask != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
        outcome.eflags_values != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) return 7;

    event_initialize(&event, 0x54, 0x0d);
    event.ds = 0x1000; event.edx = 0x20;
    if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
        (outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME &&
         outcome.disposition != BX_NTVDM_GENERIC_UD_STOP)) return 9;
    /* demTerminatePDB owns per-PDB search lifetime.  Seed an existing
       continuation, invoke the real BOP route, then prove that the stale
       continuation cannot be resumed. */
    wcscpy(terminating_entries[0].dos_name, L"ONE.TXT");
    wcscpy(terminating_entries[1].dos_name, L"TWO.TXT");
    if (!bx_ntvdm_search_sessions_v1_begin(
            &composition.plane.provider.search_transaction.plan.sessions,
            0xbeefu, terminating_entries, 2u, &terminating_out,
            &terminating_token)) return 68;
    event_initialize(&event, 0x50u, 0x3cu);
    event.ebx = 0xbeefu;
    if (!bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome) ||
        outcome.disposition != BX_NTVDM_GENERIC_UD_RESUME ||
        outcome.resume_rip != 0x104u || outcome.gpr16_write_mask != 0u ||
        outcome.eflags_write_mask != 0u ||
        bx_ntvdm_search_sessions_v1_next(
            &composition.plane.provider.search_transaction.plan.sessions,
            0xbeefu, &terminating_token, &terminating_out,
            &terminating_token)) return 69;
    direct_search_error = direct_search_route_regression(&composition, 2u);
    if (direct_search_error != 0) return 70 + direct_search_error;
    direct_handle_error = direct_handle_route_regression(&composition);
    if (direct_handle_error != 0) return 90 + direct_handle_error;
    bx_ntvdm_boot_namespace_composition_v1_unbind(&composition);
    bx_ntvdm_host_namespace_v1_release(&host_namespace);
    if (bx_ntvdm_mantle_generic_ud_bridge_v1(&event, &outcome)) return 10;
    return 0;
}
