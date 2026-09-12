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
#include "mvdm_softpc_termination.h"

/* CCPU's internal segment-register indices and pseudo-descriptor routine
 * are deliberately kept private to its generated headers.  CPU40's DPMI
 * provider needs only this source-defined real-mode cache refresh. */
extern void load_pseudo_descr(int index);
extern void c_setGDT_BASE_LIMIT(ULONG base, USHORT limit);
extern void c_setIDT_BASE_LIMIT(ULONG base, USHORT limit);
extern void c_setTR_SELECTOR(USHORT selector);
extern void c_setTR_BASE_LIMIT_AR(ULONG base, ULONG limit, USHORT ar);
#define CPU40_CS_REG 1
#define CPU40_SS_REG 2
#define CPU40_DS_REG 3

/* WOW_x86 deliberately omits LTR: native NT's VDM already has a busy task
 * whose backlink reaches a V86 task. CPU40 retains that hardware contract in
 * two private GDT slots instead of special-casing the original IRET. */
#define CPU40_NATIVE_PM_TSS_SELECTOR 0x1f0u
#define CPU40_NATIVE_V86_TSS_SELECTOR 0x1f8u
#define CPU40_TSS386_BYTES 0x68u
#define CPU40_TSS386_IOMAP_BASE 0x66u
/* A 386 task's I/O bitmap has one bit per port.  The native VDM task that
 * WOW_x86 assumes permits the virtual machine's port traffic; retain that
 * property explicitly rather than letting CCPU reject the first IN/OUT
 * because its reconstructed TSS ends at the bitmap-base field. */
#define CPU40_TSS386_IOMAP_BYTES 0x2000u
#define CPU40_TSS386_TOTAL_BYTES \
    (CPU40_TSS386_BYTES + CPU40_TSS386_IOMAP_BYTES)
#define CPU40_TSS386_EIP 0x20u
#define CPU40_TSS386_EFLAGS 0x24u
#define CPU40_TSS386_EAX 0x28u
#define CPU40_TSS386_ECX 0x2cu
#define CPU40_TSS386_EDX 0x30u
#define CPU40_TSS386_EBX 0x34u
#define CPU40_TSS386_ESP 0x38u
#define CPU40_TSS386_EBP 0x3cu
#define CPU40_TSS386_ESI 0x40u
#define CPU40_TSS386_EDI 0x44u
#define CPU40_TSS386_ES 0x48u
#define CPU40_TSS386_CS 0x4cu
#define CPU40_TSS386_SS 0x50u
#define CPU40_TSS386_DS 0x54u
#define CPU40_TSS386_FS 0x58u
#define CPU40_TSS386_GS 0x5cu
#define CPU40_TSS386_LDT 0x60u
#define CPU40_TSS386_VM 0x00020000u
#define CPU40_TSS386_NT 0x00004000u
#define CPU40_XTND_BUSY_TSS 0x0bu

/* This is session-local transition-provider state, not a DOSX descriptor.
 * It stays private so CCPU context marshaling cannot serialize it as a CPU
 * register field. */
static ULONG cpu40_native_idt_source_address;

static void cpu40_write_tss_descriptor(PLDT_ENTRY entry, ULONG base)
{
    RtlZeroMemory(entry, sizeof(*entry));
    entry->LimitLow = CPU40_TSS386_TOTAL_BYTES - 1u;
    entry->BaseLow = (USHORT)base;
    entry->HighWord.Bytes.BaseMid = (UCHAR)(base >> 16);
    entry->HighWord.Bytes.BaseHi = (UCHAR)(base >> 24);
    entry->HighWord.Bits.Type = CPU40_XTND_BUSY_TSS;
    entry->HighWord.Bits.Dpl = 3u;
    entry->HighWord.Bits.Pres = 1u;
    entry->HighWord.Bits.Sys = 0u;
}

static void cpu40_write_native_v86_task(PUCHAR task)
{
    RtlZeroMemory(task, CPU40_TSS386_TOTAL_BYTES);
    *(PUSHORT)(task + CPU40_TSS386_IOMAP_BASE) = CPU40_TSS386_BYTES;
    *(PULONG)(task + CPU40_TSS386_EIP) = getEIP();
    *(PULONG)(task + CPU40_TSS386_EFLAGS) =
        (getEFLAGS() | CPU40_TSS386_VM) & ~CPU40_TSS386_NT;
    *(PULONG)(task + CPU40_TSS386_EAX) = getEAX();
    *(PULONG)(task + CPU40_TSS386_ECX) = getECX();
    *(PULONG)(task + CPU40_TSS386_EDX) = getEDX();
    *(PULONG)(task + CPU40_TSS386_EBX) = getEBX();
    *(PULONG)(task + CPU40_TSS386_ESP) = getESP();
    *(PULONG)(task + CPU40_TSS386_EBP) = getEBP();
    *(PULONG)(task + CPU40_TSS386_ESI) = getESI();
    *(PULONG)(task + CPU40_TSS386_EDI) = getEDI();
    *(PUSHORT)(task + CPU40_TSS386_ES) = getES();
    *(PUSHORT)(task + CPU40_TSS386_CS) = getCS();
    *(PUSHORT)(task + CPU40_TSS386_SS) = getSS();
    *(PUSHORT)(task + CPU40_TSS386_DS) = getDS();
    *(PUSHORT)(task + CPU40_TSS386_FS) = getFS();
    *(PUSHORT)(task + CPU40_TSS386_GS) = getGS();
    *(PUSHORT)(task + CPU40_TSS386_LDT) = 0u;
}

static int cpu40_install_native_task_carrier(void)
{
    ULONG address;
    ULONG size;
    PUCHAR state;
    PLDT_ENTRY gdt;

    if (Cpu40LdtShadowAddress == 0u) return 0;
    if (Cpu40NativeTaskStateAddress == 0u) {
        address = 0u;
        size = CPU40_TSS386_TOTAL_BYTES * 2u;
        if (!NT_SUCCESS(DpmiAllocateVirtualMemory((PVOID)&address, &size)))
            return 0;
        Cpu40NativeTaskStateAddress = address;
    }

    state = (PUCHAR)(IntelBase + Cpu40NativeTaskStateAddress);
    RtlZeroMemory(state, CPU40_TSS386_TOTAL_BYTES);
    *(PUSHORT)state = CPU40_NATIVE_V86_TSS_SELECTOR;
    *(PUSHORT)(state + CPU40_TSS386_IOMAP_BASE) = CPU40_TSS386_BYTES;
    cpu40_write_native_v86_task(state + CPU40_TSS386_TOTAL_BYTES);

    gdt = (PLDT_ENTRY)(IntelBase + Cpu40LdtShadowAddress);
    cpu40_write_tss_descriptor(&gdt[CPU40_NATIVE_PM_TSS_SELECTOR >> 3],
        Cpu40NativeTaskStateAddress);
    cpu40_write_tss_descriptor(&gdt[CPU40_NATIVE_V86_TSS_SELECTOR >> 3],
        Cpu40NativeTaskStateAddress + CPU40_TSS386_TOTAL_BYTES);
    c_setTR_SELECTOR(CPU40_NATIVE_PM_TSS_SELECTOR);
    c_setTR_BASE_LIMIT_AR(Cpu40NativeTaskStateAddress,
        CPU40_TSS386_TOTAL_BYTES - 1u, CPU40_XTND_BUSY_TSS);
    return 1;
}

VOID
DpmiCpu40SetNativeIdtSourceAddress(
    ULONG Address
    )
{
    /* 53:00 is also used later for ordinary descriptor publication.  Only
     * DOSX's first table publication establishes the adjacent IDT layout;
     * later callers may legitimately carry unrelated small real-mode
     * segments (for example 00D7h), which must not replace that carrier. */
    if (cpu40_native_idt_source_address == 0u)
        cpu40_native_idt_source_address = Address;
}

VOID
DpmiCpu40RestoreNativeIdt(
    VOID
    )
/*++

Routine Description:

    Restores the protected-mode IDTR carrier which WOW_x86 inherits from
    native NT VDM.  Some CPU40 accelerated-context resumes restore the
    real-mode IVT-shaped IDTR even though the guest has already returned to
    PE.  The native VDM never exposed that transient state to a protected
    software interrupt, so retain the source-published DOSX table at the
    common pre-dispatch boundary.

--*/
{
    if (cpu40_native_idt_source_address != 0u)
    {
        ULONG source_address = cpu40_native_idt_source_address;

        c_setIDT_BASE_LIMIT(source_address,
            (USHORT)(256u * sizeof(LDT_ENTRY) - 1u));
    }
}

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
    if (Cpu40LdtShadowAddress != 0)
        c_setGDT_BASE_LIMIT(Cpu40LdtShadowAddress,
            (USHORT)(LDT_SIZE * sizeof(LDT_ENTRY) - 1));

    /* WOW_x86 omits LIDT because the native VDM already retains DOSX's live
     * 256-entry table.  53:00 recorded the source address and DOSX filled
     * the gates in place before this 53:01 entry.  Project that exact table
     * into CCPU's IDTR; do not synthesize gates or replace the DPMI hooks. */
    DpmiCpu40RestoreNativeIdt();

    if (!cpu40_install_native_task_carrier()) return;

    setMSW(getMSW() | MSW_PE);
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

    mvdm_softpc_record_dosx_real_mode_frame((unsigned int)getCS(),
        (unsigned int)getIP(), (unsigned int)DsSelector, (unsigned int)Sp,
        (unsigned int)SsSelector, (unsigned int)Ip,
        (unsigned int)CsSelector, (unsigned int)getMSW());

    setMSW(getMSW() & ~MSW_PE);
    setDS(DsSelector);
    load_pseudo_descr(CPU40_DS_REG);
    setSP(Sp);
    setSS(SsSelector);
    load_pseudo_descr(CPU40_SS_REG);
    setIP(Ip);
    setCS(CsSelector);
    load_pseudo_descr(CPU40_CS_REG);
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
    setMSW(getMSW() & ~MSW_PE);
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
    load_pseudo_descr(4); /* ES_REG */
    load_pseudo_descr(5); /* FS_REG */
    load_pseudo_descr(6); /* GS_REG */
#endif
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
}
