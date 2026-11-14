/*++
 *
 *  WOW v1.0
 *
 *  Copyright (c) 1991, Microsoft Corporation
 *
 *  WCALL16.C
 *  WOW32 16-bit message/callback support
 *
 *  History:
 *  Created 11-Mar-1991 by Jeff Parsons (jeffpar)
 *  Changed 18-Aug-1992 by Mike Tricker (MikeTri) Added DOS PDB and SFT functions
--*/


#include "precomp.h"
#include "wow_callback_frame_lease.h"
#line 17
#pragma hdrstop


MODNAME(wcall16.c);

#define WOWFASTEDIT

#ifdef WOWFASTEDIT

typedef struct _LOCALHANDLEENTRY {
    WORD    lhe_address;    // actual address of object
    BYTE    lhe_flags;      // flags and priority level
    BYTE    lhe_count;      // lock count
} LOCALHANDLEENTRY, *PLOCALHANDLEENTRY;

#define LA_MOVEABLE     0x0002      // moveable or fixed?

#define LHE_DISCARDED   0x0040      // Marks objects that have been discarded.

#endif

/* Common callback functions
 */
HANDLE LocalAlloc16(WORD wFlags, INT cb, HANDLE hInstance)
{
    PARM16 Parm16;
    VPVOID vp = 0;

    if (LOWORD(hInstance) == 0 ) {     /* if lo word == 0, then this is a 32-bit
					   hInstance, which makes no sense */
	WOW32ASSERT(LOWORD(hInstance));
        return (HANDLE)0;
    }

    if (cb < 0 || cb > 0xFFFF) {
        WOW32ASSERT(cb > 0 && cb <= 0xFFFF);
        return (HANDLE)0;
    }

    Parm16.WndProc.wMsg = LOWORD(hInstance) | 1;

    Parm16.WndProc.wParam = wFlags;
    Parm16.WndProc.lParam = cb;
    CallBack16(RET_LOCALALLOC, &Parm16, 0, &vp);

    if (LOWORD(vp) == 0)
        vp = 0;

    return (HANDLE)vp;
}


HANDLE LocalReAlloc16(HANDLE hMem, INT cb, WORD wFlags)
{
    PARM16 Parm16;
    VPVOID vp = 0;

    if (HIWORD(hMem) == 0 || cb < 0 || cb > 0xFFFF) {
        WOW32ASSERT(HIWORD(hMem) && cb >= 0 && cb <= 0xFFFF);
        return (HANDLE)0;
    }

    LOGDEBUG(4,("LocalRealloc DS = %x, hMem = %x, bytes = %x, flags = %x\n",HIWORD(hMem),LOWORD(hMem),cb,wFlags));
    Parm16.WndProc.lParam = (LONG)hMem;
    Parm16.WndProc.wParam = wFlags;
    Parm16.WndProc.wMsg = (WORD)cb;
    CallBack16(RET_LOCALREALLOC, &Parm16, 0, &vp);

    if (LOWORD(vp) == 0)
        vp = 0;

    return (HANDLE)vp;
}

#ifndef WOWFASTEDIT
#line 92
#error Unsupported excluded original configuration
#else
#line 126

VPVOID LocalLock16(HANDLE hMem)
{
    WORD    h16;
    LONG    retval;

    if (HIWORD(hMem) == 0) {
        WOW32ASSERT(HIWORD(hMem) != 0);
        return (VPVOID)0;
    }

    h16 = LOWORD(hMem);
    retval = (VPVOID)hMem;

    if (h16 & LA_MOVEABLE) {
        PLOCALHANDLEENTRY plhe;

        GETVDMPTR(hMem, sizeof(*plhe), plhe);

        if (plhe->lhe_flags & LHE_DISCARDED) {
            goto LOCK1;
        }

        plhe->lhe_count++;
        if (!plhe->lhe_count)
            plhe->lhe_count--;

LOCK1:
        LOW(retval) = plhe->lhe_address;
        FLUSHVDMPTR((ULONG)hMem, sizeof(*plhe), plhe);
        FREEVDMPTR(plhe);
    }

    if (LOWORD(retval) == 0)
        retval = 0;

    return retval;
}

BOOL LocalUnlock16(HANDLE hMem)
{
    WORD    h16;
    BOOL    rc;
    PLOCALHANDLEENTRY plhe;
    BYTE    count;

    if (HIWORD(hMem) == 0) {
        WOW32ASSERT(HIWORD(hMem));
        return FALSE;
    }

    rc = FALSE;
    h16 = LOWORD(hMem);

    if (!(h16 & LA_MOVEABLE)) {
        goto UNLOCK2;
    }

    GETVDMPTR(hMem, sizeof(*plhe), plhe);

    if (plhe->lhe_flags & LHE_DISCARDED)
        goto UNLOCK1;

    count = plhe->lhe_count;
    count--;

    if (count >= (BYTE)(0xff-1))
        goto UNLOCK1;

    plhe->lhe_count = count;
    rc = (BOOL)((SHORT)count);

    FLUSHVDMPTR((ULONG)hMem, sizeof(*plhe), plhe);

UNLOCK1:
    FREEVDMPTR(plhe);

UNLOCK2:
    return rc;
}

#endif  // WOWFASTEDIT
#line 208


WORD LocalSize16(HANDLE hMem)
{
    PARM16 Parm16;
    VPVOID vp = 0;

    if (HIWORD(hMem) == 0) {
        WOW32ASSERT(HIWORD(hMem));
        return FALSE;
    }

    Parm16.WndProc.lParam = (LONG)hMem;
    CallBack16(RET_LOCALSIZE, &Parm16, 0, &vp);

    return (WORD)vp;
}


HANDLE LocalFree16(HANDLE hMem)
{
    PARM16 Parm16;
    VPVOID vp = FALSE;

    if (HIWORD(hMem) == 0) {
        WOW32ASSERT(HIWORD(hMem));
        return (HANDLE)0;
    }

    Parm16.WndProc.lParam = (LONG)hMem;
    CallBack16(RET_LOCALFREE, &Parm16, 0, &vp);

    if (LOWORD(vp) == 0) {
        vp = 0;
    } else {
        WOW32ASSERT(LOWORD(vp) == LOWORD(hMem));
        vp = (VPVOID)hMem;
    }

    return (HANDLE)vp;
}


BOOL LockSegment16(WORD wSeg)
{
    PARM16 Parm16;
    VPVOID vp = FALSE;

    Parm16.WndProc.wParam = wSeg;
    CallBack16(RET_LOCKSEGMENT, &Parm16, 0, &vp);

    return (BOOL)vp;
}


BOOL UnlockSegment16(WORD wSeg)
{
    PARM16 Parm16;
    VPVOID vp = FALSE;

    Parm16.WndProc.wParam = wSeg;
    CallBack16(RET_UNLOCKSEGMENT, &Parm16, 0, &vp);

    return (BOOL)vp;
}


VPVOID  WOWGlobalAllocLock16(WORD wFlags, DWORD cb, HMEM16 *phMem)
{
    PARM16 Parm16;
    VPVOID vp = 0;

    Parm16.WndProc.wParam = wFlags;
    Parm16.WndProc.lParam = cb;
    CallBack16(RET_GLOBALALLOCLOCK, &Parm16, 0, &vp);

    if (vp) {

    // Get handle of 16-bit object
        if (phMem) {
            PCBVDMFRAME pCBFrame;

            pCBFrame = CBFRAMEPTR(CURRENTPTD()->vpCBStack);
            *phMem = pCBFrame->wGenUse1;
            FREEVDMPTR(pCBFrame);
        }
    }
    return vp;
}


HMEM16 WOWGlobalAlloc16(WORD wFlags, DWORD cb)
{
    HMEM16 hMem;
    VPVOID vp;

    if (vp = WOWGlobalAllocLock16(wFlags, cb, &hMem)) {
        WOWGlobalUnlock16(hMem);
    } else {
        hMem = 0;
    }

    return hMem;
}


VPVOID  WOWGlobalLockSize16(HMEM16 hMem, PDWORD pcb)
{
    PARM16 Parm16;
    VPVOID vp = 0;
    PCBVDMFRAME pCBFrame;

    Parm16.WndProc.wParam = hMem;
    CallBack16(RET_GLOBALLOCK, &Parm16, 0, &vp);

    // Get size of 16-bit object    (will be 0 if lock failed)
    if (pcb) {
        pCBFrame = CBFRAMEPTR(CURRENTPTD()->vpCBStack);
        *pcb = pCBFrame->wGenUse2 | (LONG)pCBFrame->wGenUse1 << 16;
        FREEVDMPTR(pCBFrame);
    }

    return vp;
}


VPVOID WOWGlobalLock16(HMEM16 hMem)
{
    return WOWGlobalLockSize16(hMem, NULL);
}


BOOL WOWGlobalUnlock16(HMEM16 hMem)
{
    PARM16 Parm16;
    VPVOID vp = FALSE;

    Parm16.WndProc.wParam = hMem;
    CallBack16(RET_GLOBALUNLOCK, &Parm16, 0, &vp);

    return (BOOL)vp;
}


HMEM16 WOWGlobalUnlockFree16(VPVOID vpMem)
{
    PARM16 Parm16;
    VPVOID vp = FALSE;

    Parm16.WndProc.lParam = vpMem;
    CallBack16(RET_GLOBALUNLOCKFREE, &Parm16, 0, &vp);

    return (HMEM16)vp;
}


HMEM16 WOWGlobalFree16(HMEM16 hMem)
{
    VPVOID vp;

    if (vp = WOWGlobalLock16(hMem)) {
        hMem = WOWGlobalUnlockFree16(vp);
    } else {
        // On failure we return the passed-in handle,
        // so there's nothing to do.
    }

    return hMem;
}


HAND16 GetExePtr16( HAND16 hInst )
{
    PARM16 Parm16;
    ULONG ul;
    PTD ptd;
    INT i;

    if (hInst == 0) return (HAND16)0;

    //
    // see if this is the hInst for the current task
    //

    ptd = CURRENTPTD();

    if (hInst == ptd->hInst16) {
	return ptd->hMod16;
    }

    //
    // check the cache
    //

    for (i = 0; i < CHMODCACHE; i++) {
        if (ghModCache[i].hInst16 == hInst)
            return ghModCache[i].hMod16;
    }

    /*
    ** Function returns a hModule, given an hInstance
    */
    Parm16.WndProc.wParam = hInst;
    CallBack16(RET_GETEXEPTR, &Parm16, 0, &ul);


    //
    // update the cache
    // slide everybody down 1 entry, put this new guy at the top
    //

    RtlMoveMemory(ghModCache+1, ghModCache, sizeof(HMODCACHE)*(CHMODCACHE-1));
    ghModCache[0].hInst16 = hInst;
    ghModCache[0].hMod16 = (HAND16)LOWORD(ul);

    return (HAND16)LOWORD(ul);
}


WORD GetModuleFileName16( HAND16 hInst, VPVOID lpszModuleName, WORD cchModuleName )
{
    PARM16 Parm16;
    ULONG ul;


    if (hInst == 0) return 0;

    Parm16.WndProc.wParam = hInst;
    Parm16.WndProc.lParam = lpszModuleName;
    Parm16.WndProc.wMsg   = cchModuleName;
    CallBack16(RET_GETMODULEFILENAME, &Parm16, 0, &ul );

    return( LOWORD(ul) );
}


ULONG GetDosPDB16(VOID)
{
    PARM16 Parm16;
    DWORD dwReturn = 0;

    CallBack16(RET_GETDOSPDB, &Parm16, 0, &dwReturn);

    return (ULONG)dwReturn;
}


ULONG GetDosSFT16(VOID)
{
    PARM16 Parm16;
    DWORD dwReturn = 0;

    CallBack16(RET_GETDOSSFT, &Parm16, 0, &dwReturn);

    return (ULONG)dwReturn;
}

// Given a data selector change it into a code selector

WORD ChangeSelector16(WORD wSeg)
{
    PARM16 Parm16;
    VPVOID vp = FALSE;

    Parm16.WndProc.wParam = wSeg;
    CallBack16(RET_CHANGESELECTOR, &Parm16, 0, &vp);

    return LOWORD(vp);
}

VPVOID RealLockResource16(HMEM16 hMem, PINT pcb)
{
    PARM16 Parm16;
    VPVOID vp = 0;
    PCBVDMFRAME pCBFrame;

    Parm16.WndProc.wParam = hMem;
    CallBack16(RET_LOCKRESOURCE, &Parm16, 0, &vp);

    // Get size of 16-bit object    (will be 0 if lock failed)
    if (pcb) {
        pCBFrame = CBFRAMEPTR(CURRENTPTD()->vpCBStack);
        *pcb = pCBFrame->wGenUse2 | (LONG)pCBFrame->wGenUse1 << 16;
        FREEVDMPTR(pCBFrame);
    }

    return vp;
}


DWORD WOWCallback16(DWORD vpFn, DWORD dwParam)
{
    PARM16 Parm16;
    VPVOID vp;

    //
    // Copy DWORD parameter to PARM16 structure.
    //

    RtlCopyMemory(&Parm16.WOWCallback16.wArgs, &dwParam, sizeof(dwParam));

    //
    // Use semi-slimy method to pass argument size to CallBack16.
    //

    vp = (VPVOID) sizeof(dwParam);

    if (!CallBack16(RET_WOWCALLBACK16, &Parm16, (VPPROC)vpFn, &vp))
        return 0;

#line 517
    return (DWORD)vp;
}


BOOL WOWCallback16Ex(
         DWORD vpFn,
         DWORD dwFlags,
         DWORD cbArgs,
         PVOID pArgs,
         PDWORD pdwRetCode
         )
{
#line 529
#ifdef DEBUG
#line 530
#error Excluded unsupported DEBUG WOW profile
#endif // DEBUG
#line 552

    if (cbArgs > sizeof(PARM16)) {
        LOGDEBUG(LOG_ALWAYS, ("WOWCallback16V: cbArgs = %u, must be <= %u",
                              cbArgs, (unsigned) sizeof(PARM16)));
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    //
    // For cdecl functions we don't want to "sub SP, cbArgs" after calling
    // the function, so we pass 0 as cbArgs to the 16-bit side.
    //

    if (dwFlags & WCB16_CDECL) {
        cbArgs = 0;
    }

    //
    // Use semi-slimy method to pass argument size to CallBack16.
    //

    *pdwRetCode = cbArgs;

    return CallBack16(RET_WOWCALLBACK16, (PPARM16)pArgs, (VPPROC)vpFn, (PVPVOID)pdwRetCode);
#line 578
}


BOOL wow_original_CallBack16(INT iRetID, PPARM16 pParm16, VPPROC vpfnProc, PVPVOID pvpReturn)
#line 582
{
#line 583
#ifdef DEBUG
#line 584
#error Excluded unsupported DEBUG WOW profile
#endif
#line 648
    register PTD ptd;
    register PVDMFRAME pFrame;
    register PCBVDMFRAME pCBFrame;
#line 651
    WORD wAX;
    VPVOID callerStack, callerCallbackStack;
    DWORD callerInit;
    wow_callback_control control = {0};
#if FASTBOPPING
#line 653
#error Unsupported excluded original configuration
#else
#line 654
    USHORT SaveIp;
#endif
#line 656
#ifdef DEBUG
#line 657
#error Excluded unsupported DEBUG WOW profile
#endif
#line 645


#line 661
    WOW32ASSERT(iRetID != RET_RETURN && iRetID != RET_DEBUGRETURN);

    ptd = CURRENTPTD();

    callerStack = ptd->vpStack;
    callerCallbackStack = ptd->vpCBStack;
    callerInit = ptd->dwFlags & TDF_INITCALLBACKSTACK;
    wow_callback_control_capture(&control);
    __try {
#line 665
    GETFRAMEPTR(ptd->vpStack, pFrame);

    // Just making sure that this thread matches the current 16-bit task

#line 669
    WOW32ASSERT((pFrame->wTDB == ptd->htask16) ||
                (ptd->dwFlags & TDF_IGNOREINPUT) ||
                (ptd->htask16 == 0));

    // Prep the frame for the callback
    // make it word aligned.

    if (ptd->dwFlags & TDF_INITCALLBACKSTACK) {
        ptd->vpCBStack = (ptd->vpStack - sizeof(CBVDMFRAME)) & (~0x1);
    }
    else {
        ptd->dwFlags |= TDF_INITCALLBACKSTACK;
        ptd->vpCBStack = (ptd->vpCBStack - sizeof(CBVDMFRAME)) & (~0x1);
    }
    GETFRAMEPTR(ptd->vpCBStack, (PVDMFRAME)pCBFrame);
    pCBFrame->vpStack = ptd->vpStack;
    pCBFrame->wRetID = (WORD)iRetID;
    pCBFrame->wTDB       = pFrame->wTDB;
    pCBFrame->wLocalBP   = pFrame->wLocalBP;

#line 689
#ifdef DEBUG
#line 690
#error Excluded unsupported DEBUG WOW profile
#endif
#line 695

    if (pParm16)
        RtlCopyMemory(&pCBFrame->Parm16, pParm16, sizeof(PARM16));

    //if (vpfnProc)     // cheaper to just do it
        STOREDWORD(pCBFrame->vpfnProc, vpfnProc);

    wAX = HIWORD(ptd->vpStack);         // Put SS in AX register for callback

    if ( iRetID == RET_WNDPROC ) {
        if ( pParm16->WndProc.hInst )
            wAX = pParm16->WndProc.hInst | 1;
    }

    pCBFrame->wAX = wAX;                // Use this AX for the callback

    //
    // Semi-slimy way we pass byte count of arguments into this function
    // for generic callbacks (WOWCallback16).
    //

    if (RET_WOWCALLBACK16 == iRetID) {
        pCBFrame->wGenUse1 = (WORD)(DWORD)*pvpReturn;
    }

#line 720
#ifdef DEBUG
#line 721
#error Excluded unsupported DEBUG WOW profile
#endif
#line 756

#line 757
    FREEVDMPTR(pFrame);
    FLUSHVDMPTR(ptd->vpCBStack, sizeof(CBVDMFRAME), pCBFrame);
    FREEVDMPTR(pCBFrame);

    // Set up to use the right 16-bit stack for this thread

#if FASTBOPPING
#line 764
#error Unsupported excluded original configuration
#else
#line 766
    SETVDMSTACK(ptd->vpCBStack);
#endif
#line 768

    //
    // do the callback!
    //

#if FASTBOPPING
#line 774
#error Unsupported excluded original configuration
#else
#line 771
    // Time to get the IEU running task-time code again
#line 779
    SaveIp = getIP();
#line 780
    host_simulate();
    setIP(SaveIp);
    ptd->vpStack = VDMSTACK();
#endif
#line 779

    // after return from callback ptd->vpStack will point to PCBVDMFRAME


    // consistency check
#line 788
    WOW32ASSERT(ptd->vpStack == vpCBStackT);

    ptd->vpCBStack = ptd->vpStack;
    GETFRAMEPTR(ptd->vpCBStack, (PVDMFRAME)pCBFrame);

    // Just making sure that this thread matches the current 16-bit task

    WOW32ASSERT((pCBFrame->wTDB == ptd->htask16) ||
        (ptd->htask16 == 0));

    if (pvpReturn) {
        LOW(*pvpReturn) = pCBFrame->wAX;
        HIW(*pvpReturn) = pCBFrame->wDX;
    }

    LOGDEBUG(9,("%04X          WIN16 %s returning: %lx\n",
        pCBFrame->wTDB, apszCallBacks[iRetID], (pvpReturn) ? *pvpReturn : 0));

    // restore the stack to its original value.
    // ie. fake the 'pop' of callback stack by resetting the vpStack
    // to its original value. The ss:sp will actually be updated when
    // the 'api thunk' returns.

    // consistency check
    WOW32ASSERT(pCBFrame->vpStack == vpStackT);

    ptd->vpStack = pCBFrame->vpStack;

    FREEVDMPTR(pCBFrame);

    } __finally {
        if (AbnormalTermination()) {
            if (wow_callback_control_restore(&control, callerStack)) {
                ptd->vpStack = callerStack;
                ptd->vpCBStack = callerCallbackStack;
                if (!(ptd->dwFlags & TDF_IGNOREINPUT))
                    ptd->dwFlags = (ptd->dwFlags & ~TDF_INITCALLBACKSTACK) | callerInit;
            }
        }
    }
#line 818
    return TRUE;
}
