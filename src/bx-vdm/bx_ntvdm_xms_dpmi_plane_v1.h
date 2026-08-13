#ifndef BX_NTVDM_XMS_DPMI_PLANE_V1_H
#define BX_NTVDM_XMS_DPMI_PLANE_V1_H

#include <stdint.h>

#include "bx_ntvdm_bop_provider_registry_v1.h"

#define BX_NTVDM_XMS_DPMI_PLANE_V1_MAGIC 0x42585831u
#define BX_NTVDM_XMS_DPMI_PLANE_V1_VERSION 1u

typedef enum bx_ntvdm_xms_dpmi_component_v1 {
    BX_NTVDM_XMS_DPMI_COMPONENT_NONE = 0u,
    BX_NTVDM_XMS_DPMI_COMPONENT_A20 = 1u,
    BX_NTVDM_XMS_DPMI_COMPONENT_EXTENDED_MEMORY = 2u,
    BX_NTVDM_XMS_DPMI_COMPONENT_UMB = 3u,
    BX_NTVDM_XMS_DPMI_COMPONENT_FIRMWARE_INTERRUPT = 4u,
    BX_NTVDM_XMS_DPMI_COMPONENT_DESCRIPTOR_MODE = 5u,
    BX_NTVDM_XMS_DPMI_COMPONENT_INTERRUPT_FAULT = 6u,
    BX_NTVDM_XMS_DPMI_COMPONENT_DOS_BRIDGE = 7u,
    BX_NTVDM_XMS_DPMI_COMPONENT_SESSION = 8u,
    BX_NTVDM_XMS_DPMI_COMPONENT_DEBUG = 9u,
    BX_NTVDM_XMS_DPMI_COMPONENT_VDD = 10u
} bx_ntvdm_xms_dpmi_component_v1;

typedef enum bx_ntvdm_xms_dpmi_machine_owner_v1 {
    BX_NTVDM_XMS_DPMI_MACHINE_OWNER_NONE = 0u,
    BX_NTVDM_XMS_DPMI_MACHINE_OWNER_BOCHS_MEMORY = 1u,
    BX_NTVDM_XMS_DPMI_MACHINE_OWNER_BOCHS_CPU = 2u,
    BX_NTVDM_XMS_DPMI_MACHINE_OWNER_BOCHS_INTERRUPT = 3u,
    BX_NTVDM_XMS_DPMI_MACHINE_OWNER_COMPOSITION = 4u
} bx_ntvdm_xms_dpmi_machine_owner_v1;

typedef enum bx_ntvdm_xms_dpmi_disposition_v1 {
    BX_NTVDM_XMS_DPMI_NOT_APPLICABLE = 0u,
    BX_NTVDM_XMS_DPMI_DEFERRED = 1u,
    BX_NTVDM_XMS_DPMI_EXPLICIT_UNAVAILABLE = 2u
} bx_ntvdm_xms_dpmi_disposition_v1;

typedef struct bx_ntvdm_xms_dpmi_plane_record_v1 {
    uint32_t magic, abi_version, struct_bytes, service;
    uint32_t provider_family, component, machine_owner, disposition;
} bx_ntvdm_xms_dpmi_plane_record_v1;

#ifdef __cplusplus
extern "C" {
#endif
void bx_ntvdm_xms_dpmi_plane_v1_clear(bx_ntvdm_xms_dpmi_plane_record_v1 *record);
int bx_ntvdm_xms_dpmi_plane_v1_valid(const bx_ntvdm_xms_dpmi_plane_record_v1 *record);
int bx_ntvdm_xms_dpmi_plane_v1_classify(const bx_ntvdm_bop_ingress_v1 *ingress,
    const bx_ntvdm_bop_provider_selection_v1 *selection,
    bx_ntvdm_xms_dpmi_plane_record_v1 *record);
#ifdef __cplusplus
}
#endif
#endif
