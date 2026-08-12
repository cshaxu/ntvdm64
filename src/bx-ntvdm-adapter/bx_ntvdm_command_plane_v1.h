#ifndef BX_NTVDM_COMMAND_PLANE_V1_H
#define BX_NTVDM_COMMAND_PLANE_V1_H

#include <stdint.h>
#include "bx_ntvdm_bop_provider_registry_v1.h"

#define BX_NTVDM_COMMAND_PLANE_V1_MAGIC 0x42584331u
#define BX_NTVDM_COMMAND_PLANE_V1_VERSION 1u

typedef enum bx_ntvdm_command_component_v1 {
    BX_NTVDM_COMMAND_COMPONENT_NONE = 0u,
    BX_NTVDM_COMMAND_COMPONENT_LIFECYCLE = 1u,
    BX_NTVDM_COMMAND_COMPONENT_LAUNCH = 2u,
    BX_NTVDM_COMMAND_COMPONENT_CONFIG = 3u,
    BX_NTVDM_COMMAND_COMPONENT_EXECUTION = 4u,
    BX_NTVDM_COMMAND_COMPONENT_CONSOLE = 5u,
    BX_NTVDM_COMMAND_COMPONENT_ENVIRONMENT = 6u,
    BX_NTVDM_COMMAND_COMPONENT_SESSION = 7u,
    BX_NTVDM_COMMAND_COMPONENT_ORIGINAL_NOOP = 8u
} bx_ntvdm_command_component_v1;

typedef enum bx_ntvdm_command_plane_disposition_v1 {
    BX_NTVDM_COMMAND_PLANE_NOT_APPLICABLE = 0u,
    BX_NTVDM_COMMAND_PLANE_DEFERRED = 1u,
    BX_NTVDM_COMMAND_PLANE_EXPLICIT_UNAVAILABLE = 2u,
    BX_NTVDM_COMMAND_PLANE_ORIGINAL_NOOP = 3u
} bx_ntvdm_command_plane_disposition_v1;

typedef struct bx_ntvdm_command_plane_record_v1 {
    uint32_t magic, abi_version, struct_bytes, service;
    uint32_t component, disposition, reserved0;
} bx_ntvdm_command_plane_record_v1;

#ifdef __cplusplus
extern "C" {
#endif
void bx_ntvdm_command_plane_v1_clear(bx_ntvdm_command_plane_record_v1 *record);
int bx_ntvdm_command_plane_v1_valid(const bx_ntvdm_command_plane_record_v1 *record);
int bx_ntvdm_command_plane_v1_classify(const bx_ntvdm_bop_ingress_v1 *ingress,
    const bx_ntvdm_bop_provider_selection_v1 *selection,
    bx_ntvdm_command_plane_record_v1 *record);
#ifdef __cplusplus
}
#endif
#endif
