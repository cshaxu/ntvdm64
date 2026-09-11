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

/* CCPU keeps the generated register-index names private.  This CPU40 DPMI
 * counterpart needs only the existing real-mode cache reload operation. */
extern void load_pseudo_descr(int index);
extern void c_setGDT_BASE_LIMIT(ULONG base, USHORT limit);
extern void c_setIDT_BASE_LIMIT(ULONG base, USHORT limit);
extern void c_setTR_SELECTOR(USHORT selector);
extern void c_setTR_BASE_LIMIT_AR(ULONG base, ULONG limit, USHORT ar);
#define CPU40_CS_REG 1
#define CPU40_SS_REG 2
#define CPU40_DS_REG 3

#define CPU40_NATIVE_PM_TSS_SELECTOR 0x1f0u
#define CPU40_NATIVE_V86_TSS_SELECTOR 0x1f8u
#define CPU40_TSS386_BYTES 0x68u
#define CPU40_TSS386_IOMAP_BASE 0x66u
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
DpmiCpu40SetNativeIdtSourceAddress(ULONG Address)
{
    if (cpu40_native_idt_source_address == 0u)
        cpu40_native_idt_source_address = Address;
}

VOID
DpmiCpu40RestoreNativeIdt(VOID)
{
    if (cpu40_native_idt_source_address != 0u)
        c_setIDT_BASE_LIMIT(cpu40_native_idt_source_address,
            (USHORT)(256u * sizeof(LDT_ENTRY) - 1u));
}

VOID
DpmiCpu40SwitchToProtectedMode(VOID)
{
    PCHAR StackPointer;
    USHORT CsSelector;
    USHORT SsSelector;
    USHORT DsSelector;
    ULONG Eip;
    ULONG Esp;

    StackPointer = Sim32GetVDMPointer(((getSS() << 16) | getSP()), 0,
        (UCHAR)(getMSW() & MSW_PE));
    CsSelector = *(PUSHORT)(StackPointer + 12);
    Eip = *(PULONG)(StackPointer + 8);
    SsSelector = *(PUSHORT)(StackPointer + 6);
    Esp = *(PULONG)(StackPointer + 2);
    DsSelector = *(PUSHORT)(StackPointer);

    if (Cpu40LdtShadowAddress == 0u) return;
    c_setGDT_BASE_LIMIT(Cpu40LdtShadowAddress,
        (USHORT)(LDT_SIZE * sizeof(LDT_ENTRY) - 1u));
    DpmiCpu40RestoreNativeIdt();
    if (!cpu40_install_native_task_carrier()) return;

    setMSW(getMSW() | MSW_PE);
    setCPL(3);
    setCS(CsSelector);
    setEIP(Eip);
    setSS(SsSelector);
    setESP(Esp);
    setDS(DsSelector);
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

    CPU40 counterpart of the original i386 `switch_to_real_mode` BOP FD
    provider.  WOW DOSX pushes the five-word real-mode continuation frame
    (DS, SP, SS, IP, CS).  Capture it in the original order, leave protected
    mode, then load the same values through CCPU's real-mode cache path.

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

    DsSelector = *(PUSHORT)(StackPointer);
    Sp = *(PUSHORT)(StackPointer + 2);
    SsSelector = *(PUSHORT)(StackPointer + 4);
    Ip = *(PUSHORT)(StackPointer + 6);
    CsSelector = *(PUSHORT)(StackPointer + 8);

    mvdm_softpc_record_dosx_real_mode_frame(getCS(), getIP(), DsSelector,
        Sp, SsSelector, Ip, CsSelector, getMSW());

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

    mvdm_softpc_record_dosx_real_mode_switch(getCS(), getIP(),
        DosxRmCodeSegment, getMSW());
    setCS(DosxRmCodeSegment);

    setMSW(getMSW() & ~MSW_PE);

#ifndef i386
    //BUGBUG This is a workaround to reload a 64k limit into SS for the
    // emulator, now that we are in real mode.
    // Not doing this would cause the emulator to do a hardware reset
    setSS_BASE_LIMIT_AR(getSS_BASE(), 0xffff, getSS_AR());
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
#endif
}
