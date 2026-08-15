#ifndef BX_NTVDM_DEM_CWD_CONTEXT_V1_H
#define BX_NTVDM_DEM_CWD_CONTEXT_V1_H

#include <stdint.h>
#include <wchar.h>

#include "bx_ntvdm_dem_profile_consumer_v1.h"
#include "bx_ntvdm_host_namespace.h"
#include "bx_ntvdm_mutation_overlay_v1.h"

#define BX_NTVDM_DEM_CWD_CONTEXT_V1_MAGIC 0x42584443u
#define BX_NTVDM_DEM_CWD_CONTEXT_V1_VERSION 1u
#define BX_NTVDM_DEM_CWD_CONTEXT_V1_MAX_RELATIVE 128u

enum bx_ntvdm_dem_cwd_result_v1 {
    BX_NTVDM_DEM_CWD_V1_OK = 1,
    BX_NTVDM_DEM_CWD_V1_REJECTED = 0,
    BX_NTVDM_DEM_CWD_V1_NOT_FOUND = -1,
    BX_NTVDM_DEM_CWD_V1_READONLY = -2,
    BX_NTVDM_DEM_CWD_V1_CAPACITY = -3
};

/* Session-private DEM current-directory state.  A value is always relative
 * to one admitted drive root.  It deliberately contains no host path, host
 * handle, BOP identity, or ambient-process current-directory state. */
typedef struct bx_ntvdm_dem_cwd_context_v1 {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t default_drive;
    bx_ntvdm_dem_profile_consumer_v1 profile;
    bx_ntvdm_mutation_overlay_v1 overlay;
    wchar_t direct_relative[26][BX_NTVDM_DEM_CWD_CONTEXT_V1_MAX_RELATIVE];
    wchar_t virtual_relative[26][BX_NTVDM_DEM_CWD_CONTEXT_V1_MAX_RELATIVE];
} bx_ntvdm_dem_cwd_context_v1;

#ifdef __cplusplus
extern "C" {
#endif

int bx_ntvdm_dem_cwd_context_v1_initialize(
    bx_ntvdm_dem_cwd_context_v1 *context,
    const bx_ntvdm_mutation_profile_v1 *profile);
int bx_ntvdm_dem_cwd_context_v1_valid(
    const bx_ntvdm_dem_cwd_context_v1 *context);
int bx_ntvdm_dem_cwd_context_v1_set(
    bx_ntvdm_dem_cwd_context_v1 *context,
    const bx_ntvdm_host_namespace_v1 *host_namespace, uint8_t drive_index,
    const wchar_t *relative_directory);
int bx_ntvdm_dem_cwd_context_v1_query(
    const bx_ntvdm_dem_cwd_context_v1 *context, uint8_t drive_index,
    wchar_t *relative_directory, uint32_t capacity);
int bx_ntvdm_dem_cwd_context_v1_set_default_drive(
    bx_ntvdm_dem_cwd_context_v1 *context,
    const bx_ntvdm_host_namespace_v1 *host_namespace, uint8_t drive_index);
int bx_ntvdm_dem_cwd_context_v1_query_default_drive(
    const bx_ntvdm_dem_cwd_context_v1 *context, uint8_t *drive_index_out);

#ifdef __cplusplus
}
#endif

#endif
