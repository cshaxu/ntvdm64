/*++

Copyright (c) 1992  Microsoft Corporation

Module Name:

    dpmimscr.c

Abstract:

    This module contains misc dpmi functions for risc.

Author:

    Dave Hart (davehart) creation-date 11-Apr-1993

Revision History:


--*/

#include "precomp.h"
#pragma hdrstop
#include "softpc.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_termination.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_fast_bop.h"

VOID
DpmiGetFastBopEntry(
    VOID
    )
/*++

Routine Description:

    This routine fails the get fast bop entry bop.  It is used by
    krnl286 on risc.

Arguments:

    None.

Return Value:

    None.

--*/
{
        USHORT Offset;
        USHORT Selector;

        if (mvdm_softpc_fast_bop_prepare(&Offset, &Selector)) {
            setBX(Offset);
            setDX(0);
            setES(Selector);
            mvdm_softpc_report_dpmi_fast_bop_result(getBX(), getDX(), getES());
            return;
        }
        //
        // krnl286 does a DPMIBOP GetFastBopAddress even on
        // risc, so just fail the call since fast-bopping
        // will only ever work on x86.
        //

        setBX(0);
        setDX(0);
        setES(0);
        mvdm_softpc_report_dpmi_fast_bop_result(getBX(), getDX(), getES());
}

VOID
DpmiDpmiInUse(
    VOID
    )
/*++

Routine Description:

    This routine notifies the CPU that the NT dpmi server is in use,
    so that IRET hooks can be used in protected mode.  If the NT DPMI
    server is not used, no protected mode iret hooks can be used, because
    we don't have a protected mode address that points to the correct bop.
    Apps that run in protected mode without using DPMI will likely change
    the LDT and GDT in unpredictable ways.

Arguments:

    None.

Return Value:

    None.

--*/
{
    EnableEmulatorIretHooks();
    EnableIntHooks();
}

VOID
DpmiDpmiNoLongerInUse(
    VOID
    )
/*++

Routine Description:

    This routine notifies the CPU that the NT dpmi server is no longer in use.
    This will cause the CPU to stop using PM iret hooks.

Arguments:

    None.

Return Value:

    None.

--*/
{
    DisableEmulatorIretHooks();
    DisableIntHooks();
}
