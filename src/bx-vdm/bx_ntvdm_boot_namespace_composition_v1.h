#ifndef BX_NTVDM_BOOT_NAMESPACE_COMPOSITION_V1_H
#define BX_NTVDM_BOOT_NAMESPACE_COMPOSITION_V1_H

#include "bx_ntvdm_boot_namespace_plane_v1.h"
#include "bx_ntvdm_command_launch_plane_v1.h"
#include "bx_ntvdm_dem_error_lock_plane_v1.h"
#include "bx_ntvdm_dem_gset_plane_v1.h"
#include "bx_ntvdm_generic_ud_bridge.h"

#define BX_NTVDM_BOOT_NAMESPACE_COMPOSITION_V1_MAGIC 0x42584e43u
#define BX_NTVDM_BOOT_NAMESPACE_COMPOSITION_V1_VERSION 1u

typedef struct bx_ntvdm_boot_namespace_composition_v1 {
    uint32_t magic, abi_version, struct_bytes, bound;
    bx_ntvdm_boot_namespace_plane_v1 plane;
    bx_ntvdm_command_launch_plane_v1 launch;
    bx_ntvdm_dem_error_lock_plane_v1 error_lock;
    bx_ntvdm_dem_gset_plane_v1 gset;
} bx_ntvdm_boot_namespace_composition_v1;

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
int bx_ntvdm_boot_namespace_composition_v1_handle(
    const struct bx_ntvdm_generic_ud_event_v1 *event,
    struct bx_ntvdm_generic_ud_outcome_v1 *outcome);
#ifdef __cplusplus
}
#endif

#endif
