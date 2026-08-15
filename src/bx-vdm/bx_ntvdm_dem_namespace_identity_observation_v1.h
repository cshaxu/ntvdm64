#ifndef BX_NTVDM_DEM_NAMESPACE_IDENTITY_OBSERVATION_V1_H
#define BX_NTVDM_DEM_NAMESPACE_IDENTITY_OBSERVATION_V1_H

#include "bx_ntvdm_cpu_result_v2.h"
#include "bx_ntvdm_readonly_namespace.h"

enum bx_ntvdm_dem_namespace_identity_class_v1 {
    BX_NTVDM_DEM_NAMESPACE_IDENTITY_V1_NONE = 0u,
    BX_NTVDM_DEM_NAMESPACE_IDENTITY_V1_INVALID = 1u,
    BX_NTVDM_DEM_NAMESPACE_IDENTITY_V1_DECLARED = 2u,
    BX_NTVDM_DEM_NAMESPACE_IDENTITY_V1_OTHER_ADMITTED = 3u,
    BX_NTVDM_DEM_NAMESPACE_IDENTITY_V1_UNADMITTED = 4u
};

/* Fixture-only, default-off result record for the existing DEM namespace
 * transaction. It stores no pathname bytes, host path, guest address, handle,
 * or provider pointer. */
typedef struct bx_ntvdm_dem_namespace_identity_observation_v1 {
    uint32_t seen_open_count, captured;
    uint32_t identity_class, namespace_attached, namespace_generation;
    uint32_t drive_index, admitted_drive, declared_slot, declared_bytes_ready;
    uint32_t disposition, gpr16_write_mask, eflags_write_mask, eflags_values;
    uint16_t gpr16_values[BX_NTVDM_CPU_DELTA_V1_GPR16_COUNT];
} bx_ntvdm_dem_namespace_identity_observation_v1;

#ifdef __cplusplus
extern "C" {
#endif
void bx_ntvdm_dem_namespace_identity_observation_v1_enable(uint32_t enabled);
int bx_ntvdm_dem_namespace_identity_observation_v1_copy(
    bx_ntvdm_dem_namespace_identity_observation_v1 *out);
void bx_ntvdm_dem_namespace_identity_observation_v1_consider(uint8_t service,
    int resolved, uint8_t drive_index, uint32_t admitted_mask,
    const bx_ntvdm_readonly_namespace_v1 *startup_namespace, int declared_match,
    uint32_t declared_slot, uint32_t declared_bytes_ready,
    const bx_ntvdm_cpu_result_v2 *result);
#ifdef __cplusplus
}
#endif

#endif
