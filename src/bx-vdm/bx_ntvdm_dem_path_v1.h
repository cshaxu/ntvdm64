#ifndef BX_NTVDM_DEM_PATH_V1_H
#define BX_NTVDM_DEM_PATH_V1_H

#include <stdint.h>
#include <wchar.h>

#include "bx_ntvdm_dem_cwd_context_v1.h"

#define BX_NTVDM_DEM_PATH_V1_MAX_OEM_BYTES 260u
#define BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE 260u

enum bx_ntvdm_dem_path_v1_result {
    BX_NTVDM_DEM_PATH_V1_OK = 1,
    BX_NTVDM_DEM_PATH_V1_REJECTED = 0,
    BX_NTVDM_DEM_PATH_V1_ENCODING = -1,
    BX_NTVDM_DEM_PATH_V1_CAPACITY = -2
};

/* Converts a bounded, NUL-terminated OEM DOS path into an admitted-drive
 * relative 8.3 path.  It does not access the host and does not accept host
 * absolute paths, dot segments, or long-name fallback. */
int bx_ntvdm_dem_path_v1_resolve(const char *oem_path,
    const bx_ntvdm_dem_cwd_context_v1 *cwd, uint8_t *drive_index_out,
    wchar_t relative_out[BX_NTVDM_DEM_PATH_V1_MAX_RELATIVE]);

#endif
