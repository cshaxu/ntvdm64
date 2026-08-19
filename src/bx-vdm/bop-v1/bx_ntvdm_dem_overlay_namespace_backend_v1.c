#include "bx_ntvdm_dem_overlay_namespace_backend_v1.h"

int bx_ntvdm_dem_overlay_namespace_backend_v1_open(
    bx_ntvdm_dem_file_session_v1 *session, bx_ntvdm_dem_overlay_file_v1 *files,
    const bx_ntvdm_host_namespace_v1 *host_namespace, uint8_t drive,
    const wchar_t *relative, uint32_t access, uint32_t share_access,
    DWORD disposition, uint32_t attributes, uint16_t pdb_owner, uint32_t *token_out,
    uint32_t *size_out, DWORD *error_out)
{
    uint32_t backend_token = 0u;
    if (token_out != 0) *token_out = 0u;
    if (error_out != 0) *error_out = ERROR_INVALID_PARAMETER;
    if (session == 0 || files == 0 || host_namespace == 0 || relative == 0 ||
        token_out == 0 || size_out == 0 || error_out == 0) return 0;
    if (!bx_ntvdm_dem_overlay_resolver_v1_open_shared(files, host_namespace,
            drive, relative, access, share_access, disposition, &backend_token,
            size_out, error_out)) return 0;
    if ((disposition == CREATE_ALWAYS || disposition == CREATE_NEW) &&
        !bx_ntvdm_dem_overlay_store_v1_put_file(files->store, drive, relative,
            attributes == 0u ? FILE_ATTRIBUTE_NORMAL : attributes, 0, 0u)) {
        (void)bx_ntvdm_dem_overlay_file_v1_close(files, backend_token);
        *error_out = ERROR_NOT_ENOUGH_MEMORY;
        return 0;
    }
    if (!bx_ntvdm_dem_file_session_v1_adopt_backend(session,
            BX_NTVDM_DEM_FILE_TOKEN_KIND_V1_OVERLAY_FILE, backend_token,
            pdb_owner, token_out)) {
        (void)bx_ntvdm_dem_overlay_file_v1_close(files, backend_token);
        *error_out = ERROR_NOT_ENOUGH_MEMORY;
        return 0;
    }
    return 1;
}
