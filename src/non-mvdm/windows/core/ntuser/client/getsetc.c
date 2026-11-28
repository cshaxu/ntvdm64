/****************************** Module Header ******************************\
* Module Name: getsetc.c
*
* Copyright (c) 1985 - 1999, Microsoft Corporation
*
* This module contains window manager information routines
*
* History:
* 10-Mar-1993 JerrySh   Pulled functions from user\server.
\***************************************************************************/

#include "precomp.h"
#pragma hdrstop

/***************************************************************************\
* _GetWindowWord (supports the GetWindowWord API)
*
* Return a window word.  Positive index values return application window words
* while negative index values return system window words.  The negative
* indices are published in WINDOWS.H.
*
* History:
* 11-26-90 darrinm      Wrote.
\***************************************************************************/

WORD _GetWindowWord(
    PWND pwnd,
    int index)
{
    if (GETFNID(pwnd) != 0) {
        if ((index >= 0) && (index <
                (int)(CBFNID(pwnd->fnid)-sizeof(WND)))) {

            switch (GETFNID(pwnd)) {
            case FNID_MDICLIENT:
                if (index == 0)
                    break;
                goto DoDefault;

            case FNID_BUTTON:
                /*
                 * CorelDraw does a get/set on the first button window word.
                 * Allow it to.
                 */
                if (index == 0) {
                    /*
                     *  Since we now use a lookaside buffer for the control's
                     *  private data, we need to indirect into this structure.
                     */
                    PBUTN pbutn = ((PBUTNWND)pwnd)->pbutn;
                    if (!pbutn || (LONG_PTR)pbutn == (LONG_PTR)-1) {
                        return 0;
                    } else {
                        return (WORD)(pbutn->buttonState);
                    }
                }
                goto DoDefault;

            case FNID_DIALOG:
                if (index == DWLP_USER)
                    return LOWORD(((PDIALOG)pwnd)->unused);
                if (index == DWLP_USER+2)
                    return HIWORD(((PDIALOG)pwnd)->unused);
                goto DoDefault;

            default:
DoDefault:
                RIPERR3(ERROR_INVALID_INDEX,
                        RIP_WARNING,
                        "GetWindowWord: Trying to read private server data pwnd=(%#p) index=(%ld) fnid=(%lX)",
                        pwnd, index, (DWORD)pwnd->fnid);
                return 0;
                break;
            }
        }
    }

    if (index == GWLP_USERDATA)
        return (WORD)pwnd->dwUserData;

    if ((index < 0) || ((UINT)index + sizeof(WORD) > (UINT)pwnd->cbwndExtra)) {
        RIPERR0(ERROR_INVALID_INDEX, RIP_VERBOSE, "");
        return 0;
    } else {
        return *((WORD UNALIGNED * KPTR_MODIFIER)((KPBYTE)(pwnd + 1) + index));
    }
}

ULONG_PTR GetWindowData(PWND pwnd, int index, BOOL bAnsi);

/***************************************************************************\
* _GetWindowLong (supports GetWindowLongA/W API)
*
* Return a window long.  Positive index values return application window longs
* while negative index values return system window longs.  The negative
* indices are published in WINDOWS.H.
*
* History:
* 11-26-90 darrinm      Wrote.
\***************************************************************************/

ULONG_PTR _GetWindowLongPtr(
    PWND pwnd,
    int index,
    BOOL bAnsi)
{
    ULONG_PTR           dwProc;
    DWORD              dwCPDType = 0;
    ULONG_PTR UNALIGNED * KPTR_MODIFIER pudw;

    /*
     * If it's a dialog window, only a few indices are permitted.
     */
    if (GETFNID(pwnd) != 0) {
        if (TestWF(pwnd, WFDIALOGWINDOW)) {
            switch (index) {
            case DWLP_DLGPROC:    // See similar case GWLP_WNDGPROC

                /*
                 * Hide the window proc from other processes
                 */
                if (!TestWindowProcess(pwnd)) {
                    RIPERR1(ERROR_ACCESS_DENIED,
                            RIP_WARNING,
                            "Access denied to \"pwnd\" (%#p) in _GetWindowLong",
                            pwnd);

                    return 0;
                }

#ifdef WOW_NATIVE_DIALOG_PROCEDURE_OWNER
                return wow_dialog_host_get(HW(pwnd), index, bAnsi);
#else
#line 131
                dwProc = (ULONG_PTR)PDLG(pwnd)->lpfnDlg;

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
                        ULONG_PTR cpd;

                        cpd = GetCPD(pwnd, dwCPDType | CPD_DIALOG, dwProc);

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
#line 163

            case DWLP_MSGRESULT:
#ifdef WOW_NATIVE_DIALOG_PROCEDURE_OWNER
                 return wow_dialog_host_get(HW(pwnd), index, bAnsi);
#else
#line 165
                 return (ULONG_PTR)((PDIALOG)pwnd)->resultWP;
#endif
#line 166

            case DWLP_USER:
                 return (ULONG_PTR)((PDIALOG)pwnd)->unused;

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
                    pudw = (ULONG_PTR UNALIGNED * KPTR_MODIFIER)((KPBYTE)(pwnd + 1));

                    /*
                     * Do not dereference the pointer if we are not in
                     *  the proper address space. Apps like Spyxx like to
                     *  do this on other process' windows
                     */
                    return (TestWindowProcess(pwnd) ? *(ULONG_PTR UNALIGNED *)*pudw : (ULONG_PTR)pudw);

                }

                RIPERR3(ERROR_INVALID_INDEX,
                        RIP_WARNING,
                        "GetWindowLong: Trying to read private server data pwnd=(%#p) index=(%ld) fnid (%lX)",
                        pwnd, index, (DWORD)pwnd->fnid);
                return 0;
            }
        }
    }

    if (index < 0) {
#ifdef WOW_ORIGINAL_PRIVATE_WORD_GETTER
        RIPERR0(ERROR_INVALID_INDEX, RIP_VERBOSE, "");
        return 0;
#else
#line 223
        return GetWindowData(pwnd, index, bAnsi);
#endif
#line 224
    } else {
        if ((UINT)index + sizeof(ULONG_PTR) > (UINT)pwnd->cbwndExtra) {
            RIPERR0(ERROR_INVALID_INDEX, RIP_VERBOSE, "");
            return 0;
        } else {

GetData:
            pudw = (ULONG_PTR UNALIGNED * KPTR_MODIFIER)((KPBYTE)(pwnd + 1) + index);
            return *pudw;
        }
    }
}


#ifdef _WIN64
#error Excluded unsupported original _WIN64 or GENERIC_INPUT branch
#endif
#line 334


/***************************************************************************\
* GetWindowData
*
* History:
* 11-26-90 darrinm      Wrote.
\***************************************************************************/

#line 491
#ifdef GENERIC_INPUT
#error Excluded unsupported original _WIN64 or GENERIC_INPUT branch
#endif // GENERIC_INPUT
#line 515

