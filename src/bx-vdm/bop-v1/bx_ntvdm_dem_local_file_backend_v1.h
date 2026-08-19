#ifndef BX_NTVDM_DEM_LOCAL_FILE_BACKEND_V1_H
#define BX_NTVDM_DEM_LOCAL_FILE_BACKEND_V1_H

#include "bx_ntvdm_dem_file_session_v1.h"
#include "bx_ntvdm_dem_file_view_v1.h"
#include "bx_ntvdm_dem_path_v1.h"
#include "bx_ntvdm_host_namespace.h"

#define BX_NTVDM_DEM_LOCAL_FILE_BACKEND_V1_MAGIC 0x42584642u
#define BX_NTVDM_DEM_LOCAL_FILE_BACKEND_V1_VERSION 1u

enum bx_ntvdm_dem_local_file_backend_v1_result {
    BX_NTVDM_DEM_LOCAL_FILE_BACKEND_V1_OK = 1,
    BX_NTVDM_DEM_LOCAL_FILE_BACKEND_V1_REJECTED = 0,
    BX_NTVDM_DEM_LOCAL_FILE_BACKEND_V1_NOT_FOUND = -1,
    BX_NTVDM_DEM_LOCAL_FILE_BACKEND_V1_READONLY = -2,
    BX_NTVDM_DEM_LOCAL_FILE_BACKEND_V1_OVERLAY = -3,
    BX_NTVDM_DEM_LOCAL_FILE_BACKEND_V1_VIRTUAL = -4,
    BX_NTVDM_DEM_LOCAL_FILE_BACKEND_V1_CAPACITY = -5
};

enum bx_ntvdm_dem_local_file_access_v1 {
    BX_NTVDM_DEM_LOCAL_FILE_ACCESS_V1_READ = 1u,
    BX_NTVDM_DEM_LOCAL_FILE_ACCESS_V1_WRITE = 2u
};

/* This is the single package-local bridge from a resolved DOS path to an
 * adapter-private handle token.  Its pointers never cross bx-vdm. */
typedef struct bx_ntvdm_dem_local_file_backend_v1 {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t struct_bytes;
    uint32_t reserved0;
    bx_ntvdm_dem_file_session_v1 *session;
    const bx_ntvdm_dem_file_view_v1 *view;
    const bx_ntvdm_host_namespace_v1 *host_namespace;
    const bx_ntvdm_dem_cwd_context_v1 *cwd;
} bx_ntvdm_dem_local_file_backend_v1;

#ifdef __cplusplus
extern "C" {
#endif

int bx_ntvdm_dem_local_file_backend_v1_initialize(
    bx_ntvdm_dem_local_file_backend_v1 *backend,
    bx_ntvdm_dem_file_session_v1 *session,
    const bx_ntvdm_dem_file_view_v1 *view,
    const bx_ntvdm_host_namespace_v1 *host_namespace,
    const bx_ntvdm_dem_cwd_context_v1 *cwd);
int bx_ntvdm_dem_local_file_backend_v1_valid(
    const bx_ntvdm_dem_local_file_backend_v1 *backend);
int bx_ntvdm_dem_local_file_backend_v1_open(
    bx_ntvdm_dem_local_file_backend_v1 *backend, const char *oem_path,
    uint32_t access, DWORD creation_disposition, uint32_t *token_out);

/* The whole DEM namespace partition needs the original demOpen share-mode
 * result and a copied host error.  This remains a package-private path-to-
 * token operation: neither the root HANDLE nor the opened HANDLE escapes. */
int bx_ntvdm_dem_local_file_backend_v1_open_ex(
    bx_ntvdm_dem_local_file_backend_v1 *backend, const char *oem_path,
    uint32_t access, ULONG share_access, DWORD creation_disposition,
    uint32_t *token_out, DWORD *win32_error_out);
int bx_ntvdm_dem_local_file_backend_v1_open_ex_owned(
    bx_ntvdm_dem_local_file_backend_v1 *backend, const char *oem_path,
    uint32_t access, ULONG share_access, DWORD creation_disposition,
    uint16_t pdb_owner, uint32_t *token_out, DWORD *win32_error_out);

#ifdef __cplusplus
}
#endif

#endif
