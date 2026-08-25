#include "dem_ingress_shim.h"

int runtime_dem_ingress_dispatch(
    const runtime_instruction_window *window,
    runtime_demhndl_call *call)
{
    if (!runtime_instruction_window_valid(window) ||
        !runtime_demhndl_call_valid(call) || window->valid_bytes < 4u ||
        window->bytes[0] != 0xc4u || window->bytes[1] != 0xc4u ||
        window->bytes[2] != 0x50u || call->service != window->bytes[3] ||
        call->service >= SVC_DEMLASTSVC)
        return 0;
    /* The retained original dispatcher deliberately has no 50:42 worker.
     * Select the separately source-mapped demRead-body seam only for this
     * exact BOP; all other services preserve original DemDispatch ordering. */
    if (call->service == 0x42u)
        return runtime_demhndl_invoke_fast_read(call);
    return runtime_demdisp_invoke(call);
}
