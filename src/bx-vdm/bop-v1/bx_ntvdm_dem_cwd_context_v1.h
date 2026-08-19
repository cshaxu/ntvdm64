#ifndef BX_NTVDM_DEM_CWD_CONTEXT_V1_H
#define BX_NTVDM_DEM_CWD_CONTEXT_V1_H

/* Compatibility names for existing DEM/path consumers.  The state owner is
 * now the shared session host-context provider, not a second DEM CWD store. */
#include "bx_ntvdm_session_host_context_v1.h"

#define BX_NTVDM_DEM_CWD_CONTEXT_V1_MAGIC BX_NTVDM_SESSION_HOST_CONTEXT_V1_MAGIC
#define BX_NTVDM_DEM_CWD_CONTEXT_V1_VERSION BX_NTVDM_SESSION_HOST_CONTEXT_V1_VERSION
#define BX_NTVDM_DEM_CWD_CONTEXT_V1_MAX_RELATIVE BX_NTVDM_SESSION_HOST_CONTEXT_V1_MAX_RELATIVE

#define BX_NTVDM_DEM_CWD_V1_OK BX_NTVDM_SESSION_HOST_CONTEXT_V1_OK
#define BX_NTVDM_DEM_CWD_V1_REJECTED BX_NTVDM_SESSION_HOST_CONTEXT_V1_REJECTED
#define BX_NTVDM_DEM_CWD_V1_NOT_FOUND BX_NTVDM_SESSION_HOST_CONTEXT_V1_NOT_FOUND
#define BX_NTVDM_DEM_CWD_V1_READONLY BX_NTVDM_SESSION_HOST_CONTEXT_V1_READONLY
#define BX_NTVDM_DEM_CWD_V1_CAPACITY BX_NTVDM_SESSION_HOST_CONTEXT_V1_CAPACITY

typedef bx_ntvdm_session_host_context_v1 bx_ntvdm_dem_cwd_context_v1;

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
int bx_ntvdm_dem_cwd_context_v1_reconcile_direct(
    bx_ntvdm_dem_cwd_context_v1 *context,
    const bx_ntvdm_host_namespace_v1 *host_namespace, uint8_t drive_index);
int bx_ntvdm_dem_cwd_context_v1_set_default_drive(
    bx_ntvdm_dem_cwd_context_v1 *context,
    const bx_ntvdm_host_namespace_v1 *host_namespace, uint8_t drive_index);
int bx_ntvdm_dem_cwd_context_v1_query_default_drive(
    const bx_ntvdm_dem_cwd_context_v1 *context, uint8_t *drive_index_out);

#ifdef __cplusplus
}
#endif

#endif
