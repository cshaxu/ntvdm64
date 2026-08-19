#ifndef BX_NTVDM_BOP_DEMHNDL_H
#define BX_NTVDM_BOP_DEMHNDL_H

#include "../../../bx_ntvdm_cpu_result_v2.h"
#include "../../../bx_ntvdm_cpu_state_abi.h"
#include "../../../bx_ntvdm_exception_abi.h"
#include "../../shim/bx_ntvdm_dem_direct_context.h"

/* OpenNT-derived Direct providers for demhndl.c.  They are intentionally
 * service providers only; selector decoding and profile routing stay outside. */
int bx_ntvdm_bop_dem_close_v2(
    bx_ntvdm_dem_direct_context *context,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu,
    bx_ntvdm_cpu_result_v2 *result);

int bx_ntvdm_bop_dem_chg_file_ptr_v2(
    bx_ntvdm_dem_direct_context *context,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu,
    bx_ntvdm_cpu_result_v2 *result);

#endif