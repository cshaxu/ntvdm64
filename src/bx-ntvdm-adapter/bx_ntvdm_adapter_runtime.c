#include "bx_ntvdm_adapter_runtime.h"

#include "bx_ntvdm_host_session.h"
#include "bx_ntvdm_bios_memory_service.h"
#include "bx_ntvdm_dem_boot_drive_service.h"
#include "bx_ntvdm_dem_debug_service.h"
#include "bx_ntvdm_emm_unavailable_service.h"
#include "bx_ntvdm_mouse_install1_mapping_service.h"
#include "bx_ntvdm_printer_unavailable_service.h"
#include "bx_ntvdm_config_done_service.h"
#include "bx_ntvdm_cmd_comspec_bootstrap_service.h"
#include "bx_ntvdm_cmd_get_next_service.h"
#include "bx_ntvdm_cmd_keyboard_layout_service.h"
#include "bx_ntvdm_controlled_stop_service.h"
#include "bx_ntvdm_vdd_create_user_notify_service.h"
#include "bx_ntvdm_spckbd_init_service.h"
#include "bx_ntvdm_wait_if_idle_service.h"
#include "bx_ntvdm_dem_dta_service.h"
#include "bx_ntvdm_dem_drive_service.h"
#include "bx_ntvdm_dem_drive_provider_v1.h"
#include "bx_ntvdm_dem_ioctl_metadata_provider_v1.h"
#include "bx_ntvdm_dem_hard_error_service.h"
#include "bx_ntvdm_dem_dpb_service.h"
#include "bx_ntvdm_dem_provider_v1.h"
#include "bx_ntvdm_dem_session_lifecycle_provider_v1.h"
#include "bx_ntvdm_dem_fastio_provider_v1.h"
#include "bx_ntvdm_host_drive_policy.h"
#include "bx_ntvdm_cmd_set_info_service.h"
#include "bx_ntvdm_boot_namespace_provider_v1.h"
#include "bx_ntvdm_cmd_current_dir_service.h"
#include "bx_ntvdm_dem_path_search_service_v1.h"
#include "bx_ntvdm_legacy_plane_gate_v1.h"
#include "bx_ntvdm_machine_profile_abi.h"
#include "bx_ntvdm_startup_snapshot_evidence.h"
#include "byob_image.h"
#include "byob_launch_plan_v2.h"
#include "byob_profile.h"

#include <string.h>
#include <windows.h>

#define BX_NTVDM_ADAPTER_ENV_PROFILE L"NTDOS64_ADAPTER_PROFILE"
#define BX_NTVDM_ADAPTER_ENV_ROOT L"NTDOS64_ADAPTER_ROOT"
#define BX_NTVDM_ADAPTER_ENV_LAUNCH_PLAN L"NTDOS64_ADAPTER_LAUNCH_PLAN"
#define BX_NTVDM_ADAPTER_MAX_OBSERVATION_BYTES 4096u

typedef struct bx_ntvdm_adapter_runtime_v1 {
    bx_ntvdm_host_session_v1 session;
    bx_ntvdm_machine_profile_v1 machine_profile;
    uint8_t observation_output[BX_NTVDM_ADAPTER_MAX_OBSERVATION_BYTES];
    bx_ntvdm_observation_transaction_v1 active_observation;
    bx_ntvdm_startup_snapshot_transaction_v1 active_startup_snapshot;
    uint64_t active_observation_output_bytes;
    uint64_t active_startup_snapshot_output_bytes;
    byob_image ntio;
    byob_image ntdos;
    byob_image command;
    byob_image target;
    byob_image terminal_quit;
    wchar_t snapshot_evidence_root[MAX_PATH];
    wchar_t snapshot_evidence_file_name[64];
    byob_host_drive_inventory_descriptor host_drive_inventory;
    bx_ntvdm_host_drive_snapshot_v1 host_drive_snapshot;
    bx_ntvdm_dem_dta_registration_v1 dem_dta_registration;
    bx_ntvdm_dem_hard_error_registration_v1 dem_hard_error_registration;
    bx_ntvdm_cmd_set_info_registration_v1 cmd_set_info_registration;
    bx_ntvdm_boot_namespace_provider_v1 boot_namespace_provider;
    bx_ntvdm_cmd_comspec_bootstrap_v1 cmd_comspec_bootstrap;
    bx_ntvdm_cmd_get_next_state_v1 cmd_get_next;
    byob_launch_declaration_v1 launch;
    byob_launch_plan_v2 launch_plan;
    byob_machine_startup_handoff_descriptor deferred_startup_handoff;
    uint8_t bulk_payload[BX_NTVDM_BULK_RESULT_TRANSACTION_V1_MAX_PAYLOAD];
    uint8_t multi_write_payload[BX_NTVDM_MULTI_WRITE_MAX_PAYLOAD];
    int installed;
    int has_machine_profile;
    int active_observation_present;
    int active_startup_snapshot_present;
    int startup_plan_enabled;
    int deferred_startup_enabled;
    int deferred_startup_taken;
    int has_host_drive_inventory;
    int has_host_drive_snapshot;
    int has_dem_dta_registration;
    int has_dem_hard_error_registration;
    int has_cmd_set_info_registration;
    int has_launch;
    int has_boot_namespace_provider;
    uint32_t boot_file_diagnostic;
    int environment_attempted;
    bx_ntvdm_adapter_install_diagnostic_v1 install_diagnostic;
} bx_ntvdm_adapter_runtime_v1;

static bx_ntvdm_adapter_runtime_v1 bx_ntvdm_adapter_runtime;

static uint32_t bx_ntvdm_adapter_runtime_v1_boot_file_diagnostic(
    bx_ntvdm_cmd_boot_file_prepare_diagnostic_v1 value)
{
    switch (value) {
    case BX_NTVDM_CMD_BOOT_FILE_PREPARE_DIAGNOSTIC_V1_ARGUMENT:
        return BX_NTVDM_ADAPTER_BOOT_FILE_DIAGNOSTIC_V1_ARGUMENT;
    case BX_NTVDM_CMD_BOOT_FILE_PREPARE_DIAGNOSTIC_V1_EVENT:
        return BX_NTVDM_ADAPTER_BOOT_FILE_DIAGNOSTIC_V1_EVENT;
    case BX_NTVDM_CMD_BOOT_FILE_PREPARE_DIAGNOSTIC_V1_CPU:
        return BX_NTVDM_ADAPTER_BOOT_FILE_DIAGNOSTIC_V1_CPU;
    case BX_NTVDM_CMD_BOOT_FILE_PREPARE_DIAGNOSTIC_V1_WINDOW:
        return BX_NTVDM_ADAPTER_BOOT_FILE_DIAGNOSTIC_V1_WINDOW;
    case BX_NTVDM_CMD_BOOT_FILE_PREPARE_DIAGNOSTIC_V1_CONTRACT:
        return BX_NTVDM_ADAPTER_BOOT_FILE_DIAGNOSTIC_V1_CONTRACT;
    case BX_NTVDM_CMD_BOOT_FILE_PREPARE_DIAGNOSTIC_V1_NAMESPACE:
        return BX_NTVDM_ADAPTER_BOOT_FILE_DIAGNOSTIC_V1_NAMESPACE;
    case BX_NTVDM_CMD_BOOT_FILE_PREPARE_DIAGNOSTIC_V1_PATH:
        return BX_NTVDM_ADAPTER_BOOT_FILE_DIAGNOSTIC_V1_PATH;
    case BX_NTVDM_CMD_BOOT_FILE_PREPARE_DIAGNOSTIC_V1_TRANSACTION:
        return BX_NTVDM_ADAPTER_BOOT_FILE_DIAGNOSTIC_V1_TRANSACTION;
    case BX_NTVDM_CMD_BOOT_FILE_PREPARE_DIAGNOSTIC_V1_ACCEPTED:
        return BX_NTVDM_ADAPTER_BOOT_FILE_DIAGNOSTIC_V1_ACCEPTED;
    default:
        return BX_NTVDM_ADAPTER_BOOT_FILE_DIAGNOSTIC_V1_ARGUMENT;
    }
}

/* The original DEM dispatcher reserves eight table slots for the one shared
 * demNotYetImplemented outcome.  This helper deliberately derives the owner
 * through the common ingress/registry/plane path before offering that whole
 * provider; it is not a 50:42 recognizer. */
static int bx_ntvdm_adapter_runtime_v1_dispatch_original_unavailable_dem(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result)
{
    bx_ntvdm_bop_ingress_v1 ingress;
    bx_ntvdm_bop_provider_selection_v1 selection;
    bx_ntvdm_dem_plane_record_v1 plane;
    return bx_ntvdm_bop_ingress_v1_classify(window, &ingress) &&
        bx_ntvdm_bop_provider_registry_v1_select(&ingress, &selection) &&
        bx_ntvdm_dem_plane_v1_classify(&ingress, &selection, &plane) &&
        bx_ntvdm_dem_provider_v1_dispatch(&ingress, &selection, &plane,
            event, cpu_before, result);
}

/* The finite boot namespace is the only selected profile that has no VDD,
 * device or ambient-host lifecycle owner. Keep the route on common ingress,
 * registry and DEM-plane records; this is not a byte recognizer. */
static int bx_ntvdm_adapter_runtime_v1_dispatch_dem_session_lifecycle(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result)
{
    bx_ntvdm_bop_ingress_v1 ingress;
    bx_ntvdm_bop_provider_selection_v1 selection;
    bx_ntvdm_dem_plane_record_v1 plane;
    return bx_ntvdm_adapter_runtime.has_boot_namespace_provider &&
        bx_ntvdm_bop_ingress_v1_classify(window, &ingress) &&
        bx_ntvdm_bop_provider_registry_v1_select(&ingress, &selection) &&
        bx_ntvdm_dem_plane_v1_classify(&ingress, &selection, &plane) &&
        bx_ntvdm_dem_session_lifecycle_provider_v1_dispatch(
            &bx_ntvdm_adapter_runtime.boot_namespace_provider, &ingress,
            &selection, &plane, event, cpu_before, result);
}

/* The CLI captures the only normal-launch drive inventory before guest
 * execution.  Route the OpenNT-shaped DEM count through the common BOP
 * ingress/registry/plane records; a guest BOP never triggers a host query. */
static int bx_ntvdm_adapter_runtime_v1_dispatch_dem_drive_snapshot(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result)
{
    bx_ntvdm_bop_ingress_v1 ingress;
    bx_ntvdm_bop_provider_selection_v1 selection;
    bx_ntvdm_dem_plane_record_v1 plane;
    return bx_ntvdm_adapter_runtime.has_host_drive_snapshot &&
        bx_ntvdm_bop_ingress_v1_classify(window, &ingress) &&
        bx_ntvdm_bop_provider_registry_v1_select(&ingress, &selection) &&
        bx_ntvdm_dem_plane_v1_classify(&ingress, &selection, &plane) &&
        bx_ntvdm_dem_drive_provider_v1_dispatch(&ingress, &selection, &plane,
            &bx_ntvdm_adapter_runtime.host_drive_snapshot, event, cpu_before,
            result);
}

/* The same immutable CLI snapshot is the complete input to the narrow
 * demGetDPBList contract.  This is deliberately a selection gate as well as
 * a producer: an empty admitted snapshot must not fall through to a retained
 * profile-static inventory.  It leaves the original empty-list continuation
 * as pass-through because the generic multi-write ABI has no empty-result
 * transport. */
static int bx_ntvdm_adapter_runtime_v1_dispatch_dem_dpb_snapshot(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result)
{
    bx_ntvdm_bop_ingress_v1 ingress;
    bx_ntvdm_bop_provider_selection_v1 selection;
    bx_ntvdm_dem_plane_record_v1 plane;
    bx_ntvdm_multi_write_transaction_v1 transaction;

    if (!bx_ntvdm_adapter_runtime.has_host_drive_snapshot || result == 0 ||
        !bx_ntvdm_bop_ingress_v1_classify(window, &ingress) ||
        !bx_ntvdm_bop_provider_registry_v1_select(&ingress, &selection) ||
        !bx_ntvdm_dem_plane_v1_classify(&ingress, &selection, &plane) ||
        ingress.service != 0x46u ||
        !bx_ntvdm_dem_dpb_service_v1_prepare(
            bx_ntvdm_adapter_runtime.host_drive_snapshot.types, event,
            cpu_before, window, &transaction,
            bx_ntvdm_adapter_runtime.multi_write_payload)) return 0;
    if (transaction.writes.write_count == 0u) return 1;
    if (!bx_ntvdm_host_session_v1_queue_multi_write(
            &bx_ntvdm_adapter_runtime.session, &transaction,
            bx_ntvdm_adapter_runtime.multi_write_payload,
            transaction.writes.payload_bytes)) return 0;
    *result = transaction.result;
    return 1;
}

/* `demIOCTL` owns both metadata and raw-media AL subfunctions.  Keep the
 * original parent plane identity, but admit only the paired 08/09 metadata
 * contract through the immutable installation-time snapshot. */
static int bx_ntvdm_adapter_runtime_v1_dispatch_dem_ioctl_metadata_snapshot(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result)
{
    bx_ntvdm_bop_ingress_v1 ingress;
    bx_ntvdm_bop_provider_selection_v1 selection;
    bx_ntvdm_dem_plane_record_v1 plane;
    return bx_ntvdm_adapter_runtime.has_host_drive_snapshot &&
        bx_ntvdm_bop_ingress_v1_classify(window, &ingress) &&
        bx_ntvdm_bop_provider_registry_v1_select(&ingress, &selection) &&
        bx_ntvdm_dem_plane_v1_classify(&ingress, &selection, &plane) &&
        bx_ntvdm_dem_ioctl_metadata_provider_v1_dispatch(&ingress, &selection,
            &plane, &bx_ntvdm_adapter_runtime.host_drive_snapshot, event,
            cpu_before, result);
}

static int bx_ntvdm_hex_nibble(wchar_t value, uint8_t *out)
{
    if (value >= L'0' && value <= L'9') { *out = (uint8_t)(value - L'0'); return 1; }
    if (value >= L'a' && value <= L'f') { *out = (uint8_t)(value - L'a' + 10); return 1; }
    return 0;
}

static int bx_ntvdm_sha256_bytes(const wchar_t *text, uint8_t out[32])
{
    size_t index;
    uint8_t high, low;
    if (text == 0 || wcslen(text) != 64u) return 0;
    for (index = 0u; index < 32u; ++index) {
        if (!bx_ntvdm_hex_nibble(text[index * 2u], &high) ||
            !bx_ntvdm_hex_nibble(text[index * 2u + 1u], &low)) return 0;
        out[index] = (uint8_t)((high << 4) | low);
    }
    return 1;
}

static int bx_ntvdm_resource_set(bx_ntvdm_session_resource_v1 *resource,
    uint32_t kind, const byob_component_descriptor *descriptor)
{
    if (resource == 0 || descriptor == 0 || descriptor->bytes == 0u ||
        !bx_ntvdm_sha256_bytes(descriptor->sha256, resource->sha256)) return 0;
    resource->kind = kind;
    resource->flags = 0u;
    resource->byte_count = descriptor->bytes;
    return 1;
}

void bx_ntvdm_adapter_runtime_v1_reset(void)
{
    bx_ntvdm_search_transaction_v1_release(
        &bx_ntvdm_adapter_runtime.boot_namespace_provider.search_transaction);
    byob_image_release(&bx_ntvdm_adapter_runtime.ntio);
    byob_image_release(&bx_ntvdm_adapter_runtime.ntdos);
    byob_image_release(&bx_ntvdm_adapter_runtime.command);
    byob_image_release(&bx_ntvdm_adapter_runtime.target);
    byob_image_release(&bx_ntvdm_adapter_runtime.terminal_quit);
    memset(&bx_ntvdm_adapter_runtime, 0, sizeof(bx_ntvdm_adapter_runtime));
}

static int bx_ntvdm_adapter_runtime_v1_reject_install(
    bx_ntvdm_adapter_install_diagnostic_v1 diagnostic)
{
    bx_ntvdm_adapter_runtime_v1_reset();
    bx_ntvdm_adapter_runtime.install_diagnostic = diagnostic;
    return 0;
}

bx_ntvdm_adapter_install_diagnostic_v1
bx_ntvdm_adapter_runtime_v1_install_diagnostic(void)
{
    return bx_ntvdm_adapter_runtime.install_diagnostic;
}

int bx_ntvdm_adapter_runtime_v1_install(const wchar_t *profile_path,
    const wchar_t *byob_root)
{
    byob_profile_selection selection;
    bx_ntvdm_session_resources_v1 resources;
    bx_ntvdm_host_service_payloads_v1 payloads;
    wchar_t launch_plan[BYOB_LAUNCH_PLAN_V2_ENV_CHARS];
    DWORD launch_plan_size;

    if (bx_ntvdm_adapter_runtime.installed) return 0;
    if (byob_profile_validate_file_select(profile_path, byob_root, &selection) !=
        BYOB_PROFILE_ACCEPTED ||
        byob_image_load_exact(byob_root, &selection.ntio,
            &bx_ntvdm_adapter_runtime.ntio) != BYOB_IMAGE_OK ||
        byob_image_load_exact(byob_root, &selection.ntdos,
            &bx_ntvdm_adapter_runtime.ntdos) != BYOB_IMAGE_OK ||
         byob_image_load_exact(byob_root, &selection.command,
             &bx_ntvdm_adapter_runtime.command) != BYOB_IMAGE_OK) {
        return bx_ntvdm_adapter_runtime_v1_reject_install(
            BX_NTVDM_ADAPTER_INSTALL_DIAGNOSTIC_V1_PROFILE_OR_IMAGE);
    }
    SetLastError(ERROR_SUCCESS);
    launch_plan_size = GetEnvironmentVariableW(BX_NTVDM_ADAPTER_ENV_LAUNCH_PLAN,
        launch_plan, sizeof(launch_plan) / sizeof(launch_plan[0]));
    if ((launch_plan_size == 0u && GetLastError() != ERROR_SUCCESS) ||
        launch_plan_size == 0u || launch_plan_size >= sizeof(launch_plan) / sizeof(launch_plan[0]) ||
        !byob_launch_plan_v2_from_environment(&bx_ntvdm_adapter_runtime.launch_plan, launch_plan) ||
        bx_ntvdm_adapter_runtime.launch_plan.slot_count != selection.declared_target_count ||
        (bx_ntvdm_adapter_runtime.launch_plan.first.target_kind == BYOB_LAUNCH_TARGET_KIND_V1_COM &&
         wcscmp(selection.target.file_name, L"TARGET.COM") != 0) ||
        (bx_ntvdm_adapter_runtime.launch_plan.first.target_kind == BYOB_LAUNCH_TARGET_KIND_V1_EXE &&
         wcscmp(selection.target.file_name, L"TARGET.EXE") != 0) ||
        (selection.declared_target_count == 2u &&
         wcscmp(selection.terminal_quit.file_name, L"QUIT.COM") != 0)) {
        return bx_ntvdm_adapter_runtime_v1_reject_install(
            BX_NTVDM_ADAPTER_INSTALL_DIAGNOSTIC_V1_LAUNCH_DECLARATION);
    }
    bx_ntvdm_adapter_runtime.launch = bx_ntvdm_adapter_runtime.launch_plan.first;
    bx_ntvdm_adapter_runtime.has_launch = 1;
    if (byob_image_load_exact(byob_root, &selection.target,
            &bx_ntvdm_adapter_runtime.target) != BYOB_IMAGE_OK ||
        (selection.declared_target_count == 2u &&
         byob_image_load_exact(byob_root, &selection.terminal_quit,
             &bx_ntvdm_adapter_runtime.terminal_quit) != BYOB_IMAGE_OK)) {
        return bx_ntvdm_adapter_runtime_v1_reject_install(
            BX_NTVDM_ADAPTER_INSTALL_DIAGNOSTIC_V1_PROFILE_OR_IMAGE);
    }
    if (selection.machine_startup_snapshot_evidence_file_name[0] != L'\0') {
        if (wcslen(byob_root) >= MAX_PATH) {
            return bx_ntvdm_adapter_runtime_v1_reject_install(
                BX_NTVDM_ADAPTER_INSTALL_DIAGNOSTIC_V1_SNAPSHOT_EVIDENCE_ROOT);
        }
        wcscpy(bx_ntvdm_adapter_runtime.snapshot_evidence_root, byob_root);
        wcscpy(bx_ntvdm_adapter_runtime.snapshot_evidence_file_name,
            selection.machine_startup_snapshot_evidence_file_name);
    }
    bx_ntvdm_adapter_runtime.startup_plan_enabled =
        selection.machine_startup_plan_enabled != 0u &&
        selection.machine_startup_entry_ntio_v0 != 0u;
    if (selection.has_machine_startup_handoff != 0u) {
        memcpy(&bx_ntvdm_adapter_runtime.deferred_startup_handoff,
            &selection.machine_startup_handoff,
            sizeof(bx_ntvdm_adapter_runtime.deferred_startup_handoff));
        bx_ntvdm_adapter_runtime.deferred_startup_enabled = 1;
    }
    if (selection.has_host_drive_inventory != 0u) {
        memcpy(&bx_ntvdm_adapter_runtime.host_drive_inventory,
            &selection.host_drive_inventory,
            sizeof(bx_ntvdm_adapter_runtime.host_drive_inventory));
        bx_ntvdm_adapter_runtime.has_host_drive_inventory = 1;
    }
    if (selection.has_command_placement != 0u &&
        (selection.declared_target_count == 1u || selection.declared_target_count == 2u) &&
        selection.has_guest_search_metadata != 0u &&
        bx_ntvdm_boot_namespace_provider_v1_initialize(
            &bx_ntvdm_adapter_runtime.boot_namespace_provider,
            &bx_ntvdm_adapter_runtime.command, &bx_ntvdm_adapter_runtime.target,
            selection.declared_target_count == 2u ?
                &bx_ntvdm_adapter_runtime.terminal_quit : 0,
            &selection)) {
        bx_ntvdm_adapter_runtime.has_boot_namespace_provider = 1;
    }
    bx_ntvdm_cmd_comspec_bootstrap_v1_initialize(
        &bx_ntvdm_adapter_runtime.cmd_comspec_bootstrap);
    bx_ntvdm_cmd_get_next_state_v1_initialize(
        &bx_ntvdm_adapter_runtime.cmd_get_next);
    bx_ntvdm_session_resources_v1_initialize(&resources);
    if (!bx_ntvdm_resource_set(&resources.resources[0],
            BX_NTVDM_SESSION_RESOURCE_NTIO, &selection.ntio) ||
        !bx_ntvdm_resource_set(&resources.resources[1],
            BX_NTVDM_SESSION_RESOURCE_NTDOS, &selection.ntdos) ||
         !bx_ntvdm_resource_set(&resources.resources[2],
             BX_NTVDM_SESSION_RESOURCE_COMMAND, &selection.command)) {
        return bx_ntvdm_adapter_runtime_v1_reject_install(
            BX_NTVDM_ADAPTER_INSTALL_DIAGNOSTIC_V1_SESSION_RESOURCES);
    }
    payloads.ntio_bytes = bx_ntvdm_adapter_runtime.ntio.bytes;
    payloads.ntio_byte_count = bx_ntvdm_adapter_runtime.ntio.byte_count;
    payloads.ntdos_bytes = bx_ntvdm_adapter_runtime.ntdos.bytes;
    payloads.ntdos_byte_count = bx_ntvdm_adapter_runtime.ntdos.byte_count;
    payloads.command_bytes = bx_ntvdm_adapter_runtime.command.bytes;
    payloads.command_byte_count = bx_ntvdm_adapter_runtime.command.byte_count;
    if (!bx_ntvdm_host_drive_snapshot_v1_capture_from_environment(
            &bx_ntvdm_adapter_runtime.host_drive_snapshot)) {
        return bx_ntvdm_adapter_runtime_v1_reject_install(
            BX_NTVDM_ADAPTER_INSTALL_DIAGNOSTIC_V1_HOST_DRIVE_SNAPSHOT);
    }
    bx_ntvdm_adapter_runtime.has_host_drive_snapshot = 1;
    if (!bx_ntvdm_host_session_v1_initialize(&bx_ntvdm_adapter_runtime.session,
        &resources, &payloads, 0x100000u)) {
        return bx_ntvdm_adapter_runtime_v1_reject_install(
            BX_NTVDM_ADAPTER_INSTALL_DIAGNOSTIC_V1_HOST_SESSION);
    }
    bx_ntvdm_machine_profile_v1_initialize(
        &bx_ntvdm_adapter_runtime.machine_profile);
    if (selection.machine_observation_count != 0u) {
        uint32_t index;
        for (index = 0u; index < selection.machine_observation_count; ++index) {
            bx_ntvdm_guest_range range;
            range.address = selection.machine_observations[index].physical_address;
            range.length = selection.machine_observations[index].byte_count;
            if (!bx_ntvdm_machine_profile_v1_set_observation(
                    &bx_ntvdm_adapter_runtime.machine_profile,
                    selection.machine_observations[index].id, &range)) {
                return bx_ntvdm_adapter_runtime_v1_reject_install(
                    BX_NTVDM_ADAPTER_INSTALL_DIAGNOSTIC_V1_MACHINE_OBSERVATION);
            }
        }
        if (!bx_ntvdm_machine_profile_v1_valid(
                &bx_ntvdm_adapter_runtime.machine_profile, 0x100000u)) {
            return bx_ntvdm_adapter_runtime_v1_reject_install(
                BX_NTVDM_ADAPTER_INSTALL_DIAGNOSTIC_V1_MACHINE_OBSERVATION);
        }
        bx_ntvdm_adapter_runtime.has_machine_profile = 1;
        if (selection.has_machine_observation_trigger != 0u) {
            uint32_t index;
            uint64_t byte_count = 0u;
            for (index = 0u; index < selection.machine_observation_count; ++index) {
                if (selection.machine_observations[index].id ==
                    selection.machine_observation_trigger.observation_id) {
                    byte_count = selection.machine_observations[index].byte_count;
                    break;
                }
            }
            if (byte_count == 0u || byte_count > BX_NTVDM_ADAPTER_MAX_OBSERVATION_BYTES ||
                !bx_ntvdm_machine_profile_v1_set_neutral_ud2_trigger(
                    &bx_ntvdm_adapter_runtime.machine_profile,
                    selection.machine_observation_trigger.observation_id)) {
                return bx_ntvdm_adapter_runtime_v1_reject_install(
                    BX_NTVDM_ADAPTER_INSTALL_DIAGNOSTIC_V1_MACHINE_TRIGGER);
            }
        }
        if (selection.has_machine_startup_snapshot_trigger != 0u) {
            uint32_t index;
            uint64_t output_bytes = 0u;
            for (index = 0u;
                index < selection.machine_startup_snapshot_trigger.observation_count;
                ++index) {
                uint32_t observation_index;
                for (observation_index = 0u;
                    observation_index < selection.machine_observation_count;
                    ++observation_index) {
                    if (selection.machine_observations[observation_index].id ==
                        selection.machine_startup_snapshot_trigger.observation_ids[index])
                        break;
                }
                if (observation_index == selection.machine_observation_count ||
                    UINT64_MAX - output_bytes <
                    selection.machine_observations[observation_index].byte_count)
                    { return bx_ntvdm_adapter_runtime_v1_reject_install(
                        BX_NTVDM_ADAPTER_INSTALL_DIAGNOSTIC_V1_MACHINE_SNAPSHOT_TRIGGER); }
                output_bytes += selection.machine_observations[observation_index].byte_count;
            }
            if (output_bytes == 0u || output_bytes > BX_NTVDM_ADAPTER_MAX_OBSERVATION_BYTES ||
                !bx_ntvdm_machine_profile_v1_set_neutral_ud2_snapshot_trigger(
                    &bx_ntvdm_adapter_runtime.machine_profile,
                    selection.machine_startup_snapshot_trigger.observation_ids,
                    selection.machine_startup_snapshot_trigger.observation_count)) {
                return bx_ntvdm_adapter_runtime_v1_reject_install(
                    BX_NTVDM_ADAPTER_INSTALL_DIAGNOSTIC_V1_MACHINE_SNAPSHOT_TRIGGER);
            }
        }
    }
    bx_ntvdm_adapter_runtime.installed = 1;
    bx_ntvdm_adapter_runtime.install_diagnostic =
        BX_NTVDM_ADAPTER_INSTALL_DIAGNOSTIC_V1_NONE;
    return 1;
}

int bx_ntvdm_adapter_runtime_v1_copy_host_drive_snapshot(
    bx_ntvdm_host_drive_snapshot_v1 *out)
{
    if (out == 0 || !bx_ntvdm_adapter_runtime.installed ||
        !bx_ntvdm_adapter_runtime.has_host_drive_snapshot ||
        !bx_ntvdm_host_drive_snapshot_v1_valid(
            &bx_ntvdm_adapter_runtime.host_drive_snapshot)) return 0;
    memcpy(out, &bx_ntvdm_adapter_runtime.host_drive_snapshot, sizeof(*out));
    return 1;
}

int bx_ntvdm_adapter_runtime_v1_copy_diagnostic_state(
    bx_ntvdm_adapter_runtime_diagnostic_state_v1 *out)
{
    if (out == 0) return 0;
    out->version = BX_NTVDM_ADAPTER_RUNTIME_DIAGNOSTIC_STATE_V1_VERSION;
    out->installed = bx_ntvdm_adapter_runtime.installed ? 1u : 0u;
    out->has_boot_namespace_provider =
        bx_ntvdm_adapter_runtime.has_boot_namespace_provider ? 1u : 0u;
    out->pending_kind = bx_ntvdm_adapter_runtime.installed ?
        bx_ntvdm_adapter_runtime.session.pending_kind : BX_NTVDM_HOST_PENDING_NONE;
    out->boot_file_diagnostic = bx_ntvdm_adapter_runtime.boot_file_diagnostic;
    return 1;
}

int bx_ntvdm_adapter_runtime_v1_install_from_environment(void)
{
    wchar_t profile[MAX_PATH], root[MAX_PATH];
    DWORD profile_size, root_size;
    if (bx_ntvdm_adapter_runtime.installed) return 1;
    if (bx_ntvdm_adapter_runtime.environment_attempted) return 0;
    bx_ntvdm_adapter_runtime.environment_attempted = 1;
    profile_size = GetEnvironmentVariableW(BX_NTVDM_ADAPTER_ENV_PROFILE,
        profile, MAX_PATH);
    root_size = GetEnvironmentVariableW(BX_NTVDM_ADAPTER_ENV_ROOT, root,
        MAX_PATH);
    if (profile_size == 0u && root_size == 0u) return 0;
    if (profile_size == 0u || root_size == 0u || profile_size >= MAX_PATH ||
        root_size >= MAX_PATH) {
        bx_ntvdm_adapter_runtime.install_diagnostic =
            BX_NTVDM_ADAPTER_INSTALL_DIAGNOSTIC_V1_ENVIRONMENT;
        return -1;
    }
    return bx_ntvdm_adapter_runtime_v1_install(profile, root) ? 1 : -1;
}

int bx_ntvdm_adapter_runtime_v1_dispatch(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_exception_result_v1 *result)
{
    uint64_t resume_rip;
    if (result == 0 || !bx_ntvdm_exception_event_v1_valid(event) ||
        !bx_ntvdm_cpu_state_v1_valid(cpu_before) ||
        !bx_ntvdm_instruction_window_v1_valid(window)) return 0;
    bx_ntvdm_exception_result_v1_pass_through(result);
    (void)bx_ntvdm_adapter_runtime_v1_install_from_environment();
    if (!bx_ntvdm_adapter_runtime.installed) return 1;
    bx_ntvdm_adapter_runtime.boot_file_diagnostic = BX_NTVDM_ADAPTER_BOOT_FILE_DIAGNOSTIC_V1_NONE;
    bx_ntvdm_adapter_runtime.active_observation_present = 0;
    bx_ntvdm_adapter_runtime.active_startup_snapshot_present = 0;
    if (!bx_ntvdm_host_session_v1_dispatch(&bx_ntvdm_adapter_runtime.session,
            event, cpu_before, window, result)) return 0;
    if (result->disposition == BX_NTVDM_EXCEPTION_RESULT_PASS_THROUGH &&
        bx_ntvdm_legacy_plane_gate_v1_dem(window, 0x3bu) &&
        bx_ntvdm_dem_debug_service_v1_dispatch(event, cpu_before, window,
            result)) return 1;
    if (result->disposition == BX_NTVDM_EXCEPTION_RESULT_PASS_THROUGH &&
        bx_ntvdm_wait_if_idle_service_v1_dispatch(event, cpu_before, window,
            result)) return 1;
    if (result->disposition == BX_NTVDM_EXCEPTION_RESULT_PASS_THROUGH &&
        bx_ntvdm_bios_memory_service_v1_dispatch(event, cpu_before, window,
            result)) return 1;
    if (result->disposition == BX_NTVDM_EXCEPTION_RESULT_PASS_THROUGH &&
        bx_ntvdm_legacy_plane_gate_v1_dem(window, 0x0du) &&
        bx_ntvdm_dem_boot_drive_service_v1_dispatch(event, cpu_before, window,
            result)) return 1;
    if (result->disposition != BX_NTVDM_EXCEPTION_RESULT_PASS_THROUGH)
        return 1;
    if (!bx_ntvdm_adapter_runtime_v1_prepare_profile_trigger(event, cpu_before,
            window, &resume_rip)) {
        if (!bx_ntvdm_machine_profile_v1_prepare_neutral_ud2_snapshot_trigger(
            &bx_ntvdm_adapter_runtime.machine_profile, event, cpu_before, window,
            0x100000u, &bx_ntvdm_adapter_runtime.active_startup_snapshot,
            &resume_rip) ||
            !bx_ntvdm_host_session_v1_queue_startup_snapshot(
                &bx_ntvdm_adapter_runtime.session,
                &bx_ntvdm_adapter_runtime.active_startup_snapshot,
                bx_ntvdm_adapter_runtime.observation_output,
                bx_ntvdm_adapter_runtime.active_startup_snapshot.output_bytes))
            return 1;
        bx_ntvdm_adapter_runtime.active_startup_snapshot_output_bytes =
            bx_ntvdm_adapter_runtime.active_startup_snapshot.output_bytes;
        bx_ntvdm_adapter_runtime.active_startup_snapshot_present = 1;
    }
    return bx_ntvdm_exception_result_v1_resume(result, resume_rip);
}

int bx_ntvdm_adapter_runtime_v2_dispatch(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result)
{
    bx_ntvdm_exception_result_v1 legacy;
    if (result == 0 || !bx_ntvdm_exception_event_v1_valid(event) ||
        !bx_ntvdm_cpu_state_v1_valid(cpu_before) ||
        !bx_ntvdm_instruction_window_v1_valid(window)) return 0;
    bx_ntvdm_cpu_result_v2_pass_through(result);
    if (!bx_ntvdm_adapter_runtime_v1_dispatch(event, cpu_before, window,
            &legacy)) return 0;
    if (legacy.disposition == BX_NTVDM_EXCEPTION_RESULT_RESUME) {
        if (!bx_ntvdm_cpu_delta_v1_valid(&legacy.cpu_delta) ||
            !bx_ntvdm_cpu_result_v2_resume(result, legacy.resume_rip)) return 0;
        result->cpu_delta = legacy.cpu_delta;
        return bx_ntvdm_cpu_result_v2_valid(result);
    }
    if (legacy.disposition != BX_NTVDM_EXCEPTION_RESULT_PASS_THROUGH ||
        !bx_ntvdm_cpu_result_v2_valid(result)) return 0;
    if (!bx_ntvdm_adapter_runtime.installed) return 1;
    if (bx_ntvdm_legacy_plane_gate_v1_command(window, 0x11u) &&
        bx_ntvdm_cmd_return_exit_code_v1_dispatch(
            &bx_ntvdm_adapter_runtime.cmd_get_next, event, cpu_before, window,
            result)) return 1;
    if (bx_ntvdm_controlled_stop_service_v1_dispatch(event, cpu_before, window,
            result)) return 1;
    if (bx_ntvdm_spckbd_init_service_v1_dispatch(event, cpu_before, window,
            result)) return 1;
    if (bx_ntvdm_emm_unavailable_service_v1_dispatch(event, cpu_before, window,
            result)) return 1;
    if (bx_ntvdm_printer_unavailable_service_v1_dispatch(event, cpu_before, window,
            result)) return 1;
    if (bx_ntvdm_config_done_service_v1_dispatch(event, cpu_before, window,
            result)) return 1;
    if (bx_ntvdm_vdd_create_user_notify_service_v1_dispatch(event, cpu_before,
            window, result)) return 1;
    if (bx_ntvdm_legacy_plane_gate_v1_command(window, 0x0eu) &&
        bx_ntvdm_cmd_keyboard_layout_v1_dispatch(event, cpu_before, window,
            result)) return 1;
    {
        bx_ntvdm_multi_write_transaction_v1 transaction;
        if (bx_ntvdm_legacy_plane_gate_v1_command(window, 0x0fu) &&
            bx_ntvdm_cmd_comspec_bootstrap_v1_prepare_environment(event,
                cpu_before, window, &bx_ntvdm_adapter_runtime.cmd_comspec_bootstrap,
                &transaction, bx_ntvdm_adapter_runtime.multi_write_payload)) {
            if (transaction.writes.write_count == 0u) {
                *result = transaction.result;
                return bx_ntvdm_cpu_result_v2_valid(result);
            }
            if (bx_ntvdm_host_session_v1_queue_multi_write(
                &bx_ntvdm_adapter_runtime.session, &transaction,
                bx_ntvdm_adapter_runtime.multi_write_payload,
                transaction.writes.payload_bytes)) {
                *result = transaction.result;
                return 1;
            }
        }
    }
    if (bx_ntvdm_legacy_plane_gate_v1_command(window, 0x05u) &&
        bx_ntvdm_cmd_set_info_service_v1_dispatch(event, cpu_before, window,
            &bx_ntvdm_adapter_runtime.cmd_set_info_registration, result)) {
        bx_ntvdm_adapter_runtime.has_cmd_set_info_registration = 1;
        return 1;
    }
    {
        bx_ntvdm_multi_write_transaction_v1 transaction;
        bx_ntvdm_cmd_boot_file_prepare_diagnostic_v1 diagnostic;
        int boot_file = bx_ntvdm_legacy_plane_gate_v1_command(window, 0x0cu) ||
            bx_ntvdm_legacy_plane_gate_v1_command(window, 0x0du);
        if (boot_file) {
            if (!bx_ntvdm_adapter_runtime.has_boot_namespace_provider) {
                bx_ntvdm_adapter_runtime.boot_file_diagnostic = BX_NTVDM_ADAPTER_BOOT_FILE_DIAGNOSTIC_V1_PROVIDER;
            } else if (!bx_ntvdm_boot_namespace_provider_v1_valid(
                    &bx_ntvdm_adapter_runtime.boot_namespace_provider)) {
                bx_ntvdm_adapter_runtime.boot_file_diagnostic = BX_NTVDM_ADAPTER_BOOT_FILE_DIAGNOSTIC_V1_PROVIDER_INVALID;
            } else if (!bx_ntvdm_boot_namespace_provider_v1_prepare_boot_file_diagnostic(
                    &bx_ntvdm_adapter_runtime.boot_namespace_provider, event, cpu_before, window,
                    &transaction, bx_ntvdm_adapter_runtime.multi_write_payload, &diagnostic)) {
                bx_ntvdm_adapter_runtime.boot_file_diagnostic =
                    bx_ntvdm_adapter_runtime_v1_boot_file_diagnostic(diagnostic);
            } else if (!bx_ntvdm_host_session_v1_queue_multi_write(
                    &bx_ntvdm_adapter_runtime.session, &transaction,
                    bx_ntvdm_adapter_runtime.multi_write_payload, transaction.writes.payload_bytes)) {
                bx_ntvdm_adapter_runtime.boot_file_diagnostic = BX_NTVDM_ADAPTER_BOOT_FILE_DIAGNOSTIC_V1_QUEUE;
            } else {
                bx_ntvdm_adapter_runtime.boot_file_diagnostic = BX_NTVDM_ADAPTER_BOOT_FILE_DIAGNOSTIC_V1_ACCEPTED;
                *result = transaction.result;
                return 1;
            }
        }
    }
    if (bx_ntvdm_legacy_plane_gate_v1_dem(window, 0x32u) &&
        bx_ntvdm_dem_hard_error_service_v1_dispatch(event, cpu_before, window,
            &bx_ntvdm_adapter_runtime.dem_hard_error_registration, result)) {
        bx_ntvdm_adapter_runtime.has_dem_hard_error_registration = 1;
        return 1;
    }
    if (bx_ntvdm_adapter_runtime.has_boot_namespace_provider &&
        bx_ntvdm_legacy_plane_gate_v1_dem(window, 0x00u) &&
        bx_ntvdm_boot_namespace_provider_v1_seek(
            &bx_ntvdm_adapter_runtime.boot_namespace_provider, event, cpu_before,
            window, result)) return 1;
    if (bx_ntvdm_adapter_runtime.has_boot_namespace_provider &&
        bx_ntvdm_legacy_plane_gate_v1_dem(window, 0x42u)) {
        bx_ntvdm_bulk_result_transaction_v1 transaction;
        if (bx_ntvdm_dem_fastio_provider_v1_dispatch(
                &bx_ntvdm_adapter_runtime.boot_namespace_provider, event, cpu_before,
                window, bx_ntvdm_adapter_runtime.bulk_payload,
                sizeof(bx_ntvdm_adapter_runtime.bulk_payload), &transaction, result)) {
            if (transaction.magic == 0u) return 1;
            return bx_ntvdm_host_session_v1_queue_bulk_result(
                &bx_ntvdm_adapter_runtime.session, &transaction,
                bx_ntvdm_adapter_runtime.bulk_payload, transaction.payload_bytes);
        }
    }
    if (bx_ntvdm_adapter_runtime_v1_dispatch_original_unavailable_dem(event,
            cpu_before, window, result)) return 1;
    if (bx_ntvdm_adapter_runtime_v1_dispatch_dem_session_lifecycle(event,
            cpu_before, window, result)) return 1;
    if (bx_ntvdm_adapter_runtime.has_boot_namespace_provider &&
        bx_ntvdm_legacy_plane_gate_v1_dem(window, 0x16u)) {
        bx_ntvdm_bulk_result_transaction_v1 transaction;
        if (bx_ntvdm_boot_namespace_provider_v1_read(
                &bx_ntvdm_adapter_runtime.boot_namespace_provider, event,
                cpu_before, window, bx_ntvdm_adapter_runtime.bulk_payload,
                sizeof(bx_ntvdm_adapter_runtime.bulk_payload), &transaction, result)) {
            if (transaction.magic == 0u) return 1;
            return bx_ntvdm_host_session_v1_queue_bulk_result(
                &bx_ntvdm_adapter_runtime.session, &transaction,
                bx_ntvdm_adapter_runtime.bulk_payload, transaction.payload_bytes);
        }
    }
    if (bx_ntvdm_adapter_runtime.has_boot_namespace_provider &&
        bx_ntvdm_legacy_plane_gate_v1_dem(window, 0x02u) &&
        bx_ntvdm_boot_namespace_provider_v1_close(
            &bx_ntvdm_adapter_runtime.boot_namespace_provider, event, cpu_before,
            window, result)) return 1;
    /* 50:12 is prepared below because it needs one copied pathname read.
     * The original-unavailable DEM provider above owns every remaining
     * demNotYetImplemented slot.  The selected finite profile overrides only
     * 50:42 through the source-derived fast-read provider above. */
    if (bx_ntvdm_adapter_runtime_v1_dispatch_dem_drive_snapshot(event,
            cpu_before, window, result)) return 1;
    if (bx_ntvdm_adapter_runtime_v1_dispatch_dem_dpb_snapshot(event,
            cpu_before, window, result)) return 1;
    if (bx_ntvdm_adapter_runtime_v1_dispatch_dem_ioctl_metadata_snapshot(event,
            cpu_before, window, result)) return 1;
    if (!bx_ntvdm_adapter_runtime.has_host_drive_inventory) return 1;
    {
        bx_ntvdm_multi_write_transaction_v1 transaction;
        uint32_t available_mask = 0u;
        uint32_t index;
        for (index = 0u; index < 26u; ++index) {
            if (bx_ntvdm_adapter_runtime.host_drive_inventory.types[index] != 0u)
                available_mask |= UINT32_C(1) << index;
        }
        if (bx_ntvdm_legacy_plane_gate_v1_command(window, 0x04u) &&
            bx_ntvdm_cmd_current_dir_service_v1_prepare(available_mask, event,
                cpu_before, window, &transaction,
                bx_ntvdm_adapter_runtime.multi_write_payload)) {
            if (transaction.writes.write_count == 0u) {
                *result = transaction.result;
                return bx_ntvdm_cpu_result_v2_valid(result);
            }
            if (!bx_ntvdm_host_session_v1_queue_multi_write(
                    &bx_ntvdm_adapter_runtime.session, &transaction,
                    bx_ntvdm_adapter_runtime.multi_write_payload,
                    transaction.writes.payload_bytes)) return 0;
            *result = transaction.result;
            return 1;
        }
    }
    if (bx_ntvdm_legacy_plane_gate_v1_dem(window, 0x0fu) &&
        bx_ntvdm_dem_drive_service_v1_dispatch(
        bx_ntvdm_adapter_runtime.host_drive_inventory.types, event, cpu_before,
        window, result)) return 1;
    {
        bx_ntvdm_multi_write_transaction_v1 transaction;
        if (bx_ntvdm_legacy_plane_gate_v1_dem(window, 0x46u) &&
            bx_ntvdm_dem_dpb_service_v1_prepare(
                bx_ntvdm_adapter_runtime.host_drive_inventory.types, event,
                cpu_before, window, &transaction,
                bx_ntvdm_adapter_runtime.multi_write_payload) &&
            bx_ntvdm_host_session_v1_queue_multi_write(
                &bx_ntvdm_adapter_runtime.session, &transaction,
                bx_ntvdm_adapter_runtime.multi_write_payload,
                transaction.writes.payload_bytes)) {
            *result = transaction.result;
            return 1;
        }
    }
    return bx_ntvdm_cpu_result_v2_valid(result);
}

int bx_ntvdm_adapter_runtime_v3_dispatch(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_guest_read_action_v1 *action)
{
    bx_ntvdm_cpu_result_v2 result;
    if (action == 0 || !bx_ntvdm_adapter_runtime_v2_dispatch(event, cpu_before,
        window, &result)) return 0;
    if (result.disposition == BX_NTVDM_CPU_RESULT_V2_RESUME)
        return bx_ntvdm_guest_read_action_v1_resume(action, result.resume_rip) &&
            ((action->cpu_result = result), 1);
    if (result.disposition == BX_NTVDM_CPU_RESULT_V2_STOP)
        return bx_ntvdm_guest_read_action_v1_stop(action);
    if (bx_ntvdm_adapter_runtime.has_boot_namespace_provider &&
        bx_ntvdm_legacy_plane_gate_v1_dem(window, 0x12u) &&
        bx_ntvdm_boot_namespace_provider_v1_prepare_open(
            &bx_ntvdm_adapter_runtime.boot_namespace_provider, event, cpu_before,
            window, action)) {
        if (!bx_ntvdm_host_session_v1_queue_guest_read(
                &bx_ntvdm_adapter_runtime.session, event, cpu_before, action,
                BX_NTVDM_HOST_GUEST_READ_DEM_FILE_OPEN)) return 0;
        return 1;
    }
    if (bx_ntvdm_legacy_plane_gate_v1_dem(window, 0x1bu) &&
        bx_ntvdm_dem_dta_service_v1_dispatch(event, cpu_before, window,
            action)) {
        if (!bx_ntvdm_adapter_runtime.installed ||
            !bx_ntvdm_host_session_v1_queue_guest_read(
                &bx_ntvdm_adapter_runtime.session, event, cpu_before, action,
                BX_NTVDM_HOST_GUEST_READ_DEM_DTA))
            return 0;
        return 1;
    }
    bx_ntvdm_guest_read_action_v1_pass_through(action);
    return 1;
}

int bx_ntvdm_adapter_runtime_v3_take_pending_guest_read(
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    bx_ntvdm_guest_read_action_v1 *action, uint8_t **bytes)
{
    if (!bx_ntvdm_adapter_runtime.installed) return 0;
    return bx_ntvdm_host_session_v1_take_pending_guest_read(
        &bx_ntvdm_adapter_runtime.session, boundary, cpu_before, action, bytes);
}

int bx_ntvdm_adapter_runtime_v3_complete_guest_read(
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_guest_read_action_v1 *action,
    const uint8_t *bytes, uint64_t byte_count,
    bx_ntvdm_cpu_result_v2 *result)
{
    bx_ntvdm_dem_dta_registration_v1 registration;
    uint32_t consumer;
    int completed;
    if (result == 0 || action == 0 || !bx_ntvdm_adapter_runtime.installed) return 0;
    bx_ntvdm_cpu_result_v2_pass_through(result);
    if (!bx_ntvdm_host_session_v1_pending_guest_read_consumer(
            &bx_ntvdm_adapter_runtime.session, boundary, cpu_before, &consumer)) {
        (void)bx_ntvdm_host_session_v1_complete_guest_read(
            &bx_ntvdm_adapter_runtime.session, boundary, cpu_before, result);
        return 0;
    }
    completed = 0;
    if (consumer == BX_NTVDM_HOST_GUEST_READ_DEM_DTA)
        completed = bx_ntvdm_dem_dta_service_v1_complete(boundary, cpu_before,
            action, bytes, byte_count, &registration, result);
    if (consumer == BX_NTVDM_HOST_GUEST_READ_DEM_FILE_OPEN)
        completed = bx_ntvdm_boot_namespace_provider_v1_complete_open(
            &bx_ntvdm_adapter_runtime.boot_namespace_provider, boundary, cpu_before,
            action, bytes, byte_count, result);
    if (!bx_ntvdm_host_session_v1_complete_guest_read(
            &bx_ntvdm_adapter_runtime.session, boundary, cpu_before, result))
        return 0;
    if (!completed) return 0;
    if (consumer == BX_NTVDM_HOST_GUEST_READ_DEM_DTA) {
        bx_ntvdm_adapter_runtime.dem_dta_registration = registration;
        bx_ntvdm_adapter_runtime.has_dem_dta_registration = 1;
    }
    return 1;
}

int bx_ntvdm_adapter_runtime_v4_dispatch(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_guest_gather_read_action_v1 *action)
{
    bx_ntvdm_startup_snapshot_transaction_v1 snapshot;
    bx_ntvdm_guest_range ranges[BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_MAX_RANGES];
    uint64_t resume_rip;
    uint32_t index;
    if (action == 0 || !bx_ntvdm_exception_event_v1_valid(event) ||
        !bx_ntvdm_cpu_state_v1_valid(cpu_before) ||
        !bx_ntvdm_instruction_window_v1_valid(window)) return 0;
    bx_ntvdm_guest_gather_read_action_v1_pass_through(action);
    (void)bx_ntvdm_adapter_runtime_v1_install_from_environment();
    if (!bx_ntvdm_adapter_runtime.installed)
        return 1;
    if (bx_ntvdm_adapter_runtime.has_boot_namespace_provider &&
        bx_ntvdm_legacy_plane_gate_v1_dem(window, 0x09u) &&
        bx_ntvdm_dem_path_search_v1_prepare_first(
            &bx_ntvdm_adapter_runtime.dem_dta_registration, event, cpu_before,
            window, action)) {
        if (bx_ntvdm_host_session_v1_queue_guest_gather_read(
                &bx_ntvdm_adapter_runtime.session, event, cpu_before, action))
            return 1;
        bx_ntvdm_guest_gather_read_action_v1_pass_through(action);
        return 1;
    }
    if (bx_ntvdm_adapter_runtime.has_boot_namespace_provider &&
        bx_ntvdm_legacy_plane_gate_v1_dem(window, 0x0bu) &&
        bx_ntvdm_dem_path_search_v1_prepare_next(
            &bx_ntvdm_adapter_runtime.dem_dta_registration, event, cpu_before,
            window, action)) {
        if (bx_ntvdm_host_session_v1_queue_guest_gather_read(
                &bx_ntvdm_adapter_runtime.session, event, cpu_before, action))
            return 1;
        bx_ntvdm_guest_gather_read_action_v1_pass_through(action);
        return 1;
    }
    if (bx_ntvdm_legacy_plane_gate_v1_command(window, 0x01u) &&
        bx_ntvdm_cmd_get_next_v1_prepare(&bx_ntvdm_adapter_runtime.cmd_get_next,
            &bx_ntvdm_adapter_runtime.launch_plan,
            event, cpu_before, window, action)) {
        if (bx_ntvdm_host_session_v1_queue_guest_gather_read(
                &bx_ntvdm_adapter_runtime.session, event, cpu_before, action)) return 1;
        bx_ntvdm_guest_gather_read_action_v1_pass_through(action);
        return 1;
    }
    if (bx_ntvdm_legacy_plane_gate_v1_command(window, 0x02u) &&
        bx_ntvdm_cmd_comspec_bootstrap_v1_prepare_comspec(
            &bx_ntvdm_adapter_runtime.boot_namespace_provider.readonly_namespace, event, cpu_before,
            window, &bx_ntvdm_adapter_runtime.cmd_comspec_bootstrap, action)) {
        if (bx_ntvdm_host_session_v1_queue_guest_gather_read(
                &bx_ntvdm_adapter_runtime.session, event, cpu_before, action))
            return 1;
        bx_ntvdm_guest_gather_read_action_v1_pass_through(action);
        return 1;
    }
    if (bx_ntvdm_mouse_install1_mapping_service_v1_prepare(event, cpu_before,
            window, action)) {
        if (bx_ntvdm_host_session_v1_queue_guest_gather_read(
                &bx_ntvdm_adapter_runtime.session, event, cpu_before, action))
            return 1;
        bx_ntvdm_guest_gather_read_action_v1_pass_through(action);
        return 1;
    }
    if (
        !bx_ntvdm_machine_profile_v1_prepare_neutral_ud2_snapshot_trigger(
            &bx_ntvdm_adapter_runtime.machine_profile, event, cpu_before,
            window, 0x100000u, &snapshot, &resume_rip) ||
        snapshot.range_count > BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_MAX_RANGES ||
        snapshot.output_bytes > BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_MAX_TOTAL_BYTES)
        return 1;
    for (index = 0u; index < snapshot.range_count; ++index)
        ranges[index] = snapshot.ranges[index].guest_read;
    if (!bx_ntvdm_guest_gather_read_action_v1_need_read_resume(action, ranges,
            snapshot.range_count, resume_rip) ||
        !bx_ntvdm_host_session_v1_queue_guest_gather_read(
            &bx_ntvdm_adapter_runtime.session, event, cpu_before, action)) {
        bx_ntvdm_guest_gather_read_action_v1_pass_through(action);
        return 1;
    }
    return 1;
}

int bx_ntvdm_adapter_runtime_v4_take_pending_guest_gather_read(
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    bx_ntvdm_guest_gather_read_action_v1 *action, uint8_t **bytes)
{
    return bx_ntvdm_adapter_runtime.installed &&
        bx_ntvdm_host_session_v1_take_pending_guest_gather_read(
            &bx_ntvdm_adapter_runtime.session, boundary, cpu_before, action,
            bytes);
}

int bx_ntvdm_adapter_runtime_v4_complete_guest_gather_read(
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_guest_gather_read_action_v1 *action,
    const uint8_t *bytes, uint64_t byte_count,
    bx_ntvdm_cpu_result_v2 *result)
{
    bx_ntvdm_multi_write_transaction_v1 transaction;
    uint32_t profile_search_payload_bytes;
    if (action == 0 || bytes == 0 || result == 0 ||
        !bx_ntvdm_adapter_runtime.installed ||
        !bx_ntvdm_guest_gather_read_action_v1_valid(action) ||
        action->disposition != BX_NTVDM_GUEST_GATHER_READ_ACTION_V1_NEED_READ ||
        action->cpu_result.disposition != BX_NTVDM_CPU_RESULT_V2_RESUME ||
        byte_count != action->total_bytes) return 0;
    if (bx_ntvdm_adapter_runtime.has_boot_namespace_provider &&
        bx_ntvdm_dem_path_search_v1_complete_first(
            &bx_ntvdm_adapter_runtime.boot_namespace_provider.search_transaction,
            &bx_ntvdm_adapter_runtime.boot_namespace_provider.search_snapshot, boundary,
            cpu_before, action, bytes, byte_count, &transaction,
            bx_ntvdm_adapter_runtime.multi_write_payload,
            &profile_search_payload_bytes)) {
        if (!bx_ntvdm_host_session_v1_complete_guest_gather_read(
                &bx_ntvdm_adapter_runtime.session, boundary, cpu_before,
                &transaction.result) ||
            !bx_ntvdm_host_session_v1_queue_multi_write(
                &bx_ntvdm_adapter_runtime.session, &transaction,
                bx_ntvdm_adapter_runtime.multi_write_payload,
                profile_search_payload_bytes)) return 0;
        *result = transaction.result;
        return 1;
    }
    if (bx_ntvdm_adapter_runtime.has_boot_namespace_provider &&
        bx_ntvdm_dem_path_search_v1_complete_next(
            &bx_ntvdm_adapter_runtime.boot_namespace_provider.search_transaction, boundary,
            cpu_before, action, bytes, byte_count, &transaction,
            bx_ntvdm_adapter_runtime.multi_write_payload,
            &profile_search_payload_bytes)) {
        if (!bx_ntvdm_host_session_v1_complete_guest_gather_read(
                &bx_ntvdm_adapter_runtime.session, boundary, cpu_before,
                &transaction.result) ||
            !bx_ntvdm_host_session_v1_queue_multi_write(
                &bx_ntvdm_adapter_runtime.session, &transaction,
                bx_ntvdm_adapter_runtime.multi_write_payload,
                profile_search_payload_bytes)) return 0;
        *result = transaction.result;
        return 1;
    }
    if (bx_ntvdm_cmd_get_next_v1_complete(&bx_ntvdm_adapter_runtime.boot_namespace_provider.readonly_namespace,
            &bx_ntvdm_adapter_runtime.launch_plan, &bx_ntvdm_adapter_runtime.host_drive_snapshot,
            &bx_ntvdm_adapter_runtime.cmd_set_info_registration,
            &bx_ntvdm_adapter_runtime.cmd_get_next, boundary, cpu_before, action, bytes,
            byte_count, &transaction, bx_ntvdm_adapter_runtime.multi_write_payload)) {
        if (!bx_ntvdm_host_session_v1_complete_guest_gather_read(
                &bx_ntvdm_adapter_runtime.session, boundary, cpu_before, &transaction.result) ||
            !bx_ntvdm_host_session_v1_queue_multi_write(&bx_ntvdm_adapter_runtime.session,
                &transaction, bx_ntvdm_adapter_runtime.multi_write_payload,
                transaction.writes.payload_bytes)) return 0;
        bx_ntvdm_cmd_get_next_state_v1_commit(&bx_ntvdm_adapter_runtime.cmd_get_next);
        *result = transaction.result;
        return 1;
    }
    if (bx_ntvdm_cmd_comspec_bootstrap_v1_complete_comspec(
            &bx_ntvdm_adapter_runtime.boot_namespace_provider.readonly_namespace, boundary, cpu_before,
            action, bytes, byte_count, &bx_ntvdm_adapter_runtime.cmd_comspec_bootstrap,
            result)) {
        return bx_ntvdm_host_session_v1_complete_guest_gather_read(
            &bx_ntvdm_adapter_runtime.session, boundary, cpu_before, result);
    }
    if (bx_ntvdm_mouse_install1_mapping_service_v1_complete(boundary,
            cpu_before, action, bytes, byte_count, &transaction,
            bx_ntvdm_adapter_runtime.multi_write_payload)) {
        if (!bx_ntvdm_host_session_v1_complete_guest_gather_read(
                &bx_ntvdm_adapter_runtime.session, boundary, cpu_before,
                &transaction.result) ||
            !bx_ntvdm_host_session_v1_queue_multi_write(
                &bx_ntvdm_adapter_runtime.session, &transaction,
                bx_ntvdm_adapter_runtime.multi_write_payload,
                transaction.writes.payload_bytes)) return 0;
        *result = transaction.result;
        return 1;
    }
    *result = action->cpu_result;
    if (!bx_ntvdm_host_session_v1_complete_guest_gather_read(
            &bx_ntvdm_adapter_runtime.session, boundary, cpu_before, result))
        return 0;
    return 1;
}

int bx_ntvdm_adapter_runtime_v1_take_pending_write(
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    bx_ntvdm_host_transaction_v1 *transaction,
    const uint8_t **payload, uint64_t *payload_bytes,
    uint64_t *resume_rip)
{
    if (!bx_ntvdm_adapter_runtime.installed) return 0;
    return bx_ntvdm_host_session_v1_take_pending_write(
        &bx_ntvdm_adapter_runtime.session, boundary, cpu_before, transaction,
        payload, payload_bytes, resume_rip);
}

int bx_ntvdm_adapter_runtime_v1_take_pending_multi_write(
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    bx_ntvdm_multi_write_transaction_v1 *transaction,
    const uint8_t **payload, uint64_t *payload_bytes)
{
    if (!bx_ntvdm_adapter_runtime.installed) return 0;
    return bx_ntvdm_host_session_v1_take_pending_multi_write(
        &bx_ntvdm_adapter_runtime.session, boundary, cpu_before, transaction,
        payload, payload_bytes);
}

int bx_ntvdm_adapter_runtime_v1_take_pending_bulk_result(
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    bx_ntvdm_bulk_result_transaction_v1 *transaction,
    const uint8_t **payload, uint64_t *payload_bytes)
{
    if (!bx_ntvdm_adapter_runtime.installed) return 0;
    return bx_ntvdm_host_session_v1_take_pending_bulk_result(
        &bx_ntvdm_adapter_runtime.session, boundary, cpu_before, transaction,
        payload, payload_bytes);
}

int bx_ntvdm_adapter_runtime_v1_prepare_observation(uint32_t id,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    uint8_t *output, uint64_t output_bytes)
{
    bx_ntvdm_observation_transaction_v1 transaction;
    if (!bx_ntvdm_adapter_runtime.installed ||
        !bx_ntvdm_adapter_runtime.has_machine_profile || output == 0 ||
        !bx_ntvdm_machine_profile_v1_prepare_observation(
            &bx_ntvdm_adapter_runtime.machine_profile, id, boundary, cpu_before,
            0x100000u, &transaction)) return 0;
    return bx_ntvdm_host_session_v1_queue_observation(
        &bx_ntvdm_adapter_runtime.session, &transaction, output, output_bytes);
}

int bx_ntvdm_adapter_runtime_v1_take_pending_observation(
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    bx_ntvdm_observation_transaction_v1 *transaction,
    uint8_t **output, uint64_t *output_bytes)
{
    if (!bx_ntvdm_adapter_runtime.installed) return 0;
    return bx_ntvdm_host_session_v1_take_pending_observation(
        &bx_ntvdm_adapter_runtime.session, boundary, cpu_before, transaction,
        output, output_bytes);
}

int bx_ntvdm_adapter_runtime_v1_take_pending_startup_snapshot(
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    bx_ntvdm_startup_snapshot_transaction_v1 *transaction,
    uint8_t **output, uint64_t *output_bytes)
{
    if (!bx_ntvdm_adapter_runtime.installed) return 0;
    return bx_ntvdm_host_session_v1_take_pending_startup_snapshot(
        &bx_ntvdm_adapter_runtime.session, boundary, cpu_before, transaction,
        output, output_bytes);
}

int bx_ntvdm_adapter_runtime_v1_prepare_profile_startup_plan(
    bx_ntvdm_startup_plan_v1 *plan, const uint8_t **payload,
    uint64_t *payload_bytes)
{
    bx_ntvdm_cpu_state_v1 entry;
    if (!bx_ntvdm_adapter_runtime.installed ||
        !bx_ntvdm_adapter_runtime.startup_plan_enabled || plan == 0 ||
        payload == 0 || payload_bytes == 0)
        return 0;
    bx_ntvdm_cpu_state_v1_initialize(&entry, BX_NTVDM_CPU_EXECUTION_REAL);
    entry.cs = 0x70u;
    return bx_ntvdm_host_session_v1_prepare_startup_plan(
        &bx_ntvdm_adapter_runtime.session, &entry, 0x700u, 0x714u, 4u,
        plan, payload, payload_bytes);
}

int bx_ntvdm_adapter_runtime_v1_prepare_execution_plan_from_environment(
    bx_ntvdm_startup_plan_v1 *plan, const uint8_t **payload,
    uint64_t *payload_bytes)
{
    int install_result = bx_ntvdm_adapter_runtime_v1_install_from_environment();
    if (install_result < 0) return -1;
    if (install_result == 0 || !bx_ntvdm_adapter_runtime.startup_plan_enabled)
        return 0;
    return bx_ntvdm_adapter_runtime_v1_prepare_profile_startup_plan(
        plan, payload, payload_bytes) ? 1 : -1;
}

int bx_ntvdm_adapter_runtime_v1_prepare_deferred_startup_from_environment(void)
{
    int install_result = bx_ntvdm_adapter_runtime_v1_install_from_environment();
    if (install_result < 0) return -1;
    if (install_result == 0 || !bx_ntvdm_adapter_runtime.deferred_startup_enabled)
        return 0;
    return 1;
}

int bx_ntvdm_adapter_runtime_v1_take_deferred_startup_plan(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_startup_plan_v1 *plan, const uint8_t **payload,
    uint64_t *payload_bytes)
{
    const byob_machine_startup_handoff_descriptor *handoff =
        &bx_ntvdm_adapter_runtime.deferred_startup_handoff;
    if (!bx_ntvdm_adapter_runtime.installed ||
        !bx_ntvdm_adapter_runtime.deferred_startup_enabled ||
        bx_ntvdm_adapter_runtime.deferred_startup_taken || event == 0 ||
        cpu_before == 0 || window == 0 || plan == 0 || payload == 0 ||
        payload_bytes == 0 || !bx_ntvdm_exception_event_v1_valid(event) ||
        !bx_ntvdm_cpu_state_v1_valid(cpu_before) ||
        !bx_ntvdm_instruction_window_v1_valid(window) ||
        event->vector != handoff->exception_vector ||
        event->fault_rip != handoff->ip ||
        cpu_before->execution_mode != BX_NTVDM_CPU_EXECUTION_REAL ||
        cpu_before->cs != handoff->cs ||
        (uint16_t)cpu_before->eip != handoff->ip ||
        window->valid_bytes < 2u ||
        window->bytes[0] != handoff->instruction_bytes[0] ||
        window->bytes[1] != handoff->instruction_bytes[1])
        return 0;
    if (!bx_ntvdm_adapter_runtime_v1_prepare_profile_startup_plan(plan,
            payload, payload_bytes)) return 0;
    bx_ntvdm_adapter_runtime.deferred_startup_taken = 1;
    return 1;
}

int bx_ntvdm_adapter_runtime_v1_prepare_profile_trigger(
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    uint64_t *resume_rip)
{
    bx_ntvdm_observation_transaction_v1 transaction;
    if (!bx_ntvdm_adapter_runtime.installed ||
        !bx_ntvdm_adapter_runtime.has_machine_profile || resume_rip == 0 ||
        !bx_ntvdm_machine_profile_v1_prepare_neutral_ud2_trigger(
            &bx_ntvdm_adapter_runtime.machine_profile, boundary, cpu_before,
            window, 0x100000u, &transaction, resume_rip)) return 0;
    if (!bx_ntvdm_host_session_v1_queue_observation(
        &bx_ntvdm_adapter_runtime.session, &transaction,
        bx_ntvdm_adapter_runtime.observation_output,
        transaction.guest_read.length)) return 0;
    bx_ntvdm_adapter_runtime.active_observation = transaction;
    bx_ntvdm_adapter_runtime.active_observation_output_bytes =
        transaction.guest_read.length;
    bx_ntvdm_adapter_runtime.active_observation_present = 1;
    return 1;
}

int bx_ntvdm_adapter_runtime_v1_complete_profile_observation(
    const bx_ntvdm_observation_transaction_v1 *transaction,
    const uint8_t *output, uint64_t output_bytes, uint64_t *digest)
{
    uint64_t value = UINT64_C(1469598103934665603);
    uint64_t index;
    if (!bx_ntvdm_adapter_runtime.installed || transaction == 0 || output == 0 ||
        digest == 0 || !bx_ntvdm_adapter_runtime.active_observation_present ||
        output != bx_ntvdm_adapter_runtime.observation_output ||
        output_bytes != bx_ntvdm_adapter_runtime.active_observation_output_bytes ||
        memcmp(transaction, &bx_ntvdm_adapter_runtime.active_observation,
            sizeof(*transaction)) != 0) return 0;
    for (index = 0u; index < output_bytes; ++index) {
        value ^= output[index];
        value *= UINT64_C(1099511628211);
    }
    bx_ntvdm_adapter_runtime.active_observation_present = 0;
    memset(&bx_ntvdm_adapter_runtime.active_observation, 0,
        sizeof(bx_ntvdm_adapter_runtime.active_observation));
    bx_ntvdm_adapter_runtime.active_observation_output_bytes = 0u;
    *digest = value;
    return 1;
}

static void bx_ntvdm_snapshot_digest_u64(uint64_t *value, uint64_t input)
{
    uint32_t index;
    for (index = 0u; index < 8u; ++index) {
        *value ^= (uint8_t)(input >> (index * 8u));
        *value *= UINT64_C(1099511628211);
    }
}

int bx_ntvdm_adapter_runtime_v1_complete_profile_startup_snapshot(
    const bx_ntvdm_startup_snapshot_transaction_v1 *transaction,
    const uint8_t *output, uint64_t output_bytes, uint64_t *digest)
{
    uint64_t value = UINT64_C(1469598103934665603);
    uint64_t output_offset = 0u;
    uint32_t range_index;
    if (!bx_ntvdm_adapter_runtime.installed || transaction == 0 || output == 0 ||
        digest == 0 || !bx_ntvdm_adapter_runtime.active_startup_snapshot_present ||
        output != bx_ntvdm_adapter_runtime.observation_output ||
        output_bytes != bx_ntvdm_adapter_runtime.active_startup_snapshot_output_bytes ||
        memcmp(transaction, &bx_ntvdm_adapter_runtime.active_startup_snapshot,
            sizeof(*transaction)) != 0) return 0;
    for (range_index = 0u; range_index < transaction->range_count; ++range_index) {
        const bx_ntvdm_startup_snapshot_range_v1 *range =
            &transaction->ranges[range_index];
        uint64_t byte_index;
        bx_ntvdm_snapshot_digest_u64(&value, range->id);
        bx_ntvdm_snapshot_digest_u64(&value, range->guest_read.address);
        bx_ntvdm_snapshot_digest_u64(&value, range->guest_read.length);
        for (byte_index = 0u; byte_index < range->guest_read.length; ++byte_index) {
            value ^= output[output_offset + byte_index];
            value *= UINT64_C(1099511628211);
        }
        output_offset += range->guest_read.length;
    }
    if (output_offset != output_bytes) return 0;
    if (bx_ntvdm_adapter_runtime.snapshot_evidence_file_name[0] != L'\0' &&
        !bx_ntvdm_startup_snapshot_evidence_v1_write(
            bx_ntvdm_adapter_runtime.snapshot_evidence_root,
            bx_ntvdm_adapter_runtime.snapshot_evidence_file_name, transaction,
            output, output_bytes, value)) return 0;
    bx_ntvdm_adapter_runtime.active_startup_snapshot_present = 0;
    memset(&bx_ntvdm_adapter_runtime.active_startup_snapshot, 0,
        sizeof(bx_ntvdm_adapter_runtime.active_startup_snapshot));
    bx_ntvdm_adapter_runtime.active_startup_snapshot_output_bytes = 0u;
    *digest = value;
    return 1;
}
