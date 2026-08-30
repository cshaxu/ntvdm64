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

/* DIVERGENCE(MVDM-SUPPORT-DIV-005): NT4 passed typed interrupt callbacks
 * through PVOID, a function/data-pointer conversion accepted by its compiler
 * but not a valid x86/x64 C ABI contract.  These callbacks are internal
 * 486-emulator control-flow entrypoints, not host-object identities. */
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

