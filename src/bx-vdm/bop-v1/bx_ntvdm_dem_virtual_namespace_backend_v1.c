#include "bx_ntvdm_dem_virtual_namespace_backend_v1.h"

int bx_ntvdm_dem_virtual_namespace_backend_v1_open(
    bx_ntvdm_dem_file_session_v1 *session, bx_ntvdm_dem_overlay_file_v1 *files,
    uint8_t drive, const wchar_t *relative, uint32_t access, uint32_t share_access,
    DWORD disposition, uint32_t attributes, uint16_t pdb_owner, uint32_t *token_out,
    uint32_t *size_out, DWORD *error_out)
{
    bx_ntvdm_dem_overlay_namespace_node_v1 node;
    uint32_t backend = 0u;
    uint16_t dos_time = 0u, dos_date = 0u;
    int exists;
    if (token_out != 0) *token_out = 0u;
    if (size_out != 0) *size_out = 0u;
    if (error_out != 0) *error_out = ERROR_INVALID_PARAMETER;
    if (session == 0 || files == 0 || files->store == 0 || relative == 0 ||
        token_out == 0 || size_out == 0 || error_out == 0 || drive >= 26u ||
        !bx_ntvdm_dem_file_session_v1_valid(session) ||
        !bx_ntvdm_dem_overlay_file_v1_valid(files) || relative[0] == L'\0' ||
        !bx_ntvdm_dem_virtual_namespace_view_v1_query(files->store, drive, relative,
            &node, error_out)) return 0;
    exists = node.kind == BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_FILE;
    if (node.kind == BX_NTVDM_DEM_OVERLAY_NAMESPACE_NODE_V1_DIRECTORY) {
        *error_out = ERROR_ACCESS_DENIED; return 1;
    }
    if ((disposition == OPEN_EXISTING || disposition == TRUNCATE_EXISTING) && !exists) {
        *error_out = ERROR_FILE_NOT_FOUND; return 1;
    }
    if (disposition == CREATE_NEW && exists) { *error_out = ERROR_FILE_EXISTS; return 1; }
    if (disposition != OPEN_EXISTING && disposition != CREATE_NEW &&
        disposition != CREATE_ALWAYS && disposition != OPEN_ALWAYS &&
        disposition != TRUNCATE_EXISTING) { *error_out = ERROR_INVALID_PARAMETER; return 0; }
    if (!bx_ntvdm_dem_overlay_file_v1_open_shared(files, drive, relative, access,
            share_access, 0, 0u, attributes == 0u ? FILE_ATTRIBUTE_NORMAL : attributes,
            exists, !exists, &backend)) { *error_out = ERROR_NOT_ENOUGH_MEMORY; return 0; }
    if ((disposition == CREATE_ALWAYS || disposition == TRUNCATE_EXISTING) &&
        !bx_ntvdm_dem_overlay_file_v1_truncate(files, backend)) {
        (void)bx_ntvdm_dem_overlay_file_v1_close(files, backend);
        *error_out = ERROR_ACCESS_DENIED; return 1;
    }
    if (!bx_ntvdm_dem_overlay_file_v1_info(files, backend, &attributes, size_out,
            &dos_time, &dos_date) ||
        !bx_ntvdm_dem_file_session_v1_adopt_backend(session,
            BX_NTVDM_DEM_FILE_TOKEN_KIND_V1_VIRTUAL_FILE, backend, pdb_owner, token_out)) {
        (void)bx_ntvdm_dem_overlay_file_v1_close(files, backend);
        *error_out = ERROR_NOT_ENOUGH_MEMORY; return 0;
    }
    *error_out = ERROR_SUCCESS;
    return 1;
}
