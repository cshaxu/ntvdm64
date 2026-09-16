/*++

Copyright (c) 1992  Microsoft Corporation

Module Name:

    dpmiint.h

Abstract:

    This is the private include file for the 32 bit dpmi and protected mode
    support

Author:

    Neil Sandlin (neilsa) 31-Jul-1995

Revision History:

--*/


/* The original i386 branch delegates these carriers to the kernel VDM
 * monitor. CPU40 is a software executor even when hosted by an x86 build,
 * so it retains the original non-monitor carrier ownership. */
#if !defined(i386) || defined(CPU_40_STYLE)

GETREGISTERFUNCTION GetRegisterByIndex[8] = {getEAX, getECX, getEDX, getECX,
                                             getESP, getEBP, getESI, getEDI};
SETREGISTERFUNCTION SetRegisterByIndex[8] = {setEAX, setECX, setEDX, setECX,
                                             setESP, setEBP, setESI, setEDI};

VDM_INTERRUPTHANDLER DpmiInterruptHandlers[256] = {0};
VDM_FAULTHANDLER DpmiFaultHandlers[32] = {0};
BOOL fDpmiHookInts = TRUE;
BOOL fDpmiIntsHaveBeenHooked = FALSE;

#define VDM_INT_INT_GATE 1
#define EFLAGS_TF_MASK 0x100
#define VDM_INT_32 2

#else

#define DpmiInterruptHandlers ((PVDM_TIB)(NtCurrentTeb()->Vdm))->VdmInterruptHandlers
#define DpmiFaultHandlers ((PVDM_TIB)(NtCurrentTeb()->Vdm))->VdmFaultHandlers


#endif // !i386 || CPU_40_STYLE

#define EFLAGS_INTERRUPT_MASK 0x200
#define LockedPMStackOffset 0x1000
#define STACK_FAULT 12
#define Frame32 ((BOOL)CurrentAppFlags)


BOOL
DpmiFaultHandler(
    ULONG IntNumber,
    ULONG ErrorCode
    );


BOOL
BuildStackFrame(
    ULONG StackOffset,
    PUCHAR *VdmStackPointer,
    ULONG *pNewSP
    );

BOOL
DpmiEmulateInstruction(
    VOID
    );

BOOL
DpmiOp0f(
    PUCHAR pCode
    );
