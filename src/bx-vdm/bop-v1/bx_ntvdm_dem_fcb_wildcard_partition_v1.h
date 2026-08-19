#ifndef BX_NTVDM_DEM_FCB_WILDCARD_PARTITION_V1_H
#define BX_NTVDM_DEM_FCB_WILDCARD_PARTITION_V1_H

#include "bx_ntvdm_dem_whole_provider_v1.h"

/* Source-derived demfcb.c wildcard namespace subset. Inputs have already
 * crossed the checked guest-memory seam as copied OEM strings. */
int bx_ntvdm_dem_fcb_wildcard_partition_v1_dispatch(
    bx_ntvdm_dem_whole_provider_v1 *provider, uint8_t service,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu_before, const char *source_oem,
    const char *destination_oem, bx_ntvdm_cpu_result_v2 *result);

#endif
