/*
 *  excptn.c - Exception functions of DBG DLL.
 *
 */
#include <precomp.h>
#pragma hdrstop



BOOL DbgGPFault2(
    PFFRAME16   pFFrame
    )
/*
    2nd chance GPFault handler (called via BOP)
*/
{
    BOOL            fResult;

    fResult = FALSE;        // Default to Event not handled

    DbgGetContext();

    vcContext.SegEs = (ULONG)pFFrame->wES;
    vcContext.SegDs = (ULONG)pFFrame->wDS;
    vcContext.SegCs = (ULONG)pFFrame->wCS;
    vcContext.SegSs = (ULONG)pFFrame->wSS;

#ifdef i386
    //
    // On x86 systems, we really might have some data in the high words
    // of these registers.  Hopefully DOSX.EXE and KRNL286.EXE don't
    // blow them away.  Here is where we attempt to recover them.
    //
    vcContext.Edi    = MAKELONG(pFFrame->wDI,   HIWORD(getEDI()));
    vcContext.Esi    = MAKELONG(pFFrame->wSI,   HIWORD(getESI()));
    vcContext.Ebx    = MAKELONG(pFFrame->wBX,   HIWORD(getEBX()));
    vcContext.Edx    = MAKELONG(pFFrame->wDX,   HIWORD(getEDX()));
    vcContext.Ecx    = MAKELONG(pFFrame->wCX,   HIWORD(getECX()));
    vcContext.Eax    = MAKELONG(pFFrame->wAX,   HIWORD(getEAX()));

    vcContext.Ebp    = MAKELONG(pFFrame->wBP,   HIWORD(getEBP()));
    vcContext.Eip    = MAKELONG(pFFrame->wIP,   HIWORD(getEIP()));
    vcContext.Esp    = MAKELONG(pFFrame->wSP,   HIWORD(getESP()));
    vcContext.EFlags = MAKELONG(pFFrame->wFlags,HIWORD(getEFLAGS()));
#else
    vcContext.Edi    = (ULONG)pFFrame->wDI;
    vcContext.Esi    = (ULONG)pFFrame->wSI;
    vcContext.Ebx    = (ULONG)pFFrame->wBX;
    vcContext.Edx    = (ULONG)pFFrame->wDX;
    vcContext.Ecx    = (ULONG)pFFrame->wCX;
    vcContext.Eax    = (ULONG)pFFrame->wAX;

    vcContext.Ebp    = (ULONG)pFFrame->wBP;
    vcContext.Eip    = (ULONG)pFFrame->wIP;
    vcContext.Esp    = (ULONG)pFFrame->wSP;
    vcContext.EFlags = (ULONG)pFFrame->wFlags;

#endif

    if ( fDebugged ) {
        fResult = SendVDMEvent(DBG_GPFAULT2);

        if ( !fResult ) {
            DWORD dw;

            dw = SetErrorMode(0);
            try {
                RaiseException((DWORD)DBG_CONTROL_BREAK, 0, 0, (LPDWORD)0);
                fResult = TRUE;
            } except (EXCEPTION_EXECUTE_HANDLER) {
                fResult = FALSE;
            }
            SetErrorMode(dw);
        }

    } else {
        char    text[100];

        // Dump a simulated context

        OutputDebugString("NTVDM:GP Fault detected, register dump follows:\n");

        wsprintf(text,"eax=%08lx ebx=%08lx ecx=%08lx edx=%08lx esi=%08lx edi=%08lx\n",
            vcContext.Eax,
            vcContext.Ebx,
            vcContext.Ecx,
            vcContext.Edx,
            vcContext.Esi,
            vcContext.Edi  );
        OutputDebugString(text);

        wsprintf(text,"eip=%08lx esp=%08lx ebp=%08lx iopl=%d         %s %s %s %s %s %s %s %s\n",
            vcContext.Eip,
            vcContext.Esp,
            vcContext.Ebp,
            (vcContext.EFlags & V86FLAGS_IOPL) >> V86FLAGS_IOPL_BITS,
            (vcContext.EFlags & V86FLAGS_OVERFLOW ) ? "ov" : "nv",
            (vcContext.EFlags & V86FLAGS_DIRECTION) ? "dn" : "up",
            (vcContext.EFlags & V86FLAGS_INTERRUPT) ? "ei" : "di",
            (vcContext.EFlags & V86FLAGS_SIGN     ) ? "ng" : "pl",
            (vcContext.EFlags & V86FLAGS_ZERO     ) ? "zr" : "nz",
            (vcContext.EFlags & V86FLAGS_AUXCARRY ) ? "ac" : "na",
            (vcContext.EFlags & V86FLAGS_PARITY   ) ? "po" : "pe",
            (vcContext.EFlags & V86FLAGS_CARRY    ) ? "cy" : "nc" );
        OutputDebugString(text);

        wsprintf(text,"cs=%04x  ss=%04x  ds=%04x  es=%04x  fs=%04x  gs=%04x             efl=%08lx\n",
            (WORD)vcContext.SegCs,
            (WORD)vcContext.SegSs,
            (WORD)vcContext.SegDs,
            (WORD)vcContext.SegEs,
            (WORD)vcContext.SegFs,
            (WORD)vcContext.SegGs,
            vcContext.EFlags );
        OutputDebugString(text);
    }

#ifdef i386
    //
    // On x86 systems, we really might have some data in the FS and GS
    // registers.  Hopefully DOSX.EXE and KRNL286.EXE don't
    // blow them away.  Here is where we attempt to restore them.
    //
    setGS((WORD)vcContext.SegGs);
    setFS((WORD)vcContext.SegFs);
#else
    // No need to set FS,GS, they don't exist
#endif

    pFFrame->wES = (WORD)vcContext.SegEs;
    pFFrame->wDS = (WORD)vcContext.SegDs;
    pFFrame->wCS = (WORD)vcContext.SegCs;
    pFFrame->wSS = (WORD)vcContext.SegSs;

#ifdef i386
    //
    // On x86 systems, we really might have some data in the high words
    // of these registers.  Hopefully DOSX.EXE and KRNL286.EXE don't
    // blow them away.  Here is where we attempt to restore them.
    //
    pFFrame->wDI = LOWORD(vcContext.Edi);
    setEDI(MAKELONG(LOWORD(getEDI()),HIWORD(vcContext.Edi)));

    pFFrame->wSI = LOWORD(vcContext.Esi);
    setESI(MAKELONG(LOWORD(getESI()),HIWORD(vcContext.Esi)));

    pFFrame->wBX = LOWORD(vcContext.Ebx);
    setEBX(MAKELONG(LOWORD(getEBX()),HIWORD(vcContext.Ebx)));

    pFFrame->wDX = LOWORD(vcContext.Edx);
    setEDX(MAKELONG(LOWORD(getEDX()),HIWORD(vcContext.Edx)));

    pFFrame->wCX = LOWORD(vcContext.Ecx);
    setECX(MAKELONG(LOWORD(getECX()),HIWORD(vcContext.Ecx)));

    pFFrame->wAX = LOWORD(vcContext.Eax);
    setEAX(MAKELONG(LOWORD(getEAX()),HIWORD(vcContext.Eax)));

    pFFrame->wBP = LOWORD(vcContext.Ebp);
    setEBP(MAKELONG(LOWORD(getEBP()),HIWORD(vcContext.Ebp)));

    pFFrame->wIP = LOWORD(vcContext.Eip);
    setEIP(MAKELONG(LOWORD(getEIP()),HIWORD(vcContext.Eip)));

    pFFrame->wFlags = LOWORD(vcContext.EFlags);
    setEFLAGS(MAKELONG(LOWORD(getEFLAGS()),HIWORD(vcContext.EFlags)));

    pFFrame->wSP = LOWORD(vcContext.Esp);
    setESP(MAKELONG(LOWORD(getESP()),HIWORD(vcContext.Esp)));
#else
    pFFrame->wDI = (WORD)vcContext.Edi;
    pFFrame->wSI = (WORD)vcContext.Esi;
    pFFrame->wBX = (WORD)vcContext.Ebx;
    pFFrame->wDX = (WORD)vcContext.Edx;
    pFFrame->wCX = (WORD)vcContext.Ecx;
    pFFrame->wAX = (WORD)vcContext.Eax;


    pFFrame->wBP = (WORD)vcContext.Ebp;
    pFFrame->wIP = (WORD)vcContext.Eip;
    pFFrame->wFlags = (WORD)vcContext.EFlags;
    pFFrame->wSP = (WORD)vcContext.Esp;
#endif

    return( fResult );
}

BOOL DbgDivOverflow2(
    PTFRAME16   pTFrame
    )
/*
    2nd chance divide exception handler
*/
{
    BOOL        fResult;

    fResult = FALSE;        // Default to Event not handled

    if ( fDebugged ) {

        DbgGetContext();

        vcContext.SegDs = (ULONG)pTFrame->wDS;
        vcContext.SegCs = (ULONG)pTFrame->wCS;
        vcContext.SegSs = (ULONG)pTFrame->wSS;

#ifdef i386
        //
        // On x86 systems, we really might have some data in the high words
        // of these registers.  Hopefully DOSX.EXE and KRNL286.EXE don't
        // blow them away.  Here is where we attempt to recover them.
        //
        vcContext.Eax    = MAKELONG(pTFrame->wAX,   HIWORD(getEAX()));
        vcContext.Eip    = MAKELONG(pTFrame->wIP,   HIWORD(getEIP()));
        vcContext.Esp    = MAKELONG(pTFrame->wSP,   HIWORD(getESP()));
        vcContext.EFlags = MAKELONG(pTFrame->wFlags,HIWORD(getEFLAGS()));
#else
        vcContext.Eax    = (ULONG)pTFrame->wAX;

        vcContext.Eip    = (ULONG)pTFrame->wIP;
        vcContext.Esp    = (ULONG)pTFrame->wSP;
        vcContext.EFlags = (ULONG)pTFrame->wFlags;

#endif

        fResult = SendVDMEvent(DBG_DIVOVERFLOW);

#ifdef i386
        //
        // On x86 systems, we really might have some data in the FS and GS
        // registers.  Hopefully DOSX.EXE and KRNL286.EXE don't
        // blow them away.  Here is where we attempt to restore them.
        //
        setGS((WORD)vcContext.SegGs);
        setFS((WORD)vcContext.SegFs);
#else
        // No need to set FS,GS, they don't exist
#endif

        setES( (WORD)vcContext.SegEs );
        pTFrame->wDS = (WORD)vcContext.SegDs;
        pTFrame->wCS = (WORD)vcContext.SegCs;
        pTFrame->wSS = (WORD)vcContext.SegSs;

#ifdef i386
        //
        // On x86 systems, we really might have some data in the high words
        // of these registers.  Hopefully DOSX.EXE and KRNL286.EXE don't
        // blow them away.  Here is where we attempt to restore them.
        //
        setEDI( vcContext.Edi );
        setESI( vcContext.Esi );
        setEBX( vcContext.Ebx );
        setEDX( vcContext.Edx );
        setECX( vcContext.Ecx );

        pTFrame->wAX = LOWORD(vcContext.Eax);
        setEAX(MAKELONG(LOWORD(getEAX()),HIWORD(vcContext.Eax)));

        setEBP( vcContext.Ebp );

        pTFrame->wIP = LOWORD(vcContext.Eip);
        setEIP(MAKELONG(LOWORD(getEIP()),HIWORD(vcContext.Eip)));

        pTFrame->wFlags = LOWORD(vcContext.EFlags);
        setEFLAGS(MAKELONG(LOWORD(getEFLAGS()),HIWORD(vcContext.EFlags)));

        pTFrame->wSP = LOWORD(vcContext.Esp);
        setESP(MAKELONG(LOWORD(getESP()),HIWORD(vcContext.Esp)));
#else
        setDI( (WORD)vcContext.Edi );
        setSI( (WORD)vcContext.Esi );
        setBX( (WORD)vcContext.Ebx );
        setDX( (WORD)vcContext.Edx );
        setCX( (WORD)vcContext.Ecx );
        pTFrame->wAX = (WORD)vcContext.Eax;


        setBP( (WORD)vcContext.Ebp );
        pTFrame->wIP    = (WORD)vcContext.Eip;
        pTFrame->wFlags = (WORD)vcContext.EFlags;
        pTFrame->wSP    = (WORD)vcContext.Esp;
#endif


    }

    return( fResult );
}



BOOL
WINAPI
xxxDbgFault(
    ULONG IntNumber
    )
/*
    This is the first chance exception handler. It is called by dpmi32
*/

{
    ULONG           vdmEip;
    int             i;
    PBYTE           lpInst;
    BOOL            fResult = FALSE;


    if ( fDebugged ) {

        switch(IntNumber) {
        case 6:
            //BUGBUG: We *could* handle these, but people might be confused by
            // the fact that krnl386 does an intentional opcode exception.
//            GetNormalContext( &vcContext, &viInfo, EventParams, DBG_INSTRFAULT, PX86 );
            break;

        case 12:
            if (*(ULONG *)(IntelMemoryBase+FIXED_NTVDMSTATE_LINEAR) & VDM_BREAK_EXCEPTIONS) {
                DbgGetContext();
                fResult = SendVDMEvent(DBG_STACKFAULT);
            }
            break;

        case 13:
            if (*(ULONG *)(IntelMemoryBase+FIXED_NTVDMSTATE_LINEAR) & VDM_BREAK_EXCEPTIONS) {
                DbgGetContext();
                fResult = SendVDMEvent(DBG_GPFAULT);
            }
            break;

        default:
            return FALSE;
        }
    }

    return fResult;

}


