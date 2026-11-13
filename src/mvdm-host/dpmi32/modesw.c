/*++

Copyright (c) 1992  Microsoft Corporation

Module Name:

    modesw.c

Abstract:

    This module provides support for performing mode switching on the 32 bit
    side.

Author:

    Dave Hastings (daveh) 24-Nov-1992

Revision History:

    Neil Sandlin (neilsa) 31-Jul-1995 - Updates for the 486 emulator

--*/
#include "precomp.h"
#pragma hdrstop
#include "softpc.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_wow_page_domain.h"

/* CCPU's internal segment-register indices and pseudo-descriptor routine
 * are deliberately kept private to its generated headers.  CPU40's DPMI
 * provider needs only this source-defined real-mode cache refresh. */
extern void load_pseudo_descr(int index);
#define CPU40_ES_REG 0
#define CPU40_SS_REG 2
#define CPU40_DS_REG 3
#define CPU40_FS_REG 4
#define CPU40_GS_REG 5

#line 27
VOID
DpmiSwitchToRealMode(
    VOID
    )
/*++

Routine Description:

    This routine performs a mode switch to real (v86) mode.  CS
    register is loaded with the dosx real mode code segment

Arguments:

    None.

Return Value:

    None.

--*/
{

#line 49
#if defined(i386)
#error DIVERGENCE MVDM-HOST-DIV-619: unsupported i386 kernel VDM host path
#line 52
#else
#line 53
    PWORD16 Data;

    Data = (PWORD16)Sim32GetVDMPointer(
        ((ULONG)DosxRmCodeSegment << 16) | 4,
        1,
        FALSE
        );

    *(Data) = DosxStackSegment;
#endif

    /* CCPU loads segment caches immediately; leave PE before loading CS. */
    mvdm_softpc_wow_page_domain_leave_protected();
#line 66
    setMSW(getMSW() & ~MSW_PE);
#ifndef i386
    /* Match the real-mode privilege before executing the DOSX continuation. */
    setCPL(0);
#endif
#line 64
    setCS(DosxRmCodeSegment);

#line 68
#ifndef i386
    /* Real-mode segment loads retain data/stack attributes in CCPU. */
    load_pseudo_descr(CPU40_SS_REG);
    load_pseudo_descr(CPU40_DS_REG);
    load_pseudo_descr(CPU40_ES_REG);
    load_pseudo_descr(CPU40_FS_REG);
    load_pseudo_descr(CPU40_GS_REG);
#endif
#line 74
}

#line 76
VOID
DpmiSwitchToProtectedMode(
    VOID
    )
/*++

Routine Description:

    This routine switches to protected mode.  It assumes that the caller
    will take care of setting up the segment registers.

Arguments:

    None.

Return Value:

    None.

--*/
{
#line 97
#if defined(i386)
#error DIVERGENCE MVDM-HOST-DIV-619: unsupported i386 kernel VDM host path
#line 100
#else
#line 101
    PWORD16 Data;

    Data = (PWORD16)Sim32GetVDMPointer(
        ((ULONG)DosxRmCodeSegment << 16) | 4,
        1,
        FALSE
        );

    *(Data) = 0xb7;
#endif

    setMSW(getMSW() | MSW_PE);

#line 114
#ifndef i386
    //BUGBUG This is a workaround to make sure the emulator goes back
    // to privilege level 3 now that we are in protect mode.
    // Not doing this would cause an access violation in dpmi32.
    setCPL(3);
    /* Nested protected DOSX helpers run before the saved frame is restored. */
    setCS(DosxRmCodeSelector);
    setSS(DosxPmDataSelector);
#endif
    (void)mvdm_softpc_wow_page_domain_reenter_protected();
#line 120
}
