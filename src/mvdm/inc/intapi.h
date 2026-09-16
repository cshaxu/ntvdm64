/*++

Copyright (c) 1992  Microsoft Corporation

Module Name:

    intapi.h

Abstract:

    This header defines the function prototypes for the interrupt
    handler support routines in the 486 emulator.

Author:

    Neil Sandlin (neilsa)

Notes:

    
Revision History:


--*/

typedef int (*VDM_HARDWARE_INT_HANDLER)(ULONG IntNumber);
typedef int (*VDM_SOFTWARE_INT_HANDLER)(ULONG IntNumber);
typedef int (*VDM_FAULT_HANDLER)(ULONG IntNumber, ULONG ErrorCode);

NTSTATUS
VdmInstallHardwareIntHandler(
    VDM_HARDWARE_INT_HANDLER HwIntHandler
    );

NTSTATUS
VdmInstallSoftwareIntHandler(
    VDM_SOFTWARE_INT_HANDLER SwIntHandler
    );

NTSTATUS
VdmInstallFaultHandler(
    VDM_FAULT_HANDLER FaultHandler
    );

