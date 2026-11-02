/*++
 *
 *  WOW v1.0
 *
 *  Copyright (c) 1991, Microsoft Corporation
 *
 *  WMSGBM.C
 *  WOW32 16-bit message thunks
 *
 *  History:
 *  Created 11-Mar-1991 by Jeff Parsons (jeffpar)
--*/


#include "precomp.h"
#pragma hdrstop

MODNAME(wmsgbm.c);


#ifdef DEBUG
#error Excluded unsupported DEBUG WOW profile
#endif
#line 43


BOOL FASTCALL ThunkBMMsg16(LPMSGPARAMEX lpmpex)
{
    WORD wMsg = lpmpex->Parm16.WndProc.wMsg;
    LOGDEBUG(7,("    Thunking 16-bit button message %s(%04x)\n", (LPSZ)GetBMMsgName(wMsg), wMsg));

    //
    // special case BM_CLICK
    //

    if (wMsg == WIN31_BM_CLICK) {
        lpmpex->uMsg = BM_CLICK;
    }
    else {
        wMsg -= WM_USER;
    
        //
        // For app defined (control) messages that are out of range
        // return TRUE.
        //
        // ChandanC Sept-15-1992
        //
    
        if (wMsg < (BM_SETSTYLE - BM_GETCHECK + 1)) {
            lpmpex->uMsg = wMsg + BM_GETCHECK;

            // The following messages should not require thunking, because
            // they contain no pointers, handles, or rearranged message parameters,
            // so consequently they are not documented in great detail here:
            //
            // BM_GETCHECK
            // BM_GETSTATE
            // BM_SETCHECK
            // BM_SETSTATE
            // BM_SETSTYLE
            //
            // And these I haven't seen documentation for yet (new for Win32???)
            //
            // BM_GETIMAGE
            // BM_SETIMAGE

            // switch(lpmpex->uMsg) {
            //          NO BM_ message needs thunking
            // }

        }
    }
    return TRUE;
}


VOID FASTCALL UnThunkBMMsg16(LPMSGPARAMEX lpmpex)
{
}
