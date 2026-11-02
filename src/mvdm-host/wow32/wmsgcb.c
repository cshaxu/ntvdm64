/*++
 *
 *  WOW v1.0
 *
 *  Copyright (c) 1991, Microsoft Corporation
 *
 *  WMSGCB.C
 *  WOW32 16-bit message thunks
 *
 *  History:
 *  Created 11-Mar-1991 by Jeff Parsons (jeffpar)
--*/


#include "precomp.h"
#pragma hdrstop
#include "wow_message_lifetime.h" /* DIVERGENCE(MVDM-HOST-DIV-347) */

MODNAME(wmsgcb.c);


#ifdef DEBUG
#error Excluded unsupported DEBUG WOW profile
#endif
#line 64



BOOL FASTCALL ThunkCBMsg16(LPMSGPARAMEX lpmpex)
{
    register PWW pww;
    WORD wMsg = lpmpex->Parm16.WndProc.wMsg;

    LOGDEBUG(7,("    Thunking 16-bit combo box message %s(%04x)\n", (LPSZ)GetCBMsgName(wMsg), wMsg));

    // Sudeepb - 04-Mar-1996
    // Fix the broken thunking for CBEC_SETCOMBOFOCUS and CBEC_KILLCOMBOFOCUS.
    // It was broken when NT user merged with Win95 where CB_MAX has changed.
    // the following code is written in such a manner that the only dependency
    // we have is that CBEC_SETCOMBOFOCUS precedes CBEC_KILLCOMBOFOCUS which
    // will always be true.


    if (wMsg == OLDCBEC_SETCOMBOFOCUS || wMsg == OLDCBEC_KILLCOMBOFOCUS) {
        lpmpex->uMsg = (WORD)(wMsg - OLDCBEC_SETCOMBOFOCUS + CBEC_SETCOMBOFOCUS);
        return  TRUE;
    }

    wMsg -= WM_USER;

    //
    // For app defined (control) messages that are out of range
    // return TRUE.
    //
    // ChandanC Sept-15-1992
    //

    if (wMsg < (CB_FINDSTRINGEXACT - CB_GETEDITSEL + 4)) {

        switch(lpmpex->uMsg = wMsg + CB_GETEDITSEL) {
            case CB_SELECTSTRING:
            case CB_FINDSTRINGEXACT:
            case CB_FINDSTRING:
            case CB_INSERTSTRING:
            case CB_ADDSTRING:
                if (!(pww = lpmpex->pww))
                    return FALSE;
        
                if (!(pww->dwStyle & (CBS_OWNERDRAWFIXED|CBS_OWNERDRAWVARIABLE)) ||
                       (pww->dwStyle & (CBS_HASSTRINGS))) {
                    GETPSZPTR(lpmpex->Parm16.WndProc.lParam, (LPSZ)lpmpex->lParam);
                }
                break;
        
        
            case CB_GETLBTEXT:
                if (NULL != (pww = lpmpex->pww)) {
	                register PTHUNKTEXTDWORD pthkdword = (PTHUNKTEXTDWORD)lpmpex->MsgBuffer;

	                //  see comments in the file wmsglb.c 
                    //

	                pthkdword->fDWORD = (pww->dwStyle & (CBS_OWNERDRAWFIXED|CBS_OWNERDRAWVARIABLE)) &&
	                                    !(pww->dwStyle & (CBS_HASSTRINGS));
	
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
	 
            case CB_DIR:
                /* DIVERGENCE(MVDM-HOST-DIV-347): optional map acquisition. */
                lpmpex->MsgBuffer->text.flags = 0;
                GETPSZPTR(lpmpex->Parm16.WndProc.lParam, (LPSZ)lpmpex->lParam);
                lpmpex->MsgBuffer->text.native = lpmpex->lParam;
                if (lpmpex->lParam) lpmpex->MsgBuffer->text.flags = WOW_TEXT_ACTIVE;

                if (W32CheckThunkParamFlag()) {
                    if (AddParamMap(lpmpex->lParam, lpmpex->Parm16.WndProc.lParam))
                        lpmpex->MsgBuffer->text.flags |= WOW_TEXT_MAPPED;
                }
                break;

            case CB_GETDROPPEDCONTROLRECT:
                lpmpex->lParam = (LONG)lpmpex->MsgBuffer;
                break;
        }
    }
    return TRUE;
}


VOID FASTCALL UnThunkCBMsg16(LPMSGPARAMEX lpmpex)
{
    /* DIV-347: native filters may replace uMsg/lParam, not our acquisition. */
    if (lpmpex->Parm16.WndProc.wMsg == OLDCB_DIR) {
        DWORD owned = lpmpex->MsgBuffer->text.flags;
        lpmpex->MsgBuffer->text.flags = 0;
        if (owned & WOW_TEXT_MAPPED)
            DeleteParamMap(lpmpex->Parm16.WndProc.lParam, PARAM_16, NULL);
        if (owned & WOW_TEXT_ACTIVE) FREEPSZPTR((LPSZ)lpmpex->MsgBuffer->text.native);
        return;
    }
    switch(lpmpex->uMsg) {

        case CB_GETLBTEXT:
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

        case CB_ADDSTRING:
        case CB_FINDSTRING:
        case CB_FINDSTRINGEXACT:
        case CB_INSERTSTRING:
        case CB_SELECTSTRING:
            FREEPSZPTR((LPSZ)lpmpex->lParam);
            break;

        case CB_GETDROPPEDCONTROLRECT:
            if (lpmpex->lParam) {
                putrect16((VPRECT16)lpmpex->Parm16.WndProc.lParam, (LPRECT)lpmpex->lParam);
            }
            break;
    }
}
