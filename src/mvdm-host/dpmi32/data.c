/*++

Copyright (c) 1992  Microsoft Corporation

Module Name:

    data.c

Abstract:

    This module contains the x86 specific data for the dpmi library.
    Common data is found in dpmi32.c

Author:

    Dave Hastings (daveh) creation-date 09-Feb-1994

Revision History:

    Neil Sandlin (neilsa) 31-Jul-1995 Updates for the 486 emulator

--*/
#include "precomp.h"
#pragma hdrstop

//
// Information about the current PSP
//
USHORT CurrentPSPSelector;

//
// Table of selector bases and limits
//
ULONG FlatAddress[LDT_SIZE];

//
// LDT information
//

PLDT_ENTRY Ldt;
USHORT LdtSel;
USHORT LdtMaxSel = 0;
USHORT LdtUserSel = 0;

//
// Address of 16 bit Idt
//
PIDT_ENTRY Idt;


//
// Pointers to the low memory buffers
//
PUCHAR SmallXlatBuffer;
PUCHAR LargeXlatBuffer;
BOOL SmallBufferInUse;
USHORT LargeBufferInUseCount = 0;
//
// Segment of realmode dosx stack
//
USHORT DosxStackSegment;

//
// segment of realmode dosx code
//
USHORT DosxRmCodeSegment;

//
// selector of realmode code
//
USHORT DosxRmCodeSelector;

//
// Address of pointer to next frame on Dosx stack
//
PWORD16 DosxStackFramePointer;

//
// Size of dosx stack frame
//
USHORT DosxStackFrameSize;

//
// Dpmi flags for the current application
//
USHORT CurrentAppFlags;

//
// Address of Bop fe for ending interrupt simulation
//
ULONG RmBopFe;
ULONG PmBopFe;

//
// Address of buffer for DTA in Dosx
//
PUCHAR DosxDtaBuffer;

//
// Information about the current DTA
//
// N.B.  The selector:offset, and CurrentDta following MAY point to
//       different linear addresses.  This will be the case if the
//       dta selector is in high memory.
//       CurrentDosDta holds the "cached" value of the Dta that has
//       actually been issued to DOS.
PUCHAR CurrentDta;
PUCHAR CurrentPmDtaAddress;
PUCHAR CurrentDosDta;
USHORT CurrentDtaSelector;
USHORT CurrentDtaOffset;

//
// These are the functions in WOW (GlobalDOSAlloc, GlobalDOSFree)
// that are used as helper functions to perform DPMI func's 100,101
// when we are running under WOW.
//

USHORT WOWAllocSeg = 0;
USHORT WOWAllocFunc;
USHORT WOWFreeSeg = 0;
USHORT WOWFreeFunc;

//
// Selector limits
//
#if DBG
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#else
#line 131
PULONG ExpSelectorLimit = NULL;
#endif

//
// Start of intel address space in process memory
//
ULONG IntelBase = 0;

//
// Variables used for supporting stack switching
// (on x86, these are in the vdmtib)
//
#ifndef i386
USHORT LockedPMStackSel;
ULONG LockedPMStackCount;
ULONG PMLockOrigEIP;
ULONG PMLockOrigSS;
ULONG PMLockOrigESP;

ULONG DosxFaultHandlerIret;
ULONG DosxFaultHandlerIretd;
ULONG DosxIntHandlerIret;
ULONG DosxIntHandlerIretd;
ULONG DosxRMReflector;

/* Guest-linear descriptor images consumed separately by CCPU.  The DOSX
 * 53:00 table is a GDT source; original process-LDT publication is a
 * distinct owner. */
ULONG Cpu40GdtShadowAddress;
ULONG Cpu40LdtShadowAddress;

BOOL
DpmiCpu40GetDosxIdtDescriptor(
    PULONG Base,
    PUSHORT Limit
    )
/* WOW DOSX publishes SEL_IDT after MoveDscrTables relocates the tables.
 * Use that final descriptor in place of the 53:00 construction table. */
{
    const USHORT DosxIdtSelector = 0x88u; /* pmdefs.inc, IFDEF WOW */
    PLDT_ENTRY Descriptor;

    if (Base == NULL || Limit == NULL || Cpu40GdtShadowAddress == 0u)
        return FALSE;

    Descriptor = (PLDT_ENTRY)(IntelBase + Cpu40GdtShadowAddress) +
        (DosxIdtSelector >> 3);
    if (!Descriptor->HighWord.Bits.Pres)
        return FALSE;

    *Base = (ULONG)Descriptor->BaseLow |
        ((ULONG)Descriptor->HighWord.Bytes.BaseMid << 16) |
        ((ULONG)Descriptor->HighWord.Bytes.BaseHi << 24);
    if (!Descriptor->HighWord.Bits.Granularity &&
        !Descriptor->HighWord.Bits.LimitHi &&
        Descriptor->LimitLow < (256u * sizeof(LDT_ENTRY) - 1u))
        return FALSE;

    *Limit = (USHORT)(256u * sizeof(LDT_ENTRY) - 1u);
    return TRUE;
}
#endif
#line 157

VDM_INTERRUPTHANDLER DpmiInterruptHandlers[256] = {0};
VDM_FAULTHANDLER DpmiFaultHandlers[32] = {0};

ULONG DosxIret;
ULONG DosxIretd;

USHORT PMReflectorSeg;

ULONG DosxRmSaveRestoreState;
ULONG DosxPmSaveRestoreState;
ULONG DosxRmRawModeSwitch;
ULONG DosxPmRawModeSwitch;

ULONG DosxVcdPmSvcCall;
ULONG DosxMsDosApi;
ULONG DosxXmsControl;
ULONG DosxHungAppExit;

//
// WORKITEM: should be in VDMTIB
//
ULONG LastLockedPMStackSS;
ULONG LastLockedPMStackESP;
