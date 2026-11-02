/*++
 *
 *  WOW v1.0
 *
 *  Copyright (c) 1991, Microsoft Corporation
 *
 *  WMSGLB.C
 *  WOW32 16-bit message thunks
 *
 *  History:
 *  Created 11-Mar-1991 by Jeff Parsons (jeffpar)
--*/


#include "precomp.h"
#pragma hdrstop

MODNAME(wmsglb.c);


#ifdef DEBUG
#error Excluded unsupported DEBUG WOW profile
#endif
#line 74


BOOL FASTCALL ThunkLBMsg16(LPMSGPARAMEX lpmpex)
{
    register PWW pww;
    WORD wMsg = lpmpex->Parm16.WndProc.wMsg;

    LOGDEBUG(7,("    Thunking 16-bit list box message %s(%04x)\n", (LPSZ)GetLBMsgName(wMsg), wMsg));

    wMsg -= WM_USER + 1;

    //
    // For app defined (control) messages that are out of range
    // return TRUE.
    //
    // ChandanC Sept-15-1992
    //

    if (wMsg < (LBCB_CARETOFF - LB_ADDSTRING + 1)) {

        switch(lpmpex->uMsg = wMsg + LB_ADDSTRING) {
    
        case LB_SELECTSTRING:
        case LB_FINDSTRING:
        case LB_FINDSTRINGEXACT:
        case LB_INSERTSTRING:
        case LB_ADDSTRING:
            if (!(pww = lpmpex->pww))
                    return FALSE;
        
            if (!(pww->dwStyle & (LBS_OWNERDRAWFIXED|LBS_OWNERDRAWVARIABLE)) ||
                 (pww->dwStyle & (LBS_HASSTRINGS))) {
                    GETPSZPTR(lpmpex->Parm16.WndProc.lParam, (LPSZ)lpmpex->lParam);
                }
            break;
    
        case LB_DIR:
            GETPSZPTR(lpmpex->Parm16.WndProc.lParam, (LPSZ)lpmpex->lParam);
            break;

        case LB_GETTEXT:
            if (NULL != (pww = lpmpex->pww)) {
                register PTHUNKTEXTDWORD pthkdword = (PTHUNKTEXTDWORD)lpmpex->MsgBuffer;

                // we set this as a flag to indicate that we retrieve a dword 
                // instead of a string there. In case when hooks are installed
                // this code prevents RISC platforms from malfunctioning in 
                // kernel (they have code like this:
                //   try {
                //       <assign to original ptr here>
                //   }
                //   except(1) {
                //       <put error message in debug>
                //   }
                // which causes this message not to return the proper value)
                // See walias.h for definition of THUNKTEXTDWORD structure as 
                // well as MSGPARAMEX structure
                // this code is complemented in UnThunkLBMsg16
                // 
                // Application: PeachTree Accounting v3.5

                pthkdword->fDWORD = (pww->dwStyle & (LBS_OWNERDRAWFIXED|LBS_OWNERDRAWVARIABLE)) &&
                                    !(pww->dwStyle & (LBS_HASSTRINGS));

                if (pthkdword->fDWORD) {
                    lpmpex->lParam = (LPARAM)(LPVOID)&pthkdword->dwDataItem;
                    break;
                }
            }
            else {
                register PTHUNKTEXTDWORD pthkdword = (PTHUNKTEXTDWORD)lpmpex->MsgBuffer;
                pthkdword->fDWORD = FALSE;
            }

            GETPSZPTR(lpmpex->Parm16.WndProc.lParam, (LPSZ)lpmpex->lParam);
            break;
    
        case LB_GETITEMRECT:
            lpmpex->lParam = (LONG)lpmpex->MsgBuffer;
            break;
    
        case LB_GETSELITEMS:
            (PVOID)lpmpex->lParam = STACKORHEAPALLOC(lpmpex->Parm16.WndProc.wParam * sizeof(INT), 
                                                       sizeof(lpmpex->MsgBuffer), lpmpex->MsgBuffer);
            /* DIVERGENCE(MVDM-HOST-DIV-346): reject absent output storage. */
            if (!lpmpex->lParam) return FALSE;
            break;
    
        case LB_SETSEL:
            // sign extend
            {
                LPARAM lParam = lpmpex->Parm16.WndProc.lParam;
                lpmpex->lParam = (LOWORD(lParam) == 0xffff) ? 
                                         INT32(LOWORD(lParam)) : (LONG)lParam; 
            }
            break;
    
        case LB_SETTABSTOPS:
            //  apparently lParam is a pointer even if wParam == 1. Recorder passes
            //  the data so.    - nandurir

            {
                INT cItems = INT32(lpmpex->Parm16.WndProc.wParam);
                if (cItems > 0) {
                    (PVOID)lpmpex->lParam = STACKORHEAPALLOC(cItems * sizeof(INT), 
                                   sizeof(lpmpex->MsgBuffer), lpmpex->MsgBuffer);
                    /* DIVERGENCE(MVDM-HOST-DIV-346): reject failed acquisition. */
                    if (!lpmpex->lParam) return FALSE;
                    getintarray16((VPINT16)lpmpex->Parm16.WndProc.lParam, cItems, (LPINT)lpmpex->lParam);
                }
            }
            break;
    
        case LB_ADDSTRING + 3:
            lpmpex->uMsg = 0;
            break;
    
        }
    }
    return TRUE;
}


VOID FASTCALL UnThunkLBMsg16(LPMSGPARAMEX lpmpex)
{
    switch(lpmpex->uMsg) {

    case LB_GETTEXT:
        {
           register PTHUNKTEXTDWORD pthkdword = (PTHUNKTEXTDWORD)lpmpex->MsgBuffer;

           if (pthkdword->fDWORD) { 
                // this is a dword, not a string
                // assign the dword as unaligned
                UNALIGNED DWORD *lpdwDataItem;

                GETVDMPTR((lpmpex->Parm16.WndProc.lParam), sizeof(DWORD), lpdwDataItem);
                *lpdwDataItem = pthkdword->dwDataItem;
                FREEVDMPTR(lpdwDataItem);
                break;
           }
 
        }

        // fall through to the common code


    case LB_ADDSTRING:      // BUGBUG 3-Jul-1991 JeffPar: for owner-draw list boxes, this can just be a 32-bit number
    case LB_DIR:
    case LB_FINDSTRING:     // BUGBUG 3-Jul-1991 JeffPar: for owner-draw list boxes, this can just be a 32-bit number
    case LB_FINDSTRINGEXACT:
    case LB_INSERTSTRING:
    case LB_SELECTSTRING:
        FREEPSZPTR((LPSZ)lpmpex->lParam);
        break;

    case LB_GETITEMRECT:
        if ((lpmpex->lParam) && (lpmpex->lReturn != -1L)) {
            putrect16((VPRECT16)lpmpex->Parm16.WndProc.lParam, (LPRECT)lpmpex->lParam);
        }
        break;

    case LB_GETSELITEMS:
        PUTINTARRAY16V((VPINT16)lpmpex->Parm16.WndProc.lParam, (INT)(lpmpex->lReturn), (LPINT)lpmpex->lParam);
        STACKORHEAPFREE((LPINT)lpmpex->lParam, lpmpex->MsgBuffer);
        break;

    case LB_SETTABSTOPS:
        /* DIVERGENCE(MVDM-HOST-DIV-346): same signed acquisition predicate. */
        if (INT32(lpmpex->Parm16.WndProc.wParam) > 0) {
            STACKORHEAPFREE((LPINT)lpmpex->lParam, lpmpex->MsgBuffer);
        }
        break;
    }
}
