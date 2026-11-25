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
#include "compat/include/dpmi_host.h"
#include "compat/include/mvdm_cpu_access.h"

#line 243
BOOL
DpmiSetDebugRegisters(
    PULONG RegisterPointer
    )
/*++

Routine Description:

    This routine is called by dpmi when an app has issued DPMI debug commands.
    The six doubleword pointed to by the input parameter are the desired values
    for the real x86 hardware debug registers. This routine lets
    ThreadSetDebugContext() do all the work.

Arguments:

    None

Return Value:

    None.

--*/
{
    BOOL bReturn = TRUE;

    if (!ThreadSetDebugContext(RegisterPointer))
        {
        ULONG ClearDebugRegisters[6] = {0, 0, 0, 0, 0, 0};

        //
        // an error occurred. Reset everything to zero
        //

        ThreadSetDebugContext (&ClearDebugRegisters[0]);
        bReturn = FALSE;
        }
    return bReturn;
}

BOOL
DpmiGetDebugRegisters(
    PULONG RegisterPointer
    )
/*++

Routine Description:

    This routine is called by DOSX when an app has issued DPMI debug commands.
    The six doubleword pointed to by the input parameter are the desired values
    for the real x86 hardware debug registers. This routine lets
    ThreadGetDebugContext() do all the work.

Arguments:

    None

Return Value:

    None.

--*/
{
    BOOL pending;
    if (!RegisterPointer) return FALSE;
    mvdm_cpu_access_current()->capture_debug(RegisterPointer, &pending);
    return TRUE;
}
