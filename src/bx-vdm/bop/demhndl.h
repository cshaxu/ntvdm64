#ifndef BX_NTVDM_BOP_DEMHNDL_H
#define BX_NTVDM_BOP_DEMHNDL_H

#include "../bx_ntvdm_dem_whole_provider_v1.h"

/* OpenNT-derived provider for demhndl.c / demChgFilePtr (BOP 50:00).
 * This is intentionally a service provider, not a BOP decoder. */
int bx_ntvdm_bop_dem_chg_file_ptr_v2(
    bx_ntvdm_dem_whole_provider_v1 *provider,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu,
    bx_ntvdm_cpu_result_v2 *result);

/* OpenNT-derived provider for demhndl.c / demClose (BOP 50:02).
 * This is intentionally a service provider, not a BOP decoder. */
int bx_ntvdm_bop_dem_close_v2(
    bx_ntvdm_dem_whole_provider_v1 *provider,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu,
    bx_ntvdm_cpu_result_v2 *result);

#endif
