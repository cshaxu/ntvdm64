#ifndef BX_NTVDM_DEM_OVERLAY_HANDLE_BACKEND_V1_H
#define BX_NTVDM_DEM_OVERLAY_HANDLE_BACKEND_V1_H

#include "bx_ntvdm_dem_file_session_v1.h"
#include "bx_ntvdm_dem_overlay_file_v1.h"

/* Common non-BOP operations for every Overlay handle service. */
int bx_ntvdm_dem_overlay_handle_backend_v1_read(bx_ntvdm_dem_file_session_v1 *session,
    bx_ntvdm_dem_overlay_file_v1 *files, uint32_t token, uint8_t *bytes,
    uint32_t capacity, uint32_t *count_out);
int bx_ntvdm_dem_overlay_handle_backend_v1_write(bx_ntvdm_dem_file_session_v1 *session,
    bx_ntvdm_dem_overlay_file_v1 *files, uint32_t token, const uint8_t *bytes,
    uint32_t count, uint32_t *written_out);
int bx_ntvdm_dem_overlay_handle_backend_v1_seek(bx_ntvdm_dem_file_session_v1 *session,
    bx_ntvdm_dem_overlay_file_v1 *files, uint32_t token, int32_t distance,
    uint32_t origin, uint32_t *position_out);
int bx_ntvdm_dem_overlay_handle_backend_v1_truncate(
    bx_ntvdm_dem_file_session_v1 *session, bx_ntvdm_dem_overlay_file_v1 *files,
    uint32_t token);
int bx_ntvdm_dem_overlay_handle_backend_v1_close(bx_ntvdm_dem_file_session_v1 *session,
    bx_ntvdm_dem_overlay_file_v1 *files, uint32_t token);
int bx_ntvdm_dem_overlay_handle_backend_v1_flush(
    bx_ntvdm_dem_file_session_v1 *session, uint32_t token);
int bx_ntvdm_dem_overlay_handle_backend_v1_file_times(
    bx_ntvdm_dem_file_session_v1 *session, bx_ntvdm_dem_overlay_file_v1 *files,
    uint32_t token, uint8_t option, uint16_t *time_in_out, uint16_t *date_in_out);

#endif
