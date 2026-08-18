#include "bx_ntvdm_boot_namespace_plane_v1.h"
#include "bx_ntvdm_bop_ingress_v1.h"
#include "bx_ntvdm_bop_provider_registry_v1.h"
#include "bx_ntvdm_dem_package_session_v1.h"
#include "bx_ntvdm_host_drive_policy.h"
#include "bx_ntvdm_host_namespace.h"
#include "bx_ntvdm_host_volume_snapshot_v1.h"
#include "bx_ntvdm_search_transaction_v1.h"
#include <string.h>
#include <time.h>

/* This fixture deliberately contains no bound global composition, COMMAND
 * session, or native CPU trace.  It proves only that every canonical
 * `50:00..48` identity has a typed session route; it is not evidence that
 * the historical DEM providers are completely recovered. */
static uint8_t ram[0x100000];
static uint16_t dispatch_ax;
static uint32_t dispatch_dpb_sentinel;
static uint16_t dispatch_bx, dispatch_ds, dispatch_dx, dispatch_cx, dispatch_si;
static uint16_t dispatch_es, dispatch_di, dispatch_bp;
static uint32_t dispatch_mode;

static int volume_snapshot_regression(void)
{
    bx_ntvdm_host_drive_snapshot_v1 drives;
    bx_ntvdm_host_volume_record_v1 records[26] = {0};
    bx_ntvdm_host_volume_snapshot_v1 volumes;
    uint8_t types[26] = {0};
    types[2] = 3u;
    records[2].available = 1u;
    records[2].serial_number = 0x12345678u;
    records[2].sectors_per_cluster = 4u;
    records[2].bytes_per_sector = 512u;
    records[2].free_clusters = 100u;
    records[2].total_clusters = 200u;
    memcpy(records[2].label, L"NTDOS64", sizeof(L"NTDOS64"));
    memcpy(records[2].file_system, L"FAT", sizeof(L"FAT"));
    /* Data for a non-admitted drive is intentionally discarded at the
     * adapter boundary rather than becoming an ambient guest capability. */
    records[3] = records[2];
    return bx_ntvdm_host_drive_snapshot_v1_apply(4u, types, 0u, 0u, &drives) &&
        bx_ntvdm_host_volume_snapshot_v1_apply(&drives, records, &volumes) &&
        bx_ntvdm_host_volume_snapshot_v1_valid(&volumes) &&
        volumes.volumes[2].available == 1u &&
        volumes.volumes[2].serial_number == 0x12345678u &&
        volumes.volumes[3].available == 0u;
}

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

static int dispatch(uint8_t service, bx_ntvdm_cpu_result_v2 *result)
{
    uint8_t ntdos_bytes[] = { 0xf4u };
    uint8_t command_bytes[] = { 0xf4u };
    uint8_t target_bytes[] = { 0xf4u };
    uint8_t drive_types[26] = { 0 }, cwd_types[26] = { 0 };
    uint8_t bytes[4] = { 0xc4u, 0xc4u, 0x50u, service };
    byob_image ntdos = { ntdos_bytes, sizeof(ntdos_bytes) };
    byob_image command = { command_bytes, sizeof(command_bytes) };
    byob_image target = { target_bytes, sizeof(target_bytes) };
    byob_profile_selection profile;
    bx_ntvdm_boot_namespace_plane_v1 namespace_plane;
    bx_ntvdm_dem_package_session_v1 session;
    bx_ntvdm_host_drive_snapshot_v1 drives, cwd_drives;
    bx_ntvdm_host_volume_snapshot_v1 volumes;
    bx_ntvdm_host_namespace_v1 host_namespace;
    bx_ntvdm_host_volume_record_v1 volume_records[26] = {0};
    bx_ntvdm_mutation_profile_v1 mutation_profile;
    bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_bop_ingress_v1 ingress;
    bx_ntvdm_bop_provider_selection_v1 selection;
    bx_ntvdm_exception_event_v1 event;
    bx_ntvdm_cpu_state_v1 cpu;
    int dispatched;

    profile_initialize(&profile);
    drive_types[0] = 2u;
    drive_types[1] = 4u;
    drive_types[2] = 3u;
    volume_records[0].available = 1u;
    volume_records[0].serial_number = 0x78563412u;
    volume_records[0].sectors_per_cluster = 4u;
    volume_records[0].bytes_per_sector = 512u;
    volume_records[0].free_clusters = 100u;
    volume_records[0].total_clusters = 200u;
    memcpy(volume_records[0].label, L"VOL", sizeof(L"VOL"));
    memcpy(volume_records[0].file_system, L"FAT", sizeof(L"FAT"));
    bx_ntvdm_mutation_profile_v1_initialize(&mutation_profile, dispatch_mode);
    if (!bx_ntvdm_boot_namespace_plane_v1_initialize(&namespace_plane,
            &ntdos, &command, &target, 0, &profile) ||
        !bx_ntvdm_dem_package_session_v1_initialize(&session, &namespace_plane) ||
        !bx_ntvdm_dem_profile_consumer_v1_register_class(&mutation_profile,
            BX_NTVDM_MUTATION_CLASS_V1_SESSION_CONTEXT, 0x0fu) ||
        !bx_ntvdm_dem_package_session_v1_set_mutation_profile(&session,
            &mutation_profile) ||
        !bx_ntvdm_host_drive_snapshot_v1_apply(7u, drive_types, 0u, 0u, &drives) ||
        !bx_ntvdm_host_volume_snapshot_v1_apply(&drives, volume_records, &volumes) ||
        !bx_ntvdm_dem_package_session_v1_set_drive_snapshot(&session, &drives) ||
        !bx_ntvdm_dem_package_session_v1_set_volume_snapshot(&session, &volumes)) return 0;
    /* Direct CWD is an OpenNT host capability: exercise it against the
     * admitted real C: root in this child fixture.  Readonly receives the
     * same namespace only for validation and must still reject mutation. */
    cwd_types[2] = (uint8_t)GetDriveTypeW(L"C:\\");
    if (cwd_types[2] == DRIVE_NO_ROOT_DIR || cwd_types[2] == DRIVE_UNKNOWN ||
        !bx_ntvdm_host_drive_snapshot_v1_apply(UINT32_C(1) << 2u, cwd_types,
            0u, 0u, &cwd_drives) ||
        !bx_ntvdm_host_namespace_v1_initialize(&host_namespace, &cwd_drives) ||
        !bx_ntvdm_dem_package_session_v1_set_drive_view_host_namespace(&session,
            &host_namespace)) {
        bx_ntvdm_host_namespace_v1_release(&host_namespace);
        return 0;
    }
    memset(&event, 0, sizeof(event));
    event.magic = BX_NTVDM_EXCEPTION_ABI_MAGIC;
    event.abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION;
    event.struct_bytes = sizeof(event);
    event.kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION;
    event.cpu_id = 1u;
    event.vector = 6u;
    event.fault_rip = 0x100u;
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.eax = dispatch_ax;
    cpu.ebx = dispatch_bx; cpu.ds = dispatch_ds; cpu.edx = dispatch_dx; cpu.ecx = dispatch_cx;
    cpu.esi = dispatch_si;
    cpu.es = dispatch_es;
    cpu.edi = dispatch_di;
    cpu.ebp = dispatch_bp;
    if (service == 0x25u && dispatch_dpb_sentinel != 0u)
        memset(ram, 0xa5, 35u);
    bx_ntvdm_instruction_window_v1_capture(&window, bytes, sizeof(bytes));
    dispatched = bx_ntvdm_bop_ingress_v1_classify(&window, &ingress) &&
        bx_ntvdm_bop_provider_registry_v1_select(&ingress, &selection) &&
        bx_ntvdm_dem_package_session_v1_dispatch(&session, &ingress,
            &selection, &event, &cpu, &window, result);
    bx_ntvdm_host_namespace_v1_release(&host_namespace);
    return dispatched;
}

static int fcb_search_core_regression(void)
{
    bx_ntvdm_search_transaction_v1 search;
    bx_ntvdm_host_namespace_entry_v1 entries[2];
    bx_ntvdm_exception_event_v1 event;
    bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_multi_write_transaction_v1 transaction;
    uint8_t path[128] = { 'C', ':', '\\', '*', '.', '*', 0 };
    uint8_t payload[51], searchbuf[53] = { 0 };
    uint32_t bytes;
    memset(entries, 0, sizeof(entries));
    memcpy(entries[0].dos_name, L"COMMAND.COM", sizeof(L"COMMAND.COM"));
    memcpy(entries[1].dos_name, L"TARGET.COM", sizeof(L"TARGET.COM"));
    entries[0].byte_count = 1u; entries[1].byte_count = 1u;
    entries[0].dos_date = entries[1].dos_date = 1u;
    memset(&event, 0, sizeof(event));
    event.magic = BX_NTVDM_EXCEPTION_ABI_MAGIC;
    event.abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION;
    event.struct_bytes = sizeof(event);
    event.kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION;
    event.vector = 6u; event.fault_rip = 0x100u;
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    bx_ntvdm_search_transaction_v1_initialize(&search);
    if (!bx_ntvdm_search_transaction_v1_fcb_first_entries(&search, entries, 2u,
            2u, &event, &cpu, 0x1234u, 0x200u, path, 0u, 0u,
            &transaction, payload, &bytes) || bytes != 51u ||
        transaction.result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME)
        return 0;
    memcpy(searchbuf + 1u, payload, 19u);
    memcpy(searchbuf + 20u, payload + 19u, 32u);
    if (!bx_ntvdm_search_transaction_v1_fcb_next(&search, &event, &cpu,
            0x1234u, 0x200u, searchbuf, &transaction, payload, &bytes) ||
        bytes != 51u || transaction.result.disposition !=
            BX_NTVDM_CPU_RESULT_V2_RESUME) return 0;
    memset(searchbuf + 32u, 0, 8u);
    if (bx_ntvdm_search_transaction_v1_fcb_next(&search, &event, &cpu,
            0x1234u, 0x200u, searchbuf, &transaction, payload, &bytes) != 0 ||
        bytes != 8u || transaction.result.cpu_delta.gpr16_write_mask != 1u ||
        transaction.result.cpu_delta.gpr16_values[0] != 0x12u ||
        transaction.result.eflags_values != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF)
        return 0;
    memset(path, 0, sizeof(path));
    memcpy(path, "C:\\*.TXT", sizeof("C:\\*.TXT"));
    if (bx_ntvdm_search_transaction_v1_fcb_first_entries(&search, entries, 2u,
            2u, &event, &cpu, 0x1234u, 0x200u, path, 0u, 0u,
            &transaction, payload, &bytes) != 0 || bytes != 8u ||
        transaction.result.cpu_delta.gpr16_write_mask != 1u ||
        transaction.result.cpu_delta.gpr16_values[0] != 0x12u ||
        transaction.result.eflags_values != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF)
        return 0;
    memset(path, 0, sizeof(path));
    memcpy(path, "C:\\*.COM", sizeof("C:\\*.COM"));
    if (bx_ntvdm_search_transaction_v1_fcb_first_entries(&search, entries, 2u,
            2u, &event, &cpu, 0x1234u, 0x200u, path, 0u, 0u,
            &transaction, payload, &bytes) != 1 || bytes != 51u)
        return 0;
    memset(searchbuf, 0, sizeof(searchbuf));
    memcpy(searchbuf + 1u, payload, 19u);
    memcpy(searchbuf + 20u, payload + 19u, 32u);
    if (bx_ntvdm_search_transaction_v1_fcb_next(&search, &event, &cpu,
            0x5678u, 0x200u, searchbuf, &transaction, payload, &bytes) != 0 ||
        bytes != 8u || transaction.result.cpu_delta.gpr16_write_mask != 1u ||
        transaction.result.cpu_delta.gpr16_values[0] != 0x12u ||
        transaction.result.eflags_values != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF)
        return 0;
    /* A continuation is opaque.  A nonzero but altered token must have the
     * same DOS-visible no-more-files result as an absent one; it must not be
     * interpreted as a host handle or select an arbitrary session. */
    if (bx_ntvdm_search_transaction_v1_fcb_first_entries(&search, entries, 2u,
            2u, &event, &cpu, 0x1234u, 0x200u, path, 0u, 0u,
            &transaction, payload, &bytes) != 1 || bytes != 51u)
        return 0;
    memset(searchbuf, 0, sizeof(searchbuf));
    memcpy(searchbuf + 1u, payload, 19u);
    memcpy(searchbuf + 20u, payload + 19u, 32u);
    searchbuf[32u] ^= 0xffu;
    if (bx_ntvdm_search_transaction_v1_fcb_next(&search, &event, &cpu,
            0x1234u, 0x200u, searchbuf, &transaction, payload, &bytes) != 0 ||
        bytes != 8u || transaction.result.cpu_delta.gpr16_write_mask != 1u ||
        transaction.result.cpu_delta.gpr16_values[0] != 0x12u ||
        transaction.result.eflags_values != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF)
        return 0;
    /* A one-entry search still has a valid first result, but its first next
     * call exhausts the same per-PDB continuation deterministically. */
    bx_ntvdm_search_transaction_v1_release(&search);
    bx_ntvdm_search_transaction_v1_initialize(&search);
    if (bx_ntvdm_search_transaction_v1_fcb_first_entries(&search, entries, 1u,
            2u, &event, &cpu, 0x1234u, 0x200u, path, 0u, 0u,
            &transaction, payload, &bytes) != 1 || bytes != 51u)
        return 0;
    memset(searchbuf, 0, sizeof(searchbuf));
    memcpy(searchbuf + 1u, payload, 19u);
    memcpy(searchbuf + 20u, payload + 19u, 32u);
    if (bx_ntvdm_search_transaction_v1_fcb_next(&search, &event, &cpu,
            0x1234u, 0x200u, searchbuf, &transaction, payload, &bytes) != 0 ||
        bytes != 8u || transaction.result.cpu_delta.gpr16_write_mask != 1u ||
        transaction.result.cpu_delta.gpr16_values[0] != 0x12u ||
        transaction.result.eflags_values != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF)
        return 0;
    bx_ntvdm_search_transaction_v1_release(&search);
    return 1;
}

/* Returns zero on success; nonzero values identify a bounded FCB transaction stage. */
static int fcb_bop_transaction_regression(void)
{
    uint8_t ntdos_bytes[] = { 0xf4u }, command_bytes[] = { 0xf4u }, target_bytes[] = { 0xf4u };
    uint8_t drive_types[26] = { 0 }, bytes[4] = { 0xc4u, 0xc4u, 0x50u, 0x0au };
    byob_image ntdos = { ntdos_bytes, sizeof(ntdos_bytes) };
    byob_image command = { command_bytes, sizeof(command_bytes) };
    byob_image target = { target_bytes, sizeof(target_bytes) };
    byob_profile_selection profile;
    bx_ntvdm_boot_namespace_plane_v1 plane;
    bx_ntvdm_dem_package_session_v1 session;
    bx_ntvdm_host_drive_snapshot_v1 drives;
    bx_ntvdm_dem_dta_registration_v1 dta = { 0x300u, 0x350u, 0x360u, 0x370u };
    bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_bop_ingress_v1 ingress;
    bx_ntvdm_bop_provider_selection_v1 selection;
    bx_ntvdm_exception_event_v1 event;
    bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_cpu_result_v2 result;
    profile_initialize(&profile); drive_types[2] = 3u;
    if (!bx_ntvdm_boot_namespace_plane_v1_initialize(&plane, &ntdos, &command,
            &target, 0, &profile)) return 1;
    if (!bx_ntvdm_dem_package_session_v1_initialize(&session, &plane)) return 2;
    if (!bx_ntvdm_host_drive_snapshot_v1_apply(4u, drive_types, 0u, 0u, &drives)) return 3;
    if (!bx_ntvdm_dem_package_session_v1_set_drive_snapshot(&session, &drives)) return 4;
    if (!bx_ntvdm_boot_namespace_plane_v1_set_dta(&plane, &dta)) return 5;
    memset(ram, 0, sizeof(ram)); ram[0x350u] = 0x34u; ram[0x351u] = 0x12u;
    /* The profile exposes exactly COMMAND.COM and TARGET.COM through this
     * pattern, so the three calls below prove first, continuation, then the
     * source-derived no-more-files result without assuming the total boot
     * namespace size. */
    memcpy(ram + 0x500u, "C:\\*.COM", sizeof("C:\\*.COM"));
    memset(&event, 0, sizeof(event)); event.magic = BX_NTVDM_EXCEPTION_ABI_MAGIC;
    event.abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION; event.struct_bytes = sizeof(event);
    event.kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION; event.vector = 6u; event.fault_rip = 0x100u;
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.esi = 0x400u; cpu.edi = 0x500u;
    bx_ntvdm_instruction_window_v1_capture(&window, bytes, sizeof(bytes));
    if (!bx_ntvdm_bop_ingress_v1_classify(&window, &ingress)) return 2;
    if (!bx_ntvdm_bop_provider_registry_v1_select(&ingress, &selection)) return 3;
    if (!bx_ntvdm_dem_package_session_v1_dispatch(&session, &ingress, &selection,
            &event, &cpu, &window, &result)) return 4;
    if (result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME) return 5;
    if (result.eflags_values != 0u) return 6;
    if ((ram[0x420u] | ram[0x421u] | ram[0x422u] | ram[0x423u] |
         ram[0x424u] | ram[0x425u] | ram[0x426u] | ram[0x427u]) == 0u) return 11;
    bytes[3] = 0x0cu; bx_ntvdm_instruction_window_v1_capture(&window, bytes, sizeof(bytes));
    if (!bx_ntvdm_bop_ingress_v1_classify(&window, &ingress) ||
        !bx_ntvdm_bop_provider_registry_v1_select(&ingress, &selection) ||
        !bx_ntvdm_dem_package_session_v1_dispatch(&session, &ingress, &selection,
            &event, &cpu, &window, &result) || result.disposition !=
            BX_NTVDM_CPU_RESULT_V2_RESUME || result.eflags_values != 0u) return 8;
    if (!bx_ntvdm_dem_package_session_v1_dispatch(&session, &ingress, &selection,
            &event, &cpu, &window, &result) || result.cpu_delta.gpr16_write_mask != 1u ||
        result.cpu_delta.gpr16_values[0] != 0x12u || result.eflags_values !=
            BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) return 9;
    memset(ram + 0x500u, 0, 128u);
    memcpy(ram + 0x500u, "C:\\*.TXT", sizeof("C:\\*.TXT"));
    bytes[3] = 0x0au;
    bx_ntvdm_instruction_window_v1_capture(&window, bytes, sizeof(bytes));
    if (!bx_ntvdm_bop_ingress_v1_classify(&window, &ingress) ||
        !bx_ntvdm_bop_provider_registry_v1_select(&ingress, &selection) ||
        !bx_ntvdm_dem_package_session_v1_dispatch(&session, &ingress, &selection,
            &event, &cpu, &window, &result) || result.disposition !=
            BX_NTVDM_CPU_RESULT_V2_RESUME || result.cpu_delta.gpr16_write_mask != 1u ||
        result.cpu_delta.gpr16_values[0] != 0x12u || result.eflags_values !=
            BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF || ram[0x420u] != 0u ||
        ram[0x421u] != 0u || ram[0x422u] != 0u || ram[0x423u] != 0u ||
         ram[0x424u] != 0u || ram[0x425u] != 0u || ram[0x426u] != 0u ||
         ram[0x427u] != 0u) return 10;
    /* The FCB first-search path requires a 128-byte ES:DI pathname read.
     * An aperture-crossing pointer must not reach a host namespace lookup or
     * leave a pending mechanical action; its FCB-family unavailable result is
     * deterministic. */
    cpu.es = 0xffffu; cpu.edi = 0x10u;
    bytes[3] = 0x0au;
    bx_ntvdm_instruction_window_v1_capture(&window, bytes, sizeof(bytes));
    if (!bx_ntvdm_bop_ingress_v1_classify(&window, &ingress) ||
        !bx_ntvdm_bop_provider_registry_v1_select(&ingress, &selection) ||
        !bx_ntvdm_dem_package_session_v1_dispatch(&session, &ingress, &selection,
            &event, &cpu, &window, &result) || result.disposition !=
            BX_NTVDM_CPU_RESULT_V2_RESUME || result.cpu_delta.gpr16_write_mask != 1u ||
        result.cpu_delta.gpr16_values[0] != 5u || result.eflags_write_mask !=
            BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF || result.eflags_values !=
            BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) return 11;
    return 0;
}

/* This is deliberately package-level rather than a detached token unit test:
 * it reaches the selected 50:3C route and the subsequent 50:02 close through
 * one Direct DEM session.  OpenNT executes SVC_PDBTERMINATE before DOS_ABORT,
 * whose JFT/SFT walk emits SVC_DEMCLOSE; therefore 3C may release search
 * continuations but must not pre-close the opaque Direct SFT token. */
static int direct_pdb_lifecycle_regression(void)
{
    uint8_t ntdos_bytes[] = { 0xf4u }, command_bytes[] = { 0xf4u };
    uint8_t target_bytes[] = { 0xf4u }, drive_types[26] = { 0u };
    uint8_t bytes[] = { 0xc4u, 0xc4u, 0x50u, 0x3cu };
    byob_image ntdos = { ntdos_bytes, sizeof(ntdos_bytes) };
    byob_image command = { command_bytes, sizeof(command_bytes) };
    byob_image target = { target_bytes, sizeof(target_bytes) };
    byob_profile_selection profile;
    bx_ntvdm_boot_namespace_plane_v1 plane;
    bx_ntvdm_dem_package_session_v1 session;
    bx_ntvdm_host_drive_snapshot_v1 drives;
    bx_ntvdm_host_namespace_v1 host_namespace;
    bx_ntvdm_mutation_profile_v1 mutation_profile;
    bx_ntvdm_instruction_window_v1 window;
    bx_ntvdm_bop_ingress_v1 ingress;
    bx_ntvdm_bop_provider_selection_v1 selection;
    bx_ntvdm_exception_event_v1 event;
    bx_ntvdm_cpu_state_v1 cpu;
    bx_ntvdm_cpu_result_v2 result;
    wchar_t temporary[MAX_PATH], path[MAX_PATH];
    HANDLE file = INVALID_HANDLE_VALUE, looked_up = INVALID_HANDLE_VALUE;
    DWORD written = 0u;
    uint32_t token = 0u;
    int status = 1;

    memset(&session, 0, sizeof(session));
    memset(&host_namespace, 0, sizeof(host_namespace));
    profile_initialize(&profile);
    bx_ntvdm_mutation_profile_v1_initialize(&mutation_profile,
        BX_NTVDM_MUTATION_MODE_V1_DIRECT);
    if (!bx_ntvdm_dem_profile_consumer_v1_register_class(&mutation_profile,
            BX_NTVDM_MUTATION_CLASS_V1_SESSION_CONTEXT, 0x0fu) ||
        !bx_ntvdm_dem_profile_consumer_v1_register_class(&mutation_profile,
            BX_NTVDM_MUTATION_CLASS_V1_NAMESPACE_CONTENT, 0x0fu) ||
        !bx_ntvdm_dem_profile_consumer_v1_register_class(&mutation_profile,
            BX_NTVDM_MUTATION_CLASS_V1_FILE_METADATA, 0x0fu)) goto cleanup;
    drive_types[2] = (uint8_t)GetDriveTypeW(L"C:\\");
    if (drive_types[2] == DRIVE_NO_ROOT_DIR || drive_types[2] == DRIVE_UNKNOWN ||
        !bx_ntvdm_boot_namespace_plane_v1_initialize(&plane, &ntdos, &command,
            &target, 0, &profile) ||
        !bx_ntvdm_dem_package_session_v1_initialize(&session, &plane) ||
        !bx_ntvdm_host_drive_snapshot_v1_apply(UINT32_C(1) << 2u, drive_types,
            0u, 0u, &drives) ||
        !bx_ntvdm_dem_package_session_v1_set_drive_snapshot(&session, &drives) ||
        !bx_ntvdm_dem_package_session_v1_set_mutation_profile(&session,
            &mutation_profile) ||
        !bx_ntvdm_host_namespace_v1_initialize(&host_namespace, &drives) ||
        !bx_ntvdm_dem_package_session_v1_set_host_namespace(&session,
            &host_namespace)) goto cleanup;
    if (!GetTempPathW(MAX_PATH, temporary) ||
        !GetTempFileNameW(temporary, L"n64", 0u, path)) goto cleanup;
    file = CreateFileW(path, GENERIC_READ | GENERIC_WRITE, 0, 0, CREATE_ALWAYS,
        FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE, 0);
    if (file == INVALID_HANDLE_VALUE || !WriteFile(file, "ONE", 3u, &written, 0) ||
        written != 3u || !bx_ntvdm_dem_file_session_v1_adopt_owned(
            &session.whole_provider.files, file, 0xbeefu, &token)) goto cleanup;
    file = INVALID_HANDLE_VALUE;
    memset(&event, 0, sizeof(event));
    event.magic = BX_NTVDM_EXCEPTION_ABI_MAGIC;
    event.abi_version = BX_NTVDM_EXCEPTION_ABI_VERSION;
    event.struct_bytes = sizeof(event);
    event.kind = BX_NTVDM_EXCEPTION_EVENT_CPU_EXCEPTION;
    event.cpu_id = 1u; event.vector = 6u; event.fault_rip = 0x100u;
    bx_ntvdm_cpu_state_v1_initialize(&cpu, BX_NTVDM_CPU_EXECUTION_REAL);
    cpu.ebx = 0xbeefu;
    bx_ntvdm_instruction_window_v1_capture(&window, bytes, sizeof(bytes));
    if (!bx_ntvdm_bop_ingress_v1_classify(&window, &ingress) ||
        !bx_ntvdm_bop_provider_registry_v1_select(&ingress, &selection) ||
        !bx_ntvdm_dem_package_session_v1_dispatch(&session, &ingress,
            &selection, &event, &cpu, &window, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        result.resume_rip != 0x104u || result.cpu_delta.gpr16_write_mask != 0u ||
        result.eflags_write_mask != 0u ||
        !bx_ntvdm_dem_file_session_v1_lookup(&session.whole_provider.files,
            token, &looked_up)) goto cleanup;
    bytes[3] = 0x02u;
    cpu.eax = token >> 16u; cpu.ebp = token & 0xffffu;
    bx_ntvdm_instruction_window_v1_capture(&window, bytes, sizeof(bytes));
    if (!bx_ntvdm_bop_ingress_v1_classify(&window, &ingress) ||
        !bx_ntvdm_bop_provider_registry_v1_select(&ingress, &selection) ||
        !bx_ntvdm_dem_package_session_v1_dispatch(&session, &ingress,
            &selection, &event, &cpu, &window, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        result.eflags_values != 0u || bx_ntvdm_dem_file_session_v1_lookup(
            &session.whole_provider.files, token, &looked_up)) goto cleanup;
    status = 0;
cleanup:
    if (file != INVALID_HANDLE_VALUE) { CloseHandle(file); DeleteFileW(path); }
    bx_ntvdm_dem_package_session_v1_teardown(&session);
    bx_ntvdm_host_namespace_v1_release(&host_namespace);
    return status;
}
static int misc_family_regression(void)
{
    static const uint8_t cli_no_debug_services[] = {
        0x1du, 0x23u, 0x34u, 0x35u, 0x36u, 0x37u, 0x38u, 0x39u,
        0x3au, 0x3eu, 0x45u
    };
    bx_ntvdm_cpu_result_v2 result;
    uint32_t index;

    /* These are original ordinary-return branches in a non-debug, non-VDD,
     * non-WOW, no-device CLI profile.  `50:3C` is intentionally excluded:
     * it is a PDB lifecycle notification whose retained search-state cleanup
     * has its own owner regression. */
    for (index = 0u; index < sizeof(cli_no_debug_services); ++index) {
        if (!dispatch(cli_no_debug_services[index], &result) ||
            result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
            result.resume_rip != 0x104u ||
            result.cpu_delta.gpr16_write_mask != 0u ||
            result.eflags_write_mask != 0u) return 0;
    }

    /* demIsDebug is deliberately different: demmisc.c calls setAL(), so
     * contained no-debug must clear AL but preserve AH and all flags. */
    dispatch_ax = 0xa5ffu;
    if (!dispatch(0x3bu, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        result.resume_rip != 0x104u || result.cpu_delta.gpr16_write_mask != 1u ||
        result.cpu_delta.gpr16_values[0] != 0xa500u ||
        result.eflags_write_mask != 0u) return 0;
    dispatch_ax = 0u;
    /* demNotYetImplemented clears CF.  50:24 must not be intercepted by
     * the 50:36 entry lifecycle provider merely because its decimal index is
     * also 36. */
    if (!dispatch(0x24u, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        result.resume_rip != 0x104u || result.cpu_delta.gpr16_write_mask != 0u ||
        result.eflags_write_mask != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
        result.eflags_values != 0u) return 0;
    return 1;
}

int main(int argc, char **argv)
{
    uint32_t service;
    if (argc != 2 && argc != 3) return 93;
    if (strcmp(argv[1], "direct") == 0) {
        dispatch_mode = BX_NTVDM_MUTATION_MODE_V1_DIRECT;
    } else if (strcmp(argv[1], "readonly") == 0) {
        dispatch_mode = BX_NTVDM_MUTATION_MODE_V1_READONLY;
    } else {
        return 94;
    }
    if (argc == 3) {
        if (strcmp(argv[2], "pdb-lifecycle") != 0) return 96;
        return dispatch_mode == BX_NTVDM_MUTATION_MODE_V1_DIRECT ?
            direct_pdb_lifecycle_regression() : 97;
    }
    static const uint8_t fcb_unavailable[] = {
        0x07u, 0x20u, 0x2cu, 0x2du, 0x2fu, 0x31u
    };
    static const uint8_t readonly_namespace_mutations[] = {
        0x01u, 0x03u, 0x04u, 0x05u, 0x06u, 0x08u, 0x17u, 0x22u
    };
    static const uint8_t pipe_unavailable[] = { 0x47u, 0x48u };
    bx_ntvdm_cpu_result_v2 result;
    if (!volume_snapshot_regression()) return 89;
    if (!fcb_search_core_regression()) return 90;
    { int fcb_status = fcb_bop_transaction_regression(); if (fcb_status != 0) return 91 + fcb_status; }
    if (!misc_family_regression()) return 92;
    if (dispatch_mode == BX_NTVDM_MUTATION_MODE_V1_DIRECT &&
        direct_pdb_lifecycle_regression() != 0) return 95;
    for (service = 0u; service < 73u; ++service) {
        if (!dispatch((uint8_t)service, &result) ||
            !bx_ntvdm_cpu_result_v2_valid(&result) ||
            result.disposition == BX_NTVDM_CPU_RESULT_V2_PASS_THROUGH)
            return (int)(100u + service);
    }
    if (!dispatch(0x10u, &result)) return 179;
    if (ram[2u] != 0x12u || ram[3u] != 0x34u || ram[4u] != 0x56u ||
        ram[5u] != 0x78u || memcmp(ram + 6u, "VOL", 3u) != 0 ||
        memcmp(ram + 17u, "FAT", 3u) != 0) return 179;
    /* demGSetMediaID's set form is not a writable-volume capability.  The
     * original dispatcher takes the CF-only failure branch before it asks the
     * host for volume state; preserve that boundary even with an admitted
     * immutable snapshot. */
    dispatch_ax = 1u;
    if (!dispatch(0x10u, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        result.resume_rip != 0x104u || result.cpu_delta.gpr16_write_mask != 0u ||
        result.eflags_write_mask != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
        result.eflags_values != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) return 192;
    dispatch_ax = 0u;
    /* GSET is one state component: its ordinary drive query, immutable-volume
     * geometry, full DPB and compact DPB-list forms must agree on the same
     * admitted inventory.  These are package assertions, not trace-derived
     * endpoint tests. */
    dispatch_ax = 0xab00u;
    if (!dispatch(0x0du, &result) ||
        result.cpu_delta.gpr16_write_mask != 1u ||
        result.cpu_delta.gpr16_values[0] != 0xab03u ||
        result.eflags_write_mask != 0u) return 205;
    dispatch_ax = 0u;
    if (!dispatch(0x0eu, &result) ||
        result.cpu_delta.gpr16_write_mask != 0x1fu ||
        result.cpu_delta.gpr16_values[0] != 0u ||
        result.cpu_delta.gpr16_values[1] != 100u ||
        result.cpu_delta.gpr16_values[2] != 512u ||
        result.cpu_delta.gpr16_values[3] != 200u ||
        result.cpu_delta.gpr16_values[4] != 4u ||
        result.eflags_write_mask != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
        result.eflags_values != 0u) return 206;
    if (!dispatch(0x0fu, &result) ||
        result.cpu_delta.gpr16_write_mask != 1u ||
        result.cpu_delta.gpr16_values[0] != 3u ||
        result.eflags_write_mask != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
        result.eflags_values != 0u) return 207;
    /* All volume-observation services use demClientError's direct host-query
     * failure shape.  An unadmitted drive must neither query a live host
     * volume nor alter its caller buffer, and reports ERROR_INVALID_DRIVE. */
    dispatch_ax = 25u;
    if (!dispatch(0x0eu, &result) ||
        result.cpu_delta.gpr16_write_mask != 1u ||
        result.cpu_delta.gpr16_values[0] != 15u ||
        result.eflags_write_mask != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
        result.eflags_values != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) return 216;
    dispatch_ax = 0u; dispatch_bx = 25u;
    memset(ram + 2u, 0xa5, 23u);
    if (!dispatch(0x10u, &result) ||
        result.cpu_delta.gpr16_write_mask != 1u ||
        result.cpu_delta.gpr16_values[0] != 15u ||
        result.eflags_write_mask != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
        result.eflags_values != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
        ram[2u] != 0xa5u || ram[24u] != 0xa5u) return 217;
    dispatch_ax = 25u; dispatch_di = 0x200u;
    memset(ram + dispatch_di, 0xa5, 35u);
    if (!dispatch(0x25u, &result) ||
        result.cpu_delta.gpr16_write_mask != 1u ||
        result.cpu_delta.gpr16_values[0] != 15u ||
        result.eflags_write_mask != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
        result.eflags_values != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
        ram[dispatch_di] != 0xa5u || ram[dispatch_di + 34u] != 0xa5u) return 220;
    dispatch_ax = 0u; dispatch_di = 0u;
    dispatch_bx = 0u;
    /* demSetDate and demSetTime set AL only on failure.  The contained CLI
     * profile must retain that register contract without writing the ambient
     * host clock or synthesizing a carry result. */
    dispatch_ax = 0xa500u;
    if (!dispatch(0x19u, &result) ||
        result.cpu_delta.gpr16_write_mask != 1u ||
        result.cpu_delta.gpr16_values[0] != 0xa5ffu ||
        result.eflags_write_mask != 0u) return 218;
    dispatch_ax = 0x5a00u;
    if (!dispatch(0x1cu, &result) ||
        result.cpu_delta.gpr16_write_mask != 1u ||
        result.cpu_delta.gpr16_values[0] != 0x5affu ||
        result.eflags_write_mask != 0u) return 219;
    dispatch_ax = 0u;
    dispatch_di = 0x200u;
    memset(ram + dispatch_di, 0xa5, 35u);
    if (!dispatch(0x25u, &result) ||
        result.cpu_delta.gpr16_write_mask != 1u ||
        result.cpu_delta.gpr16_values[0] != 0u || ram[0x200u] != 0u ||
        ram[0x202u] != 0u || ram[0x203u] != 2u || ram[0x219u] != 0xf8u ||
        ram[0x21au] != 10u || ram[0x21bu] != 0xffu || ram[0x21cu] != 0xffu ||
        ram[0x21du] != 0xffu || ram[0x21eu] != 0xffu || ram[0x221u] != 100u ||
        ram[0x222u] != 0u || result.eflags_values != 0u) return 208;
    dispatch_es = 0u; dispatch_bp = 0x300u;
    memset(ram + dispatch_bp, 0xa5, 71u);
    if (!dispatch(0x46u, &result)) return 209;
    if (result.cpu_delta.gpr16_write_mask != (1u << 5) ||
        result.cpu_delta.gpr16_values[5] != 0x346u) return 211;
    if (ram[0x300u] != 0u || ram[0x301u] != 0u) return 212;
    if (ram[0x31bu] != 0x23u || ram[0x31cu] != 0x03u) return 213;
    if (ram[0x323u] != 2u || ram[0x324u] != 2u) return 214;
    if (ram[0x346u] != 0xa5u || result.eflags_write_mask != 0u) return 215;
    dispatch_ds = 0xffffu; dispatch_di = 0x10u;
    if (!dispatch(0x25u, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        result.cpu_delta.gpr16_write_mask != 1u || result.cpu_delta.gpr16_values[0] != 5u ||
        result.eflags_write_mask != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
        result.eflags_values != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) return 210;
    dispatch_ds = dispatch_es = dispatch_di = dispatch_bp = 0u;
    /* The no-device component still exposes the two original IOCTL metadata
     * subfunctions for an admitted immutable drive, while an ordinary IOCTL
     * takes ERROR_INVALID_FUNCTION rather than touching a host device. */
    dispatch_ax = 0x4408u; dispatch_bx = 2u;
    if (!dispatch(0x21u, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        result.cpu_delta.gpr16_write_mask != 1u ||
        result.cpu_delta.gpr16_values[0] != 1u ||
        result.eflags_write_mask != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
        result.eflags_values != 0u) return 203;
    dispatch_ax = 0x4409u;
    if (!dispatch(0x21u, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        result.cpu_delta.gpr16_write_mask != ((1u << 0) | (1u << 3)) ||
        result.cpu_delta.gpr16_values[0] != 0x4400u ||
        result.cpu_delta.gpr16_values[3] != 0x0800u ||
        result.eflags_write_mask != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
        result.eflags_values != 0u) return 204;
    /* The paired AL=08/09 source slice includes removable and remote forms,
     * plus the original invalid-drive branch.  Neither case may be absorbed
     * by the generic unsupported-IOCTL result. */
    dispatch_ax = 0x4408u; dispatch_bx = 0u;
    if (!dispatch(0x21u, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        result.cpu_delta.gpr16_write_mask != 1u ||
        result.cpu_delta.gpr16_values[0] != 0u ||
        result.eflags_write_mask != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
        result.eflags_values != 0u) return 217;
    dispatch_ax = 0x4409u; dispatch_bx = 1u;
    if (!dispatch(0x21u, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        result.cpu_delta.gpr16_write_mask != ((1u << 0) | (1u << 3)) ||
        result.cpu_delta.gpr16_values[0] != 0x4400u ||
        result.cpu_delta.gpr16_values[3] != 0x1000u ||
        result.eflags_write_mask != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
        result.eflags_values != 0u) return 218;
    dispatch_ax = 0x4408u; dispatch_bx = 3u;
    if (!dispatch(0x21u, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        result.cpu_delta.gpr16_write_mask != 1u ||
        result.cpu_delta.gpr16_values[0] != 15u ||
        result.eflags_write_mask != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
        result.eflags_values != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) return 219;
    dispatch_ax = 0u; dispatch_bx = 0u;
    dispatch_ds = 0x10u; dispatch_dx = 0x20u; dispatch_cx = 0xa5b7u;
    ram[0x120u] = 0xa5u;
    if (!dispatch(0x41u, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        result.resume_rip != 0x104u || result.cpu_delta.gpr16_write_mask != (1u << 2) ||
        result.eflags_write_mask != 0u) return 195;
    /* demgset.c exposes the actual host computer name through
     * GetComputerNameOem, sets CX=01ff on success, and has a source-defined
     * empty-name fallback which clears CH only.  The current provider uses
     * GetComputerNameW plus an explicit OEM conversion; this package fixture
     * accepts precisely those two original outcomes, never a fixture name. */
    if (result.cpu_delta.gpr16_values[2] == 0x01ffu) {
        if (ram[0x120u] == 0u || ram[0x12fu] != 0u) return 195;
    } else if (result.cpu_delta.gpr16_values[2] == 0x00b7u) {
        if (ram[0x120u] != 0u) return 195;
    } else return 195;
    dispatch_ds = dispatch_dx = dispatch_cx = 0u;
    /* demSetDefaultDrive first rejects a DS:SI string whose first byte does
     * not agree with DL.  Once that source guard passes, this CLI profile has
     * no writable current-directory backend, so it takes the explicit
     * unavailable result rather than consulting the host process directory. */
    dispatch_ds = 0x10u; dispatch_si = 0x20u; dispatch_dx = 2u;
    ram[0x120u] = (uint8_t)'D';
    if (!dispatch(0x1au, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        result.resume_rip != 0x104u || result.cpu_delta.gpr16_write_mask != 1u ||
        result.cpu_delta.gpr16_values[0] != 1u ||
        result.eflags_write_mask != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
        result.eflags_values != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) return 196;
    ram[0x120u] = (uint8_t)'C'; ram[0x121u] = ':';
    ram[0x122u] = '\\'; ram[0x123u] = 0u;
    if (!dispatch(0x1au, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        result.resume_rip != 0x104u ||
        result.eflags_write_mask != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
        (dispatch_mode == BX_NTVDM_MUTATION_MODE_V1_DIRECT &&
         (result.cpu_delta.gpr16_write_mask != 0u || result.eflags_values != 0u)) ||
        (dispatch_mode == BX_NTVDM_MUTATION_MODE_V1_READONLY &&
         (result.cpu_delta.gpr16_write_mask != 1u || result.cpu_delta.gpr16_values[0] != 5u ||
          result.eflags_values != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF))) return 197;
    /* The same immutable root capability repairs a CDS for QueryCurrentDir
     * and accepts SetCurrentDir only when it names that admitted root. */
    dispatch_ax = 2u; dispatch_ds = 0x10u; dispatch_si = 0x20u;
    memset(ram + 0x120u, 0xa5, 71u);
    if (!dispatch(0x13u, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        result.eflags_write_mask != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
        result.eflags_values != 0u || ram[0x120u] != 'C' ||
        ram[0x121u] != ':' || ram[0x122u] != '\\' || ram[0x123u] != 0u ||
        ram[0x165u] != 2u || ram[0x166u] != 0u) return 201;
    dispatch_ax = 0u; dispatch_dx = 0x20u;
    if (!dispatch(0x18u, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        (dispatch_mode == BX_NTVDM_MUTATION_MODE_V1_DIRECT &&
         (result.eflags_write_mask != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
          result.eflags_values != 0u)) ||
        (dispatch_mode == BX_NTVDM_MUTATION_MODE_V1_READONLY &&
         (result.cpu_delta.gpr16_write_mask != 1u || result.cpu_delta.gpr16_values[0] != 5u ||
          result.eflags_write_mask != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
          result.eflags_values != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF))) return 202;
    dispatch_ds = dispatch_dx = dispatch_cx = dispatch_si = 0u;
    /* All namespace mutations share one immutable-root policy.  Regress the
     * full source-owner family together so a new endpoint cannot silently
     * fall back to the generic package terminal result. */
    for (service = 0u; service < sizeof(readonly_namespace_mutations); ++service) {
        if (!dispatch(readonly_namespace_mutations[service], &result) ||
            result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
            result.cpu_delta.gpr16_write_mask != 1u ||
            result.cpu_delta.gpr16_values[0] != 5u ||
            result.eflags_write_mask != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
            result.eflags_values != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) return 216;
    }
    /* demCommit is in the same original handle component, but is not a
     * mutation failure: demhndl.c ignores FlushFileBuffers failure and clears
     * CF.  The adapter must preserve that contract without inventing a host
     * writable handle. */
    dispatch_ax = 0xa5a5u;
    if (!dispatch(0x27u, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        result.resume_rip != 0x104u || result.cpu_delta.gpr16_write_mask != 0u ||
        result.eflags_write_mask != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
        result.eflags_values != 0u) return 220;
    dispatch_ax = 0u;
    /* demCheckPath retains its original DOS device-directory success without
     * trying CreateFile("NUL") against the host.  Any other path stays inside
     * the immutable CLI namespace and returns the contained failure form. */
    dispatch_ds = 0x10u; dispatch_si = 0x20u; dispatch_dx = 2u;
    memset(ram + 0x120u, 0, 128u);
    memcpy(ram + 0x120u, "\\DEV\\", sizeof("\\DEV\\"));
    if (!dispatch(0x44u, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        result.resume_rip != 0x104u || result.cpu_delta.gpr16_write_mask != (1u << 2) ||
        result.cpu_delta.gpr16_values[2] != 0u ||
        result.eflags_write_mask != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
        result.eflags_values != 0u) return 222;
    memcpy(ram + 0x120u, "\\OTHER\\", sizeof("\\OTHER\\"));
    if (!dispatch(0x44u, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        result.resume_rip != 0x104u || result.cpu_delta.gpr16_write_mask !=
            ((1u << 0) | (1u << 2)) || result.cpu_delta.gpr16_values[0] != 5u ||
        result.cpu_delta.gpr16_values[2] != 0u ||
        result.eflags_write_mask != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
        result.eflags_values != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) return 223;
    dispatch_ds = dispatch_si = dispatch_dx = 0u;
    /* The complete pipe subfamily requires COMMAND's NT-handle pipe table
     * and asynchronous state.  That table is intentionally absent from the
     * CLI profile, so both identities explicitly reject the non-projected
     * AX:BP handle with ERROR_INVALID_HANDLE, not the generic AX=5 route. */
    for (service = 0u; service < sizeof(pipe_unavailable); ++service) {
        if (!dispatch(pipe_unavailable[service], &result) ||
            result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
            result.resume_rip != 0x104u || result.cpu_delta.gpr16_write_mask != 1u ||
            result.cpu_delta.gpr16_values[0] != 6u ||
            result.eflags_write_mask != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
            result.eflags_values != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) return 221;
    }
    /* FCB host-handle operations are a single contained unavailable family;
     * each must retain AX=5/CF rather than rely on the package catch-all. */
    for (service = 0u; service < sizeof(fcb_unavailable); ++service) {
        if (!dispatch(fcb_unavailable[service], &result) ||
            result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
            result.cpu_delta.gpr16_write_mask != 1u ||
            result.cpu_delta.gpr16_values[0] != 5u ||
            result.eflags_write_mask != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
            result.eflags_values != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) return 198;
    }
    dispatch_ax = 1u;
    if (!dispatch(0x2eu, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        result.cpu_delta.gpr16_write_mask != 1u ||
        result.cpu_delta.gpr16_values[0] != 5u ||
        result.eflags_write_mask != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
        result.eflags_values != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) return 199;
    dispatch_ax = 0u;
    /* `demClose` and DTA registration are both input-sensitive routes.  The
     * zeroed real-mode record intentionally takes the controlled failure
     * path, proving neither can leak a raw #UD / invalid typed result. */
    if (!dispatch(0x02u, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        result.resume_rip != 0x104u ||
        result.cpu_delta.gpr16_write_mask != 0u ||
        result.eflags_write_mask != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
        result.eflags_values != 0u) return 180;
    if (!dispatch(0x1bu, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        result.resume_rip != 0x104u ||
        result.cpu_delta.gpr16_write_mask != 1u ||
        result.cpu_delta.gpr16_values[0] != 5u ||
        result.eflags_write_mask != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
        result.eflags_values != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) return 181;
    if (!dispatch(0x03u, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        result.resume_rip != 0x104u ||
        result.cpu_delta.gpr16_write_mask != 1u ||
        result.cpu_delta.gpr16_values[0] != 5u ||
        result.eflags_write_mask != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
        result.eflags_values != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) return 182;
    if (!dispatch(0x2eu, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        result.resume_rip != 0x104u || result.cpu_delta.gpr16_write_mask != 0u ||
        result.eflags_write_mask != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
        result.eflags_values != 0u) return 183;
    if (!dispatch(0x30u, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        result.resume_rip != 0x104u ||
        result.cpu_delta.gpr16_write_mask != ((1u << 0) | (1u << 2)) ||
        result.eflags_write_mask != 0u) return 184;
    if (!dispatch(0x33u, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        result.cpu_delta.gpr16_write_mask != 1u ||
        result.cpu_delta.gpr16_values[0] != 0xffu ||
        result.eflags_write_mask != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
        result.eflags_values != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) return 185;
    if (!dispatch(0x3fu, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        result.cpu_delta.gpr16_write_mask != 1u ||
        result.cpu_delta.gpr16_values[0] != 5u ||
        result.eflags_write_mask != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
        result.eflags_values != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) return 186;
    if (!dispatch(0x23u, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        result.resume_rip != 0x104u || result.cpu_delta.gpr16_write_mask != 0u ||
        result.eflags_write_mask != 0u) return 187;
    if (!dispatch(0x21u, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        result.cpu_delta.gpr16_write_mask != 1u ||
        result.cpu_delta.gpr16_values[0] != 1u || result.eflags_values !=
            BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) return 188;
    if (!dispatch(0x29u, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        result.cpu_delta.gpr16_write_mask != 1u ||
        result.cpu_delta.gpr16_values[0] != 21u || result.eflags_values !=
            BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) return 189;
    if (!dispatch(0x2au, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        result.cpu_delta.gpr16_write_mask != 1u ||
        result.cpu_delta.gpr16_values[0] != 21u || result.eflags_values !=
            BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) return 190;
    if (!dispatch(0x0eu, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        result.cpu_delta.gpr16_write_mask != 0x1fu ||
        result.cpu_delta.gpr16_values[0] != 0u ||
        result.cpu_delta.gpr16_values[1] != 100u ||
        result.cpu_delta.gpr16_values[2] != 512u ||
        result.cpu_delta.gpr16_values[3] != 200u ||
        result.cpu_delta.gpr16_values[4] != 4u || result.eflags_values != 0u)
        return 191;
    dispatch_dpb_sentinel = 1u;
    if (!dispatch(0x25u, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        result.resume_rip != 0x104u || result.cpu_delta.gpr16_write_mask != 1u ||
        result.cpu_delta.gpr16_values[0] != 0u || result.eflags_values != 0u ||
        ram[0u] != 0u || ram[1u] != 0u || ram[2u] != 0u || ram[3u] != 2u ||
        ram[4u] != 3u || ram[5u] != 0u || ram[6u] != 2u || ram[7u] != 0u ||
        ram[8u] != 1u || ram[9u] != 0u || ram[10u] != 2u || ram[11u] != 63u ||
        ram[12u] != 0u || ram[13u] != 0xa5u || ram[14u] != 0xa5u ||
        ram[15u] != 201u || ram[16u] != 0u || ram[17u] != 0u || ram[18u] != 2u ||
        ram[19u] != 1u || ram[20u] != 0u || ram[21u] != 0xbcu || ram[22u] != 0x04u ||
        ram[23u] != 0xbcu || ram[24u] != 0x04u || ram[25u] != 0xf8u ||
        ram[26u] != 10u || ram[27u] != 0xffu || ram[28u] != 0xffu ||
        ram[29u] != 0xffu || ram[30u] != 0xffu || ram[31u] != 0xa5u ||
        ram[32u] != 0xa5u || ram[33u] != 100u || ram[34u] != 0u) return 193;
    dispatch_dpb_sentinel = 0u;
    dispatch_ax = 1u;
    if (!dispatch(0x25u, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        result.cpu_delta.gpr16_write_mask != 1u ||
        result.cpu_delta.gpr16_values[0] != 5u ||
        result.eflags_write_mask != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF ||
        result.eflags_values != BX_NTVDM_CPU_RESULT_V2_EFLAGS_CF) return 194;
    dispatch_ax = 0u;
    /* demExitVDM is a lifecycle termination request in demmisc.c, not a
     * harmless no-op.  The non-invasive CLI replacement is the existing
     * typed controlled stop, so it must not manufacture a guest resume. */
    if (!dispatch(0x3du, &result) ||
        result.disposition != BX_NTVDM_CPU_RESULT_V2_STOP ||
        result.resume_rip != 0u || result.cpu_delta.gpr16_write_mask != 0u ||
        result.eflags_write_mask != 0u || result.eflags_values != 0u) return 200;
    return 0;
}
