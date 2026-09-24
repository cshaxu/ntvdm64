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
#include "ntvdm-exe/softpc/include/mvdm_softpc_termination.h"
#include "ntvdm-exe/softpc/include/mvdm_softpc_wow_page_domain.h"
/* DIVERGENCE(MVDM-HOST-DIV-310): publish the original RM_BIT mode gate
 * through guest SAS after each CCPU PE transition, not host address 0714h. */
#include "ntvdm-exe/softpc/include/mvdm_softpc_fast_bop.h"

/* CCPU's internal segment-register indices and pseudo-descriptor routine
 * are deliberately kept private to its generated headers.  CPU40's DPMI
 * provider needs only this source-defined real-mode cache refresh. */
extern void load_pseudo_descr(int index);
extern void c_setGDT_BASE_LIMIT(ULONG base, USHORT limit);
extern void c_setIDT_BASE_LIMIT(ULONG base, USHORT limit);
extern ULONG c_getCR3(void);
extern ULONG c_getDS_BASE(void);
extern ULONG c_getDS_LIMIT(void);
#define CPU40_ES_REG 0
#define CPU40_CS_REG 1
#define CPU40_SS_REG 2
#define CPU40_DS_REG 3
#define CPU40_FS_REG 4
#define CPU40_GS_REG 5

VOID
DpmiCpu40SwitchToProtectedMode(
    VOID
    )
/*++

Routine Description:

    DIVERGENCE(MVDM-HOST-DIV-222): performs the `53:01` DPMI protected-mode entry for the CPU40 profile.
    The original i386 owner restores the DOSX-supplied register frame before
    setting PE.  Its remaining VDM-state-bit writes belong to the NT kernel
    VDM and have no CPU40 carrier; CPU40 instead retains its existing CPL-3
    transition rule.

--*/
{
    PCHAR StackPointer;
    USHORT CsSelector;
    USHORT SsSelector;
    USHORT DsSelector;
    ULONG Eip;
    ULONG Esp;
    ULONG IdtBase;
    USHORT IdtLimit;

    StackPointer = Sim32GetVDMPointer(((getSS() << 16) | getSP()),
        0, (UCHAR)(getMSW() & MSW_PE));

    /* Capture the exact original frame while its real-mode SS cache is
     * valid.  Unlike the kernel VDM's passive CONTEXT fields, CCPU loads a
     * segment cache when each setter runs. */
    CsSelector = *(PUSHORT)(StackPointer + 12);
    Eip = *(PULONG)(StackPointer + 8);
    SsSelector = *(PUSHORT)(StackPointer + 6);
    Esp = *(PULONG)(StackPointer + 2);
    DsSelector = *(PUSHORT)(StackPointer);

    /* WOW DOSX intentionally does not execute the ordinary LGDT path.  Its
     * preceding 53:00/53:02 publications have populated the CPU40 shadow
     * from the source-built descriptor table; install that image at the
     * actual 53:01 transition boundary, immediately before PE makes the
     * source-supplied selectors architecturally live. */
    if (Cpu40GdtShadowAddress != 0)
        c_setGDT_BASE_LIMIT(Cpu40GdtShadowAddress,
            (USHORT)(LDT_SIZE * sizeof(LDT_ENTRY) - 1));

    /* WOW_x86 omits LIDT because native VDM installs the final SEL_IDT
     * descriptor. `dxboot.asm` publishes that descriptor after moving the
     * temporary tables to XMS. Mirror that final descriptor at 53:01;
     * never retain the freed 53:00 construction buffer. */
    if (!DpmiCpu40GetDosxIdtDescriptor(&IdtBase, &IdtLimit))
        return;
    c_setIDT_BASE_LIMIT(IdtBase, IdtLimit);

    /* DIV-222: vdmmisc.asm::VdmSwapContexts clears NT with
     * EFLAGS_USER_SANITIZE on PM entry. CCPU retains its own IF/IOPL policy. */
    setEFLAGS(getEFLAGS() & ~EFLAGS_NT_MASK);
    setMSW(getMSW() | MSW_PE);
    mvdm_softpc_fast_bop_sync_mode();
    setCPL(3);
    setCS(CsSelector);
    setEIP(Eip);
    setSS(SsSelector);
    setESP(Esp);
    setDS(DsSelector);
    /* The source-defined protected entry invalidates these inherited
     * real-mode selectors before the first protected instruction. */
    setES(0);
    setGS(0);
    setFS(0);
    /* DIVERGENCE(MVDM-HOST-DIV-222): restore the selected worker WOW view
     * after DOSX reentry, as the native user GDT survives a V86 excursion. */
    (void)mvdm_softpc_wow_page_domain_reenter_protected();
    mvdm_softpc_report_dpmi_mode_state("53:01-protected", (unsigned long)getCR0(),
        (unsigned long)c_getCR3(), getCS(), getIP(),
        (unsigned long)c_getDS_BASE(), (unsigned long)c_getDS_LIMIT());
}

VOID
switch_to_real_mode(
    VOID
    )
/*++

Routine Description:

    DIVERGENCE(MVDM-HOST-DIV-229): CPU40 counterpart of the original
    i386 `switch_to_real_mode` BOP FD provider.  DOSX pushes the five-word
    real-mode continuation frame (DS, SP, SS, IP, CS).  Capture that exact
    frame in the original order, clear PE, then load the captured values.
    The original host stored passive CONTEXT selectors before V86 resumed;
    CCPU must load them after PE changes so its descriptor caches become the
    required real-mode caches.  The omitted fixed NTVDM-state-page writes are
    kernel-VDM bookkeeping with no CCPU40 carrier; they do not define the
    guest transition.

--*/
{
    PCHAR StackPointer;
    USHORT DsSelector;
    USHORT Sp;
    USHORT SsSelector;
    USHORT Ip;
    USHORT CsSelector;

    StackPointer = Sim32GetVDMPointer(((getSS() << 16) | getSP()),
        0, (UCHAR)(getMSW() & MSW_PE));

    /* Capture before changing mode: the original passive CONTEXT leaves the
     * five real-mode values in this exact order. */
    DsSelector = *(PUSHORT)(StackPointer);
    Sp = *(PUSHORT)(StackPointer + 2);
    SsSelector = *(PUSHORT)(StackPointer + 4);
    Ip = *(PUSHORT)(StackPointer + 6);
    CsSelector = *(PUSHORT)(StackPointer + 8);

    mvdm_softpc_wow_page_domain_leave_protected();
    setMSW(getMSW() & ~MSW_PE);
    mvdm_softpc_fast_bop_sync_mode();
    setDS(DsSelector);
    load_pseudo_descr(CPU40_DS_REG);
    setSP(Sp);
    setSS(SsSelector);
    load_pseudo_descr(CPU40_SS_REG);
    setIP(Ip);
    setCS(CsSelector);
    load_pseudo_descr(CPU40_CS_REG);
    mvdm_softpc_report_dpmi_mode_state("bop-fd-real", (unsigned long)getCR0(),
        (unsigned long)c_getCR3(), getCS(), getIP(),
        (unsigned long)c_getDS_BASE(), (unsigned long)c_getDS_LIMIT());
}

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

#if defined(i386)
    // bugbug hack hack
    *((PUSHORT)(DosxRmCodeSegment << 4) + 2) = DosxStackSegment;
#else
    PWORD16 Data;

    Data = (PWORD16)Sim32GetVDMPointer(
        ((ULONG)DosxRmCodeSegment << 16) | 4,
        1,
        FALSE
        );

    *(Data) = DosxStackSegment;
#endif

    /* DIVERGENCE(MVDM-HOST-DIV-242): the original kernel-VDM setCS
     * operation only populated a passive CONTEXT field, so it could retain
     * the real-mode DOSX segment until the following PE clear took effect.
     * CPU40's setCS immediately invokes the protected-mode descriptor
     * loader; validating the real-mode segment (for example D1CDh) as an
     * LDT selector rejects the source-defined transition before PE changes.
     * Clear PE first, then load the identical source-supplied CS through the
     * CCPU real-mode cache path. */
    mvdm_softpc_wow_page_domain_leave_protected();
    setMSW(getMSW() & ~MSW_PE);
    mvdm_softpc_fast_bop_sync_mode();
    setCS(DosxRmCodeSegment);

#ifndef i386
    /* DIVERGENCE(MVDM-HOST-DIV-230): the kernel VDM obtains the real-mode
     * hidden segment caches from hardware.  CPU40 retains its protected-mode
     * caches after PE is cleared, so reload the original CCPU pseudo
     * descriptors for every live segment before DOSX resumes real-mode code.
     * This is the same cache contract used by CCPU's real/V86 IRET paths; it
     * changes neither DOSX's frame nor its visible segment values. */
    load_pseudo_descr(CPU40_CS_REG);
    load_pseudo_descr(CPU40_SS_REG);
    load_pseudo_descr(CPU40_DS_REG);
    load_pseudo_descr(CPU40_ES_REG);
    load_pseudo_descr(CPU40_FS_REG);
    load_pseudo_descr(CPU40_GS_REG);
#endif
    mvdm_softpc_report_dpmi_mode_state("switch-real", (unsigned long)getCR0(),
        (unsigned long)c_getCR3(), getCS(), getIP(),
        (unsigned long)c_getDS_BASE(), (unsigned long)c_getDS_LIMIT());
}

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
#if defined(i386)
    // bugbug hack hack
    *((PUSHORT)(DosxRmCodeSegment << 4) + 2) = 0xb7;
#else
    PWORD16 Data;

    Data = (PWORD16)Sim32GetVDMPointer(
        ((ULONG)DosxRmCodeSegment << 16) | 4,
        1,
        FALSE
        );

    *(Data) = 0xb7;
#endif

    setMSW(getMSW() | MSW_PE);
    mvdm_softpc_fast_bop_sync_mode();

#ifndef i386
    //BUGBUG This is a workaround to make sure the emulator goes back
    // to privilege level 3 now that we are in protect mode.
    // Not doing this would cause an access violation in dpmi32.
    setCPL(3);
    /* DIVERGENCE(MVDM-HOST-DIV-243): x86's kernel VDM can retain the
     * real-mode visible CS in its passive CONTEXT until the caller restores
     * the protected register frame.  CPU40 executes the intervening DPMI
     * helper immediately (notably DpmiSegmentToSelector), so PE=1 with the
     * real-mode DOSX segment would be interpreted as an LDT selector.  DOSX
     * supplies its matching protected code selector in the shared layout;
     * use it solely as the transition carrier until the original caller
     * restores its saved CS/IP frame. */
    setCS(DosxRmCodeSelector);
    /* The same passive-CONTEXT gap applies to SS.  DPMI may enter a nested
     * protected helper before its caller restores the complete frame; give
     * that helper DOSX's source-provided protected stack/data selector rather
     * than preserving the real-mode stack segment in PE=1 state. */
    setSS(DosxPmDataSelector);
#endif
    (void)mvdm_softpc_wow_page_domain_reenter_protected();
    mvdm_softpc_report_dpmi_mode_state("switch-protected", (unsigned long)getCR0(),
        (unsigned long)c_getCR3(), getCS(), getIP(),
        (unsigned long)c_getDS_BASE(), (unsigned long)c_getDS_LIMIT());
}
