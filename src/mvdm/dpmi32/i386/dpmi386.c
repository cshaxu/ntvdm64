/*++

Copyright (c) 1991  Microsoft Corporation

Module Name:

    dpmi386.c

Abstract:

    This file contains support for 386/486 only dpmi bops

Author:

    Dave Hastings (daveh) 27-Jun-1991

Revision History:

    Matt Felton (mattfe) Dec 6 1992 removed unwanted verification
    Dave Hastings (daveh) 24-Nov-1992  Moved to mvdm\dpmi32
    Matt Felton (mattfe) 8 Feb 1992 optimize getvdmpointer for regular protect mode path.

--*/

#include "precomp.h"
#pragma hdrstop
#include <softpc.h>
#include <memory.h>
#include <malloc.h>
#if defined(CPU_40_STYLE)
extern BOOL ThreadSetDebugContext(PULONG Registers);
extern BOOL mvdm_debugger_read_debug_registers(USHORT Selector,
    USHORT Offset, PULONG Registers);
#endif

/* DIVERGENCE(MVDM-HOST-DIV-281): CPU40 composes the original debug-register
 * service below, not the process-LDT/kernel-monitor/fast-BOP providers. */


VOID
DpmiSetDebugRegisters(
    VOID
    )
/*++

Routine Description:

    This routine is called by DOSX when an app has issued DPMI debug commands.
    The six doubleword pointed to by the VDM's DS:SI are the desired values
    for the real x86 hardware debug registers. This routine lets
    ThreadSetDebugContext() do all the work.

Arguments:

    None

Return Value:

    None.

--*/
{
    PCHAR RegisterPointer;
#if defined(CPU_40_STYLE)
    ULONG DebugRegisters[6];
#endif

    setCF(0);

#if defined(CPU_40_STYLE)
    /* MVDM-HOST-DIV-281: SIM32 ignores its size argument; use a copied lease. */
    RegisterPointer = mvdm_debugger_read_debug_registers(getDS(), getSI(),
        DebugRegisters) ? (PCHAR)DebugRegisters : NULL;
#else
    RegisterPointer = Sim32GetVDMPointer(((getDS() << 16) | getSI()),
        0,
        (UCHAR) (getMSW() & MSW_PE)
        );

#endif

    if (RegisterPointer == NULL || !ThreadSetDebugContext((PULONG) RegisterPointer))
        {
        ULONG ClearDebugRegisters[6] = {0, 0, 0, 0, 0, 0};

        //
        // an error occurred. Reset everything to zero
        //

        ThreadSetDebugContext (&ClearDebugRegisters[0]);
        setCF(1);
        }

}
