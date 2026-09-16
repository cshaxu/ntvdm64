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

#include "monitor_context.h"

BOOLEAN MonitorInitializePrinterInfo(
     WORD   Ports,
     PWORD  PortTable,
     PUCHAR State,
     PUCHAR Control,
     PUCHAR Status,
     PUCHAR HostState)
{
    int     i;

    ASSERT (Ports == 3);
    ASSERT (Status != NULL);

    // only do this if the structure has not been initialized -- meaning
    // the pointers can be set once.
    if (NULL == VdmTib.PrinterInfo.prt_Status) {

	VdmTib.PrinterInfo.prt_PortAddr[0] = PortTable[0];
	VdmTib.PrinterInfo.prt_PortAddr[1] = PortTable[1];
	VdmTib.PrinterInfo.prt_PortAddr[2] = PortTable[2];

	VdmTib.PrinterInfo.prt_Handle[0] =
	VdmTib.PrinterInfo.prt_Handle[1] =
	VdmTib.PrinterInfo.prt_Handle[2] = NULL;


	// primarily for dongle
	VdmTib.PrinterInfo.prt_BytesInBuffer[0] =
	VdmTib.PrinterInfo.prt_BytesInBuffer[1] =
	VdmTib.PrinterInfo.prt_BytesInBuffer[2] = 0;

	// primarily for simulating printer status read in kernel
	VdmTib.PrinterInfo.prt_State = State;
	VdmTib.PrinterInfo.prt_Control = Control;
	VdmTib.PrinterInfo.prt_Status = Status;
	VdmTib.PrinterInfo.prt_HostState = HostState;


	// deal with mode carefully. VDD may have hooked printer ports
	// before we get here. If the mode is undefined, we can
	// safely initialize it to our default, otherwise, just leave
	// it alone.
	for (i = 0; i < 3; i++) {
	    if (PRT_MODE_NO_SIMULATION == VdmTib.PrinterInfo.prt_Mode[i])
		VdmTib.PrinterInfo.prt_Mode[i] = PRT_MODE_SIMULATE_STATUS_PORT;
	}

	return TRUE;
    }
    else
	return FALSE;
}

BOOLEAN MonitorEnablePrinterDirectAccess(WORD adapter, HANDLE handle, BOOLEAN Enable)
{
    ASSERT(VDM_NUMBER_OF_LPT > adapter);
    if (Enable) {
	// if the adapter has been allocated by a third party VDD,
	// can't do direct io.
	if (PRT_MODE_VDD_CONNECTED != VdmTib.PrinterInfo.prt_Mode[adapter]) {
	    VdmTib.PrinterInfo.prt_Mode[adapter] = PRT_MODE_DIRECT_IO;
	    VdmTib.PrinterInfo.prt_Handle[adapter] = handle;
	    // NtVdmControl(VdmPrinterDirectIoOpen, &adapter);
	    return TRUE;
	}
	else
	    return FALSE;
    }
    else {
	// disabling direct i/o. reset it back to status port simulation
	if (VdmTib.PrinterInfo.prt_Handle[adapter] == handle) {
	    NtVdmControl(VdmPrinterDirectIoClose, &adapter);
	    VdmTib.PrinterInfo.prt_Mode[adapter] = PRT_MODE_SIMULATE_STATUS_PORT;
	    VdmTib.PrinterInfo.prt_Handle[adapter] = NULL;
	    VdmTib.PrinterInfo.prt_BytesInBuffer[adapter] = 0;
	    return TRUE;
	}
	else
	    return FALSE;
    }
}

BOOLEAN MonitorPrinterWriteData(WORD Adapter, BYTE Value)
{
    USHORT BytesInBuffer;

    ASSERT(VDM_NUMBER_OF_LPT > Adapter);
    BytesInBuffer = VdmTib.PrinterInfo.prt_BytesInBuffer[Adapter];
    VdmTib.PrinterInfo.prt_Buffer[Adapter][BytesInBuffer] = Value;
    VdmTib.PrinterInfo.prt_BytesInBuffer[Adapter]++;

    return TRUE;
}
