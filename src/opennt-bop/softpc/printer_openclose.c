/*
 * Source-shaped mirror from:
 *   base/mvdm/softpc.new/base/comms/printer_.c:printer_io
 *   base/mvdm/softpc.new/base/comms/printer_.c:printer_bop_openclose
 *   base/mvdm/softpc.new/host/src/nt_lpt.c:host_lpt_dos_open/close
 *
 * DIVERGENCE (T245 S7): the original translation unit proceeds after this
 * early BOP switch into BDA/port I/O and may use monitor printer buffers and
 * host handles.  This package admits only the independent SI=1/2 lifecycle;
 * every other branch is marked unavailable instead of fabricating printer
 * behavior.
 */
#include "../../adapter-softpc/softpc_printer_openclose_shim.h"

#define NUM_PARALLEL_PORTS 3

static void printer_bop_openclose(int func)
{
    int adapter;

    adapter = runtime_softpc_printer_get_dx() % NUM_PARALLEL_PORTS;

    /* Original `printer_bop_openclose`: func must be 1 (open) or 2 (close). */
    if (func == 1)
        runtime_softpc_printer_host_lpt_dos_open(adapter);
    else
        runtime_softpc_printer_host_lpt_dos_close(adapter);
}

void printer_io(void)
{
    int bopsubfunction = runtime_softpc_printer_get_si();

    switch (bopsubfunction) {
        case 1:
        case 2:
            /* Original comment: track a DOS open/close on LPTn. */
            printer_bop_openclose(bopsubfunction);
            return;
        default:
            runtime_softpc_printer_unsupported();
            return;
    }
}
