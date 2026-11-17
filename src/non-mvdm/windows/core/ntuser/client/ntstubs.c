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

#if defined(WOW_ORIGINAL_CONTROL_TEXT) || defined(WOW_ORIGINAL_DIALOG_GETSET)
#include "precomp.h"
#elif defined(WOW_ORIGINAL_CONTROL_COLOR) || defined(WOW_ORIGINAL_CONTROL_BRUSH_CLIENT)
#include "wow_edit_call_bindings.h"
#else
#define WOW_ORIGINAL_CLASS_QUERY
#include "wow_class_query_bindings.h"
#endif
#line 13
#pragma hdrstop

#define CLIENTSIDE 1

#include "ntsend.h"

#line 77
#ifdef FE_IME
#line 78
#error
#line 83
#endif

#ifdef WOW_ORIGINAL_CLASS_QUERY
#line 138
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
#endif

#line 358
#ifdef LATER
#line 359
#error
#line 368
#endif

#line 380
#ifdef MEMPHIS_MENUS
#line 381
#error
#line 439
#else // MEMPHIS_MENUS
#line 440
#line 498
#endif // MEMPHIS_MENUS

#line 500
#ifdef MEMPHIS_MENU_WATERMARKS
#line 501
#error
#line 522
#endif // MEMPHIS_MENU_WATERMARKS

#ifdef WOW_ORIGINAL_CONTROL_BRUSH_CLIENT
#line 623
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
#endif

#ifdef WOW_ORIGINAL_CONTROL_COLOR
#line 641
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

#line 693
#ifdef FE_SB // _PeekMessage()
#line 694
#error
#line 701
#endif // FE_SB

#line 717
#ifdef FE_IME // _PeekMessage()
#line 718
#error
#line 720
#else
#line 721
#line 722
#endif // FE_IME

#line 753
#ifdef FE_SB // _PeekMessage()
#line 754
#error
#line 758
#else
#line 759
#line 760
#ifdef DEBUG
#line 761
#error
#line 764
#endif
#line 765
#line 766
#endif // FE_SB

#line 769
#ifdef FE_SB // _PeekMessage()
#line 770
#error
#line 779
#else
#line 780
#line 782
#endif // FE_SB

#line 784
#ifdef FE_SB // _PeekMessage()
#line 785
#error
#line 792
#else
#line 793
#line 794
#endif // FE_SB

#line 797
#ifdef FE_SB // _PeekMessage()
#line 798
#error
#endif // FE_SB

#ifdef WOW_ORIGINAL_DIALOG_GETSET
#line 827
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
            return wow_dialog_host_set(hwnd, nIndex, dwNewLong, bAnsi);
#else
#line 858
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
#line 925
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

#line 953
#ifdef DEBUG
#line 954
#error
#endif
#line 956

        dwNewLong &= ~(LBS_OWNERDRAWFIXED |
                       LBS_OWNERDRAWVARIABLE |
                       LBS_HASSTRINGS);

        dwNewLong |= pwnd->style & (LBS_OWNERDRAWFIXED |
                                    LBS_OWNERDRAWVARIABLE |
                                    LBS_HASSTRINGS);

#line 965
#ifdef DEBUG
#line 966
#error
#line 969
#endif
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
#endif

#line 1020
#ifdef FE_IME //@TK
#line 1021
#error
#line 1033
#endif

#ifdef WOW_ORIGINAL_CONTROL_TEXT
#line 1291
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
#endif

#line 1611
#ifdef FE_SB // fnINWPARAMDBCSCHAR()
#line 1612
#error
#line 1645
#endif // FE_SB

#line 2204
#ifdef DEBUG
#line 2205
#error
#line 2209
#endif
