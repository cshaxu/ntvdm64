#ifndef BX_NTVDM_BOP_DEMFILE_H
#define BX_NTVDM_BOP_DEMFILE_H

#include "../../../bx_ntvdm_cpu_result_v2.h"
#include "../../../bx_ntvdm_cpu_state_abi.h"
#include "../../../bx_ntvdm_exception_abi.h"
#include "../../shim/bx_ntvdm_dem_direct_context.h"

/* OpenNT-derived provider for demfile.c / demChMod (BOP 50:01).
 * This is a Direct source-parity service provider, not a BOP decoder. */
int bx_ntvdm_bop_dem_ch_mod_v2(
    bx_ntvdm_dem_direct_context *context,
    const bx_ntvdm_exception_event_v1 *boundary,
    const bx_ntvdm_cpu_state_v1 *cpu,
    uint8_t drive,
    const wchar_t *relative_path,
    bx_ntvdm_cpu_result_v2 *result);

#endif