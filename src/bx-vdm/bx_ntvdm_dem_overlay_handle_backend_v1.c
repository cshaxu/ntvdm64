#include "bx_ntvdm_dem_overlay_handle_backend_v1.h"

static int backend(bx_ntvdm_dem_file_session_v1 *session, uint32_t token,
    uint32_t *backend_token_out)
{
    return bx_ntvdm_dem_file_session_v1_lookup_backend(session, token,
        BX_NTVDM_DEM_FILE_TOKEN_KIND_V1_OVERLAY_FILE, backend_token_out);
}

int bx_ntvdm_dem_overlay_handle_backend_v1_read(bx_ntvdm_dem_file_session_v1 *session,
    bx_ntvdm_dem_overlay_file_v1 *files, uint32_t token, uint8_t *bytes,
    uint32_t capacity, uint32_t *count_out)
{ uint32_t value; return backend(session, token, &value) &&
    bx_ntvdm_dem_overlay_file_v1_read(files, value, bytes, capacity, count_out); }
int bx_ntvdm_dem_overlay_handle_backend_v1_write(bx_ntvdm_dem_file_session_v1 *session,
    bx_ntvdm_dem_overlay_file_v1 *files, uint32_t token, const uint8_t *bytes,
    uint32_t count, uint32_t *written_out)
{ uint32_t value; return backend(session, token, &value) &&
    bx_ntvdm_dem_overlay_file_v1_write(files, value, bytes, count, written_out); }
int bx_ntvdm_dem_overlay_handle_backend_v1_seek(bx_ntvdm_dem_file_session_v1 *session,
    bx_ntvdm_dem_overlay_file_v1 *files, uint32_t token, int32_t distance,
    uint32_t origin, uint32_t *position_out)
{ uint32_t value; return backend(session, token, &value) &&
    bx_ntvdm_dem_overlay_file_v1_seek(files, value, distance, origin, position_out); }
int bx_ntvdm_dem_overlay_handle_backend_v1_truncate(
    bx_ntvdm_dem_file_session_v1 *session, bx_ntvdm_dem_overlay_file_v1 *files,
    uint32_t token)
{ uint32_t value; return backend(session, token, &value) &&
    bx_ntvdm_dem_overlay_file_v1_truncate(files, value); }
int bx_ntvdm_dem_overlay_handle_backend_v1_close(bx_ntvdm_dem_file_session_v1 *session,
    bx_ntvdm_dem_overlay_file_v1 *files, uint32_t token)
{ uint32_t value; return backend(session, token, &value) &&
    bx_ntvdm_dem_overlay_file_v1_close(files, value) &&
    bx_ntvdm_dem_file_session_v1_release_backend(session, token,
        BX_NTVDM_DEM_FILE_TOKEN_KIND_V1_OVERLAY_FILE); }
int bx_ntvdm_dem_overlay_handle_backend_v1_flush(
    bx_ntvdm_dem_file_session_v1 *session, uint32_t token)
{ uint32_t value; return backend(session, token, &value); /* volatile COW: no host flush */ }
