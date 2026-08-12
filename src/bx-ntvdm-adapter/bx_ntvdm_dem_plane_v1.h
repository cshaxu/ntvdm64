#ifndef BX_NTVDM_DEM_PLANE_V1_H
#define BX_NTVDM_DEM_PLANE_V1_H

#include <stdint.h>

#include "bx_ntvdm_bop_provider_registry_v1.h"

#define BX_NTVDM_DEM_PLANE_V1_MAGIC 0x42584431u
#define BX_NTVDM_DEM_PLANE_V1_VERSION 1u

typedef enum bx_ntvdm_dem_component_v1 {
    BX_NTVDM_DEM_COMPONENT_NONE = 0u,
    BX_NTVDM_DEM_COMPONENT_NAMESPACE = 1u,
    BX_NTVDM_DEM_COMPONENT_FCB = 2u,
    BX_NTVDM_DEM_COMPONENT_GSET = 3u,
    BX_NTVDM_DEM_COMPONENT_RAW_MEDIA = 4u,
    BX_NTVDM_DEM_COMPONENT_MISC = 5u,
    BX_NTVDM_DEM_COMPONENT_ERROR_LOCK = 6u,
    BX_NTVDM_DEM_COMPONENT_V86_BRIDGE = 7u,
    BX_NTVDM_DEM_COMPONENT_ORIGINAL_NOOP = 8u
} bx_ntvdm_dem_component_v1;

typedef enum bx_ntvdm_dem_plane_disposition_v1 {
    BX_NTVDM_DEM_PLANE_NOT_APPLICABLE = 0u,
    BX_NTVDM_DEM_PLANE_DEFERRED = 1u,
    BX_NTVDM_DEM_PLANE_ORIGINAL_NOOP = 2u
} bx_ntvdm_dem_plane_disposition_v1;

/* A source-derived route fact.  It contains no provider callback, guest
 * pointer, host pointer, or outcome.  The component grouping comes from the
 * original demdisp.c handler definitions, not from reached trace order. */
typedef struct bx_ntvdm_dem_plane_record_v1 {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t service;
    uint32_t component;
    uint32_t disposition;
    uint32_t reserved0;
} bx_ntvdm_dem_plane_record_v1;

#ifdef __cplusplus
extern "C" {
#endif

void bx_ntvdm_dem_plane_v1_clear(bx_ntvdm_dem_plane_record_v1 *record);
int bx_ntvdm_dem_plane_v1_valid(const bx_ntvdm_dem_plane_record_v1 *record);
int bx_ntvdm_dem_plane_v1_classify(
    const bx_ntvdm_bop_ingress_v1 *ingress,
    const bx_ntvdm_bop_provider_selection_v1 *selection,
    bx_ntvdm_dem_plane_record_v1 *record);

#ifdef __cplusplus
}
#endif

#endif
