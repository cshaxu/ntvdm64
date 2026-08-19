#include "dem_ingress_shim.h"

int bx_ntvdm_dem_ingress_dispatch(
    const bx_ntvdm_instruction_window_v1 *window,
    bx_ntvdm_demhndl_call *call)
{
    if (!bx_ntvdm_instruction_window_v1_valid(window) ||
        !bx_ntvdm_demhndl_call_valid(call) || window->valid_bytes < 4u ||
        window->bytes[0] != 0xc4u || window->bytes[1] != 0xc4u ||
        window->bytes[2] != 0x50u || call->service != window->bytes[3] ||
        call->service >= SVC_DEMLASTSVC)
        return 0;
    return bx_ntvdm_demdisp_invoke(call);
}
