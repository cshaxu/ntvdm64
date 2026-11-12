/****************************** Module Header ******************************\
* Module Name: getset.c
*
* Copyright (c) 1985-1995, Microsoft Corporation
*
* This module contains window manager information routines
*
* History:
* 22-Oct-1990 MikeHar   Ported functions from Win 3.0 sources.
* 13-Feb-1991 MikeKe    Added Revalidation code (None)
* 08-Feb-1991 IanJa     Unicode/ANSI aware and neutral
\***************************************************************************/

/* DIVERGENCE(OPENNT-HOST-086): complete xxxSetWindowLong only.
 * Retained local WND/control state; no USER server runtime import. */
#line 14
#include "precomp.h"
#pragma hdrstop

/***************************************************************************\
* xxxSetWindowLong (API)
*
* Set a window long.  Positive index values set application window longs
* while negative index values set system window longs.  The negative
* indices are published in WINDOWS.H.
*
* History:
* 26-Nov-1990 DarrinM   Wrote.
\***************************************************************************/

#line 256
DWORD xxxSetWindowLong(
    PWND  pwnd,
    int   index,
    DWORD dwData,
    BOOL  bAnsi)
{
    DWORD dwOld;
    DWORD dwCPDType = 0;

    /*
     * Hide the window proc from other processes
     */
#line 268
#ifdef DEBUG
#line 269
#error DIVERGENCE OPENNT-HOST-146: DEBUG user profile is not selected
#line 272
#endif
#line 273

    /*
     * CheckLock(pwnd);  The only case that leaves the critical section is
     * where xxxSetWindowData is called, which does this.  Saves us some locks.
     *
     *
     * Don't allow setting of words belonging to a system thread if the caller
     * is not a system thread. Same goes for winlogon.
     */
    if (!FCallerOk(pwnd)) {
        RIPERR0(ERROR_INVALID_INDEX, RIP_VERBOSE, "");
        return 0;
    }

    /*
     * If it's a dialog window, only a few indices are permitted.
     */
    if (GETFNID(pwnd) != 0) {
        if (TestWF(pwnd, WFDIALOGWINDOW)) {
            switch (index) {
            case DWL_MSGRESULT:
#ifdef WOW_NATIVE_DIALOG_PROCEDURE_OWNER
                 /* DIVERGENCE(OPENNT-HOST-121): same slot as native callbacks. */
                 return wow_dialog_host_set(HW(pwnd), index, dwData, bAnsi);
#else
#line 294
                 dwOld = (DWORD)((PDIALOG)(pwnd))->resultWP;
                 ((PDIALOG)(pwnd))->resultWP = (long)dwData;
                 return dwOld;
#endif

#line 298
            case DWL_USER:
                 dwOld = (DWORD)((PDIALOG)(pwnd))->unused;
                 ((PDIALOG)(pwnd))->unused = (long)dwData;
                 return dwOld;

            default:
                if (index >= 0 && index < DLGWINDOWEXTRA) {
                    RIPERR0(ERROR_PRIVATE_DIALOG_INDEX, RIP_VERBOSE, "");
                    return 0;
                }
            }
        } else {
            if (index >= 0 &&
                    (index < (int)(CBFNID(pwnd->fnid)-sizeof(WND)))) {
                switch (GETFNID(pwnd)) {
                case FNID_BUTTON:
                case FNID_COMBOBOX:
                case FNID_COMBOLISTBOX:
                case FNID_DIALOG:
                case FNID_LISTBOX:
                case FNID_STATIC:
                case FNID_EDIT:
#line 320
#ifdef FE_IME
#line 321
#error DIVERGENCE OPENNT-HOST-132: FE_IME branch is not selected; see mirror register.
#endif
#line 323
                    /*
                     * Allow the 0 index for controls to be set if it's
                     * still NULL or the window is being destroyed. This
                     * is where controls store their private data.
                     */
                    if (index == 0) {
                        dwOld = *((LPDWORD)(pwnd + 1));
                        if (dwOld == 0 || TestWF(pwnd, WFDESTROYED))
                            goto SetData;
                    }
                    break;

                case FNID_MDICLIENT:
                    /*
                     * Allow the 0 index (which is reserved) to be set/get.
                     * Quattro Pro 1.0 uses this index!
                     *
                     * Allow the 4 index to be set if it's still NULL or
                     * the window is being destroyed. This is where we
                     * store our private data.
                     */
                    if (index == 0) {
                        goto SetData;
                    }
                    if (index == 4) {
                        dwOld = *((LPDWORD)(pwnd + 1));
                        if (dwOld == 0 || TestWF(pwnd, WFDESTROYED))
                            goto SetData;
                    }
                    break;
                }

                RIPERR3(ERROR_INVALID_INDEX,
                        RIP_WARNING,
                        "SetWindowLong: Trying to set private server data pwnd=(%lX) index=(%ld) FNID=(%lX)",
                        pwnd, index, (DWORD)pwnd->fnid);
                return 0;
            }
        }
    }

    if (index < 0) {
        return xxxSetWindowData(pwnd, index, dwData, bAnsi);
    } else {
        if (index + (int)sizeof(DWORD) > pwnd->cbwndExtra) {
            RIPERR0(ERROR_INVALID_INDEX, RIP_VERBOSE, "");
            return 0;
        } else {
            DWORD UNALIGNED *pudw;

SetData:
            pudw = (DWORD UNALIGNED *)((BYTE *)(pwnd + 1) + index);
            dwOld = *pudw;
            *pudw = dwData;
            return dwOld;
        }
    }
}


#line 749
#ifdef DEBUG
#line 750
#error Excluded unsupported original DEBUG branch
#endif
#line 752

#line 759
#ifdef DEBUG
#line 760
#error Excluded unsupported original DEBUG branch
#line 769
#endif
#line 770

#line 776
#ifdef DEBUG
#line 777
#error Excluded unsupported original DEBUG branch
#endif
#line 779

#line 781
#ifdef DEBUG
#line 782
#error Excluded unsupported original DEBUG branch
#line 793
#endif // DEBUG
#line 794

