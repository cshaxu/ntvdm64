#ifndef BX_NTVDM_DEM_FILE_VIEW_V1_H
#define BX_NTVDM_DEM_FILE_VIEW_V1_H

#include <stdint.h>

#include "bx_ntvdm_dem_profile_consumer_v1.h"

#define BX_NTVDM_DEM_FILE_VIEW_V1_MAGIC 0x42584656u
#define BX_NTVDM_DEM_FILE_VIEW_V1_VERSION 1u

/* One DEM whole-provider view selection.  This is intentionally package
 * scoped: a BOP leaf may ask whether an operation is a read or mutation, but
 * may not select a host-mutation mode or bypass this owner. */
enum bx_ntvdm_dem_file_view_v1_kind {
    BX_NTVDM_DEM_FILE_VIEW_V1_DIRECT = 1u,
    BX_NTVDM_DEM_FILE_VIEW_V1_READONLY = 2u,
    BX_NTVDM_DEM_FILE_VIEW_V1_OVERLAY = 3u,
    BX_NTVDM_DEM_FILE_VIEW_V1_VIRTUAL = 4u
};

enum bx_ntvdm_dem_file_view_v1_result {
    BX_NTVDM_DEM_FILE_VIEW_V1_OK = 1,
    BX_NTVDM_DEM_FILE_VIEW_V1_REJECTED = 0,
    BX_NTVDM_DEM_FILE_VIEW_V1_DENIED_READONLY = -1,
    /* These two results are deliberately not a permission to use Direct.
     * They remain until S3 installs their package-owned namespace backends. */
    BX_NTVDM_DEM_FILE_VIEW_V1_NEEDS_OVERLAY_BACKEND = -2,
    BX_NTVDM_DEM_FILE_VIEW_V1_NEEDS_VIRTUAL_BACKEND = -3
};

typedef struct bx_ntvdm_dem_file_view_v1 {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t kind;
    bx_ntvdm_dem_profile_consumer_v1 profile;
} bx_ntvdm_dem_file_view_v1;

#ifdef __cplusplus
extern "C" {
#endif

int bx_ntvdm_dem_file_view_v1_initialize(
    bx_ntvdm_dem_file_view_v1 *view,
    const bx_ntvdm_mutation_profile_v1 *profile);
int bx_ntvdm_dem_file_view_v1_valid(const bx_ntvdm_dem_file_view_v1 *view);
/* `mutation_class` remains an owner-wide capability decision.  A read uses
 * NONE_MECHANICAL; writes/creates/metadata pass their original DEM class. */
int bx_ntvdm_dem_file_view_v1_admit(
    const bx_ntvdm_dem_file_view_v1 *view, uint32_t mutation_class);

#ifdef __cplusplus
}
#endif

#endif
