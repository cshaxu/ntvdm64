/*
 * Source-proven kernel-VDM semantic carrier.
 *
 * Original owner: base/mvdm/v86/monitor/i386/monitor.c,
 * MonitorInitializePrinterInfo (OpenNT SHA-256
 * 80E97A535AE7760089B2EE9FAD8F6AE8202936240F7BC47C17227611AA0268B6).
 *
 * This selected function operates only on the bound worker's original-layout
 * VDM_TIB.  It neither installs a monitor nor owns CPU execution.
 */

#include "include/monitor_context.h"
#define ASSERT(Expression) ((void)0)

BOOLEAN MonitorInitializePrinterInfo(
     WORD   Ports,
     PWORD  PortTable,
     PUCHAR State,
     PUCHAR Control,
     PUCHAR Status,
     PUCHAR HostState)
{
    int     i;
    mvdm_monitor_printer_view view;
    if (!mvdm_monitor_get_printer(&view)) return FALSE;

    ASSERT (Ports == 3);
    ASSERT (Status != NULL);

    // only do this if the structure has not been initialized -- meaning
    // the pointers can be set once.
    if (NULL == (*view.status)) {

	view.ports[0] = PortTable[0];
	view.ports[1] = PortTable[1];
	view.ports[2] = PortTable[2];

	view.handles[0] =
	view.handles[1] =
	view.handles[2] = NULL;


	// primarily for dongle
	view.counts[0] =
	view.counts[1] =
	view.counts[2] = 0;

	// primarily for simulating printer status read in kernel
	(*view.state) = State;
	(*view.control) = Control;
	(*view.status) = Status;
	(*view.host_state) = HostState;


	// deal with mode carefully. VDD may have hooked printer ports
	// before we get here. If the mode is undefined, we can
	// safely initialize it to our default, otherwise, just leave
	// it alone.
	for (i = 0; i < 3; i++) {
	    if (view.no_simulation == view.modes[i])
		view.modes[i] = view.simulate;
	}

	return TRUE;
    }
    else
	return FALSE;
}

BOOLEAN MonitorEnablePrinterDirectAccess(WORD adapter, HANDLE handle, BOOLEAN Enable)
{
    mvdm_monitor_printer_view view;
    if (!mvdm_monitor_get_printer(&view)) return FALSE;
    ASSERT(view.count > adapter);
    if (Enable) {
	// if the adapter has been allocated by a third party VDD,
	// can't do direct io.
	if (view.vdd != view.modes[adapter]) {
	    view.modes[adapter] = view.direct;
	    view.handles[adapter] = handle;
	    // NtVdmControl(VdmPrinterDirectIoOpen, &adapter);
	    return TRUE;
	}
	else
	    return FALSE;
    }
    else {
	// disabling direct i/o. reset it back to status port simulation
	if (view.handles[adapter] == handle) {
	    mvdm_monitor_close_printer(adapter);
	    view.modes[adapter] = view.simulate;
	    view.handles[adapter] = NULL;
	    view.counts[adapter] = 0;
	    return TRUE;
	}
	else
	    return FALSE;
    }
}

BOOLEAN MonitorPrinterWriteData(WORD Adapter, BYTE Value)
{
    USHORT BytesInBuffer;

    mvdm_monitor_printer_view view;
    if (!mvdm_monitor_get_printer(&view)) return FALSE;
    ASSERT(view.count > Adapter);
    BytesInBuffer = view.counts[Adapter];
    view.buffers[Adapter * view.stride + BytesInBuffer] = Value;
    view.counts[Adapter]++;

    return TRUE;
}
