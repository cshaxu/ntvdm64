/*
 *  dbg.c - Main Module of DBG DLL.
 *
 *  BobDay 13-Jan-1992 Created
 *
 * WARNING: THIS MODULE APPEARS TO HAVE MANY FUNCTIONS THAT APPEAR IDENTICAL
 *          THEY ARE NOT IDENTICAL.  THEY ARE NEARLY IDENTICAL.  IF YOU CHOOSE
 *          TO COMBINE SIMILAR FUNCTIONALITY INTO SEVERAL SMALLER ROUTINES,
 *          BE VERY CAREFUL TO LOOK FOR THE SMALL SUBTLE DIFFERENCES.
 */

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <ntdbg.h>
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <mvdm.h>
#include <bop.h>
#include <softpc.h>
#include <dbgexp.h>
#include <dbgsvc.h>
#include <vdmdbg.h>
#include <dbginfo.h>

#define MAX_MODULE  64

#define MAX_DBG_FRAME   10

ULONG   ulTHHOOK = 0L;          // Address registered from 16-bit world
LPVOID  lpRemoteAddress = NULL; // Address registered from WOW32
DWORD   lpRemoteBlock   = 0;    // Address registered from WOW32
BOOL    f386;

typedef struct _trapframe {
    WORD    wCode;          /* Noise from DbgDispatchBop */
    WORD    wAX;            /* AX at time of fault */
    WORD    wDS;            /* DS at time of fault */
    WORD    wRetIP;         /* Noise from DPMI */
    WORD    wRetCS;         /* Noise from DPMI */
    WORD    wErrCode;       /* Noise from 16-bit kernel */
    WORD    wIP;            /* IP at time of fault */
    WORD    wCS;            /* CS at time of fault */
    WORD    wFlags;         /* Flags at time of fault */
    WORD    wSP;            /* SS at time of fault */
    WORD    wSS;            /* SP at time of fault */
} TFRAME16;
typedef TFRAME16 UNALIGNED *PTFRAME16;

typedef struct _faultframe {
    WORD    wES;            /* ES at time of fault */
    WORD    wDS;            /* DS at time of fault */
    WORD    wDI;            /* DI at time of fault */
    WORD    wSI;            /* SI at time of fault */
    WORD    wTempBP;        /* Noise from 16-bit kernel stack frame */
    WORD    wTempSP;        /* Noise from 16-bit kernel stack frame */
    WORD    wBX;            /* BX at time of fault */
    WORD    wDX;            /* DX at time of fault */
    WORD    wCX;            /* CX at time of fault */
    WORD    wAX;            /* AX at time of fault */
    WORD    wBP;            /* BP at time of fault */
    WORD    npszMsg;        /* Noise from 16-bit kernel */
    WORD    wPrevIP;        /* Noise from DPMI */
    WORD    wPrevCS;        /* Noise from DPMI */
    WORD    wRetIP;         /* Noise from DPMI */
    WORD    wRetCS;         /* Noise from DPMI */
    WORD    wErrCode;       /* Noise from 16-bit kernel */
    WORD    wIP;            /* IP at time of fault */
    WORD    wCS;            /* CS at time of fault */
    WORD    wFlags;         /* Flags at time of fault */
    WORD    wSP;            /* SS at time of fault */
    WORD    wSS;            /* SP at time of fault */
} FFRAME16;
typedef FFRAME16 UNALIGNED *PFFRAME16;

typedef struct _newtaskframe {
    DWORD   dwNoise;            /* Noise from InitTask         */
    DWORD   dwModulePath;       /* Module path address         */
    DWORD   dwModuleName;       /* Module name address         */
    WORD    hModule;            /* 16-bit Module handle        */
    WORD    hTask;              /* 16-bit Task handle          */
    WORD    wFlags;             /* Flags at time to task start */
    WORD    wDX;                /* DX at time of task start    */
    WORD    wBX;                /* BX at time of task start    */
    WORD    wES;                /* ES at time of task start    */
    WORD    wCX;                /* CX at time of task start    */
    WORD    wAX;                /* AX at time of task start    */
    WORD    wDI;                /* DI at time of task start    */
    WORD    wSI;                /* SI at time of task start    */
    WORD    wDS;                /* DS at time of task start    */
    WORD    wBP;                /* BP at time of task start    */
    WORD    wIP;                /* IP for task start           */
    WORD    wCS;                /* CS for task start           */
} NTFRAME16;
typedef NTFRAME16 UNALIGNED *PNTFRAME16;

#pragma pack(2)

typedef struct _stoptaskframe {
    WORD    wCode;              /* Noise from BOP Dispatcher  */
    DWORD   dwModulePath;       /* Module path address        */
    DWORD   dwModuleName;       /* Module name address        */
    WORD    hModule;            /* 16-bit Module handle       */
    WORD    hTask;              /* 16-bit Task handle         */
} STFRAME16;
typedef STFRAME16 UNALIGNED *PSTFRAME16;

typedef struct _newdllframe {
    WORD    wCode;              /* Noise from DbgDispatchBop  */
    DWORD   dwModulePath;       /* Module path address        */
    DWORD   dwModuleName;       /* Module name address        */
    WORD    hModule;            /* 16-bit Module handle       */
    WORD    hTask;              /* 16-bit Task handle         */
    WORD    wDS;                /* DS at time of dll start    */
    WORD    wAX;                /* AX at time of dll start    */
    WORD    wIP;                /* IP at time of dll start    */
    WORD    wCS;                /* CS at time of dll start    */
    WORD    wFlags;             /* Flags at time of dll start */
} NDFRAME16;
typedef NDFRAME16 UNALIGNED *PNDFRAME16;

#pragma pack()


BOOL  fDebugged = FALSE;

/* DBGInit - DBG Initialiazation routine. (This name may change when DBG is
 *	     converted to DLL).
 *
 * Entry
 *	None
 *
 * Exit
 *	None
 */

BOOL DBGInit (int argc, char *argv[])
{
    HANDLE      hProcess;
    HANDLE      MyDebugPort;
    DWORD       st;

    hProcess = NtCurrentProcess();

    st = NtQueryInformationProcess(
                hProcess,
                ProcessDebugPort,
                (PVOID)&MyDebugPort,
                sizeof(MyDebugPort),
                NULL );
    if ( NT_SUCCESS(st) ) {
        fDebugged = (MyDebugPort != NULL);
    } else {
        fDebugged = FALSE;
    }
    return( TRUE );
}


BOOL SendVDMEvent(
    LPDWORD lpParams
) {
    BOOL    fResult;

    // Slimyness to determine whether the exception was handled or not.

    try {
        RaiseException( STATUS_VDM_EVENT,
                        0,
                        4,
                        lpParams );
        fResult = TRUE;
    } except(EXCEPTION_EXECUTE_HANDLER) {
        fResult = FALSE;
    }
    return( fResult );
}


/* GetNormalContext() - Get the VDM's current context
 *
 * Most of the routines that send VDMEvents need to have a context record
 * associated with them.  This routine is a quick way to get most of the
 * general registers.  Redundant work is being done because AX for example
 * is often on the stack and as such must really be pulled from the frame.
 * Hopefully this is OK because it is fast.
 */
#ifdef i386
#error DIVERGENCE MVDM-HOST-DIV-588: historical i386 debugger branch is not selected
#else
#line 192
#define PX86    0
#endif
#line 194

VOID GetNormalContext(
    LPVDMCONTEXT        lpvcContext,
    LPVDMINTERNALINFO   lpviInfo,
    LPDWORD             lpdwExceptionParams,
    WORD                wEventType,
#ifdef i386
#error DIVERGENCE MVDM-HOST-DIV-588: historical i386 debugger branch is not selected
#else
#line 203
    DWORD               dw
#endif
#line 205
) {
    //
    // Everything defaults to 0.
    //
    RtlFillMemory( lpvcContext, sizeof(VDMCONTEXT), (UCHAR)0 );
    RtlFillMemory( lpviInfo, sizeof(VDMINTERNALINFO), (UCHAR)0 );
    RtlFillMemory( lpdwExceptionParams, 4*sizeof(DWORD), (UCHAR)0 );

    //
    // Fill in the exception parameters
    //
    lpdwExceptionParams[0] = MAKELONG( wEventType, 0 );
    lpdwExceptionParams[3] = (DWORD)lpviInfo;

    //
    // Fill in the internal info structure
    //
    lpviInfo->wKernelSeg      = HIWORD(ulTHHOOK);
    lpviInfo->dwOffsetTHHOOK  = (DWORD)(LOWORD(ulTHHOOK));
    lpviInfo->vdmContext      = lpvcContext;
    lpviInfo->lpRemoteAddress = lpRemoteAddress;
    lpviInfo->lpRemoteBlock   = lpRemoteBlock;
    lpviInfo->f386            = f386;

    //
    // Fill in the context structure
    //
    lpvcContext->SegEs = (ULONG)getES();
    lpvcContext->SegDs = (ULONG)getDS();
    lpvcContext->SegCs = (ULONG)getCS();
    lpvcContext->SegSs = (ULONG)getSS();

    lpvcContext->EFlags = 0;            // BUGBUG - We need the 16-bit's Flags

#ifdef i386
#error DIVERGENCE MVDM-HOST-DIV-588: historical i386 debugger branch is not selected
#else
#line 266
    // Leave GS & FS zero.

    lpvcContext->Edi = (ULONG)getDI();
    lpvcContext->Esi = (ULONG)getSI();
    lpvcContext->Ebx = (ULONG)getBX();
    lpvcContext->Edx = (ULONG)getDX();
    lpvcContext->Ecx = (ULONG)getCX();
    lpvcContext->Eax = (ULONG)getAX();

    lpvcContext->Ebp = (ULONG)getBP();
    lpvcContext->Eip = (ULONG)getIP();
    lpvcContext->Esp = (ULONG)getSP();

    if ( (getMSW() & MSW_PE) == 0 ) {
        lpvcContext->EFlags |= V86FLAGS_V86;
    }

    // BUGBUG initialize the GDT/LDT stuff for the emulator
#endif
#line 285
}

void SegmentLoad(
    LPSTR   lpModuleName,
    LPSTR   lpPathName,
    WORD    Selector,
    WORD    Segment,
    BOOL    fData

) {

    if ( fDebugged ) {
        DWORD           EventParams[4];
        VDMCONTEXT      vcContext;
        VDMINTERNALINFO viInfo;
        SEGMENT_NOTE    se;
        UINT            length;
#ifdef i386
#error DIVERGENCE MVDM-HOST-DIV-588: historical i386 debugger branch is not selected
#endif
#line 308

        GetNormalContext( &vcContext, &viInfo, EventParams, DBG_SEGLOAD, PX86 );

        RtlFillMemory( &se, sizeof(se), (UCHAR)0 );

        se.Selector1 = Selector;
        se.Segment   = Segment;
        se.Type      = fData;

        length = strlen(lpPathName);
        if ( length >= MAX_PATH16 ) {
            length = MAX_PATH16-1;
        }
        strncpy( se.FileName, lpPathName, length );
        se.FileName[length] = '\0';

        length = strlen(lpModuleName);
        if ( length >= MAX_MODULE ) {
            length = MAX_MODULE-1;
        }
        strncpy( se.Module, lpModuleName, length );
        se.Module[length] = '\0';

        EventParams[2] = (DWORD)&se;

        SendVDMEvent( EventParams );
    }
}

void SegmentMove(
    WORD    OldSelector,
    WORD    NewSelector
) {

    if ( fDebugged ) {
        DWORD           EventParams[4];
        VDMCONTEXT      vcContext;
        VDMINTERNALINFO viInfo;
        SEGMENT_NOTE    se;
#ifdef i386
#error DIVERGENCE MVDM-HOST-DIV-588: historical i386 debugger branch is not selected
#endif
#line 353

        GetNormalContext( &vcContext, &viInfo, EventParams, DBG_SEGMOVE, PX86 );

        RtlFillMemory( &se, sizeof(se), (UCHAR)0 );

        se.Selector1   = OldSelector;
        se.Selector2   = NewSelector;

        EventParams[2] = (DWORD)&se;

        SendVDMEvent( EventParams );
    }
}

void SegmentFree(
    WORD    Selector,
    BOOL    fBPRelease
) {

    if ( fDebugged ) {
        DWORD           EventParams[4];
        VDMCONTEXT      vcContext;
        VDMINTERNALINFO viInfo;
        SEGMENT_NOTE    se;
#ifdef i386
#error DIVERGENCE MVDM-HOST-DIV-588: historical i386 debugger branch is not selected
#endif
#line 383

        GetNormalContext( &vcContext, &viInfo, EventParams, DBG_SEGFREE, PX86 );

        RtlFillMemory( &se, sizeof(se), (UCHAR)0 );

        se.Selector1   = Selector;
        se.Type        = (WORD)fBPRelease;

        EventParams[2] = (DWORD)&se;

        SendVDMEvent( EventParams );
    }
}

void ModuleLoad(
    LPSTR   lpModuleName,
    LPSTR   lpPathName,
    WORD    Segment,
    DWORD   Length
) {

    if ( fDebugged ) {
        DWORD           EventParams[4];
        VDMCONTEXT      vcContext;
        VDMINTERNALINFO viInfo;
        SEGMENT_NOTE    se;
        UINT            length;
#ifdef i386
#error DIVERGENCE MVDM-HOST-DIV-588: historical i386 debugger branch is not selected
#endif
#line 416

        GetNormalContext( &vcContext, &viInfo, EventParams, DBG_MODLOAD, PX86 );

        RtlFillMemory( &se, sizeof(se), (UCHAR)0 );

        se.Segment = Segment;
        se.Length  = Length;

        length = strlen(lpPathName);
        if ( length >= MAX_PATH16 ) {
            length = MAX_PATH16-1;
        }
        strncpy( se.FileName, lpPathName, length );
        se.FileName[length] = '\0';

        length = strlen(lpModuleName);
        if ( length >= MAX_MODULE ) {
            length = MAX_MODULE-1;
        }
        strncpy( se.Module, lpModuleName, length );
        se.Module[length] = '\0';

        EventParams[2] = (DWORD)&se;

        SendVDMEvent( EventParams );
    }
}

void ModuleSegmentMove(
    LPSTR   lpModuleName,
    LPSTR   lpPathName,
    WORD    OldSelector,
    WORD    NewSelector
) {

    if ( fDebugged ) {
        DWORD           EventParams[4];
        VDMCONTEXT      vcContext;
        VDMINTERNALINFO viInfo;
        SEGMENT_NOTE    se;
        UINT            length;
#ifdef i386
#error DIVERGENCE MVDM-HOST-DIV-588: historical i386 debugger branch is not selected
#endif
#line 463

        GetNormalContext( &vcContext, &viInfo, EventParams, DBG_SEGMOVE, PX86 );

        RtlFillMemory( &se, sizeof(se), (UCHAR)0 );

        se.Segment     = OldSelector;
        se.Selector2   = NewSelector;
        length = strlen(lpPathName);

        if ( length >= MAX_PATH16 ) {
            length = MAX_PATH16-1;
        }
        strncpy( se.FileName, lpPathName, length );
        se.FileName[length] = '\0';

        length = strlen(lpModuleName);
        if ( length >= MAX_MODULE ) {
            length = MAX_MODULE-1;
        }
        strncpy( se.Module, lpModuleName, length );
        se.Module[length] = '\0';

        EventParams[2] = (DWORD)&se;

        SendVDMEvent( EventParams );
    }
}

void ModuleFree(
    LPSTR   lpModuleName,
    LPSTR   lpPathName
) {

    if ( fDebugged ) {
        DWORD           EventParams[4];
        VDMCONTEXT      vcContext;
        VDMINTERNALINFO viInfo;
        SEGMENT_NOTE    se;
        UINT            length;
#ifdef i386
#error DIVERGENCE MVDM-HOST-DIV-588: historical i386 debugger branch is not selected
#endif
#line 508

        GetNormalContext( &vcContext, &viInfo, EventParams, DBG_MODFREE, PX86 );

        RtlFillMemory( &se, sizeof(se), (UCHAR)0 );

        length = strlen(lpPathName);
        if ( length >= MAX_PATH16 ) {
            length = MAX_PATH16-1;
        }
        strncpy( se.FileName, lpPathName, length );
        se.FileName[length] = '\0';

        length = strlen(lpModuleName);
        if ( length >= MAX_MODULE ) {
            length = MAX_MODULE-1;
        }
        strncpy( se.Module, lpModuleName, length );
        se.Module[length] = '\0';

        EventParams[2] = (DWORD)&se;

        SendVDMEvent( EventParams );
    }
}

BOOL SingleStep(
    PTFRAME16   pTFrame
) {
    BOOL        fResult;

    fResult = FALSE;        // Default to Event not handled

    if ( fDebugged ) {
        DWORD           EventParams[4];
        VDMCONTEXT      vcContext;
        VDMINTERNALINFO viInfo;
#ifdef i386
#error DIVERGENCE MVDM-HOST-DIV-588: historical i386 debugger branch is not selected
#endif
#line 550

        GetNormalContext( &vcContext, &viInfo, EventParams, DBG_SINGLESTEP, PX86 );

        vcContext.SegDs  = (ULONG)pTFrame->wDS;
        vcContext.SegCs  = (ULONG)pTFrame->wCS;
        vcContext.SegSs  = (ULONG)pTFrame->wSS;

#ifdef i386
#error DIVERGENCE MVDM-HOST-DIV-588: historical i386 debugger branch is not selected
#else
#line 568
        vcContext.Eax    = (ULONG)pTFrame->wAX;

        vcContext.Eip    = (ULONG)pTFrame->wIP;
        vcContext.Esp    = (ULONG)pTFrame->wSP;
        vcContext.EFlags = (ULONG)pTFrame->wFlags;

        if ( (getMSW() & MSW_PE) == 0 ) {
            vcContext.EFlags |= V86FLAGS_V86;
        }
#endif
#line 578

        fResult = SendVDMEvent( EventParams );

#ifdef i386
#error DIVERGENCE MVDM-HOST-DIV-588: historical i386 debugger branch is not selected
#else
#line 590
        // No need to set FS,GS, they don't exist
#endif
#line 592

        setES( (WORD)vcContext.SegEs );
        pTFrame->wDS = (WORD)vcContext.SegDs;
        pTFrame->wCS = (WORD)vcContext.SegCs;
        pTFrame->wSS = (WORD)vcContext.SegSs;

#ifdef i386
#error DIVERGENCE MVDM-HOST-DIV-588: historical i386 debugger branch is not selected
#else
#line 624
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
#line 637
    }
    return( fResult );
}

BOOL Breakpoint(
    PTFRAME16   pTFrame
) {
    BOOL        fResult;

    fResult = FALSE;        // Default to Event not handled

    if ( fDebugged ) {
        DWORD           EventParams[4];
        VDMCONTEXT      vcContext;
        VDMINTERNALINFO viInfo;
#ifdef i386
#error DIVERGENCE MVDM-HOST-DIV-588: historical i386 debugger branch is not selected
#endif
#line 658

        GetNormalContext( &vcContext, &viInfo, EventParams, DBG_BREAK, PX86 );

        vcContext.SegDs  = (ULONG)pTFrame->wDS;
        vcContext.SegCs  = (ULONG)pTFrame->wCS;
        vcContext.SegSs  = (ULONG)pTFrame->wSS;

#ifdef i386
#error DIVERGENCE MVDM-HOST-DIV-588: historical i386 debugger branch is not selected
#else
#line 677
        vcContext.Eax    = (ULONG)pTFrame->wAX;

        vcContext.Ebp    = (ULONG)getBP();
        vcContext.Eip    = (ULONG)pTFrame->wIP;
        vcContext.Esp    = (ULONG)pTFrame->wSP;
        vcContext.EFlags = (ULONG)pTFrame->wFlags;

        if ( (getMSW() & MSW_PE) == 0 ) {
            vcContext.EFlags |= V86FLAGS_V86;
        }
#endif
#line 688

        fResult = SendVDMEvent( EventParams );

#ifdef i386
#error DIVERGENCE MVDM-HOST-DIV-588: historical i386 debugger branch is not selected
#else
#line 700
        // No need to set FS,GS, they don't exist
#endif
#line 702

        setES( (WORD)vcContext.SegEs );
        pTFrame->wDS = (WORD)vcContext.SegDs;
        pTFrame->wCS = (WORD)vcContext.SegCs;
        pTFrame->wSS = (WORD)vcContext.SegSs;

#ifdef i386
#error DIVERGENCE MVDM-HOST-DIV-588: historical i386 debugger branch is not selected
#else
#line 734
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
#line 747

    }

    return( fResult );
}

BOOL GPFault(
    PFFRAME16   pFFrame
) {
    DWORD           EventParams[4];
    BOOL            fResult;
    VDMCONTEXT      vcContext;
    VDMINTERNALINFO viInfo;
#ifdef i386
#error DIVERGENCE MVDM-HOST-DIV-588: historical i386 debugger branch is not selected
#endif
#line 763

    fResult = FALSE;        // Default to Event not handled

#ifdef i386
#error DIVERGENCE MVDM-HOST-DIV-588: historical i386 debugger branch is not selected
#endif
#line 771

    GetNormalContext( &vcContext, &viInfo, EventParams, DBG_GPFAULT, PX86 );

    vcContext.SegEs = (ULONG)pFFrame->wES;
    vcContext.SegDs = (ULONG)pFFrame->wDS;
    vcContext.SegCs = (ULONG)pFFrame->wCS;
    vcContext.SegSs = (ULONG)pFFrame->wSS;

#ifdef i386
#error DIVERGENCE MVDM-HOST-DIV-588: historical i386 debugger branch is not selected
#else
#line 797
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

    if ( (getMSW() & MSW_PE) == 0 ) {
        vcContext.EFlags |= V86FLAGS_V86;
    }
#endif
#line 813

    if ( fDebugged ) {
        fResult = SendVDMEvent( EventParams );

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
#error DIVERGENCE MVDM-HOST-DIV-588: historical i386 debugger branch is not selected
#else
#line 881
    // No need to set FS,GS, they don't exist
#endif
#line 883

    pFFrame->wES = (WORD)vcContext.SegEs;
    pFFrame->wDS = (WORD)vcContext.SegDs;
    pFFrame->wCS = (WORD)vcContext.SegCs;
    pFFrame->wSS = (WORD)vcContext.SegSs;

#ifdef i386
#error DIVERGENCE MVDM-HOST-DIV-588: historical i386 debugger branch is not selected
#else
#line 925
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
#line 938

    return( fResult );
}

BOOL DivOverflow(
    PTFRAME16   pTFrame
) {
    BOOL        fResult;

    fResult = FALSE;        // Default to Event not handled

    if ( fDebugged ) {
        DWORD           EventParams[4];
        VDMCONTEXT      vcContext;
        VDMINTERNALINFO viInfo;
#ifdef i386
#error DIVERGENCE MVDM-HOST-DIV-588: historical i386 debugger branch is not selected
#endif
#line 959

        GetNormalContext( &vcContext, &viInfo, EventParams, DBG_DIVOVERFLOW, PX86 );

        vcContext.SegDs = (ULONG)pTFrame->wDS;
        vcContext.SegCs = (ULONG)pTFrame->wCS;
        vcContext.SegSs = (ULONG)pTFrame->wSS;

#ifdef i386
#error DIVERGENCE MVDM-HOST-DIV-588: historical i386 debugger branch is not selected
#else
#line 977
        vcContext.Eax    = (ULONG)pTFrame->wAX;

        vcContext.Eip    = (ULONG)pTFrame->wIP;
        vcContext.Esp    = (ULONG)pTFrame->wSP;
        vcContext.EFlags = (ULONG)pTFrame->wFlags;

        if ( (getMSW() & MSW_PE) == 0 ) {
            vcContext.EFlags |= V86FLAGS_V86;
        }
#endif
#line 987

        fResult = SendVDMEvent( EventParams );

#ifdef i386
#error DIVERGENCE MVDM-HOST-DIV-588: historical i386 debugger branch is not selected
#else
#line 999
        // No need to set FS,GS, they don't exist
#endif
#line 1001

        setES( (WORD)vcContext.SegEs );
        pTFrame->wDS = (WORD)vcContext.SegDs;
        pTFrame->wCS = (WORD)vcContext.SegCs;
        pTFrame->wSS = (WORD)vcContext.SegSs;

#ifdef i386
#error DIVERGENCE MVDM-HOST-DIV-588: historical i386 debugger branch is not selected
#else
#line 1033
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
#line 1046


    }

    return( fResult );
}


BOOL DllStart(
    PNDFRAME16  pNDFrame
) {
    BOOL        fResult;

    fResult = FALSE;        // Default to Event not handled

    if ( fDebugged ) {
        LPSTR           lpModuleName;
        LPSTR           lpModulePath;
        UINT            length;
        UCHAR           fPE;
        DWORD           EventParams[4];
        VDMCONTEXT      vcContext;
        VDMINTERNALINFO viInfo;
        IMAGE_NOTE      im;
#ifdef i386
#error DIVERGENCE MVDM-HOST-DIV-588: historical i386 debugger branch is not selected
#endif
#line 1076

        GetNormalContext( &vcContext, &viInfo, EventParams, DBG_DLLSTART, PX86 );

        EventParams[2] = (DWORD)&im;

        vcContext.SegDs = (ULONG)pNDFrame->wDS;
        vcContext.SegCs = (ULONG)pNDFrame->wCS;

#ifdef i386
#error DIVERGENCE MVDM-HOST-DIV-588: historical i386 debugger branch is not selected
#else
#line 1094
        vcContext.Eax    = (ULONG)pNDFrame->wAX;
        vcContext.Eip    = (ULONG)pNDFrame->wIP;
        vcContext.EFlags = (ULONG)pNDFrame->wFlags;

        if ( (getMSW() & MSW_PE) == 0 ) {
            vcContext.EFlags |= V86FLAGS_V86;
        }
#endif
#line 1102

        // The code in TASK.ASM pops the frame off the stack before it IRETs
        vcContext.Esp += sizeof(NDFRAME16);

        // Get the module's path and name

        fPE = ISPESET;

        lpModuleName = (LPSTR)Sim32GetVDMPointer(
                            (ULONG)pNDFrame->dwModuleName,
                            MAX_MODULE,
                            fPE );

        lpModulePath = (LPSTR)Sim32GetVDMPointer(
                            (ULONG)pNDFrame->dwModulePath,
                            MAX_PATH,
                            fPE );

        length = (UINT)((UCHAR)*lpModuleName++);

        strncpy( im.Module, lpModuleName, length );
        im.Module[length] = '\0';

        length = (UINT)((UCHAR)*lpModulePath);
        lpModulePath += 8;
        length -= 8;

        strncpy( im.FileName, lpModulePath, length );
        im.FileName[length] = '\0';

        im.hModule = pNDFrame->hModule;
        im.hTask   = pNDFrame->hTask;

        fResult = SendVDMEvent( EventParams );

        // See comment about what the code does above
        vcContext.Esp -= sizeof(NDFRAME16);

#ifdef i386
#error DIVERGENCE MVDM-HOST-DIV-588: historical i386 debugger branch is not selected
#else
#line 1149
        // No need to set FS,GS, they don't exist
#endif
#line 1151

        setES( (WORD)vcContext.SegEs );
        pNDFrame->wDS = (WORD)vcContext.SegDs;
        pNDFrame->wCS = (WORD)vcContext.SegCs;
        setSS( (WORD)vcContext.SegSs );

#ifdef i386
#error DIVERGENCE MVDM-HOST-DIV-588: historical i386 debugger branch is not selected
#else
#line 1182
        setDI( (WORD)vcContext.Edi );
        setSI( (WORD)vcContext.Esi );
        setBX( (WORD)vcContext.Ebx );
        setDX( (WORD)vcContext.Edx );
        setCX( (WORD)vcContext.Ecx );
        pNDFrame->wAX = (WORD)vcContext.Eax;


        setBP( (WORD)vcContext.Ebp );
        pNDFrame->wIP    = (WORD)vcContext.Eip;
        pNDFrame->wFlags = (WORD)vcContext.EFlags;

        setSP( (WORD)vcContext.Esp );
#endif
#line 1196


    }

    return( fResult );
}


BOOL TaskStop(
    PSTFRAME16  pSTFrame
) {
    BOOL        fResult;

    fResult = FALSE;        // Default to Event not handled

    if ( fDebugged ) {
        LPSTR           lpModuleName;
        LPSTR           lpModulePath;
        UINT            length;
        UCHAR           fPE;
        DWORD           EventParams[4];
        VDMCONTEXT      vcContext;
        VDMINTERNALINFO viInfo;
        IMAGE_NOTE      im;
#ifdef i386
#error DIVERGENCE MVDM-HOST-DIV-588: historical i386 debugger branch is not selected
#endif
#line 1226

        GetNormalContext( &vcContext, &viInfo, EventParams, DBG_TASKSTOP, PX86 );

        EventParams[2] = (DWORD)&im;

        // The code in TASK.ASM pops the frame off the stack before it IRETs
        vcContext.Esp += sizeof(STFRAME16);

        // Get the module's path and name

        fPE = ISPESET;

        lpModuleName = (LPSTR)Sim32GetVDMPointer(
                            (ULONG)pSTFrame->dwModuleName,
                            MAX_MODULE,
                            fPE );

        lpModulePath = (LPSTR)Sim32GetVDMPointer(
                            (ULONG)pSTFrame->dwModulePath,
                            MAX_PATH,
                            fPE );

        length = (UINT)((UCHAR)*lpModuleName++);

        strncpy( im.Module, lpModuleName, length );
        im.Module[length] = '\0';

        length = (UINT)((UCHAR)*lpModulePath);
        lpModulePath += 8;
        length -= 8;

        strncpy( im.FileName, lpModulePath, length );
        im.FileName[length] = '\0';

        im.hModule = pSTFrame->hModule;
        im.hTask   = pSTFrame->hTask;

        fResult = SendVDMEvent( EventParams );

        // See comment about what the code does above
        vcContext.Esp -= sizeof(STFRAME16);
    }

    return( fResult );
}









//
// I looked through the Win 3.0/Win 3.1 sources, and all the SDM_... commands
// I could find were this:   (The only ones we support for now will be
//                             SDM_LOADSEG,SDM_MOVESEG,SDM_FREESEG/RELEASESEG)
//
// ax = SDM_LOADSEG     =  50h
//       es:di = Module Name
//       bx    = segment number
//       cx    = loaded seg
//       dx    = instance number
//       si    = DataOrCodeSegment
// ax = SDM_MOVESEG     =  51h
//       push  destseg
//       push  sourceseg
//       push  SDM_MOVESEG
// SDM_FREESEG     =  52h
//       bx    = segment addr
// SDM_RELEASESEG  =  5Ch
//       bx    = segment addr
// SDM_5A (DebugDebug)
//       cx    = dataoffset hGlobalHeap
//       dx    = ds (SetKernelDS)
// SDM_4F (DebugInit)
//       push  SDM_4F
// SDM_0 (Out Char)
//       dl    = character to output
// SDM_0F (Out Symbol)
//       dx    = offset
//       cx    = selector
//       es    = data segment of DOSX
// SDM_GLOBALHEAP  =  3 (real mode only)
//       push  pGlobalHeap
//       push  ax
// SDM_CONWRITE    =  12h
//       ds:si = Pointer to string
//       cx    = length of string
// SDM_CONREAD     =  1
//       nothing, returns character read in al
// SDM_DGH         =  56h
//       cx    = seg DumpGlobalHeap
//       bx    = codeoffset DumpGlobalHeap
// SDM_DFL         =  57h
//       cx    = seg DumpFreeList
//       bx    = codeoffset DumpFreeList
// SDM_DLL         =  58h
//       cx    = seg DumpLRUList
//       bx    = codeoffset DumpLRUList
// SDM_LOADTASK    =  59h
//       not used
// SDM_POSTLOAD    =  60h
//       push    ax
// SDM_EXITCALL    =  62h
//       push    ax
// SDM_INT2        =  63h (Ctrl-alt-sysreq)
//       nothing.
// SDM_LOADDLL     =  64h
//       not used
// SDM_DELMODULE   =  65h
//       push    module handle
// SDM_RIN         =  9
//       push    seg ReplaceInst
//       push    codeoffset ReplaceInst
//       push    ax
// SDM_BANKLINE    = 10
//       push    EMS_calc_swap_line
//       push    ax
// SDM_NEWTASK     = 11
//       push   EMS PID
//       push   ax
// SDM_FLUSHTASK   = 12
//       push   EMS PID
//       push   ax
// SDM_SWITCHOUT   = 13
//       ds    = TDB
// SDM_SWITCHIN    = 14
//       ds    = TDB
// SDM_KEYBOARD    = 15
//       not used (thankfully, it is the same number as SDM_0F)
// SDM_MAXFUNC     = 15
//       not used

void DBGDispatch()
{
    UNALIGNED WORD  *stack;
    WORD            mode;
    WORD            selector;
    WORD            segment;
    WORD            new_selector;
    BOOL            fBPRelease;
    BOOL            fData;
    LPSTR           lpModuleName;
    LPSTR           lpPathName;
    UCHAR           fPE;
    PFFRAME16       pFFrame;
    PTFRAME16       pTFrame;
    PNDFRAME16      pNDFrame;
    PSTFRAME16      pSTFrame;
    WORD            wFrame;

    fPE = ISPESET;

    stack = (UNALIGNED WORD *)Sim32GetVDMPointer(
                        ((ULONG)getSS() << 16) + (ULONG)getSP(),
                        MAX_DBG_FRAME, fPE );

    mode = *stack++;
    switch( mode ) {
        case DBG_SEGLOAD:
            selector = *stack++;
            segment  = *stack++;
            lpModuleName = (LPSTR)Sim32GetVDMPointer(
                                    (ULONG)*stack + ((ULONG)(*(stack+1)) << 16),
                                    0, fPE );
            stack += 2;
            lpPathName = (LPSTR)Sim32GetVDMPointer(
                                    (ULONG)*stack + ((ULONG)(*(stack+1)) << 16),
                                    0, fPE );
            if ( lpPathName == NULL ) {
                lpPathName = "";
            }

            stack += 2;
            fData = (BOOL)(*stack++);
            SegmentLoad( lpModuleName, lpPathName, selector, segment, fData );
            break;
        case DBG_SEGMOVE:
            selector = *stack++;
            new_selector = *stack++;
            SegmentMove( selector, new_selector );
            break;
        case DBG_SEGFREE:
            fBPRelease = (BOOL)*stack++;
            selector = *stack++;
            SegmentFree( selector, fBPRelease );
            break;
        case DBG_MODFREE:
            lpModuleName = (LPSTR)Sim32GetVDMPointer(
                                    (ULONG)*stack + ((ULONG)(*(stack+1)) << 16),
                                    0, fPE );
            stack += 2;
            lpPathName = (LPSTR)Sim32GetVDMPointer(
                                    (ULONG)*stack + ((ULONG)(*(stack+1)) << 16),
                                    0, fPE );
            if ( lpPathName == NULL ) {
                lpPathName = "";
            }
            ModuleFree( lpModuleName, lpPathName );
            break;
        case DBG_MODLOAD:

            // Why doesn't this do anything? - See JonLe

            break;

        case DBG_SINGLESTEP:
            pTFrame = (PTFRAME16)Sim32GetVDMPointer(
                        (ULONG)((ULONG)getSS() << 16) + (ULONG)getSP(),
                        MAX_DBG_FRAME, fPE );


            fData = SingleStep( pTFrame );

            setAX((WORD)fData);
            break;

        case DBG_BREAK:
            pTFrame = (PTFRAME16)Sim32GetVDMPointer(
                        (ULONG)((ULONG)getSS() << 16) + (ULONG)getSP(),
                        MAX_DBG_FRAME, fPE );


            fData = Breakpoint( pTFrame );

            setAX((WORD)fData);
            break;
        case DBG_GPFAULT:
            wFrame = getBP() - (WORD)(FIELD_OFFSET(FFRAME16,wBP));

            pFFrame = (PFFRAME16)Sim32GetVDMPointer(
                        ((ULONG)getSS() << 16) + (ULONG)wFrame,
                        MAX_DBG_FRAME, fPE );


            fData = GPFault( pFFrame );

            setAX((WORD)fData);
            break;
        case DBG_DIVOVERFLOW:
            pTFrame = (PTFRAME16)Sim32GetVDMPointer(
                        (ULONG)((ULONG)getSS() << 16) + (ULONG)getSP(),
                        MAX_DBG_FRAME, fPE );


            fData = DivOverflow( pTFrame );

            setAX((WORD)fData);
            break;
        case DBG_DLLSTART:
            pNDFrame = (PNDFRAME16)Sim32GetVDMPointer(
                        (ULONG)((ULONG)getSS() << 16) + (ULONG)getSP(),
                        MAX_DBG_FRAME, fPE );


            fData = DllStart( pNDFrame );

            setAX((WORD)fData);
            break;
        case DBG_TASKSTOP:
            pSTFrame = (PSTFRAME16)Sim32GetVDMPointer(
                        (ULONG)((ULONG)getSS() << 16) + (ULONG)getSP(),
                        MAX_DBG_FRAME, fPE );

            fData = TaskStop( pSTFrame );
            break;
        case DBG_ATTACH:
            break;

        case DBG_TOOLHELP:
            ulTHHOOK = (ULONG)*stack + ((ULONG)(*(stack+1)) << 16);
            stack += 2;
            f386 = (BOOL)*stack;
            break;

        default:
            setAX(0);       // Event not handled
            break;
    }
}

VOID DBGNotifyNewTask(
    LPVOID	lpvNTFrame,
    UINT	uFrameSize
) {
    BOOL        fResult;
    PNTFRAME16	pNTFrame;

    pNTFrame = (PNTFRAME16)lpvNTFrame;

    if ( fDebugged ) {
        LPSTR           lpModuleName;
        LPSTR           lpModulePath;
        UINT            length;
        UCHAR           fPE;
        DWORD           EventParams[4];
        VDMCONTEXT      vcContext;
        VDMINTERNALINFO viInfo;
        IMAGE_NOTE      im;
#ifdef i386
#error DIVERGENCE MVDM-HOST-DIV-588: historical i386 debugger branch is not selected
#endif
#line 1533

        GetNormalContext( &vcContext, &viInfo, EventParams, DBG_TASKSTART, PX86 );

        EventParams[2] = (DWORD)&im;

        vcContext.SegEs = (ULONG)pNTFrame->wES;
        vcContext.SegDs = (ULONG)pNTFrame->wDS;
        vcContext.SegCs = (ULONG)pNTFrame->wCS;

#ifdef i386
#error DIVERGENCE MVDM-HOST-DIV-588: historical i386 debugger branch is not selected
#else
#line 1560
        vcContext.Edi    = (ULONG)pNTFrame->wDI;
        vcContext.Esi    = (ULONG)pNTFrame->wSI;
        vcContext.Ebx    = (ULONG)pNTFrame->wBX;
        vcContext.Edx    = (ULONG)pNTFrame->wDX;
        vcContext.Ecx    = (ULONG)pNTFrame->wCX;
        vcContext.Eax    = (ULONG)pNTFrame->wAX;

        vcContext.Ebp    = (ULONG)pNTFrame->wBP;
        vcContext.Eip    = (ULONG)pNTFrame->wIP;
        vcContext.EFlags = (ULONG)pNTFrame->wFlags;

        if ( (getMSW() & MSW_PE) == 0 ) {
            vcContext.EFlags |= V86FLAGS_V86;
        }
#endif
#line 1575
        // The code in TASKING.ASM does a dec bp before the IRET
        vcContext.Ebp -= 1;
        // The code copies frame off the stack then IRETs
        vcContext.Esp += sizeof(NTFRAME16) + uFrameSize;

        // Get the module's path and name

        fPE = ISPESET;

        lpModuleName = (LPSTR)Sim32GetVDMPointer(
                            (ULONG)pNTFrame->dwModuleName,
                            MAX_MODULE,
                            fPE );

        lpModulePath = (LPSTR)Sim32GetVDMPointer(
                            (ULONG)pNTFrame->dwModulePath,
                            MAX_PATH,
                            fPE );

        length = (UINT)((UCHAR)*lpModuleName++);

        strncpy( im.Module, lpModuleName, length );
        im.Module[length] = '\0';

        length = (UINT)((UCHAR)*lpModulePath);
        lpModulePath += 8;
        length -= 8;

        strncpy( im.FileName, lpModulePath, length );
        im.FileName[length] = '\0';

        im.hModule = pNTFrame->hModule;
        im.hTask   = pNTFrame->hTask;

        fResult = SendVDMEvent( EventParams );

        // See comments about what the code does above
        vcContext.Ebp += 1;
        vcContext.Esp -= sizeof(NTFRAME16) + uFrameSize;

#ifdef i386
#error DIVERGENCE MVDM-HOST-DIV-588: historical i386 debugger branch is not selected
#else
#line 1624
        // No need to set FS,GS, they don't exist
#endif
#line 1626

        pNTFrame->wES = (WORD)vcContext.SegEs;
        pNTFrame->wDS = (WORD)vcContext.SegDs;
        pNTFrame->wCS = (WORD)vcContext.SegCs;
        setSS( (WORD)vcContext.SegSs );

#ifdef i386
#error DIVERGENCE MVDM-HOST-DIV-588: historical i386 debugger branch is not selected
#else
#line 1667
        pNTFrame->wDI    = (WORD)vcContext.Edi;
        pNTFrame->wSI    = (WORD)vcContext.Esi;
        pNTFrame->wBX    = (WORD)vcContext.Ebx;
        pNTFrame->wDX    = (WORD)vcContext.Edx;
        pNTFrame->wCX    = (WORD)vcContext.Ecx;
        pNTFrame->wAX    = (WORD)vcContext.Eax;

        pNTFrame->wBP    = (WORD)vcContext.Ebp;

        pNTFrame->wIP    = (WORD)vcContext.Eip;
        pNTFrame->wFlags = (WORD)vcContext.EFlags;

        setSP( (WORD)vcContext.Esp );
#endif
#line 1681


    }


}


VOID DBGNotifyRemoteThreadAddress(
    LPVOID  lpAddress,
    DWORD   lpBlock
) {
    lpRemoteAddress = lpAddress;
    lpRemoteBlock   = lpBlock;
}

VOID DBGNotifyDebugged(
    BOOL    fNewDebugged
) {
    fDebugged = fNewDebugged;
}
