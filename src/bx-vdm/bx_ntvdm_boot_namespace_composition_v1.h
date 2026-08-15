#ifndef BX_NTVDM_BOOT_NAMESPACE_COMPOSITION_V1_H
#define BX_NTVDM_BOOT_NAMESPACE_COMPOSITION_V1_H

#include "bx_ntvdm_boot_namespace_plane_v1.h"
#include "bx_ntvdm_command_package_session_v1.h"
#include "bx_ntvdm_dem_package_session_v1.h"
#include "bx_ntvdm_generic_ud_bridge.h"

#define BX_NTVDM_BOOT_NAMESPACE_COMPOSITION_V1_MAGIC 0x42584e43u
#define BX_NTVDM_BOOT_NAMESPACE_COMPOSITION_V1_VERSION 3u

typedef struct bx_ntvdm_boot_namespace_composition_v1 {
    uint32_t magic, abi_version, struct_bytes, bound;
    uint32_t guest_display_state;
    bx_ntvdm_boot_namespace_plane_v1 plane;
    bx_ntvdm_dem_package_session_v1 dem;
    bx_ntvdm_command_package_session_v1 command;
} bx_ntvdm_boot_namespace_composition_v1;

/* Fixed adapter-local observation only.  It deliberately excludes pathname,
 * image pointer, guest address, BOP identity and provider selection state. */
typedef struct bx_ntvdm_boot_namespace_diagnostic_v1 {
    uint32_t version;
    uint32_t open;
    uint32_t open_file_index;
    uint32_t generation;
    uint64_t offset;
} bx_ntvdm_boot_namespace_diagnostic_v1;

#define BX_NTVDM_BOOT_NAMESPACE_DIAGNOSTIC_V1_VERSION 1u

#ifdef __cplusplus
extern "C" {
#endif
int bx_ntvdm_boot_namespace_composition_v1_initialize(
    bx_ntvdm_boot_namespace_composition_v1 *value, const byob_image *ntdos,
    const byob_image *command,
    const byob_image *target, const byob_image *quit,
    const byob_profile_selection *selection);
int bx_ntvdm_boot_namespace_composition_v1_bind(
    bx_ntvdm_boot_namespace_composition_v1 *value);
void bx_ntvdm_boot_namespace_composition_v1_unbind(
    bx_ntvdm_boot_namespace_composition_v1 *value);
int bx_ntvdm_boot_namespace_composition_v1_set_drive_snapshot(
    bx_ntvdm_boot_namespace_composition_v1 *value,
    const bx_ntvdm_host_drive_snapshot_v1 *snapshot);
/* Volume metadata is captured once by the CLI-owned admission boundary and
 * copied into the DEM session before the composition is bound.  This does not
 * grant a BOP provider an ambient host-volume query. */
int bx_ntvdm_boot_namespace_composition_v1_set_volume_snapshot(
    bx_ntvdm_boot_namespace_composition_v1 *value,
    const bx_ntvdm_host_volume_snapshot_v1 *snapshot);
/* Installs the session-selected common profile into DEM's package consumer.
 * It does not select a BOP leaf or perform host I/O. */
int bx_ntvdm_boot_namespace_composition_v1_set_dem_mutation_profile(
    bx_ntvdm_boot_namespace_composition_v1 *value,
    const bx_ntvdm_mutation_profile_v1 *profile);
int bx_ntvdm_boot_namespace_composition_v1_set_command_mutation_profile(
    bx_ntvdm_boot_namespace_composition_v1 *value,
    const bx_ntvdm_mutation_profile_v1 *profile);
int bx_ntvdm_boot_namespace_composition_v1_set_command_host_context(
    bx_ntvdm_boot_namespace_composition_v1 *value,
    const bx_ntvdm_command_host_context_v1 *context);
int bx_ntvdm_boot_namespace_composition_v1_set_dem_host_namespace(
    bx_ntvdm_boot_namespace_composition_v1 *value,
    const bx_ntvdm_host_namespace_v1 *host_namespace);
int bx_ntvdm_boot_namespace_composition_v1_set_dem_boot_drive(
    bx_ntvdm_boot_namespace_composition_v1 *value, uint32_t drive_index);
int bx_ntvdm_boot_namespace_composition_v1_set_launch_plan(
    bx_ntvdm_boot_namespace_composition_v1 *value,
    const byob_launch_plan_v2 *plan);
int bx_ntvdm_boot_namespace_composition_v1_handle(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    struct bx_ntvdm_generic_ud_outcome_v1 *outcome);
int bx_ntvdm_boot_namespace_composition_v1_copy_namespace_diagnostic(
    const bx_ntvdm_boot_namespace_composition_v1 *value,
    bx_ntvdm_boot_namespace_diagnostic_v1 *out);
#ifdef __cplusplus
}
#endif

#endif
