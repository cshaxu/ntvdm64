#ifndef BX_NTVDM_BOOT_NAMESPACE_PLANE_V1_H
#define BX_NTVDM_BOOT_NAMESPACE_PLANE_V1_H

#include "bx_ntvdm_boot_namespace_provider_v1.h"
#include "bx_ntvdm_command_plane_v1.h"
#include "bx_ntvdm_dem_plane_v1.h"
#include "bx_ntvdm_dem_dta_service.h"
#include "bx_ntvdm_dem_load_dos_service.h"
#include "bx-mantle/bx_ntvdm_mechanical_action_v1.h"

#define BX_NTVDM_BOOT_NAMESPACE_PLANE_V1_MAGIC 0x42584e51u
#define BX_NTVDM_BOOT_NAMESPACE_PLANE_V1_VERSION 1u

enum bx_ntvdm_boot_namespace_pending_v1_kind {
    BX_NTVDM_BOOT_NAMESPACE_PENDING_V1_NONE = 0u,
    BX_NTVDM_BOOT_NAMESPACE_PENDING_V1_OPEN = 1u,
    BX_NTVDM_BOOT_NAMESPACE_PENDING_V1_PATH_FIRST = 2u,
    BX_NTVDM_BOOT_NAMESPACE_PENDING_V1_PATH_NEXT = 3u,
    BX_NTVDM_BOOT_NAMESPACE_PENDING_V1_DTA_REGISTRATION = 4u
};

typedef struct bx_ntvdm_boot_namespace_plane_v1 {
    uint32_t magic, abi_version, struct_bytes, next_action_id;
    bx_ntvdm_boot_namespace_provider_v1 provider;
    byob_image ntdos;
    byob_component_descriptor ntdos_identity;
    bx_ntvdm_dem_dta_registration_v1 dta;
    uint32_t has_dta, pending_kind, pending_action_id;
    bx_ntvdm_exception_event_v1 pending_event;
    bx_ntvdm_cpu_state_v1 pending_cpu;
    bx_ntvdm_guest_read_action_v1 pending_read;
    bx_ntvdm_guest_gather_read_action_v1 pending_gather;
} bx_ntvdm_boot_namespace_plane_v1;

int bx_ntvdm_boot_namespace_plane_v1_initialize(
    bx_ntvdm_boot_namespace_plane_v1 *plane, const byob_image *ntdos,
    const byob_image *command,
    const byob_image *target, const byob_image *terminal_quit,
    const byob_profile_selection *selection);
int bx_ntvdm_boot_namespace_plane_v1_set_dta(
    bx_ntvdm_boot_namespace_plane_v1 *plane,
    const bx_ntvdm_dem_dta_registration_v1 *dta);
int bx_ntvdm_boot_namespace_plane_v1_dispatch(
    bx_ntvdm_boot_namespace_plane_v1 *plane,
    const bx_ntvdm_bop_ingress_v1 *ingress,
    const bx_ntvdm_bop_provider_selection_v1 *selection,
    const bx_ntvdm_exception_event_v1 *event,
    const bx_ntvdm_cpu_state_v1 *cpu,
    const bx_ntvdm_instruction_window_v1 *window,
    struct bx_ntvdm_mechanical_action_v1 *action,
    bx_ntvdm_cpu_result_v2 *result);
int bx_ntvdm_boot_namespace_plane_v1_complete_read(
    bx_ntvdm_boot_namespace_plane_v1 *plane,
    const struct bx_ntvdm_mechanical_action_v1 *read_action,
    struct bx_ntvdm_mechanical_action_v1 *next_action,
    bx_ntvdm_cpu_result_v2 *result);

#endif
