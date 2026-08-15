#include "bx_ntvdm_dem_local_file_backend_v1.h"

#include <string.h>

int bx_ntvdm_dem_local_file_backend_v1_valid(
    const bx_ntvdm_dem_local_file_backend_v1 *backend)
{
    return backend != 0 &&
        backend->magic == BX_NTVDM_DEM_LOCAL_FILE_BACKEND_V1_MAGIC &&
        backend->abi_version == BX_NTVDM_DEM_LOCAL_FILE_BACKEND_V1_VERSION &&
        backend->struct_bytes == sizeof(*backend) && backend->reserved0 == 0u &&
        bx_ntvdm_dem_file_session_v1_valid(backend->session) &&
        bx_ntvdm_host_namespace_v1_valid(backend->host_namespace) &&
        bx_ntvdm_dem_cwd_context_v1_valid(backend->cwd);
}

int bx_ntvdm_dem_local_file_backend_v1_initialize(
    bx_ntvdm_dem_local_file_backend_v1 *backend,
    bx_ntvdm_dem_file_session_v1 *session,
    const bx_ntvdm_host_namespace_v1 *host_namespace,
    const bx_ntvdm_dem_cwd_context_v1 *cwd)
{
    if (backend == 0 || !bx_ntvdm_dem_file_session_v1_valid(session) ||
        !bx_ntvdm_host_namespace_v1_valid(host_namespace) ||
        !bx_ntvdm_dem_cwd_context_v1_valid(cwd)) return 0;
    memset(backend, 0, sizeof(*backend));
    backend->magic = BX_NTVDM_DEM_LOCAL_FILE_BACKEND_V1_MAGIC;
    backend->abi_version = BX_NTVDM_DEM_LOCAL_FILE_BACKEND_V1_VERSION;
    backend->struct_bytes = sizeof(*backend);
    backend->session = session;
    backend->host_namespace = host_namespace;
    backend->cwd = cwd;
    return bx_ntvdm_dem_local_file_backend_v1_valid(backend);
}

int bx_ntvdm_dem_local_file_backend_v1_open(
    bx_ntvdm_dem_local_file_backend_v1 *backend, const char *oem_path,
    uint32_t access, DWORD creation_disposition, uint32_t *token_out)
{
    wchar_t relative[BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE];
    uint8_t drive;
    uint32_t mode;
    uint32_t policy;
    ACCESS_MASK desired_access;
    HANDLE handle = INVALID_HANDLE_VALUE;
    int path_result;
    if (token_out != 0) *token_out = 0u;
    if (!bx_ntvdm_dem_local_file_backend_v1_valid(backend) || token_out == 0 ||
        (access & ~(BX_NTVDM_DEM_LOCAL_FILE_ACCESS_V1_READ |
            BX_NTVDM_DEM_LOCAL_FILE_ACCESS_V1_WRITE)) != 0u || access == 0u)
        return BX_NTVDM_DEM_LOCAL_FILE_BACKEND_V1_REJECTED;
    mode = backend->session->profile.profile.mode;
    if ((access & BX_NTVDM_DEM_LOCAL_FILE_ACCESS_V1_WRITE) != 0u ||
        creation_disposition != OPEN_EXISTING) {
        if (!bx_ntvdm_dem_profile_consumer_v1_resolve(&backend->session->profile,
                BX_NTVDM_MUTATION_CLASS_V1_NAMESPACE_CONTENT, &policy))
            return BX_NTVDM_DEM_LOCAL_FILE_BACKEND_V1_REJECTED;
        if (policy == BX_NTVDM_MUTATION_POLICY_V1_REJECT_READONLY)
            return BX_NTVDM_DEM_LOCAL_FILE_BACKEND_V1_READONLY;
        if (policy == BX_NTVDM_MUTATION_POLICY_V1_USE_OVERLAY)
            return BX_NTVDM_DEM_LOCAL_FILE_BACKEND_V1_OVERLAY;
        if (policy == BX_NTVDM_MUTATION_POLICY_V1_USE_VIRTUAL)
            return BX_NTVDM_DEM_LOCAL_FILE_BACKEND_V1_VIRTUAL;
        if (policy != BX_NTVDM_MUTATION_POLICY_V1_DIRECT_HOST)
            return BX_NTVDM_DEM_LOCAL_FILE_BACKEND_V1_REJECTED;
    }
    if (mode == BX_NTVDM_MUTATION_MODE_V1_VIRTUAL)
        return BX_NTVDM_DEM_LOCAL_FILE_BACKEND_V1_VIRTUAL;
    if (mode == BX_NTVDM_MUTATION_MODE_V1_OVERLAY)
        return BX_NTVDM_DEM_LOCAL_FILE_BACKEND_V1_OVERLAY;
    path_result = bx_ntvdm_dem_path_v1_resolve(oem_path, backend->cwd, &drive, relative);
    if (path_result == BX_NTVDM_DEM_PATH_V1_CAPACITY)
        return BX_NTVDM_DEM_LOCAL_FILE_BACKEND_V1_CAPACITY;
    if (path_result != BX_NTVDM_DEM_PATH_V1_OK)
        return BX_NTVDM_DEM_LOCAL_FILE_BACKEND_V1_REJECTED;
    desired_access = (access & BX_NTVDM_DEM_LOCAL_FILE_ACCESS_V1_READ ? GENERIC_READ : 0u) |
        (access & BX_NTVDM_DEM_LOCAL_FILE_ACCESS_V1_WRITE ? GENERIC_WRITE : 0u);
    if (!bx_ntvdm_host_namespace_v1_open_file(backend->host_namespace, drive,
            relative, desired_access, FILE_SHARE_READ | FILE_SHARE_WRITE |
            FILE_SHARE_DELETE, creation_disposition, &handle))
        return BX_NTVDM_DEM_LOCAL_FILE_BACKEND_V1_NOT_FOUND;
    if (!bx_ntvdm_dem_file_session_v1_adopt(backend->session, handle, token_out)) {
        CloseHandle(handle);
        return BX_NTVDM_DEM_LOCAL_FILE_BACKEND_V1_CAPACITY;
    }
    return BX_NTVDM_DEM_LOCAL_FILE_BACKEND_V1_OK;
}
