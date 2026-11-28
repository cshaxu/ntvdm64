/**************************************************************************\
* Module Name: ntstubs.c
*
* Copyright (c) 1985 - 1999, Microsoft Corporation
*
* client side API stubs
*
* History:
* 03-19-95 JimA             Created.
\**************************************************************************/

#if defined(WOW_ORIGINAL_CONTROL_TEXT) || defined(WOW_ORIGINAL_DIALOG_GETSET)
#line 12
#include "precomp.h"
#else
#define WOW_ORIGINAL_CLASS_QUERY
#include "nt.h"
#include "wow_edit_lookaside_bindings.h"
#include "wow_class_query_bindings.h"
#include "wow_control_resource_abi.h"
#endif
#line 13
#pragma hdrstop

#define CLIENTSIDE 1

#include <dbt.h>

#include "ntsend.h"
#include "cfgmgr32.h"
#line 22

extern BOOL GetRemoteKeyboardLayout(PWCHAR);
#line 48
#ifdef IMM_PER_LOGON
#error DIVERGENCE: original configuration not selected.
#line 66
#endif

#line 77
#ifdef IMM_PER_LOGON
#error DIVERGENCE: original configuration not selected.
#line 83
#endif

#ifdef WOW_ORIGINAL_CLASS_QUERY
#line 140
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

        pcls = (PCLS)((KPBYTE)pcls - GetClientInfo()->ulClientDelta);
        retval = _GetClassData(pcls, NULL, GCLP_WOWWORDS, TRUE);

    ERRORTRAP(0);
    CLEANUPLPSTRW(strClassName);
    ENDCALL(LONG);
}
#endif
#line 467
#ifdef MESSAGE_PUMP_HOOK
#error DIVERGENCE: original configuration not selected.
#line 622
#endif // MESSAGE_PUMP_HOOK

#ifdef WOW_ORIGINAL_DIALOG_GETSET
#line 908
LONG_PTR _SetWindowLongPtr(
    HWND hwnd,
    int nIndex,
    LONG_PTR dwNewLong,
    BOOL bAnsi)
{
    PWND pwnd;
    LONG_PTR dwOldLong;
    DWORD dwCPDType = 0;

    pwnd = ValidateHwnd(hwnd);

    if (pwnd == NULL)
        return 0;

    if (TestWF(pwnd, WFDIALOGWINDOW)) {
        switch (nIndex) {
        case DWLP_DLGPROC:    // See similar case GWL_WNDPROC

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
#line 939
            /*
             * Get the old window proc address
             */
            dwOldLong = (LONG_PTR)PDLG(pwnd)->lpfnDlg;

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
                ULONG_PTR cpd;

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
                    dwNewLong = KERNEL_ULONG_PTR_TO_ULONG_PTR(pCPD->pfnClientPrevious);
                    bAnsi = pCPD->wType & CPD_UNICODE_TO_ANSI;
                }
            }

            /*
             * If an app 'unsubclasses' a server-side window proc we need to
             * restore everything so SendMessage and friends know that it's
             * a server-side proc again.  Need to check against client side
             * stub addresses.
             */
            PDLG(pwnd)->lpfnDlg = (DLGPROC)dwNewLong;
            if (bAnsi) {
                PDLG(pwnd)->flags |= DLGF_ANSI;
            } else {
                PDLG(pwnd)->flags &= ~DLGF_ANSI;
            }

            return dwOldLong;

#endif
#line 1006
        case DWLP_USER:
#ifdef BUILD_WOW6432
#error DIVERGENCE: original configuration not selected.
#line 1010
#endif
#line 1011
        case DWLP_MSGRESULT:
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

#if DBG
#error DIVERGENCE: original configuration not selected.
#line 1040
#endif
#line 1041

        dwNewLong &= ~(LBS_OWNERDRAWFIXED |
                       LBS_OWNERDRAWVARIABLE |
                       LBS_HASSTRINGS);

        dwNewLong |= pwnd->style & (LBS_OWNERDRAWFIXED |
                                    LBS_OWNERDRAWVARIABLE |
                                    LBS_HASSTRINGS);

#if DBG
#error DIVERGENCE: original configuration not selected.
#line 1054
#endif
#line 1055
    }


        retval = (ULONG_PTR)NtUserSetWindowLongPtr(
                hwnd,
                nIndex,
                dwNewLong,
                bAnsi);

    ERRORTRAP(0);
    ENDCALL(LONG_PTR);
}
#endif
#line 1068
#ifdef _WIN64
#error DIVERGENCE: original configuration not selected.
#line 1145
#endif

#line 1316
#if DBG
#error DIVERGENCE: original configuration not selected.
#line 1356
#endif // DBG

#ifdef WOW_ORIGINAL_CONTROL_TEXT
#line 1494
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
#ifdef WOW_ORIGINAL_CLASS_QUERY
#line 2107
DWORD CalcCharacterPositionAtoW(
    DWORD dwCharPosA,
    LPSTR lpszCharStr,
    DWORD dwCodePage)
{
    DWORD dwCharPosW = 0;

    while (dwCharPosA != 0) {
        if (IsDBCSLeadByteEx(dwCodePage, *lpszCharStr)) {
            if (dwCharPosA >= 2) {
                dwCharPosA -= 2;
            }
            else {
                dwCharPosA--;
            }
            lpszCharStr += 2;
        }
        else {
            dwCharPosA--;
            lpszCharStr++;
        }
        dwCharPosW++;
    }

    return dwCharPosW;
}

int UnicodeToMultiByteSize(DWORD dwCodePage, LPCWSTR pwstr)
{
    char dummy[2], *lpszDummy = dummy;
    return WCSToMBEx((WORD)dwCodePage, pwstr, 1, &lpszDummy, sizeof(WCHAR), FALSE);
}

DWORD CalcCharacterPositionWtoA(
    DWORD dwCharPosW,
    LPWSTR lpwszCharStr,
    DWORD  dwCodePage)
{
    DWORD dwCharPosA = 0;
    ULONG MultiByteSize;

    while (dwCharPosW != 0) {
        MultiByteSize = UnicodeToMultiByteSize(dwCodePage, lpwszCharStr);
        if (MultiByteSize == 2) {
            dwCharPosA += 2;
        }
        else {
            dwCharPosA++;
        }
        dwCharPosW--;
        lpwszCharStr++;
    }

    return dwCharPosA;
}
#endif
#line 2163
#ifdef LATER
#error DIVERGENCE: original configuration not selected.
#line 2421
#endif

#line 2827
#if DBG
#error DIVERGENCE: original configuration not selected.
#line 2832
#endif

