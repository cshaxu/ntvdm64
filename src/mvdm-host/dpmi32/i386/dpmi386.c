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
extern BOOL ThreadSetDebugContext(PULONG Registers);
extern BOOL mvdm_debugger_read_debug_registers(USHORT Selector,
    USHORT Offset, PULONG Registers);

#include "ntvdm-exe/softpc/include/mvdm_softpc_wow_page_domain.h"
extern void load_pseudo_descr(int index);
extern void c_setGDT_BASE_LIMIT(ULONG base, USHORT limit);
extern void c_setIDT_BASE_LIMIT(ULONG base, USHORT limit);
#define CPU40_SS_REG 2
#define CPU40_DS_REG 3


#line 119
VOID
switch_to_protected_mode(
    VOID
    )

/*++

Routine Description:

    This routine switches the dos applications context to protected mode.

Arguments:

    None

Return Value:

    None.

--*/

{

    PCHAR StackPointer;
    ULONG IdtBase;
    USHORT IdtLimit;
#line 143

    StackPointer = Sim32GetVDMPointer(((getSS() << 16) | getSP()),
        0,
        (UCHAR) (getMSW() & MSW_PE)
        );

    if (Cpu40GdtShadowAddress != 0)
        c_setGDT_BASE_LIMIT(Cpu40GdtShadowAddress,
            (USHORT)(LDT_SIZE * sizeof(LDT_ENTRY) - 1));
    if (!DpmiCpu40GetDosxIdtDescriptor(&IdtBase, &IdtLimit))
        return;
    c_setIDT_BASE_LIMIT(IdtBase, IdtLimit);
    setEFLAGS(getEFLAGS() & ~EFLAGS_NT_MASK);
    setMSW(getMSW() | MSW_PE);
    setCPL(3);
#line 149
    setCS(*(PUSHORT)(StackPointer + 12));

    setEIP(*(PULONG)(StackPointer + 8));
    setSS(*(PUSHORT)(StackPointer + 6));
    setESP(*(PULONG)(StackPointer + 2));
    setDS(*(PUSHORT)(StackPointer));
    // Necessary to prevent loads of invalid selectors.
    setES(0);
    setGS(0);
    setFS(0);

    (void)mvdm_softpc_wow_page_domain_reenter_protected();
#line 184
}

#line 187
VOID
switch_to_real_mode(
    VOID
    )

/*++

Routine Description:

    This routine services the switch to real mode bop.  It is included in
    DPMI.c so that all of the mode switching services are in the same place

Arguments:

    None

Return Value:

    None.

--*/

{
    PCHAR StackPointer;

    StackPointer = Sim32GetVDMPointer(((getSS() << 16) | getSP()),
        0,
        (UCHAR) (getMSW() & MSW_PE)
        );

    mvdm_softpc_wow_page_domain_leave_protected();
    setMSW(getMSW() & ~MSW_PE);
    setCPL(0);
#line 217
    setDS(*(PUSHORT)(StackPointer));
    load_pseudo_descr(CPU40_DS_REG);
#line 218
    setSP(*(PUSHORT)(StackPointer + 2));
    setSS(*(PUSHORT)(StackPointer + 4));
    load_pseudo_descr(CPU40_SS_REG);
#line 220
    setIP((*(PUSHORT)(StackPointer + 6)));
    setCS(*(PUSHORT)(StackPointer + 8));
#line 222

#line 246
}

#line 342
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
    ULONG DebugRegisters[6];

#line 367
    setCF(0);

#if defined(CPU_40_STYLE)
    RegisterPointer = mvdm_debugger_read_debug_registers(getDS(), getSI(),
        DebugRegisters) ? (PCHAR)DebugRegisters : NULL;
#else
#line 369
#error DIVERGENCE MVDM-HOST-DIV-513: non-CPU40 branch is not selected; see mirror register.
#endif

#line 374
    if (!ThreadSetDebugContext((PULONG) RegisterPointer))
#line 375
        {
        ULONG ClearDebugRegisters[6] = {0, 0, 0, 0, 0, 0};

        //
        // an error occurred. Reset everything to zero
        //

        ThreadSetDebugContext (&ClearDebugRegisters[0]);
        setCF(1);
        }

}
