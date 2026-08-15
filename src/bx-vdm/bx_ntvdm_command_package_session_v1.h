#ifndef BX_NTVDM_COMMAND_PACKAGE_SESSION_V1_H
#define BX_NTVDM_COMMAND_PACKAGE_SESSION_V1_H

#include "bx_ntvdm_boot_namespace_plane_v1.h"
#include "bx_ntvdm_cmd_comspec_bootstrap_service.h"
#include "bx_ntvdm_cmd_get_next_service.h"
#include "bx_ntvdm_command_launch_plane_v1.h"
#include "bx_ntvdm_command_package_facade_v1.h"
#include "bx_ntvdm_dem_gset_plane_v1.h"
#include "bx_ntvdm_command_profile_consumer_v1.h"

#define BX_NTVDM_COMMAND_PACKAGE_SESSION_V1_MAGIC 0x42584353u
#define BX_NTVDM_COMMAND_PACKAGE_SESSION_V1_VERSION 1u

/* One adapter-owned COMMAND package state.  The two borrowed planes remain
 * owned by their respective namespace and DEM components; this session owns
 * only COMMAND's copied bootstrap, launch and one-shot delivery state. */
typedef struct bx_ntvdm_command_package_session_v1 {
    uint32_t magic, abi_version, struct_bytes, initialized;
    bx_ntvdm_boot_namespace_plane_v1 *namespace_plane;
    bx_ntvdm_dem_gset_plane_v1 *gset;
    bx_ntvdm_cmd_comspec_bootstrap_v1 bootstrap;
    bx_ntvdm_command_launch_plane_v1 launch;
    byob_launch_plan_v2 launch_plan;
    bx_ntvdm_cmd_get_next_state_v1 get_next;
    bx_ntvdm_command_profile_consumer_v1 mutation_profile;
    uint32_t has_launch_plan, has_mutation_profile;
} bx_ntvdm_command_package_session_v1;

#ifdef __cplusplus
extern "C" {
#endif
int bx_ntvdm_command_package_session_v1_initialize(
    bx_ntvdm_command_package_session_v1 *session,
    bx_ntvdm_boot_namespace_plane_v1 *namespace_plane,
    bx_ntvdm_dem_gset_plane_v1 *gset);
int bx_ntvdm_command_package_session_v1_set_launch_plan(
    bx_ntvdm_command_package_session_v1 *session,
    const byob_launch_plan_v2 *plan);
int bx_ntvdm_command_package_session_v1_set_mutation_profile(
    bx_ntvdm_command_package_session_v1 *session,
    const bx_ntvdm_mutation_profile_v1 *profile);
int bx_ntvdm_command_package_session_v1_resolve_mutation_class(
    const bx_ntvdm_command_package_session_v1 *session,
    uint32_t mutation_class, uint32_t *policy_result_out);
int bx_ntvdm_command_package_session_v1_valid(
    const bx_ntvdm_command_package_session_v1 *session);
int bx_ntvdm_command_package_session_v1_dispatch(
    bx_ntvdm_command_package_session_v1 *session,
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
