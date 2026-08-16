#include "bx_ntvdm_dem_fcb_overlay_backend_v1.h"

int bx_ntvdm_dem_fcb_overlay_backend_v1_open(
    bx_ntvdm_dem_file_session_v1 *session, bx_ntvdm_dem_overlay_file_v1 *files,
    const bx_ntvdm_host_namespace_v1 *host_namespace, uint8_t drive,
    const wchar_t *relative, uint32_t access, uint32_t share_access,
    DWORD disposition, uint32_t attributes, uint32_t *token_out,
    uint32_t *size_out, uint16_t *time_out, uint16_t *date_out, DWORD *error_out)
{
    uint32_t token = 0u, backend = 0u;
    if (token_out != 0) *token_out = 0u;
    if (size_out != 0) *size_out = 0u;
    if (time_out != 0) *time_out = 0u;
    if (date_out != 0) *date_out = 0u;
    if (session == 0 || files == 0 || host_namespace == 0 || relative == 0 ||
        token_out == 0 || size_out == 0 || time_out == 0 || date_out == 0 ||
        error_out == 0) return 0;
    if (!bx_ntvdm_dem_overlay_namespace_backend_v1_open(session, files,
            host_namespace, drive, relative, access, share_access, disposition,
            attributes, 0u, &token, size_out, error_out)) return 0;
    if (!bx_ntvdm_dem_file_session_v1_lookup_backend(session, token,
            BX_NTVDM_DEM_FILE_TOKEN_KIND_V1_OVERLAY_FILE, &backend) ||
        !bx_ntvdm_dem_overlay_file_v1_info(files, backend, &attributes, size_out,
            time_out, date_out)) {
        (void)bx_ntvdm_dem_overlay_handle_backend_v1_close(session, files, token);
        *error_out = ERROR_INVALID_DATA;
        return 0;
    }
    *token_out = token; *error_out = ERROR_SUCCESS;
    return 1;
}

int bx_ntvdm_dem_fcb_overlay_backend_v1_info(
    bx_ntvdm_dem_file_session_v1 *session, bx_ntvdm_dem_overlay_file_v1 *files,
    uint32_t token, uint32_t *attributes_out, uint32_t *size_out,
    uint16_t *time_out, uint16_t *date_out)
{
    uint32_t backend;
    return bx_ntvdm_dem_file_session_v1_lookup_backend(session, token,
        BX_NTVDM_DEM_FILE_TOKEN_KIND_V1_OVERLAY_FILE, &backend) &&
        bx_ntvdm_dem_overlay_file_v1_info(files, backend, attributes_out,
            size_out, time_out, date_out);
}
