/*++

Copyright (c) 1992  Microsoft Corporation

Module Name:

    Stack.c

Abstract:

    This module implements routines for manipulating the 16 bit stack

Author:

    Dave Hastings (daveh) 24-Nov-1992

Revision History:

--*/
#include "precomp.h"
#pragma hdrstop
#include "softpc.h"
#include "mvdm_realmode_stack.h"

VOID
DpmiSwitchToDosxStack(
    BOOL ProtectedMode
    )
/*++

Routine Description:

    This routine switches to the dos extender stack, and allocates a
    new frame.

Arguments:

    None.

Return Value:

    None.

--*/
{

    if (ProtectedMode) {
        setSS(DosxPmDataSelector);
    } else {
        setSS(DosxStackSegment);
    }

    setSP(*DosxStackFramePointer);
    *DosxStackFramePointer -= DosxStackFrameSize;
}

VOID
DpmiSwitchFromDosxStack(
    VOID
    )
/*++

Routine Description:

    This routine deallocates a frame from the dosx stack

Arguments:

    None.

Return Value:

    None.

--*/
{
    *DosxStackFramePointer += DosxStackFrameSize;
}

VOID
DpmiPushRmInt(
    USHORT InterruptNumber
    )
/*++

Routine Description:

    This routine pushes an interrupt frame on the stack and sets up cs:ip
    for the specified interrupt.

Arguments:

    InterruptNumber -- Specifies the index of the interrupt

Return Value:

    None.

--*/
{
    // bugbug stack wrap???

    ASSERT((getSP() > 6));
    ASSERT((!(getMSW() & MSW_PE)));

    /* DIVERGENCE(MVDM-HOST-DIV-019): the original body retains the same
     * return BOP, FLAGS, IVT and CS:IP ordering, but its two
     * Sim32GetVDMPointer aliases were unbounded host pointers. The named
     * SoftPC facade uses fresh bounded session leases and one guarded
     * selector-blind real-mode frame commit instead. */
    (void)mvdm_realmode_push_interrupt(InterruptNumber, RmBopFe);
}

VOID
DpmiSimulateIretCF(
    VOID
    )
/*++

Routine Description:

    This routine simulates a far return

Arguments:

    None

Return Value:

    None.

Notes:

    This routine does not have to deal with 32 bit stacks, because by
    the time we get here we know we are running on a stack that only
    has 16 bits worth of sp information.  Either the int 21 was executed
    on a 16 bit stack, or we have switched stacks.

--*/
{
    /* DIVERGENCE(MVDM-HOST-DIV-019): preserve the original low-word FLAGS
     * merge and far return through a bounded source-shaped stack copy and
     * the guarded real-mode frame transaction described above. */
    (void)mvdm_realmode_simulate_iret_cf();
}
