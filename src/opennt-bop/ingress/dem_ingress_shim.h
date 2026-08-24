#ifndef BX_NTVDM_BOP_SHIM_DEM_INGRESS_SHIM_H
#define BX_NTVDM_BOP_SHIM_DEM_INGRESS_SHIM_H

/* OpenNT's nt_bop.c selected DEM before calling DemDispatch.  That product
 * shell is intentionally not linked into the CLI composition.  This seam
 * replaces only the copied four-byte selection boundary C4 C4 50 <service>.
 * It neither classifies another selector nor implements a DEM service. */

#include "opennt-host/dem/demdisp_shim.h"
#include "adapter-softpc/bx_ntvdm_instruction_window_abi.h"

/* A non-DEM window, incomplete BOP, mismatched service, or invalid call is
 * declined without modifying the supplied result. */
int bx_ntvdm_dem_ingress_dispatch(
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_demhndl_call *call);

#endif
