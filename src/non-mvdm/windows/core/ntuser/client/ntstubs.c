/* DIVERGENCE(OPENNT-HOST-118): unchanged original stored-text wrapper. */
#if defined(WOW_ORIGINAL_CONTROL_TEXT)
#include "precomp.h"
#include "../inc/ntsend.h"

BOOL _DefSetText(
    HWND hwnd,
    LPCWSTR lpszText,
    BOOL bAnsi)
{
    LARGE_STRING str;

    BEGINCALL()

        if (lpszText) {
            if (bAnsi)
                RtlInitLargeAnsiString((PLARGE_ANSI_STRING)&str,
                        (LPSTR)lpszText, (UINT)-1);
            else
                RtlInitLargeUnicodeString((PLARGE_UNICODE_STRING)&str,
                        lpszText, (UINT)-1);
        }

        retval = (DWORD)NtUserDefSetText(
                hwnd,
                lpszText ? &str : NULL);

    ERRORTRAP(0);
    ENDCALL(BOOL);
}

/* OPENNT-HOST-098: original explicit-recipient control color entry. */
#elif defined(WOW_ORIGINAL_CONTROL_COLOR) || defined(WOW_ORIGINAL_CONTROL_BRUSH_CLIENT)
#include "wow_edit_call_bindings.h"
#include "../inc/ntsend.h"

#ifdef WOW_ORIGINAL_CONTROL_BRUSH_CLIENT
/* OPENNT-HOST-102: unchanged implicit-recipient client wrapper. */
HBRUSH GetControlBrush(
    HWND hwnd,
    HDC hdc,
    UINT msg)
{

    BEGINCALL()
        FIXUP_HANDLE(hdc);

        if (hdc == (HDC)0)
            MSGERROR();

        retval = (DWORD)NtUserGetControlBrush(hwnd, hdc, msg);

    ERRORTRAP(0);
    ENDCALL(HBRUSH);
}

#else
HBRUSH GetControlColor(
    HWND hwndParent,
    HWND hwndCtl,
    HDC hdc,
    UINT msg)
{
    BEGINCALL()

        FIXUP_HANDLE(hdc);

        retval = (DWORD)NtUserGetControlColor(
                hwndParent,
                hwndCtl,
                hdc,
                msg);

    ERRORTRAP(0);
    ENDCALL(HBRUSH);
}
#endif
/* OPENNT-HOST-086: retain the complete dialog get/set client owner. */
#elif defined(WOW_ORIGINAL_DIALOG_GETSET)
#include "precomp.h"
#pragma hdrstop

LONG _SetWindowLong(
    HWND hwnd,
    int nIndex,
    LONG dwNewLong,
    BOOL bAnsi)
{
    PWND pwnd;
    LONG dwOldLong;
    DWORD dwCPDType = 0;

    pwnd = ValidateHwnd(hwnd);

    if (pwnd == NULL)
        return 0;

    if (TestWF(pwnd, WFDIALOGWINDOW)) {
        switch (nIndex) {
        case DWL_DLGPROC:     // See similar case GWL_WNDGPROC

            /*
             * Hide the window proc from other processes
             */
            if (!TestWindowProcess(pwnd)) {
                RIPERR1(ERROR_ACCESS_DENIED,
                        RIP_WARNING,
                        "Access denied to hwnd (%#lx) in _SetWindowLong",
                        hwnd);

                return 0;
            }

#ifdef WOW_NATIVE_DIALOG_PROCEDURE_OWNER
            /* DIVERGENCE(OPENNT-HOST-121): native HWND owns opaque A/W
             * procedure storage. Never decode a modern token as NT4 CPD. */
            return wow_dialog_host_set(hwnd, nIndex, dwNewLong, bAnsi);
#else
            /*
             * Get the old window proc address
             */
            dwOldLong = (LONG)PDLG(pwnd)->lpfnDlg;

            /*
             * We always store the actual address in the wndproc; We only
             * give the CallProc handles to the application
             */
            UserAssert(!ISCPDTAG(dwOldLong));

            /*
             * May need to return a CallProc handle if there is an
             * Ansi/Unicode tranistion
             */

            if (bAnsi != ((PDLG(pwnd)->flags & DLGF_ANSI) ? TRUE : FALSE)) {
                dwCPDType |= bAnsi ? CPD_ANSI_TO_UNICODE : CPD_UNICODE_TO_ANSI;
            }

            /*
             * If we detected a transition create a CallProc handle for
             * this type of transition and this wndproc (dwOldLong)
             */
            if (dwCPDType) {
                DWORD cpd;

                cpd = GetCPD(pwnd, dwCPDType | CPD_DIALOG, dwOldLong);

                if (cpd) {
                    dwOldLong = cpd;
                } else {
                    RIPMSG0(RIP_WARNING, "SetWindowLong (DWL_DLGPROC) unable to alloc CPD returning handle\n");
                }
            }

            /*
             * Convert a possible CallProc Handle into a real address.
             * The app may have kept the CallProc Handle from some
             * previous mixed GetClassinfo or SetWindowLong.
             *
             * WARNING bAnsi is modified here to represent real type of
             * proc rather than if SetWindowLongA or W was called
             */
            if (ISCPDTAG(dwNewLong)) {
                PCALLPROCDATA pCPD;
                if (pCPD = HMValidateHandleNoRip((HANDLE)dwNewLong, TYPE_CALLPROC)) {
                    dwNewLong = pCPD->pfnClientPrevious;
                    bAnsi = pCPD->wType & CPD_UNICODE_TO_ANSI;
                }
            }

            /*
             * If an app 'unsubclasses' a server-side window proc we need to
             * restore everything so SendMessage and friends know that it's
             * a server-side proc again.  Need to check against client side
             * stub addresses.
             */
            PDLG(pwnd)->lpfnDlg = (WNDPROC)dwNewLong;
            if (bAnsi) {
                PDLG(pwnd)->flags |= DLGF_ANSI;
            } else {
                PDLG(pwnd)->flags &= ~DLGF_ANSI;
            }

            return dwOldLong;
#endif
        case DWL_MSGRESULT:
        case DWL_USER:
            break;

        default:
            if (nIndex >= 0 && nIndex < DLGWINDOWEXTRA) {
                RIPERR0(ERROR_PRIVATE_DIALOG_INDEX, RIP_VERBOSE, "");
                return 0;
            }
        }
    }

    BEGINCALL()

    /*
     * If this is a listbox window and the listbox structure has
     * already been initialized, don't allow the app to override the
     * owner draw styles. We need to do this since Windows only
     * used the styles in creating the structure, but we also use
     * them to determine if strings need to be thunked.
     *
     */

    if (nIndex == GWL_STYLE &&
        GETFNID(pwnd) == FNID_LISTBOX &&
        ((PLBWND)pwnd)->pLBIV != NULL &&
        (!TestWindowProcess(pwnd) || ((PLBWND)pwnd)->pLBIV->fInitialized)) {

#endif /* original client-owner selection */
#line 953
#ifdef DEBUG
#error Excluded unsupported original DEBUG window-style tracing
#endif
#if defined(WOW_ORIGINAL_DIALOG_GETSET) && !defined(WOW_ORIGINAL_CONTROL_TEXT) && !defined(WOW_ORIGINAL_CONTROL_COLOR) && !defined(WOW_ORIGINAL_CONTROL_BRUSH_CLIENT)
#line 956

        dwNewLong &= ~(LBS_OWNERDRAWFIXED |
                       LBS_OWNERDRAWVARIABLE |
                       LBS_HASSTRINGS);

        dwNewLong |= pwnd->style & (LBS_OWNERDRAWFIXED |
                                    LBS_OWNERDRAWVARIABLE |
                                    LBS_HASSTRINGS);

#endif /* WOW_ORIGINAL_DIALOG_GETSET */
#line 965
#ifdef DEBUG
#error Excluded unsupported original DEBUG window-style tracing
#endif
#if defined(WOW_ORIGINAL_DIALOG_GETSET) && !defined(WOW_ORIGINAL_CONTROL_TEXT) && !defined(WOW_ORIGINAL_CONTROL_COLOR) && !defined(WOW_ORIGINAL_CONTROL_BRUSH_CLIENT)
#line 970
    }


        retval = (DWORD)NtUserSetWindowLong(
                hwnd,
                nIndex,
                dwNewLong,
                bAnsi);

    ERRORTRAP(0);
    ENDCALL(LONG);
}
#elif !defined(WOW_ORIGINAL_CONTROL_TEXT) && !defined(WOW_ORIGINAL_CONTROL_COLOR) && !defined(WOW_ORIGINAL_CONTROL_BRUSH_CLIENT)
/**************************************************************************\
* Module Name: ntstubs.c
*
* Copyright (c) Microsoft Corp. 1990 All Rights Reserved
*
* client side API stubs
*
* History:
* 03-19-95 JimA             Created.
\**************************************************************************/

/* DIVERGENCE(OPENNT-HOST-041): retain original GetClassWOWWords only.
 * Other client services and the private precompiled environment are excluded.
 * A synchronous native class borrow replaces the private shared CLS view. */
#define WOW_ORIGINAL_CLASS_QUERY
#include "wow_class_query_bindings.h"
#include "../inc/ntsend.h"

LONG GetClassWOWWords(
    HINSTANCE hInstance,
    LPCTSTR pString)
{
    IN_STRING strClassName;
    PCLS pcls;

    /*
     * Make sure cleanup will work successfully
     */
    strClassName.fAllocated = FALSE;

    BEGINCALL()

        FIRSTCOPYLPSTRW(&strClassName, pString);

        pcls = NtUserGetWOWClass(hInstance, strClassName.pstr);

        if (pcls == NULL) {
            MSGERRORCODE(ERROR_CLASS_DOES_NOT_EXIST);
        }

        pcls = (PCLS)((PBYTE)pcls - GetClientInfo()->ulClientDelta);
        retval = _GetClassData(pcls, NULL, GCL_WOWWORDS, TRUE);

    ERRORTRAP(0);
    CLEANUPLPSTRW(strClassName);
    ENDCALL(LONG);
}
#endif /* WOW_ORIGINAL_DIALOG_GETSET */
#line 380
#ifdef MEMPHIS_MENUS
#error Excluded unsupported MEMPHIS_MENUS client owner
#else // MEMPHIS_MENUS
#line 440
#endif // MEMPHIS_MENUS
#line 500
#ifdef MEMPHIS_MENU_WATERMARKS
#error Excluded unsupported MEMPHIS_MENU_WATERMARKS client owner
#endif // MEMPHIS_MENU_WATERMARKS
#line 1611
#ifdef FE_SB // fnINWPARAMDBCSCHAR()
#error Excluded unsupported FE_SB client message owner
#endif // FE_SB


#line 693
#ifdef FE_SB // _PeekMessage()
#line 694
#error Excluded unsupported original FE_SB branch
#endif // FE_SB
#line 702

#line 797
#ifdef FE_SB // _PeekMessage()
#line 798
#error Excluded unsupported original FE_SB branch
#endif // FE_SB
#line 800


#line 753
#ifdef FE_SB // _PeekMessage()
#line 754
#error Excluded unsupported original FE_SB branch
#else
#line 759
#endif // FE_SB
#line 767

#line 769
#ifdef FE_SB // _PeekMessage()
#line 770
#error Excluded unsupported original FE_SB branch
#else
#line 780
#endif // FE_SB
#line 783

#line 784
#ifdef FE_SB // _PeekMessage()
#line 785
#error Excluded unsupported original FE_SB branch
#else
#line 793
#endif // FE_SB
#line 795


#line 358
#ifdef LATER
#line 359
#error Excluded unsupported original LATER branch
#endif
#line 369


#line 2204
#ifdef DEBUG
#line 2205
#error Unsupported DEBUG original branch
#endif
#line 2210

#line 77
#ifdef FE_IME
#line 78
#error Unsupported FE_IME original branch
#endif
#line 84

#line 717
#ifdef FE_IME // _PeekMessage()
#line 718
#error Unsupported FE_IME original branch
#else
#line 721
#endif // FE_IME
#line 723

#line 1020
#ifdef FE_IME //@TK
#line 1021
#error Unsupported FE_IME original branch
#endif
#line 1034
