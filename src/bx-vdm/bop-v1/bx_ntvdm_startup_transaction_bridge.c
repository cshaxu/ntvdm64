#include "bx_ntvdm_startup_transaction_bridge.h"

#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "bx_ntvdm_host_service_catalog.h"
#include "bx_ntvdm_host_service_plane.h"
#include "bx_ntvdm_host_drive_policy.h"
#include "bx_ntvdm_namespace_root_set.h"
#include "bx_ntvdm_cmd_current_dir_service.h"
#include "bx_ntvdm_dem_dpb_service.h"
#include "bx_ntvdm_session_resource_abi.h"
#include "bx_ntvdm_startup_session_environment.h"

typedef struct bx_ntvdm_startup_transaction_bridge_state_v1 {
    int attempted, installed, consumed, pending;
    bx_ntvdm_startup_session_v1 startup_session;
    bx_ntvdm_session_resources_v1 resources;
    bx_ntvdm_host_drive_snapshot_v1 host_drive_snapshot;
    bx_ntvdm_namespace_root_set_v1 namespace_roots;
    bx_ntvdm_host_service_payloads_v1 payloads;
    bx_ntvdm_exception_event_v1 boundary;
    bx_ntvdm_cpu_state_v1 cpu_before;
    bx_ntvdm_host_transaction_v1 transaction;
    uint64_t resume_rip;
    int has_dta_registration;
    bx_ntvdm_dem_dta_registration_v1 dta_registration;
    int has_hard_error_registration;
    bx_ntvdm_dem_hard_error_registration_v1 hard_error_registration;
    int has_cmd_set_info_registration;
    bx_ntvdm_cmd_set_info_registration_v1 cmd_set_info_registration;
    int pending_multi_write;
    bx_ntvdm_multi_write_transaction_v1 multi_write;
    uint8_t multi_write_payload[BX_NTVDM_MULTI_WRITE_MAX_PAYLOAD];
} bx_ntvdm_startup_transaction_bridge_state_v1;

static bx_ntvdm_startup_transaction_bridge_state_v1 bridge;

static void bx_ntvdm_startup_transaction_bridge_v1_release_namespace(void)
{
    bx_ntvdm_namespace_root_set_v1_release(&bridge.namespace_roots);
}

static void bx_ntvdm_startup_transaction_bridge_v1_clear_pending(void)
{
    bridge.pending = 0;
    memset(&bridge.boundary, 0, sizeof(bridge.boundary));
    memset(&bridge.cpu_before, 0, sizeof(bridge.cpu_before));
    memset(&bridge.transaction, 0, sizeof(bridge.transaction));
    bridge.resume_rip = 0u;
}

static int bx_ntvdm_startup_transaction_bridge_v1_install(void)
{
    int status;
    if (bridge.installed) return 1;
    if (bridge.attempted) return 0;
    bridge.attempted = 1;
    status = bx_ntvdm_startup_session_v1_install_from_environment(
        &bridge.startup_session);
    if (status <= 0) return status;
    bx_ntvdm_session_resources_v1_initialize(&bridge.resources);
    bridge.resources.resources[0].kind = BX_NTVDM_SESSION_RESOURCE_NTIO;
    bridge.resources.resources[0].byte_count = bridge.startup_session.ntio.byte_count;
    bridge.resources.resources[1].kind = BX_NTVDM_SESSION_RESOURCE_NTDOS;
    bridge.resources.resources[1].byte_count = bridge.startup_session.ntdos.byte_count;
    bridge.resources.resources[2].kind = BX_NTVDM_SESSION_RESOURCE_COMMAND;
    bridge.resources.resources[2].byte_count = bridge.startup_session.command.byte_count;
    bridge.payloads.ntio_bytes = bridge.startup_session.ntio.bytes;
    bridge.payloads.ntio_byte_count = bridge.startup_session.ntio.byte_count;
    bridge.payloads.ntdos_bytes = bridge.startup_session.ntdos.bytes;
    bridge.payloads.ntdos_byte_count = bridge.startup_session.ntdos.byte_count;
    bridge.payloads.command_bytes = bridge.startup_session.command.bytes;
    bridge.payloads.command_byte_count = bridge.startup_session.command.byte_count;
    if (!bx_ntvdm_session_resources_v1_valid(&bridge.resources) ||
        !bx_ntvdm_host_drive_snapshot_v1_capture_from_environment(
            &bridge.host_drive_snapshot) ||
        !bx_ntvdm_namespace_root_set_v1_install(&bridge.namespace_roots,
            &bridge.host_drive_snapshot) ||
        atexit(bx_ntvdm_startup_transaction_bridge_v1_release_namespace) != 0) {
        bx_ntvdm_namespace_root_set_v1_release(&bridge.namespace_roots);
        return -1;
    }
    bridge.installed = 1;
    return 1;
}

int bx_ntvdm_startup_transaction_bridge_v1_prepare_from_environment(
    bx_ntvdm_startup_plan_v1 *plan, const uint8_t **payload,
    uint64_t *payload_bytes)
{
    int status = bx_ntvdm_startup_transaction_bridge_v1_install();
    if (status <= 0) return status;
    return bx_ntvdm_startup_session_v1_prepare_ntio_plan(&bridge.startup_session,
        plan, payload, payload_bytes) ? 1 : -1;
}

int bx_ntvdm_startup_transaction_bridge_v1_dispatch(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_exception_result_v1 *result)
{
    bx_ntvdm_host_service_request_v1 request;
    bx_ntvdm_host_service_plane_result_v1 prepared;
    int status;
    if (result == 0 || !bx_ntvdm_exception_event_v1_valid(event) ||
        !bx_ntvdm_cpu_state_v1_valid(cpu_before) ||
        !bx_ntvdm_instruction_window_v1_valid(window)) return 0;
    bx_ntvdm_exception_result_v1_pass_through(result);
    status = bx_ntvdm_startup_transaction_bridge_v1_install();
    if (status < 0) return 0;
    if (status == 0 || bridge.consumed || event->vector != 6u ||
        !bx_ntvdm_host_service_catalog_v1_lookup(window, &request) ||
        event->fault_rip > UINT64_MAX - 4u) return 1;
    bx_ntvdm_startup_transaction_bridge_v1_clear_pending();
    prepared = bx_ntvdm_host_service_plane_v1_prepare(&request, cpu_before,
        &bridge.resources, &bridge.payloads, UINT64_C(0x100000),
        &bridge.transaction);
    if (prepared != BX_NTVDM_HOST_SERVICE_PLANE_READY) return 1;
    bridge.pending = 1;
    bridge.boundary = *event;
    bridge.cpu_before = *cpu_before;
    bridge.resume_rip = event->fault_rip + 4u;
    return bx_ntvdm_exception_result_v1_resume(result, bridge.resume_rip);
}

int bx_ntvdm_startup_transaction_bridge_v1_take_pending_write(
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    bx_ntvdm_host_transaction_v1 *transaction,
    const uint8_t **payload, uint64_t *payload_bytes,
    uint64_t *resume_rip)
{
    int valid = bridge.pending && boundary != 0 && cpu_before != 0 &&
        transaction != 0 && payload != 0 && payload_bytes != 0 && resume_rip != 0 &&
        memcmp(&bridge.boundary, boundary, sizeof(*boundary)) == 0 &&
        memcmp(&bridge.cpu_before, cpu_before, sizeof(*cpu_before)) == 0 &&
        bx_ntvdm_host_transaction_v1_preflight(&bridge.transaction,
            UINT64_C(0x100000), bridge.payloads.ntdos_byte_count);
    if (!valid) {
        bx_ntvdm_startup_transaction_bridge_v1_clear_pending();
        return 0;
    }
    *transaction = bridge.transaction;
    *payload = bridge.payloads.ntdos_bytes;
    *payload_bytes = bridge.payloads.ntdos_byte_count;
    *resume_rip = bridge.resume_rip;
    bridge.consumed = 1;
    bx_ntvdm_startup_transaction_bridge_v1_clear_pending();
    return 1;
}


int bx_ntvdm_startup_transaction_bridge_v1_copy_host_drive_inventory(
    uint8_t out_types[26])
{
    if (!bridge.installed) return 0;
    return bx_ntvdm_startup_session_v1_copy_host_drive_inventory(
        &bridge.startup_session, out_types);
}

int bx_ntvdm_startup_transaction_bridge_v1_store_dta_registration(
    const bx_ntvdm_dem_dta_registration_v1 *registration)
{
    if (!bridge.installed || registration == 0) return 0;
    bridge.dta_registration = *registration;
    bridge.has_dta_registration = 1;
    return 1;
}

int bx_ntvdm_startup_transaction_bridge_v1_store_hard_error_registration(
    const bx_ntvdm_dem_hard_error_registration_v1 *registration)
{
    if (!bridge.installed || registration == 0) return 0;
    bridge.hard_error_registration = *registration;
    bridge.has_hard_error_registration = 1;
    return 1;
}

int bx_ntvdm_startup_transaction_bridge_v1_store_cmd_set_info_registration(
    const bx_ntvdm_cmd_set_info_registration_v1 *registration)
{
    if (!bridge.installed || registration == 0) return 0;
    bridge.cmd_set_info_registration = *registration;
    bridge.has_cmd_set_info_registration = 1;
    return 1;
}

int bx_ntvdm_startup_transaction_bridge_v1_prepare_multi_write(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window)
{
    uint8_t inventory_types[26];
    int prepared = 0;
    if (!bridge.installed || bridge.pending_multi_write || event == 0 ||
        cpu_before == 0 || window == 0) return 0;
    if (bx_ntvdm_startup_session_v1_copy_host_drive_inventory(
            &bridge.startup_session, inventory_types)) {
        prepared = bx_ntvdm_dem_dpb_service_v1_prepare(inventory_types, event,
            cpu_before, window, &bridge.multi_write,
            bridge.multi_write_payload);
    }
    if (!prepared) prepared = bx_ntvdm_cmd_current_dir_service_v1_prepare(
        bridge.namespace_roots.available_mask, 0, event, cpu_before, window,
        &bridge.multi_write, bridge.multi_write_payload);
    if (!prepared) return 0;
    bridge.pending_multi_write = 1;
    return 1;
}

int bx_ntvdm_startup_transaction_bridge_v1_take_pending_multi_write(
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    bx_ntvdm_multi_write_transaction_v1 *transaction,
    const uint8_t **payload, uint64_t *payload_bytes)
{
    int valid = bridge.pending_multi_write && event != 0 && cpu_before != 0 &&
        transaction != 0 && payload != 0 && payload_bytes != 0 &&
        memcmp(&bridge.multi_write.boundary, event, sizeof(*event)) == 0 &&
        memcmp(&bridge.multi_write.cpu_before, cpu_before,
            sizeof(*cpu_before)) == 0 &&
        ((bridge.multi_write.writes.write_count == 0u &&
          bridge.multi_write.writes.payload_bytes == 0u &&
          bx_ntvdm_cpu_result_v2_valid(&bridge.multi_write.result) &&
          bridge.multi_write.result.disposition == BX_NTVDM_CPU_RESULT_V2_RESUME) ||
         bx_ntvdm_multi_write_transaction_v1_preflight(&bridge.multi_write,
            UINT64_C(0x100000), bridge.multi_write.writes.payload_bytes));
    if (!valid) {
        bridge.pending_multi_write = 0;
        return 0;
    }
    *transaction = bridge.multi_write;
    *payload = bridge.multi_write_payload;
    *payload_bytes = bridge.multi_write.writes.payload_bytes;
    bridge.pending_multi_write = 0;
    return 1;
}
