#ifndef BX_NTVDM_SYSTEM_PLANE_V1_H
#define BX_NTVDM_SYSTEM_PLANE_V1_H

#include <stdint.h>
#include "bx_ntvdm_bop_provider_registry_v1.h"

#define BX_NTVDM_SYSTEM_PLANE_V1_MAGIC 0x42585331u
#define BX_NTVDM_SYSTEM_PLANE_V1_VERSION 1u

typedef enum bx_ntvdm_system_component_v1 {
    BX_NTVDM_SYSTEM_COMPONENT_NONE = 0u,
    BX_NTVDM_SYSTEM_COMPONENT_REDIR_LIFECYCLE = 1u,
    BX_NTVDM_SYSTEM_COMPONENT_REDIR_NAMED_PIPE = 2u,
    BX_NTVDM_SYSTEM_COMPONENT_REDIR_MAILSLOT = 3u,
    BX_NTVDM_SYSTEM_COMPONENT_REDIR_NETWORK = 4u,
    BX_NTVDM_SYSTEM_COMPONENT_REDIR_IDENTITY = 5u,
    BX_NTVDM_SYSTEM_COMPONENT_REDIR_ASYNC_DEVICE = 6u,
    BX_NTVDM_SYSTEM_COMPONENT_REDIR_ASSIGNMENT = 7u,
    BX_NTVDM_SYSTEM_COMPONENT_DEBUG_METADATA = 8u,
    BX_NTVDM_SYSTEM_COMPONENT_DEBUG_EXCEPTION = 9u,
    BX_NTVDM_SYSTEM_COMPONENT_DEBUG_TASK = 10u,
    BX_NTVDM_SYSTEM_COMPONENT_DEBUG_ATTACH = 11u,
    BX_NTVDM_SYSTEM_COMPONENT_DEBUG_DISPATCH = 12u
} bx_ntvdm_system_component_v1;

typedef enum bx_ntvdm_system_disposition_v1 {
    BX_NTVDM_SYSTEM_NOT_APPLICABLE = 0u,
    BX_NTVDM_SYSTEM_DEFERRED = 1u,
    BX_NTVDM_SYSTEM_EXPLICIT_UNAVAILABLE = 2u
} bx_ntvdm_system_disposition_v1;

typedef struct bx_ntvdm_system_plane_record_v1 {
    uint32_t magic, abi_version, struct_bytes, service;
    uint32_t provider_family, component, disposition, reserved0;
} bx_ntvdm_system_plane_record_v1;

#ifdef __cplusplus
extern "C" {
#endif
void bx_ntvdm_system_plane_v1_clear(bx_ntvdm_system_plane_record_v1 *record);
int bx_ntvdm_system_plane_v1_valid(const bx_ntvdm_system_plane_record_v1 *record);
int bx_ntvdm_system_plane_v1_classify(const bx_ntvdm_bop_ingress_v1 *ingress,
    const bx_ntvdm_bop_provider_selection_v1 *selection,
    bx_ntvdm_system_plane_record_v1 *record);
#ifdef __cplusplus
}
#endif
#endif
