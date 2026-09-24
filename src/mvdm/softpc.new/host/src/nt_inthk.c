/*[
 *
 *  Name:	    nt_inthk.c
 *
 *  Derived From:   (original)
 *
 *  Author:	    Dave Bartlett
 *
 *  Created On:     11 Jan 1995
 *
 *  Coding Stds:    2.4
 *
 *  Purpose:        This module implements the memory management functions
 *                  required for 486 NT.
 *
 *  Include File:   nt_inthk.h
 *
 *  Copyright Insignia Solutions Ltd., 1994. All rights reserved.
 *
]*/


#ifdef CPU_40_STYLE

/* Need all of the following to include nt.h and windows.h in the same file. */
#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>

#include "insignia.h"
#include "host_def.h"
#include CpuH

#include "gdpvar.h"
#include "nt_inthk.h"
#include "debug.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_termination.h"

/* Make local symbols visible if debugging. */
#ifndef PROD
#define LOCAL
#endif /* not PROD */

/* Macros and typedefs. */


/* Hardware interrupt handler */
LOCAL BOOL (*HWIntHandler)(ULONG) = NULL;

#if defined(CCPU) || !defined(PROD)
/* Software interrupt handler */
LOCAL BOOL (*SWIntHandler)(ULONG) = NULL;

/* Exception interrupt handler */
LOCAL BOOL (*EXIntHandler)(ULONG,ULONG) = NULL;

#endif /* CCPU */

/* Global Functions. */

/*(
============================= host_hwint_hook =================================
PURPOSE:
	This function is called from the ICA during the process of ACKing an
	hardware interrupt. This function will call a hardware interrupt handler
	if one is defined.

INPUT:
	Interrupt vector number generated from hardware interrupt
OUTPUT:
	Return value - TRUE    hardware interrupt processed
		       FALSE   process hardware interrupt in normal way
================================================================================
)*/


#if defined(CCPU) || !defined(PROD)
GLOBAL BOOL host_hwint_hook IFN1(IS32, int_no)
{
    BOOL returnStatus = FALSE;

    /* hardware interrupt handler defined ? */
    if(HWIntHandler)
    {
	returnStatus = (HWIntHandler)((ULONG) int_no);

#ifndef PROD
	if(!returnStatus)
	    always_trace0("Hardware interrupt handler failed");

#endif /* PROD */
    }

    return( returnStatus );
}
#endif

/*(
======================= VdmInstallHardwareIntHandler ===========================
PURPOSE:
	Register a hardware interrupt handler called before the CPU dispatches
	the interrupt.

INPUT:
	Hardware interrupt handler function

OUTPUT:
	Return value - NTSTATUS

================================================================================
)*/

GLOBAL NTSTATUS	VdmInstallHardwareIntHandler IFN1(PVOID, HardwareIntHandler)
{
#ifdef CCPU
    HWIntHandler = HardwareIntHandler;
#else
    GLOBAL_VDM_HwIntHandler = HardwareIntHandler;
#endif
    return(STATUS_SUCCESS);
}

/*(
============================= host_swint_hook =================================
PURPOSE:
	This function is called from the CCPU prior to provessing a software
	interrupt. This function will call a software interrupt handler
	if one is defined.

INPUT:
	Interrupt number
OUTPUT:
	Return value - TRUE    software interrupt processed
		       FALSE   process software interrupt in normal way

================================================================================
)*/


#if defined(CCPU) || !defined(PROD)
GLOBAL BOOL host_swint_hook IFN1(IS32, int_no)
{
    BOOL returnStatus = FALSE;

    /* DIVERGENCE(MVDM-HOST-DIV-307): observation only for the original
     * WOW_x86 INT 2Ah fast LDT publication protocol.  NT4's kernel trap
     * recognizes EAX==EBP==F0F0F0F1 and calls NtSetLdtEntries with EBX,
     * ECX and EDX.  CPU40 deliberately retains the normal software-
     * interrupt dispatch here; this witness records the existing protocol
     * before any standalone carrier is selected. */
    if (int_no == 0x2a && getEAX() == 0xf0f0f0f1UL &&
            getEBP() == 0xf0f0f0f1UL) {
        mvdm_softpc_report_wow_int2a_ldt((unsigned short)getEBX(),
            (unsigned long)getECX(), (unsigned long)getEDX());
    }
    if (int_no == 0x2a) {
        /* DIVERGENCE(MVDM-HOST-DIV-309): default-off scalar witness for the
         * original WOW_x86 kernel-LDT carrier.  It precedes no decision and
         * cannot alter the existing software-interrupt dispatch. */
        mvdm_softpc_report_wow_int2a((unsigned long)getEAX(),
            (unsigned long)getEBX(), (unsigned long)getECX(),
            (unsigned long)getEDX(), (unsigned long)getEBP());
    }

    /* software interrupt handler defined ? */
    if(SWIntHandler)
    {
	returnStatus = (SWIntHandler)((ULONG) int_no);

#ifndef PROD

	if(!returnStatus)
	    always_trace0("Software interrupt handler failed");

#endif /* PROD */
    }

    return( returnStatus );
}
#endif /* CCPU */

/*(
======================= VdmInstallSoftwareIntHandler ===========================
PURPOSE:
	Register a software interrupt handler called before the CPU dispatches
	the software interrupt.

INPUT:
	Software interrupt handler function

OUTPUT:
	Return value - NTSTATUS
================================================================================
)*/

GLOBAL NTSTATUS	VdmInstallSoftwareIntHandler IFN1(PVOID, SoftwareIntHandler)
{
#ifdef CCPU
    SWIntHandler = SoftwareIntHandler;
#else
    GLOBAL_VDM_SoftIntHandler = SoftwareIntHandler;
#endif
    return(STATUS_SUCCESS);
}

/*(
============================= host_exint_hook =================================
PURPOSE:
	This function is called from the CPU prior to processing a CPU
	exception interrupt. This function will call a exception interrupt
	handler if one is defined.

INPUT:
	Exception number
	Exception error code
OUTPUT:
	Return value - TRUE    hardware interrupt processed
		       FALSE   process hardware interrupt in normal way

================================================================================
)*/


#if defined(CCPU) || !defined(PROD)
GLOBAL BOOL host_exint_hook IFN2(IS32, exp_no, IS32, error_code)
{
    BOOL returnStatus = FALSE;
    BOOL handlerInstalled = EXIntHandler != NULL;

    /* exception interrupt handler defined ? */
    if(EXIntHandler)
    {
	returnStatus = (EXIntHandler)((ULONG) exp_no, (ULONG) error_code);

#ifndef PROD

	if(!returnStatus)
	    always_trace0("Exception interrupt handler failed (%x)");

#endif /* PROD */
    }

    /* DIVERGENCE(MVDM-HOST-DIV-308): default-off observation of the
     * source-defined CCPU exception-hook result.  This locates a missing
     * DOSX fault-vector publication without changing the hook contract. */
    mvdm_softpc_report_wow_exception_hook((ULONG)exp_no, (ULONG)error_code,
        (ULONG)handlerInstalled, (ULONG)returnStatus);

    return( returnStatus );
}
#endif /* CCPU */

/*(
======================= VdmInstallFaultHandler ===========================
PURPOSE:
	Register a CPU exception interrupt handler called before the CPU
	dispatches the exceptioninterrupt.

INPUT:
	Exception interrupt handler function

OUTPUT:
	Return value - NTSTATUS
================================================================================
)*/

GLOBAL NTSTATUS	VdmInstallFaultHandler IFN1(PVOID, FaultHandler)
{
#ifdef CCPU
    EXIntHandler = FaultHandler;
#else
    GLOBAL_VDM_FaultHandler = FaultHandler;
#endif
    return(STATUS_SUCCESS);
}

#endif /* CPU_40_STYLE */
