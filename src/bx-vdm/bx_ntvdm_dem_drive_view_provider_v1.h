#ifndef BX_NTVDM_DEM_DRIVE_VIEW_PROVIDER_V1_H
#define BX_NTVDM_DEM_DRIVE_VIEW_PROVIDER_V1_H

#include "bx_ntvdm_dem_cwd_context_v1.h"
#include "bx_ntvdm_dem_gset_plane_v1.h"
#include "bx_ntvdm_dem_profile_consumer_v1.h"
#include "bx_ntvdm_multi_write_transaction.h"
#include "bx_ntvdm_guest_gather_read_action_v1.h"

#define BX_NTVDM_DEM_DRIVE_VIEW_PROVIDER_V1_MAGIC 0x42584456u
#define BX_NTVDM_DEM_DRIVE_VIEW_PROVIDER_V1_VERSION 1u

/* One DEM-owned session state for all drive-view/CWD/GSET services.  It
 * contains copied adapter state only; no guest pointer, host handle, Bochs
 * object or selector/service identity crosses this boundary. */
typedef struct bx_ntvdm_dem_drive_view_provider_v1 {
    uint32_t magic, abi_version, struct_bytes, initialized;
    bx_ntvdm_dem_gset_plane_v1 gset;
    bx_ntvdm_dem_profile_consumer_v1 mutation_profile;
    bx_ntvdm_dem_cwd_context_v1 cwd;
    uint32_t has_mutation_profile, boot_drive_index, has_boot_drive;
} bx_ntvdm_dem_drive_view_provider_v1;

void bx_ntvdm_dem_drive_view_provider_v1_clear(
    bx_ntvdm_dem_drive_view_provider_v1 *provider);
int bx_ntvdm_dem_drive_view_provider_v1_initialize(
    bx_ntvdm_dem_drive_view_provider_v1 *provider);
int bx_ntvdm_dem_drive_view_provider_v1_valid(
    const bx_ntvdm_dem_drive_view_provider_v1 *provider);
int bx_ntvdm_dem_drive_view_provider_v1_set_drive_snapshot(
    bx_ntvdm_dem_drive_view_provider_v1 *provider,
    const bx_ntvdm_host_drive_snapshot_v1 *snapshot);
int bx_ntvdm_dem_drive_view_provider_v1_set_volume_snapshot(
    bx_ntvdm_dem_drive_view_provider_v1 *provider,
    const bx_ntvdm_host_volume_snapshot_v1 *snapshot);
int bx_ntvdm_dem_drive_view_provider_v1_set_mutation_profile(
    bx_ntvdm_dem_drive_view_provider_v1 *provider,
    const bx_ntvdm_mutation_profile_v1 *profile);
int bx_ntvdm_dem_drive_view_provider_v1_set_boot_drive(
    bx_ntvdm_dem_drive_view_provider_v1 *provider, uint32_t drive_index);

/* Owns the source-admitted observation/layout group.  It returns an already
 * completed CPU result or one checked multi-write transaction; the package
 * session alone executes the transaction through the mechanical boundary. */
int bx_ntvdm_dem_drive_view_provider_v1_dispatch_observation(
    bx_ntvdm_dem_drive_view_provider_v1 *provider,
    const bx_ntvdm_bop_ingress_v1 *ingress,
    const bx_ntvdm_bop_provider_selection_v1 *selection,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_multi_write_transaction_v1 *transaction,
    uint8_t payload[BX_NTVDM_MULTI_WRITE_MAX_PAYLOAD],
    uint32_t *needs_write,
    bx_ntvdm_cpu_result_v2 *result);

int bx_ntvdm_dem_drive_view_provider_v1_owns_observation(uint8_t service);

/* Direct/Readonly CWD handoff only. Overlay and Virtual deliberately decline
 * so their retained namespace route stays unchanged until separately admitted. */
int bx_ntvdm_dem_drive_view_provider_v1_prepare_cwd(
    const bx_ntvdm_dem_drive_view_provider_v1 *provider, uint8_t service,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    bx_ntvdm_guest_gather_read_action_v1 *action);
int bx_ntvdm_dem_drive_view_provider_v1_complete_cwd(
    bx_ntvdm_dem_drive_view_provider_v1 *provider,
    const bx_ntvdm_host_namespace_v1 *host_namespace, uint8_t service,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_guest_gather_read_action_v1 *action,
    const uint8_t *bytes, uint64_t byte_count,
    bx_ntvdm_multi_write_transaction_v1 *transaction,
    uint8_t payload[BX_NTVDM_MULTI_WRITE_MAX_PAYLOAD]);
#endif
