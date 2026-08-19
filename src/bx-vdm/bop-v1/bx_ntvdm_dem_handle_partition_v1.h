#ifndef BX_NTVDM_DEM_HANDLE_PARTITION_V1_H
#define BX_NTVDM_DEM_HANDLE_PARTITION_V1_H

#include "bx_ntvdm_dem_whole_provider_v1.h"

/* Source-derived backend for demhndl.c's complete local-file family.  For
 * read/write, bytes are already copied by the whole-provider gather seam. */
int bx_ntvdm_dem_handle_partition_v1_dispatch(
    bx_ntvdm_dem_whole_provider_v1 *provider, uint8_t service,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before,
    uint8_t *io_bytes, uint32_t io_capacity, uint32_t *io_byte_count,
    bx_ntvdm_cpu_result_v2 *result);

#endif
