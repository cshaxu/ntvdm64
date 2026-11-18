/*++

Copyright (c) 1991  Microsoft Corporation

Module Name:

    dpmiint.c

Abstract:

    This file contains the interrupt support for DPMI. Most of this is
    for supporting the 486 emulator on risc platforms, but some code
    is shared with x86.

Author:

    Neil Sandlin (neilsa) 1-Jun-1995

Revision History:

Comments:

    DPMI stack switching is accomplished by keeping a "locked pm stack"
    count, and when the count is zero, a stack switch occurs. This keeps
    track of the situation with recursive interrupts where the client
    may switch to its own stack. So, a stack switch to our locked stack
    occurs on the first level interrupt, and on subsequent nested interrupts,
    only the count is maintained. This is identical to how win31 managed
    the stack.

    If a client specifies that it is a 32-bit dpmi client, this only affects
    the "width" of a stack frame. A 16-bit client gets 16-bit frames, and
    a 32 bit client gets 32-bit frames. It is still necessary to check
    the size of the stack segment to determine if SP or ESP should be used.

--*/

#include "precomp.h"
#pragma hdrstop
#include <softpc.h>
#include <dpmiint.h>
#include <intapi.h>
extern ULONG c_getCS_BASE(void), c_getCS_LIMIT(void);
extern USHORT c_getCS_AR(void);
extern int c_getCPL(void);
extern int c_getVM(void);
extern VOID IN8(PULONG, ULONG), IN16(PULONG, ULONG), IN32(PULONG, ULONG);
extern VOID OUT8(ULONG, ULONG), OUT16(ULONG, ULONG), OUT32(ULONG, ULONG);
extern void ccpu386CompleteInstruction(void);
#line 43


VOID
DpmiFatalExceptionHandler(
    UCHAR XNumber, 
    PCHAR VdmStackPointer
    );

#line 51
VOID
DpmiSetProtectedmodeInterrupt(
    VOID
    )

/*++

Routine Description:

    This function services the SetProtectedmodeInterrupt bop.  It retrieves
    the handler information from the Dos application stack, and puts it into
    the VdmTib, for use by instruction emulation.


--*/

{

    PVDM_INTERRUPTHANDLER Handlers = DpmiInterruptHandlers;
    USHORT IntNumber;
    PCHAR StackPointer;

    StackPointer = Sim32GetVDMPointer(((((ULONG)getSS()) << 16) | getSP()),
        0,
        (UCHAR) (getMSW() & MSW_PE)
        );

#line 78
    IntNumber = *(PWORD16)(StackPointer + 6);

    Handlers[IntNumber].Flags = *(PWORD16)(StackPointer + 8);
    Handlers[IntNumber].CsSelector = *(PWORD16)(StackPointer + 4);
    Handlers[IntNumber].Eip = *(PDWORD16)(StackPointer);

    DBGTRACE(DPMI_SET_PMODE_INT_HANDLER, IntNumber,
                                         Handlers[IntNumber].CsSelector,
                                         Handlers[IntNumber].Eip);
    
#line 88
#ifdef i386
#line 89
#error Excluded unsupported VdmSetInt21Handler kernel VDM path
#endif      //i386
#line 107

    setAX(0);
}

VOID
DpmiSetFaultHandler(
    VOID
    )

/*++

Routine Description:

    This function services the SetFaultHandler bop.  It retrieves
    the handler information from the Dos application stack, and puts it into
    the VdmTib, for use by instruction emulation.


--*/

{

    PVDM_FAULTHANDLER Handlers = DpmiFaultHandlers;
    USHORT IntNumber;
    PCHAR StackPointer;

    StackPointer = Sim32GetVDMPointer(((((ULONG)getSS()) << 16) | getSP()),
        0,
        (UCHAR) (getMSW() & MSW_PE)
        );

#line 138
    IntNumber = *(PWORD16)(StackPointer + 12);

    Handlers[IntNumber].Flags = *(PDWORD16)(StackPointer + 14);
    Handlers[IntNumber].CsSelector = *(PWORD16)(StackPointer + 10);
    Handlers[IntNumber].Eip = *(PDWORD16)(StackPointer + 6);
    Handlers[IntNumber].SsSelector = *(PWORD16)(StackPointer + 4);
    Handlers[IntNumber].Esp = *(PDWORD16)(StackPointer);


    DBGTRACE(DPMI_SET_FAULT_HANDLER, IntNumber,
                                     Handlers[IntNumber].CsSelector,
                                     Handlers[IntNumber].Eip);
    setAX(0);
}

VOID
DpmiUnhandledExceptionHandler(
    VOID
    )
/*++

Routine Description:

    This function gets control when a PM fault occurs that isn't handled
    by an installed handler. The body of this function emulates Win31
    DPMI behavior, where a fault that is reflected to the end of the
    PM fault handler chain is then reflected to the PM *interrupt* 
    chain. 

Arguments:

    client SS:(E)SP points to dpmi fault stack frame


--*/

{
    PVDM_INTERRUPTHANDLER Handlers = DpmiInterruptHandlers;
#line 176
    USHORT SegSs, SegCs;
    UCHAR XNumber;
    PCHAR VdmStackPointer;
    PCHAR VdmCodePointer;
    USHORT FaultingCS;
    ULONG FaultingEip;

    SegSs = getSS();
    VdmStackPointer = Sim32GetVDMPointer(SegSs<<16, 1, TRUE);

    if (SEGMENT_IS_BIG(SegSs)) {
        VdmStackPointer += getESP();
    } else {
        VdmStackPointer += getSP();
    }

    SegCs = getCS();
    VdmCodePointer = Sim32GetVDMPointer(SegCs<<16, 1, TRUE);

    if (SEGMENT_IS_BIG(SegCs)) {
        VdmCodePointer += getEIP();
    } else {
        VdmCodePointer += getIP();
    }

    XNumber = *(VdmCodePointer);

    if ((XNumber > 7) || (XNumber == 6)) {
        DpmiFatalExceptionHandler(XNumber, VdmStackPointer);
        return;
    }


    if (Frame32) {

        PCHAR VdmNewStackPointer;
        ULONG FrameSS, FrameSP, FrameCS, FrameIP, FrameFlags;

        //
        // Build an iret frame on the faulting stack
        //
        FrameSS = *(PDWORD16) (VdmStackPointer+28);
        FrameSP = *(PDWORD16) (VdmStackPointer+24) - 12;
        *(PDWORD16) (VdmStackPointer+24) = FrameSP;
        VdmNewStackPointer = Sim32GetVDMPointer((ULONG)(FrameSS << 16), 1, TRUE);
        VdmNewStackPointer += FrameSP;

        FrameIP = *(PDWORD16) (VdmStackPointer+12);
        *(PDWORD16) (VdmStackPointer+12) = Handlers[XNumber].Eip;
        *(PDWORD16) (VdmNewStackPointer) = FrameIP;

        FrameCS = *(PDWORD16) (VdmStackPointer+16);
        *(PDWORD16) (VdmStackPointer+16) = (ULONG) Handlers[XNumber].CsSelector;
        *(PDWORD16) (VdmNewStackPointer+4) = FrameCS;

        FrameFlags = *(PDWORD16) (VdmStackPointer+20);
#line 232
        *(PDWORD16) (VdmNewStackPointer+8) = FrameFlags;
#line 233
        FrameFlags &= ~(EFLAGS_INTERRUPT_MASK | EFLAGS_TF_MASK);
        *(PDWORD16) (VdmStackPointer+20) = FrameFlags;

        //
        // Simulate a dpmi fault handler retf
        //
        setCS((USHORT)*(PDWORD16)(VdmStackPointer+4));
        setEIP(*(PDWORD16)(VdmStackPointer));
        setESP(getESP() + 8);

    } else {

#line 245
        USHORT FrameSS, FrameSP, FrameCS, FrameIP, FrameFlags;
        FrameSS = *(PWORD16) (VdmStackPointer+14);
        FrameCS = *(PWORD16) (VdmStackPointer+8);
        FrameFlags = *(PWORD16) (VdmStackPointer+10);

        if (!SEGMENT_IS_BIG(FrameSS) && !SEGMENT_IS_BIG(FrameCS)) {

            PCHAR VdmNewStackPointer;

            //
            // Build an iret frame on the faulting stack
            //
            FrameSP = *(PWORD16) (VdmStackPointer+12) - 6;
            *(PWORD16) (VdmStackPointer+12) = FrameSP;
            VdmNewStackPointer = Sim32GetVDMPointer((ULONG)(FrameSS << 16)+FrameSP, 1, TRUE);

            FrameIP = *(PWORD16) (VdmStackPointer+6);
            *(PWORD16) (VdmStackPointer+6) = (WORD) Handlers[XNumber].Eip;
            *(PWORD16) (VdmNewStackPointer) = FrameIP;

            *(PWORD16) (VdmStackPointer+8) = Handlers[XNumber].CsSelector;
            *(PWORD16) (VdmNewStackPointer+2) = FrameCS;

            *(PWORD16) (VdmNewStackPointer+4) = FrameFlags;
            FrameFlags &= ~(EFLAGS_INTERRUPT_MASK | EFLAGS_TF_MASK);
            *(PWORD16) (VdmStackPointer+10) = FrameFlags;

            //
            // Simulate a dpmi fault handler retf
            //
            setCS(*(PWORD16)(VdmStackPointer+2));
            setEIP((DWORD)*(PWORD16)(VdmStackPointer));
            setSP(getSP() + 4);

        } else {
            //
            // Build an iret frame on the locked DPMI stack
            //

            FrameCS = *(PWORD16) (VdmStackPointer+2);
            FrameIP = *(PWORD16) (VdmStackPointer);
            FrameFlags &= ~EFLAGS_INTERRUPT_MASK;

            setSP(getSP() - 2);

            *(PWORD16)(VdmStackPointer-2) = FrameIP;
            *(PWORD16)(VdmStackPointer)   = FrameCS;
            *(PWORD16)(VdmStackPointer+2) = FrameFlags;
            setCS(Handlers[XNumber].CsSelector);
            setEIP((DWORD)LOWORD(Handlers[XNumber].Eip));
            setSTATUS((WORD) FrameFlags & ~EFLAGS_TF_MASK);
        }

    }

}


VOID
DpmiFatalExceptionHandler(
    UCHAR XNumber, 
    PCHAR VdmStackPointer
    )
/*++

Routine Description:

    This function gets control when a PM fault 6, 8-1f occurs that isn't
    handled by an installed handler. It pops up an error dialog for the
    user.

Arguments:

    XNumber - exception number (0-1fh)
    VdmStackPointer - flat pointer to stack frame


--*/

{
    char szBuffer[255];
    USHORT FaultingCS;
    ULONG FaultingEip;

    if (Frame32) {
        FaultingCS  = (USHORT)*(PDWORD16)(VdmStackPointer+16);
        FaultingEip = *(PDWORD16)(VdmStackPointer+12);
    } else {
        FaultingCS  = *(PWORD16)(VdmStackPointer+8);
        FaultingEip = (ULONG)*(PWORD16)(VdmStackPointer+6);
    }

    wsprintf(szBuffer, "X#=%.02X, CS=%.04X IP=%.08X",
                            XNumber, FaultingCS, FaultingEip);
       
    RcErrorDialogBox(EG_BAD_FAULT, szBuffer, NULL);

    //
    // Need to try to ignore it. Since we are on a dpmi exception frame
    // we can just simulate a retf.
    //
    if (Frame32) {
        setCS((USHORT)*(PDWORD16)(VdmStackPointer+4));
        setEIP(*(PDWORD16)(VdmStackPointer));
        setESP(getESP() + 8);
    } else {
        setCS(*(PWORD16)(VdmStackPointer+2));
        setEIP((DWORD)*(PWORD16)(VdmStackPointer));
        setSP(getSP() + 4);
    }
}

VOID
DpmiPassPmStackInfo(
    VOID
    )
/*++

Routine Description:

    This routine is called via BOP by DOSX to initialize values related
    to stack handling.

Arguments:

    Client ES = selector of locked PM stack

Return Value:

    None

Notes:

    The offset of the locked pm stack is hard-coded to 0x1000, per dpmi
    and win31.

--*/
{

    LockedPMStackSel = getES();
    LockedPMStackCount = 0;

#line 387
#ifdef i386
#error Excluded unsupported i386 kernel VDM stack publication
#endif
#line 397
}

VOID
BeginUseLockedPMStack(
    VOID
    )
/*++

Routine Description:

    This routine switches to the protected DPMI stack as specified by
    the DPMI spec. We remember the original values of EIP and ESP in
    global variables if we are at level zero. This allows us to correctly
    return to a 32 bit routine if we are dispatching a 16-bit interrupt
    frame.


--*/

{
    if (!LockedPMStackCount++) {
        PMLockOrigEIP = getEIP();
        PMLockOrigSS = getSS();
        PMLockOrigESP = getESP();
        setSS(LockedPMStackSel);
        setESP(LockedPMStackOffset);
    }
}

BOOL
EndUseLockedPMStack(
    VOID
    )
/*++

Routine Description:

    This routine switches the stack back off the protected DPMI stack,
    if we are popping off the last frame on the stack.

Return Value:

    TRUE if the stack was switched back, FALSE otherwise

--*/


{

    if (!--LockedPMStackCount) {
        setEIP(PMLockOrigEIP);
        setSS((WORD)PMLockOrigSS);
        setESP(PMLockOrigESP);
        return TRUE;
    }
    return FALSE;

}

void
ReflectV86Int(
    ULONG IntNumber,
    ULONG Eflags
    )
/*++

Routine Description:

    This routine is responsible for simulating a real mode interrupt. It
    uses the real mode IVT at 0:0.

Arguments:

    IntNumber - interrupt vector number
    Eflags - client flags to save on the stack


--*/

{
    PUCHAR VdmStackPointer;
    PWORD16 pIVT;
    USHORT VdmSP;

    VdmStackPointer = Sim32GetVDMPointer(((ULONG)getSS())<<16, 1, FALSE);
    VdmSP = getSP() - 2;
    *(PWORD16)(VdmStackPointer+VdmSP) = (WORD) Eflags;
    VdmSP -= 2;
    *(PWORD16)(VdmStackPointer+VdmSP) = (WORD) getCS();
    VdmSP -= 2;
    *(PWORD16)(VdmStackPointer+VdmSP) = (WORD) getIP();
    setSP(VdmSP);
    pIVT = (PWORD16) (IntelBase + IntNumber*4);
    setIP(*pIVT++);
    setCS(*pIVT);
}

#line 495
static BOOL BuildStackFrameWithWidth(ULONG StackUnits, BOOL WideFrame,
    PUCHAR *pVdmStackPointer, ULONG *pNewSP);

#line 494
BOOL
#line 495
DpmiSwIntHandler(
    ULONG IntNumber
    )
/*++

Routine Description:

    This routine is called by the emulator to dispatch a SW interrupt.

Arguments:

    IntNumber - interrupt vector number

Return Value:

    TRUE if the interrupt was dispatched, FALSE otherwise

--*/

{
    PVDM_INTERRUPTHANDLER Handlers = DpmiInterruptHandlers;
    PUCHAR VdmStackPointer;
    ULONG SaveEFLAGS;
    ULONG NewSP;

    DBGTRACE(DPMI_SW_INT, IntNumber, 0, 0);

#line 522
    if (!SEGMENT_IS_PRESENT(Handlers[IntNumber].CsSelector)) { 
        return FALSE;
    }

#line 526
    SaveEFLAGS = getEFLAGS();
    //BUGBUG turn off task bits
    SaveEFLAGS &= ~0x4000;
#line 530

    if (!(getMSW() & MSW_PE)) {

#line 534
        setEFLAGS(SaveEFLAGS & ~EFLAGS_TF_MASK);
#line 533
        ReflectV86Int(IntNumber, SaveEFLAGS);

    } else {
        PUCHAR VdmStackPointer;

        // Protect mode

#line 541
        /* DIVERGENCE(MVDM-HOST-DIV-481): the x86 monitor's PushInt
         * selects the return frame from the registered handler flags, not
         * the interrupted application's default width. */
        BOOL WideFrame = (Handlers[IntNumber].Flags & VDM_INT_32) != 0;
        if (!BuildStackFrameWithWidth(3, WideFrame, &VdmStackPointer, &NewSP)) {
#line 541
            return FALSE;
        }

#line 545
        /* A rejected frame does not commit the monitor's flag updates. */
        setEFLAGS(SaveEFLAGS & ~EFLAGS_TF_MASK);
#line 544
        if (WideFrame) {

            *(PDWORD16)(VdmStackPointer-4) =  SaveEFLAGS;
#line 547
            *(PDWORD16)(VdmStackPointer-8) =  getCS();
#line 548
            *(PDWORD16)(VdmStackPointer-12) = getEIP();
#line 549
            setEIP(Handlers[IntNumber].Eip);
#line 552
        } else {

            *(PWORD16)(VdmStackPointer-2) = (WORD) SaveEFLAGS;
            *(PWORD16)(VdmStackPointer-4) = (WORD) getCS();
            *(PWORD16)(VdmStackPointer-6) = (WORD) getEIP();
#line 557
            setEIP((DWORD)LOWORD(Handlers[IntNumber].Eip));
#line 560
        }

#line 563
        setESP(NewSP);
#line 562
        setCS(Handlers[IntNumber].CsSelector);

#line 564
#if DBG
#line 565
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 573
    }

    DBGTRACE(DPMI_DISPATCH_INT, IntNumber, 0, 0);
    return TRUE;
}

BOOL
DpmiHwIntHandler(
    ULONG IntNumber
    )

/*++

Routine Description:

    This routine is called by the emulator to dispatch a HW interrupt.

Arguments:

    IntNumber - interrupt vector number

Return Value:

    TRUE if the interrupt was dispatched, FALSE otherwise

--*/

{
    PVDM_INTERRUPTHANDLER Handlers = DpmiInterruptHandlers;
    PUCHAR VdmStackPointer;
    ULONG SaveEFLAGS;
    ULONG NewSP;

    DBGTRACE(DPMI_HW_INT, IntNumber, 0, 0);

#line 608
    SaveEFLAGS = getEFLAGS();
    //BUGBUG turn off task bits
    SaveEFLAGS &= ~0x4000;
    setEFLAGS(SaveEFLAGS & ~(EFLAGS_INTERRUPT_MASK | EFLAGS_TF_MASK));

    if (!(getMSW() & MSW_PE)) {

        ReflectV86Int(IntNumber, SaveEFLAGS);

    } else {
        PUCHAR VdmStackPointer;

        BeginUseLockedPMStack();

        if (!BuildStackFrame(6, &VdmStackPointer, &NewSP)) {
            EndUseLockedPMStack();
            return FALSE;
        }

        if (Frame32) {
            *(PDWORD16)(VdmStackPointer-4) = SaveEFLAGS;
            *(PDWORD16)(VdmStackPointer-8) = getCS();
            *(PDWORD16)(VdmStackPointer-12) = getEIP();
            *(PDWORD16)(VdmStackPointer-16) = getEFLAGS();
            *(PDWORD16)(VdmStackPointer-20) = (DWORD)HIWORD(DosxIntHandlerIretd);
            *(PDWORD16)(VdmStackPointer-24) = (DWORD)LOWORD(DosxIntHandlerIretd);
            setEIP(Handlers[IntNumber].Eip);
            setESP(NewSP);
        } else {
            *(PWORD16)(VdmStackPointer-2) = (WORD)SaveEFLAGS;
            *(PWORD16)(VdmStackPointer-4) = (WORD)getCS();
            *(PWORD16)(VdmStackPointer-6) = (WORD)getIP();
            *(PWORD16)(VdmStackPointer-8) = (WORD)getEFLAGS();
            *(PWORD16)(VdmStackPointer-10) = HIWORD(DosxIntHandlerIret);
            *(PWORD16)(VdmStackPointer-12) = LOWORD(DosxIntHandlerIret);
            setEIP((DWORD)LOWORD(Handlers[IntNumber].Eip));
            setSP((WORD)NewSP);
        }
        setCS(Handlers[IntNumber].CsSelector);
    }

    DBGTRACE(DPMI_DISPATCH_INT, IntNumber, 0, 0);
    return TRUE;
}


VOID
#line 655
DpmiIntHandlerIret16(
    VOID
    )

/*++

Routine Description:

    This routine is an IRET hook called via a BOP in dosx. It is called
    at the end of a 16-bit HW or SW interrupt. The main reason we want
    to come in here is to maintain the DPMI stack, and know when to restore
    the original values when we pop back out to level zero.


--*/

{
    PUCHAR VdmStackPointer;
    ULONG NewSP;
    USHORT SegSs;
    BOOL bSsBig;

    SegSs = getSS();
    VdmStackPointer = Sim32GetVDMPointer(SegSs<<16, 1, TRUE);

    if (bSsBig = SEGMENT_IS_BIG(SegSs)) {
        VdmStackPointer += getESP();
    } else {
        VdmStackPointer += getSP();
    }

#line 686
    //
    // Fast iret (without executing final 16-bit iret)
    //
#line 689
#ifdef i386
#line 690
#error Excluded unsupported i386 kernel VDM interrupt return
#else
#line 717
    if (EndUseLockedPMStack()) {
        ULONG NewEIP, NewEFLAGS, NewCS;

        NewEIP    = getEIP();
        NewCS     = (ULONG) *(PWORD16)(VdmStackPointer+2);
        NewEFLAGS = (getEFLAGS()&0xffff0000) | *(PWORD16)(VdmStackPointer+4);

        //
        // Since EndUseLockedPMStack() has restored all of EIP, and we may be
        // returning to a 32-bit code segment, build a 32-bit iret frame
        // even if this is a 16-bit client. That way, EIP will be restored
        // correctly.
        // Pass 6 to BuildStackFrame since 6 words = 3 dwords
        //
        if (!BuildStackFrame(6, &VdmStackPointer, &NewSP)) {
#line 732
#if DBG
#line 733
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 735
            DpmiFaultHandler(STACK_FAULT, 0);
            return;
        }

        //
        // SS has changed, so we need to check LDT again
        //
        if (SEGMENT_IS_BIG(getSS())) {
            setESP(NewSP);
        } else {
            setSP((WORD)NewSP);
        }

        *(PDWORD16)(VdmStackPointer-4)  = NewEFLAGS;
        *(PDWORD16)(VdmStackPointer-8)  = NewCS;
        *(PDWORD16)(VdmStackPointer-12) = NewEIP;
        setCS(HIWORD(DosxIretd));
        setEIP((ULONG)LOWORD(DosxIretd));

    } else {

        // still on locked stack, just do a real iret (16-bit frame)
        setCS(HIWORD(DosxIret));
        setEIP((ULONG)LOWORD(DosxIret));

    }
#endif // i386

    #line 763
    DBGTRACE(DPMI_INT_IRET16, 0, 0, 0);
}

VOID
#line 767
DpmiIntHandlerIret32(
    VOID
    )

/*++

Routine Description:

    This routine is an IRET hook called via a BOP in dosx. It is called
    at the end of a 32-bit HW or SW interrupt. The main reason we want
    to come in here is to maintain the DPMI stack, and know when to restore
    the original values when we pop back out to level zero.


--*/

{
    PUCHAR VdmStackPointer;
    ULONG NewSP;
    USHORT SegSs;
    BOOL bSsBig;

    SegSs = getSS();
    VdmStackPointer = Sim32GetVDMPointer(SegSs<<16, 1, TRUE);

    if (bSsBig = SEGMENT_IS_BIG(SegSs)) {
        VdmStackPointer += getESP();
    } else {
        VdmStackPointer += getSP();
    }

#line 798
#ifdef i386
#line 799
#error Excluded unsupported i386 kernel VDM interrupt return
#else
#line 822
    if (EndUseLockedPMStack()) {
        ULONG NewEIP, NewCS, NewEFLAGS;

        NewEIP    = getEIP();
        NewCS     = *(PDWORD16)(VdmStackPointer+4);
        NewEFLAGS = *(PDWORD16)(VdmStackPointer+8);


        if (!BuildStackFrame(3, &VdmStackPointer, &NewSP)) {
#line 831
#if DBG
#line 832
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif
#line 834
            DpmiFaultHandler(STACK_FAULT, 0);
            return;
        }

        //
        // SS has changed, so we need to check LDT again
        //
        if (SEGMENT_IS_BIG(getSS())) {
            setESP(NewSP);
        } else {
            setSP((WORD)NewSP);
        }

        *(PDWORD16)(VdmStackPointer-4) =  NewEFLAGS;
        *(PDWORD16)(VdmStackPointer-8) =  NewCS;
        *(PDWORD16)(VdmStackPointer-12) = NewEIP;
    }

    setCS(HIWORD(DosxIretd));
    setEIP((ULONG)LOWORD(DosxIretd));
#endif // i386

    #line 856
    DBGTRACE(DPMI_INT_IRET32, 0, 0, 0);
}

#ifndef i386

BOOL
#line 862
DpmiFaultHandler(
    ULONG IntNumber,
    ULONG ErrorCode
    )

/*++

Routine Description:

    This routine is called by the emulator when an exception occurs.

Arguments:

    IntNumber - exception number (0-1f)
    ErrorCode - exception error code to be placed on the stack

Return Value:

    TRUE if the interrupt was dispatched, FALSE otherwise

--*/

{
    PVDM_FAULTHANDLER Handlers = DpmiFaultHandlers;
    PUCHAR VdmStackPointer;
    ULONG SaveSS, SaveESP, SaveEFLAGS, SaveCS, SaveEIP;
    ULONG StackOffset;
    ULONG NewSP;

    DBGTRACE(DPMI_FAULT, IntNumber, ErrorCode, 0);

#line 893
    SaveSS = getSS();
    SaveESP = getESP();
    SaveEFLAGS = getEFLAGS();
    SaveEIP = getEIP();
    SaveCS  = getCS();
    /* Completed emulation owns the instruction's FLAGS. Only exception
     * dispatch clears IF/TF; successful I/O and CR0 moves do not. */
    if ((getMSW() & MSW_PE) && ((IntNumber == 13) || (IntNumber == 6)) &&
            DpmiEmulateInstruction()) {
        return TRUE;
    }
#line 898
    setEFLAGS(SaveEFLAGS & ~(EFLAGS_INTERRUPT_MASK | EFLAGS_TF_MASK));
#line 899

    if (!(getMSW() & MSW_PE)) {
        ReflectV86Int(IntNumber, getEFLAGS());
        return TRUE;
    }

#line 910

    if (!SEGMENT_IS_PRESENT(Handlers[IntNumber].CsSelector)) { 
#if defined(CPU_40_STYLE) && defined(CCPU)
        setEFLAGS(SaveEFLAGS);
#endif
#line 912
        return FALSE;
    }

    //
    // switch stacks
    //

    BeginUseLockedPMStack();

    //
    // Win31 has an undocumented feature of creating a 32byte area on the
    // stack. Krnl386 sticks stuff in there, so we emulate the behavior here.
    //

#line 926
    setESP(getESP()-0x20);

    //
    // allocate space on new stack
    //

    if (!BuildStackFrame(8, &VdmStackPointer, &NewSP)) {
        //BUGBUG Check for double fault
        EndUseLockedPMStack();
#if defined(CPU_40_STYLE) && defined(CCPU)
        /* EndUseLockedPMStack restores the outer stack only at depth 0. */
        setESP(SaveESP);
        setEFLAGS(SaveEFLAGS);
#endif
#line 935
        return FALSE;
    }
                                              
    if (Frame32) {
        *(PDWORD16)(VdmStackPointer-4) = SaveSS;
        *(PDWORD16)(VdmStackPointer-8) = SaveESP;
        *(PDWORD16)(VdmStackPointer-12) = SaveEFLAGS;
        *(PDWORD16)(VdmStackPointer-16) = SaveCS;
        *(PDWORD16)(VdmStackPointer-20) = SaveEIP;
        *(PDWORD16)(VdmStackPointer-24) = ErrorCode;
        *(PDWORD16)(VdmStackPointer-28) = (ULONG) HIWORD(DosxFaultHandlerIretd);
        *(PDWORD16)(VdmStackPointer-32) = (ULONG) LOWORD(DosxFaultHandlerIretd);
        setEIP(Handlers[IntNumber].Eip);
        setESP(NewSP);
    } else {
        *(PWORD16)(VdmStackPointer-2) = (WORD) SaveSS;
        *(PWORD16)(VdmStackPointer-4) = (WORD) SaveESP;
        *(PWORD16)(VdmStackPointer-6) = (WORD) SaveEFLAGS;
        *(PWORD16)(VdmStackPointer-8) = (WORD) SaveCS;
        *(PWORD16)(VdmStackPointer-10) = (WORD) SaveEIP;
        *(PWORD16)(VdmStackPointer-12) = (WORD) ErrorCode;
        *(PDWORD16)(VdmStackPointer-16) = DosxFaultHandlerIret;
        setEIP(LOWORD(Handlers[IntNumber].Eip));
        setSP((WORD)NewSP);
    }

#line 961
    setCS(Handlers[IntNumber].CsSelector);

#if DBG
#line 964
#error DIVERGENCE MVDM-HOST-DIV-516: DBG debug branch is not selected; see mirror register.
#endif

#line 973
    DBGTRACE(DPMI_DISPATCH_FAULT, IntNumber, 0, 0);
    return TRUE;
}

#endif // i386

VOID
#line 980
DpmiFaultHandlerIret16(
    VOID
    )

/*++

Routine Description:

    This routine is an IRET hook called via a BOP in dosx. It is called
    at the end of the execution of a 16-bit fault handler.


--*/

{
    PUCHAR VdmStackPointer;
    USHORT SegSs;

    SegSs = getSS();
    VdmStackPointer = Sim32GetVDMPointer(SegSs<<16, 1, TRUE);
    if (SEGMENT_IS_BIG(SegSs)) {
        VdmStackPointer += getESP();
    } else {
        VdmStackPointer += getSP();
    }

#line 1006
    EndUseLockedPMStack();

#line 1008
    setEIP((DWORD)*(PWORD16)(VdmStackPointer+2));
    setCS(*(PWORD16)(VdmStackPointer+4));
    setSTATUS(*(PWORD16)(VdmStackPointer+6));
    setSP(*(PWORD16)(VdmStackPointer+8));
    setSS(*(PWORD16)(VdmStackPointer+10));

    DBGTRACE(DPMI_FAULT_IRET, 0, 0, 0);
}

VOID
DpmiFaultHandlerIret32(
    VOID
    )

/*++

Routine Description:

    This routine is an IRET hook called via a BOP in dosx. It is called
    at the end of the execution of a 32-bit fault handler.


--*/

{
    PUCHAR VdmStackPointer;
    USHORT SegSs;

    SegSs = getSS();
    VdmStackPointer = Sim32GetVDMPointer(SegSs<<16, 1, TRUE);
    if (SEGMENT_IS_BIG(SegSs)) {
        VdmStackPointer += getESP();
    } else {
        VdmStackPointer += getSP();
    }


    EndUseLockedPMStack();

    setEIP(*(PDWORD16)(VdmStackPointer+4));
    setCS((USHORT)*(PDWORD16)(VdmStackPointer+8));
    setEFLAGS(*(PDWORD16)(VdmStackPointer+12));
    setESP(*(PDWORD16)(VdmStackPointer+16));
    setSS((USHORT)*(PWORD16)(VdmStackPointer+20));

    DBGTRACE(DPMI_FAULT_IRET, 0, 0, 0);
}


#line 1058
static BOOL
BuildStackFrameWithWidth(
#line 1059
    ULONG StackUnits,
    BOOL WideFrame,
#line 1060
    PUCHAR *pVdmStackPointer,
    ULONG *pNewSP
    )
/*++

Routine Description:

    This routine builds stack frames for the caller. It figures if it needs
    to use a 16 or 32-bit frame, and adjusts SP or ESP appropriately based
    on the number of "stack units". It also returns a flat pointer to the
    top of the frame to the caller.

Arguments:

    StackUnits = number of registers needed to be saved on the frame. For
                 example, 3 is how many elements there are on an iret frame
                 (flags, cs, ip)

Return Value:

    This function returns TRUE on success, FALSE on failure

    VdmStackPointer - flat address pointing to the "top" of the frame

Notes:

    BUGBUG This routine doesn't check for stack faults or 'UP' direction
           stacks
--*/

{
    USHORT SegSs;
    ULONG VdmSp;
    PUCHAR VdmStackPointer;
    ULONG StackOffset;
    ULONG Limit;
    ULONG SelIndex;
    ULONG NewSP;
    BOOL bExpandDown;

    SegSs = getSS();
    SelIndex = (SegSs & ~0x7)/sizeof(LDT_ENTRY);

    Limit = (ULONG) (Ldt[SelIndex].HighWord.Bits.LimitHi << 16) | 
                     Ldt[SelIndex].LimitLow;

    //
    // 
    // bugbug is this really correct?
#line 1109
    Limit++;
#line 1110
    if (Ldt[SelIndex].HighWord.Bits.Granularity) {
        Limit = (Limit << 12) | 0xfff;
    }


    if (Ldt[SelIndex].HighWord.Bits.Default_Big) {
        VdmSp = getESP();
    } else {
        VdmSp = getSP();
    }

#line 1122
    if (WideFrame) {
#line 1122
        StackOffset = StackUnits*sizeof(DWORD);
    } else {
        StackOffset = StackUnits*sizeof(WORD);
    }

    NewSP = VdmSp - StackOffset;
    bExpandDown = (BOOL) (Ldt[SelIndex].HighWord.Bits.Type & 4);
#line 1129
    if ((StackOffset > VdmSp) || 
        (!bExpandDown && (VdmSp > Limit)) ||
        (bExpandDown && (NewSP < Limit))) {
        // failed limit check
        return FALSE;
    }

    *pNewSP = NewSP;
    VdmStackPointer = Sim32GetVDMPointer(((ULONG)SegSs)<<16, 1, TRUE);
#line 1138
    VdmStackPointer += VdmSp;
    *pVdmStackPointer = VdmStackPointer;
    return(TRUE);

}

#line 1145
BOOL
BuildStackFrame(ULONG StackUnits, PUCHAR *pVdmStackPointer, ULONG *pNewSP)
{
    return BuildStackFrameWithWidth(StackUnits, Frame32, pVdmStackPointer, pNewSP);
}

#line 1144
VOID
#line 1145
EnableIntHooks(
    VOID
    )

/*++

Routine Description:

    This routine is called during startup to install our handlers with
    the emulator.

Arguments:

    None

Return Value:

    None.

--*/

{
#ifndef i386
#line 1168
    if (fDpmiHookInts) {
        VdmInstallHardwareIntHandler(DpmiHwIntHandler);
        VdmInstallSoftwareIntHandler(DpmiSwIntHandler);
        VdmInstallFaultHandler(DpmiFaultHandler);
        fDpmiIntsHaveBeenHooked = TRUE;
    }
#endif // i386
#line 1175
}


VOID
DisableIntHooks(
    VOID
    )

/*++

Routine Description:

    This routine is called to uninstall the our interrupt and exception
    handlers from the emulator.

Arguments:

    None

Return Value:

    None.

--*/

{
#ifndef i386
#line 1202
    if (fDpmiIntsHaveBeenHooked) {
        VdmInstallHardwareIntHandler(NULL);
        VdmInstallSoftwareIntHandler(NULL);
        VdmInstallFaultHandler(NULL);
        fDpmiIntsHaveBeenHooked = FALSE;
    }
#endif // i386
#line 1209
}


BOOL
CheckEIP(
    ULONG Increment
    )
/*++

Routine Description:

    This routine does a limit check on EIP.

Arguments:

    None

Return Value:

    TRUE if EIP is ok, FALSE otherwise

--*/

{
    ULONG Offset = (c_getCS_AR() & 0x4000) ? getEIP() : getIP();
    ULONG Limit = c_getCS_LIMIT();
    return Offset <= Limit && Increment <= Limit - Offset;
}

#ifndef i386
#if defined(CPU_40_STYLE) && defined(CCPU)
/* Fetch through the active CCPU CS cache and its page translation. A failed
 * lookup declines emulation; it must not read an unrelated contiguous alias. */
static BOOL
DpmiMappedCodeByte(ULONG Base, ULONG Offset, PUCHAR Value)
{
    extern int xtrn2phy(unsigned long, unsigned char, unsigned long *);
    extern unsigned char *c_GetPhyAdd(unsigned long);
    ULONG physical = Base + Offset;

    if (Offset > c_getCS_LIMIT() ||
        ((getCR0() & 0x80000000UL) &&
         !xtrn2phy(physical, c_getCPL() == 3 ? 6 : 4, &physical)))
        return FALSE;
    /* Device memory is not a host instruction alias. */
    if (physical >= 0xa0000UL && physical <= 0xbffffUL)
        return FALSE;
    *Value = *c_GetPhyAdd(physical);
    return TRUE;
}

static BOOL
DpmiNextCodeByte(ULONG Offset, PULONG Length, PUCHAR Value)
{
    if (*Length >= 15 || !CheckEIP(*Length) ||
        !DpmiMappedCodeByte(c_getCS_BASE(), Offset + *Length, Value))
        return FALSE;
    ++*Length;
    return TRUE;
}
#endif
#line 1238
BOOL
DpmiEmulateInstruction(
    VOID
    )
/*++

Routine Description:

    This routine checks to see if the instruction which caused the
    fault really needs to be emulated. For example, the MS C compiler (v7.00)
    uses instructions to manipulate the FP flags in CR0. The compiler
    expects them to just work as they do on win31, which also emulates them.

Arguments:

    None

Return Value:

    TRUE if the instruction was emulated, FALSE otherwise

--*/

{
    PUCHAR pCode;
    UCHAR Opcode, Operands[2];
    ULONG SegCS = getCS(), Length = 0;
    BOOL Big = (c_getCS_AR() & 0x4000) != 0, Operand32 = Big;
    ULONG Offset = Big ? getEIP() : getIP();
#line 1265
    BOOL bReturn = FALSE;

    /* Only DPMI protected-mode exceptions reach this service. Prefixes do
     * not acquire side effects; all operands are validated before dispatch. */
    if (!(getMSW() & MSW_PE) || c_getVM())
        return FALSE;
    for (;;) {
        if (!DpmiNextCodeByte(Offset, &Length, &Opcode))
            return FALSE;
        switch (Opcode) {
        case 0x66: Operand32 = !Big; continue;
        case 0x67: case 0x26: case 0x2e: case 0x36: case 0x3e:
        case 0x64: case 0x65: case 0xf2: case 0xf3: continue;
        case 0xf0: return FALSE; /* LOCK is invalid for these instructions. */
        }
        break;
    }
    switch (Opcode) {
        case 0xfa:
        case 0xfb:
        {
            extern unsigned char *c_GetPhyAdd(unsigned long);
            volatile LONG *State = (volatile LONG *)c_GetPhyAdd(FIXED_NTVDMSTATE_LINEAR);
            if (Opcode == 0xfb)
                InterlockedOr(State, VIRTUAL_INTERRUPT_BIT);
            else
                InterlockedAnd(State, ~VIRTUAL_INTERRUPT_BIT);
            setIF(Opcode == 0xfb);
            bReturn = TRUE;
            break;
        }
        case 0xe4: case 0xe5: case 0xe6: case 0xe7:
        case 0xec: case 0xed: case 0xee: case 0xef:
        {
            ULONG Port = getDX(), Value;
            if (Opcode < 0xe8) {
                if (!DpmiNextCodeByte(Offset, &Length, &Operands[0]))
                    return FALSE;
                Port = Operands[0];
            }
            switch (Opcode & 3) {
            case 0: IN8(&Value, Port); setAL((UCHAR)Value); break;
            case 1:
                if (Operand32) { IN32(&Value, Port); setEAX(Value); }
                else { IN16(&Value, Port); setAX((USHORT)Value); }
                break;
            case 2: OUT8(Port, getAL()); break;
            case 3:
                if (Operand32) OUT32(Port, getEAX());
                else OUT16(Port, getAX());
                break;
            }
            bReturn = TRUE;
            break;
        }
        case 0xf:
            if (!DpmiNextCodeByte(Offset, &Length, &Operands[0]) ||
                (Operands[0] != 0x20 && Operands[0] != 0x22) ||
                !DpmiNextCodeByte(Offset, &Length, &Operands[1]))
                break;
#line 1282
            bReturn = DpmiOp0f(Operands);
            break;

        case 0x8e:
            //
            // This is WIN31 compatibility. If we are trying to dispatch
            // the client, and we get a fault loading the segment registers,
            // then zero them out.
            // BUGBUG currently only looking for FS, GS
            //
            if (!DpmiNextCodeByte(Offset, &Length, &Operands[0]))
                break;
#line 1295
            //
            // Look for code in dxutil.asm EnterProtectedMode
            //
#line 1298
            if ((SegCS == DosxRmCodeSelector) &&
                ((Operands[0] == 0xe0) ||          // mov fs, ax
                 (Operands[0] == 0xe8))            // mov gs, ax
                    ) {
#line 1303
                bReturn = TRUE;
            }
            break;
    }

#line 1308
    DBGTRACE(DPMI_OP_EMULATION, Opcode, (ULONG) bReturn, 0);
    if (bReturn) {
        setEIP(Offset + Length);
        /* Like native CCPU STI, delay interrupt delivery by one instruction. */
        if (Opcode != 0xfb)
            ccpu386CompleteInstruction();
    }
#line 1309
    return bReturn;
}

#define MI_GET_CRx_OPCODE 0x20
#define MI_SET_CRx_OPCODE 0x22
#define MI_MODMASK 0xC0
#define MI_MODMOVSPEC 0xC0
#define MI_REGMASK 0x38
#define MI_RMMASK  0x7

BOOL
DpmiOp0f(
    PUCHAR pCode
    )
/*++

Routine Description:

    This routine emulates instructions that have 0x0F as the first byte.

Arguments:

    None

Return Value:

    TRUE if the instruction was emulated, FALSE otherwise

--*/
{
    ULONG Value;

    switch (*pCode++) {
        case MI_GET_CRx_OPCODE:

            if (!CheckEIP(2)) {
                break;
            }

            if ((*pCode & MI_MODMASK) != MI_MODMOVSPEC) {
                break;
            }

            if (*pCode & MI_REGMASK) {
                Value = 0;              // not CR0
            } else {
                Value = getCR0();
            }

            SetRegisterByIndex[*pCode & MI_RMMASK](Value);
            setEIP(getEIP()+3);
            return TRUE;

        case MI_SET_CRx_OPCODE:

            if (!CheckEIP(2)) {
                break;
            }

            if ((*pCode & MI_MODMASK) != MI_MODMOVSPEC) {
                break;
            }

            if (*pCode & MI_REGMASK) {
                break;                  // not CR0
            }

            setCR0(GetRegisterByIndex[*pCode & MI_RMMASK]());
            setEIP(getEIP()+3);
            return TRUE;
    }

    return FALSE;
}
#endif
