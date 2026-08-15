#ifndef BX_NTVDM_DEM_PACKAGE_SESSION_V1_H
#define BX_NTVDM_DEM_PACKAGE_SESSION_V1_H

#include "bx_ntvdm_boot_namespace_plane_v1.h"
#include "bx_ntvdm_dem_error_lock_plane_v1.h"
#include "bx_ntvdm_dem_gset_plane_v1.h"
#include "bx_ntvdm_dem_package_facade_v1.h"
#include "bx_ntvdm_dem_profile_consumer_v1.h"
#include "bx_ntvdm_dem_cwd_context_v1.h"

#define BX_NTVDM_DEM_PACKAGE_SESSION_V1_MAGIC 0x42584453u
#define BX_NTVDM_DEM_PACKAGE_SESSION_V1_VERSION 1u

/* DEM owns its copied session state.  The namespace plane remains its own
 * component because it owns the immutable guest file namespace and pending
 * memory-action state; DEM borrows it through this single package session. */
typedef struct bx_ntvdm_dem_package_session_v1 {
    uint32_t magic, abi_version, struct_bytes, initialized;
    bx_ntvdm_boot_namespace_plane_v1 *namespace_plane;
    bx_ntvdm_dem_error_lock_plane_v1 error_lock;
    bx_ntvdm_dem_gset_plane_v1 gset;
    bx_ntvdm_dem_profile_consumer_v1 mutation_profile;
    bx_ntvdm_dem_cwd_context_v1 cwd;
    uint32_t has_mutation_profile;
    uint32_t boot_drive_index;
    uint32_t has_boot_drive;
} bx_ntvdm_dem_package_session_v1;

#ifdef __cplusplus
extern "C" {
#endif
int bx_ntvdm_dem_package_session_v1_initialize(
    bx_ntvdm_dem_package_session_v1 *session,
    bx_ntvdm_boot_namespace_plane_v1 *namespace_plane);
int bx_ntvdm_dem_package_session_v1_valid(
    const bx_ntvdm_dem_package_session_v1 *session);
int bx_ntvdm_dem_package_session_v1_set_drive_snapshot(
    bx_ntvdm_dem_package_session_v1 *session,
    const bx_ntvdm_host_drive_snapshot_v1 *snapshot);
int bx_ntvdm_dem_package_session_v1_set_volume_snapshot(
    bx_ntvdm_dem_package_session_v1 *session,
    const bx_ntvdm_host_volume_snapshot_v1 *snapshot);
int bx_ntvdm_dem_package_session_v1_set_mutation_profile(
    bx_ntvdm_dem_package_session_v1 *session,
    const bx_ntvdm_mutation_profile_v1 *profile);
int bx_ntvdm_dem_package_session_v1_set_host_namespace(
    bx_ntvdm_dem_package_session_v1 *session,
    const bx_ntvdm_host_namespace_v1 *host_namespace);
int bx_ntvdm_dem_package_session_v1_set_boot_drive(
    bx_ntvdm_dem_package_session_v1 *session, uint32_t drive_index);
int bx_ntvdm_dem_package_session_v1_resolve_mutation_class(
    const bx_ntvdm_dem_package_session_v1 *session,
    uint32_t mutation_class, uint32_t *policy_result_out);
int bx_ntvdm_dem_package_session_v1_dispatch(
    bx_ntvdm_dem_package_session_v1 *session,
    const bx_ntvdm_bop_ingress_v1 *ingress,
    const bx_ntvdm_bop_provider_selection_v1 *selection,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_cpu_result_v2 *result);
#ifdef __cplusplus
}
#endif

#endif
