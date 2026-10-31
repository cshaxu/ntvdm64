/****************************** Module Header ******************************\
* Module Name: getset.c
*
* Copyright (c) 1985-91, Microsoft Corporation
*
* This module contains window manager information routines
*
* History:
* 10-22-90 MikeHar      Ported functions from Win 3.0 sources.
* 13-Feb-1991 mikeke    Added Revalidation code (None)
* 08-Feb-1991 IanJa     Unicode/ANSI aware and neutral
\***************************************************************************/

/* DIVERGENCE(OPENNT-HOST-085): complete _GetWindowLong only.
 * Original body unchanged; scoped retained WND and finite private bindings.
 * No native modern USER object-layout overlay or server import. */
#include "precomp.h"
#pragma hdrstop

DWORD GetWindowData(PWND, int, BOOL);

DWORD _GetWindowLong(
    PWND pwnd,
    int index,
    BOOL bAnsi)
{
    DWORD           dwProc;
    DWORD           dwCPDType = 0;
    DWORD UNALIGNED *pudw;

    /*
     * If it's a dialog window, only a few indices are permitted.
     */
    if (GETFNID(pwnd) != 0) {
        if (TestWF(pwnd, WFDIALOGWINDOW)) {
            switch (index) {
            case DWL_DLGPROC:     // See similar case GWL_WNDGPROC

                /*
                 * Hide the window proc from other processes
                 */
                if (!TestWindowProcess(pwnd)) {
                    RIPERR1(ERROR_ACCESS_DENIED,
                            RIP_WARNING,
                            "Access denied to \"pwnd\" (%#lx) in _GetWindowLong",
                            pwnd);

                    return 0;
                }

#ifdef WOW_NATIVE_DIALOG_PROCEDURE_OWNER
                /* DIVERGENCE(OPENNT-HOST-121): public native slot, not CPD layout. */
                return wow_dialog_host_get(HW(pwnd), index, bAnsi);
#else
                dwProc = (DWORD)PDLG(pwnd)->lpfnDlg;

                /*
                 * If a proc exists check it to see if we need a translation
                 */
                if (dwProc) {

                    /*
                     * May need to return a CallProc handle if there is an
                     * Ansi/Unicode transition
                     */
                    if (bAnsi != ((PDLG(pwnd)->flags & DLGF_ANSI) ? TRUE : FALSE)) {
                        dwCPDType |= bAnsi ? CPD_ANSI_TO_UNICODE : CPD_UNICODE_TO_ANSI;
                    }

                    if (dwCPDType) {
                        DWORD cpd;

                        cpd = (DWORD)GetCPD(pwnd, dwCPDType | CPD_DIALOG, dwProc);

                        if (cpd) {
                            dwProc = cpd;
                        } else {
                            RIPMSG0(RIP_WARNING, "GetWindowLong unable to alloc CPD returning handle\n");
                        }
                    }
                }

                /*
                 * return proc (or CPD handle)
                 */
                return dwProc;
#endif

            case DWL_MSGRESULT:
#ifdef WOW_NATIVE_DIALOG_PROCEDURE_OWNER
                 return wow_dialog_host_get(HW(pwnd), index, bAnsi);
#else
                 return (DWORD)((PDIALOG)pwnd)->resultWP;
#endif

            case DWL_USER:
                 return (DWORD)((PDIALOG)pwnd)->unused;

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
                case FNID_MDICLIENT:
                    /*
                     * Allow the 0 index (which is reserved) to be set/get.
                     * Quattro Pro 1.0 uses this index!
                     */
                    if (index != 0)
                        break;

                    goto GetData;
                    break;

                case FNID_EDIT:

                    if (index != 0)
                        break;

                    /*
                     * If we get to this point we need to return the first
                     * entry in the lookaside.  This will provide backward
                     * compatibilty for 3.51 that allowed edit-controls to
                     * do this.  PeachTree is one app which required this.
                     */
                    pudw = (DWORD UNALIGNED *)((BYTE *)(pwnd + 1));

                    /*
                     * Do not dereference the pointer if we are not in
                     *  the proper address space. Apps like Spyxx like to
                     *  do this on other process' windows
                     */
                    return (TestWindowProcess(pwnd) ? *(DWORD UNALIGNED *)*pudw : (DWORD)pudw);

                }

#ifndef _USERK_
                RIPERR3(ERROR_INVALID_INDEX,
                        RIP_WARNING,
                        "GetWindowLong: Trying to read private server data pwnd=(%lX) index=(%ld) fnid (%lX)",
                        pwnd, index, (DWORD)pwnd->fnid);
                return 0;
#endif
            }
        }
    }

    if (index < 0) {
#ifdef WOW_ORIGINAL_PRIVATE_WORD_GETTER
        /* DIVERGENCE(OPENNT-HOST-085): production selects only private
         * extra-word access. Its sole caller rejects negative indices;
         * public negative access keeps its existing owner. Do not import
         * the unused NT4 server/client-procedure mapping closure. */
        RIPERR0(ERROR_INVALID_INDEX, RIP_VERBOSE, "");
        return 0;
#else
        return GetWindowData(pwnd, index, bAnsi);
#endif
    } else {
        if (index + (int)sizeof(DWORD) > pwnd->cbwndExtra) {
            RIPERR0(ERROR_INVALID_INDEX, RIP_VERBOSE, "");
            return 0;
        } else {

GetData:
            pudw = (DWORD UNALIGNED *)((BYTE *)(pwnd + 1) + index);
            return *pudw;
        }
    }
}
