/****************************** Module Header ******************************\
* Module Name: ClMsg.c
*
* Copyright (c) 1985-91, Microsoft Corporation
*
* Includes the mapping table for messages when calling the server.
*
* 04-11-91 ScottLu Created.
\***************************************************************************/

/* DIVERGENCE(OPENNT-HOST-120): complete original GetCPD, separate selection.
 * Remaining clmsg bodies outside admitted default/GetCPD slices are omitted. */
#if defined(WOW_ORIGINAL_CLIENT_CPD)
#include "precomp.h"
#pragma hdrstop

/***********************************************************************\
* GetCPD
*
* This function calls the server to allocate a CPD structure.
*
* History:
* 11-15-94 JimA         Created.
\***********************************************************************/

DWORD GetCPD(
    PVOID pWndOrCls,
    DWORD options,
    DWORD dwData)
{
    return NtUserGetCPD(HW(pWndOrCls), options, dwData);
}
#endif

/* OPENNT-HOST-089: complete original client default-window algorithm. */
#if defined(WOW_ORIGINAL_DEFAULT_WINDOW)
#include "precomp.h"
#pragma hdrstop
#include "wow_default_window_bindings.h"

LONG DefWindowProcWorker(
    PWND pwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam,
    DWORD fAnsi)
{
    HWND hwnd = HWq(pwnd);
    int icolBack;
    int icolFore;
    PWND pwndParent;
#ifdef FE_IME
    HWND hwndDefIme;
    PWND pwndDefIme;
    PIMEUI pimeui;
#endif

#ifdef DEBUG
    if (!gfTurboDWP) {
        return CsSendMessage(hwnd, message, wParam, lParam, 0L,
                FNID_DEFWINDOWPROC, fAnsi);
    } else {
#endif


    if (FDEFWINDOWMSG(message, DefWindowMsgs)) {
        return CsSendMessage(hwnd, message, wParam, lParam, 0L,
                FNID_DEFWINDOWPROC, fAnsi);
    } else if (!FDEFWINDOWMSG(message, DefWindowSpecMsgs)) {
        return 0;
    }

    /*
     * Important:  If you add cases to the switch statement below,
     *             add the messages to server.c's gawDefWindowSpecMsgs.
     *             Similarly if you add cases to dwp.c's DefWindowProc
     *             which can come from the client, add the messages
     *             to gawDefWindowMsgs.
     */

    switch (message) {

    case WM_HELP:
        {
        PWND  pwndDest;

        /*
         * If this window is a child window, Help message must be passed on
         * to it's parent; Else, this must be passed on to the owner window.
         */
        pwndDest = (TestwndChild(pwnd) ? pwnd->spwndParent : pwnd->spwndOwner);
        if (pwndDest) {
            pwndDest = REBASEPTR(pwnd, pwndDest);
            if (pwndDest != _GetDesktopWindow())
                return SendMessageW(HWq(pwndDest), WM_HELP, wParam, lParam);;
        }
        return(0L);
        }

    case WM_MOUSEWHEEL:
        if (TestwndChild(pwnd)) {
            pwndParent = REBASEPWND(pwnd, spwndParent);
            SendMessageW(HW(pwndParent), WM_MOUSEWHEEL, wParam, lParam);
        }
        break;

    case WM_CONTEXTMENU:
        if (TestwndChild(pwnd)) {
            pwndParent = REBASEPWND(pwnd, spwndParent);
            SendMessageW(HW(pwndParent), WM_CONTEXTMENU,
                    (WPARAM)hwnd, lParam);
        }
        break;

    /*
     * Default handling for WM_CONTEXTMENU support
     */
    case WM_RBUTTONUP:
        lParam = MAKELONG(LOWORD(lParam) + pwnd->rcClient.left, HIWORD(lParam) + pwnd->rcClient.top);
        SendMessageWorker(pwnd, WM_CONTEXTMENU, (WPARAM)hwnd, lParam, fAnsi);
        break;

    case WM_WINDOWPOSCHANGED: {
        PWINDOWPOS ppos = (PWINDOWPOS)lParam;

        if (!(ppos->flags & SWP_NOCLIENTMOVE)) {
            pwndParent = REBASEPWND(pwnd, spwndParent);
            SendMessageWorker(pwnd, WM_MOVE, FALSE,
                    MAKELONG(pwnd->rcClient.left - pwndParent->rcClient.left,
                    pwnd->rcClient.top  - pwndParent->rcClient.top), fAnsi);
        }

        /* DIVERGENCE(OPENNT-HOST-089): a WM_MOVE callback can destroy the
         * native window; retained copied WND storage does not pin its HWND. */
        if (TestWF(pwnd, WFDESTROYED) || (pwnd->fnid & FNID_STATUS_BITS)) return 0;
        if ((ppos->flags & SWP_STATECHANGE) || !(ppos->flags & SWP_NOCLIENTSIZE)) {
            UINT cmd;
            RECT rc;

            if (TestWF(pwnd, WFMINIMIZED))
                cmd = SIZEICONIC;
            else if (TestWF(pwnd, WFMAXIMIZED))
                cmd = SIZEFULLSCREEN;
            else
                cmd = SIZENORMAL;

        /*
         *  HACK ALERT:
         *  If the window is minimized then the real client width and height are
         *  zero. But, in win3.1 they were non-zero. Under Chicago, PrintShop
         *  Deluxe ver 1.2 hits a divide by zero. To fix this we fake the width
         *  and height for old apps to be non-zero values.
         *  GetClientRect does that job for us.
         */
            _GetClientRect(pwnd, &rc);
            SendMessageWorker(pwnd, WM_SIZE, cmd,
                    MAKELONG(rc.right - rc.left,
                    rc.bottom - rc.top), fAnsi);
        }
        return 0;
        }

    case WM_MOUSEACTIVATE: {
        PWND pwndT;
        LONG lt;

        /*
         * GetChildParent returns either a kernel pointer or NULL.
         */
        pwndT = GetChildParent(pwnd);
        if (pwndT != NULL) {
            pwndT = REBASEPTR(pwnd, pwndT);
            lt = SendMessageWorker(pwndT, WM_MOUSEACTIVATE, wParam, lParam, fAnsi);
            if (lt != 0)
                return lt;
        }

        /*
         * Moving, sizing or minimizing? Activate AFTER we take action.
         */
        return ((LOWORD(lParam) == HTCAPTION) && (HIWORD(lParam) == WM_LBUTTONDOWN )) ?
                (LONG)MA_NOACTIVATE : (LONG)MA_ACTIVATE;
        }

    case WM_CTLCOLORSCROLLBAR:
        if ((oemInfo.BitCount < 8) || (SYSRGB(3DHILIGHT) != SYSRGB(SCROLLBAR)) ||
            (SYSRGB(3DHILIGHT) == SYSRGB(WINDOW)))
        {
            /*
             * Remove call to UnrealizeObject().  GDI Handles this for
             * brushes on NT.
             *
             * UnrealizeObject(ghbrGray);
             */

            SetBkColor((HDC)wParam, SYSRGB(3DHILIGHT));
            SetTextColor((HDC)wParam, SYSRGB(3DFACE));
            return((LRESULT)ghbrGray);
        }

        icolBack = COLOR_3DHILIGHT;
        icolFore = COLOR_BTNTEXT;
        goto SetColor;

    case WM_CTLCOLORBTN:
        if (pwnd == NULL)
            goto ColorDefault;

        if (TestWF(pwnd, WFWIN40COMPAT)) {
            icolBack = COLOR_3DFACE;
            icolFore = COLOR_BTNTEXT;
        } else {
            goto ColorDefault;
        }
        goto SetColor;

    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLORDLG:
    case WM_CTLCOLORMSGBOX:
        // We want static controls in dialogs to have the 3D
        // background color, but statics in windows to inherit
        // their parents' background.

        if (pwnd == NULL)
            goto ColorDefault;

        if (TestWF(pwnd, WFWIN40COMPAT)) {
            icolBack = COLOR_3DFACE;
            icolFore = COLOR_WINDOWTEXT;
            goto SetColor;
        }
        // ELSE FALL THRU...

    case WM_CTLCOLOR:              // here for WOW only
    case WM_CTLCOLORLISTBOX:
    case WM_CTLCOLOREDIT:
      ColorDefault:
        icolBack = COLOR_WINDOW;
        icolFore = COLOR_WINDOWTEXT;

      SetColor:
        SetBkColor((HDC)wParam, gpsi->argbSystem[icolBack]);
        SetTextColor((HDC)wParam, gpsi->argbSystem[icolFore]);
        return (LONG)ahbrSystem[icolBack];

    case WM_NCHITTEST:
        return FindNCHit(pwnd, lParam);

    case WM_GETTEXT:
        if (wParam != 0) {

            LPWSTR lpszText;
            UINT   cchSrc;

            if (pwnd->strName.Length) {

                lpszText = REBASE(pwnd, strName.Buffer);
                cchSrc = (UINT)pwnd->strName.Length / sizeof(WCHAR);

                if (fAnsi) {

                    LPSTR lpName = (LPSTR)lParam;

                    /*
                     * Non-zero retval means some text to copy out.  Do not
                     * copy out more than the requested byte count
                     * 'chMaxCount'.
                     */
                    cchSrc = WCSToMB(lpszText,
                                     cchSrc,
                                     (LPSTR *)&lpName,
                                     (wParam - 1),
                                     FALSE);

                    lpName[cchSrc] = '\0';

                } else {

                    LPWSTR lpwName = (LPWSTR)lParam;

                    cchSrc = min(cchSrc, (UINT)(wParam - 1));
                    RtlCopyMemory(lpwName, lpszText, cchSrc * sizeof(WCHAR));
                    lpwName[cchSrc] = 0;
                }

                return cchSrc;
            }

            /*
             * else Null terminate the text buffer since there is no text.
             */
            if (fAnsi) {
                ((LPSTR)lParam)[0] = 0;
            } else {
                ((LPWSTR)lParam)[0] = 0;
            }
        }

        return 0;

    case WM_GETTEXTLENGTH:
        if (pwnd->strName.Length) {
            UINT cch;
            if (fAnsi) {
                RtlUnicodeToMultiByteSize(&cch,
                                          REBASE(pwnd, strName.Buffer),
                                          pwnd->strName.Length);
            } else {
                cch = pwnd->strName.Length / sizeof(WCHAR);
            }
            return cch;
        }
        return 0L;

    case WM_QUERYOPEN:
    case WM_QUERYENDSESSION:
    case WM_DEVICECHANGE:
    case WM_POWERBROADCAST:
        return TRUE;

    case WM_KEYDOWN:
        if (wParam == VK_F10)
            return CsSendMessage(hwnd, message, wParam, lParam, 0L,
                    FNID_DEFWINDOWPROC, fAnsi);
        break;

    case WM_SYSKEYDOWN:
        if ((HIWORD(lParam) & SYS_ALTERNATE) || (wParam == VK_F10) ||
                (wParam == VK_ESCAPE))
            return CsSendMessage(hwnd, message, wParam, lParam, 0L,
                    FNID_DEFWINDOWPROC, fAnsi);
        break;

    case WM_CHARTOITEM:
    case WM_VKEYTOITEM:
        /*
         * Do default processing for keystrokes into owner draw listboxes.
         */
        return -1;

    case WM_ACTIVATE:
        if (LOWORD(wParam))
            return CsSendMessage(hwnd, message, wParam, lParam, 0L,
                    FNID_DEFWINDOWPROC, fAnsi);
        break;

    case WM_SHOWWINDOW:
        if (lParam != 0)
            return CsSendMessage(hwnd, message, wParam, lParam, 0L,
                    FNID_DEFWINDOWPROC, fAnsi);
        break;

    case WM_DROPOBJECT:
       return DO_DROPFILE;

    case WM_WINDOWPOSCHANGING:
        /*
         * If the window's size is changing, adjust the passed-in size
         */
        #define ppos ((WINDOWPOS *)lParam)
        if (!(ppos->flags & SWP_NOSIZE))
            return CsSendMessage(hwnd, message, wParam, lParam, 0L,
                    FNID_DEFWINDOWPROC, fAnsi);
        #undef ppos
        break;

    case WM_KLUDGEMINRECT:
        {
        SHELLHOOKINFO shi;
        LPRECT lprc = (LPRECT)lParam;

        shi.hwnd = (HWND)wParam;
        CopyRect(&(shi.rc),(LPRECT)lParam);
        if (gpsi->uiShellMsg == 0)
            SetTaskmanWindow(NULL);
        SendMessageWorker(pwnd, gpsi->uiShellMsg, HSHELL_GETMINRECT,
                (LPARAM)&shi, fAnsi);
        //
        // Now convert the RECT back from two POINTS structures into two POINT
        // structures.
        //
        lprc->left   = (SHORT)LOWORD(shi.rc.left);  // Sign extend
        lprc->top    = (SHORT)HIWORD(shi.rc.left);  // Sign extend
        lprc->right  = (SHORT)LOWORD(shi.rc.top);   // Sign extend
        lprc->bottom = (SHORT)HIWORD(shi.rc.top);   // Sign extend
        break;
        }

    case WM_NOTIFYFORMAT:
        if (lParam == NF_QUERY)
            return(TestWF(pwnd, WFANSICREATOR) ? NFR_ANSI : NFR_UNICODE);
        break;

#if defined(FE_IME) // DefWindowProcWorker()
    case WM_IME_KEYDOWN:
        if (fAnsi)
            PostMessageA(hwnd, WM_KEYDOWN, wParam, lParam);
        else
            PostMessageW(hwnd, WM_KEYDOWN, wParam, lParam);
        break;

    case WM_IME_KEYUP:
        if (fAnsi)
            PostMessageA(hwnd, WM_KEYUP, wParam, lParam);
        else
            PostMessageW(hwnd, WM_KEYUP, wParam, lParam);
        break;

    case WM_IME_CHAR:
        //if (TestCF(pwnd, CFIME))
        //    break;

        if ( fAnsi ) {
            // LATER : IsDBCSLeadByteEx() should be called with the codepage
            //         retrieved from the current keyboard layout
            if( IsDBCSLeadByte((BYTE)(wParam >> 8)) ) {
                PostMessageA(hwnd,
                             WM_CHAR,
                             (WPARAM)((BYTE)(wParam >> 8)),   // leading byte
                             1L);
                PostMessageA(hwnd,
                             WM_CHAR,
                             (WPARAM)((BYTE)wParam),         // trailing byte
                             1L);
            }
            else
                PostMessageA(hwnd,
                             WM_CHAR,
                             (WPARAM)(wParam),
                             1L);
        } else {
            PostMessageW(hwnd, WM_CHAR, wParam, 1L);
        }
        break;

    case WM_IME_COMPOSITION:
        //if (TestCF(pwnd, CFIME))
        //    break;

        if (lParam & GCS_RESULTSTR) {
            HIMC  hImc;
            DWORD cbLen;

            if ((hImc = ImmGetContext(hwnd)) == NULL_HIMC)
                goto dwpime_ToIMEWnd_withchk;

            if (fAnsi) {
                LPSTR pszBuffer, psz;

                /*
                 * ImmGetComposition returns the size of buffer needed in byte.
                 */
                if (!(cbLen = ImmGetCompositionStringA(hImc, GCS_RESULTSTR, NULL, 0))) {
                    ImmReleaseContext(hwnd, hImc);
                    goto dwpime_ToIMEWnd_withchk;
                }

                pszBuffer = psz = (LPSTR)UserLocalAlloc(HEAP_ZERO_MEMORY,
                                                        cbLen + sizeof(CHAR));

                if (pszBuffer == NULL) {
                    ImmReleaseContext(hwnd, hImc);
                    goto dwpime_ToIMEWnd_withchk;
                }

                ImmGetCompositionStringA(hImc, GCS_RESULTSTR, psz, cbLen);

                while (*psz) {
                    // LATER : IsDBCSLeadByteEx() should be called with the codepage
                    //         retrieved from the current keyboard layout
                    if (IsDBCSLeadByte(*psz)) { // Should be IsDBCSLeadByteEx
                        if (*(psz+1)) {
                            SendMessageA( hwnd,
                                          WM_IME_CHAR,
                                          MAKEWPARAM(MAKEWORD(*(psz+1), *psz), 0),
                                          1L );
                            psz++;
                        }
                        psz++;
                    }
                    else
                        SendMessageA( hwnd,
                                      WM_IME_CHAR,
                                      MAKEWPARAM(MAKEWORD(*(psz++), 0), 0),
                                      1L );
                }

                UserLocalFree(pszBuffer);

                ImmReleaseContext(hwnd, hImc);
            }
            else {
                LPWSTR pwszBuffer, pwsz;

                /*
                 * ImmGetComposition returns the size of buffer needed in byte
                 */
                if (!(cbLen = ImmGetCompositionStringW(hImc, GCS_RESULTSTR, NULL, 0))) {
                    ImmReleaseContext(hwnd, hImc);
                    goto dwpime_ToIMEWnd_withchk;
                }

                pwszBuffer = pwsz = (LPWSTR)UserLocalAlloc(HEAP_ZERO_MEMORY,
                                                           cbLen + sizeof(WCHAR));

                if (pwszBuffer == NULL) {
                    ImmReleaseContext(hwnd, hImc);
                    goto dwpime_ToIMEWnd_withchk;
                }

                ImmGetCompositionStringW(hImc, GCS_RESULTSTR, pwsz, cbLen);

                while (*pwsz)
                    SendMessageW(hwnd, WM_IME_CHAR, MAKEWPARAM(*pwsz++, 0), 1L);

                UserLocalFree(pwszBuffer);

                ImmReleaseContext(hwnd, hImc);
            }
        }

        /*
         * Fall through to send to Default IME Window with checking
         * activated hIMC.
         */

    case WM_IME_STARTCOMPOSITION:
    case WM_IME_ENDCOMPOSITION:
dwpime_ToIMEWnd_withchk:
        //if (TestCF(pwnd, CFIME))
        //    break;

        /*
         * We assume this Wnd uses DefaultIMEWindow.
         * If this window has its own IME window, it have to call
         * ImmIsUIMessage()....
         */
        hwndDefIme = ImmGetDefaultIMEWnd(hwnd);

        if (hwndDefIme == hwnd) {
            /*
             * VC++ 1.51 TLW0NCL.DLL subclass IME class window
             * and pass IME messages to DefWindowProc().
             */
            RIPMSG1(RIP_WARNING,
                "IME Class window is hooked and IME message [%X] are sent to DefWindowProc",
                message);
            ImeWndProcWorker(pwnd, message, wParam, lParam, fAnsi);
            break;
        }

        if ((pwndDefIme = ValidateHwndNoRip(hwndDefIme)) != NULL) {
            /*
             * If hImc of this window is not activated for IME window,
             * we don't send WM_IME_NOTIFY.
             */
            pimeui = ((PIMEWND)pwndDefIme)->pimeui;
            if (pimeui->hIMC == ImmGetContext(hwnd))
                return SendMessageWorker(pwndDefIme, message, wParam, lParam, fAnsi);
            else
                RIPMSG1(RIP_WARNING,
                    "DefWindowProc can not send WM_IME_message [%X] now",
                    message);
        }
        break;

dwpime_ToTopLevel_withchk:
        //if (TestCF(pwnd, CFIME))
        //    break;

        /*
         * We assume this Wnd uses DefaultIMEWindow.
         * If this window has its own IME window, it have to call
         * ImmIsUIMessage()....
         */
        hwndDefIme = ImmGetDefaultIMEWnd(hwnd);

        if (hwndDefIme == hwnd) {
            /*
             * VC++ 1.51 TLW0NCL.DLL subclass IME class window
             * and pass IME messages to DefWindowProc().
             */
            RIPMSG1(RIP_WARNING,
                "IME Class window is hooked and IME message [%X] are sent to DefWindowProc",
                message);
            ImeWndProcWorker(pwnd, message, wParam, lParam, fAnsi);
            break;
        }

        pwndDefIme = ValidateHwndNoRip(hwndDefIme);

        if ((pwndDefIme = ValidateHwndNoRip(hwndDefIme)) != NULL) {
            PWND pwndT, pwndParent;

            pwndT = pwnd;

            while (TestwndChild(pwndT)) {
                pwndParent = REBASEPWND(pwndT, spwndParent);
                if (GETPTI(pwndParent) != GETPTI(pwnd))
                    break;
                pwndT = pwndParent;
            }

            /*
             * If hImc of this window is not activated for IME window,
             * we don't send WM_IME_NOTIFY.
             */
            if (pwndT != pwnd) {
                pimeui = ((PIMEWND)pwndDefIme)->pimeui;
                if (pimeui->hIMC == ImmGetContext(hwnd))
                    return SendMessageWorker(pwndT, message, wParam, lParam, fAnsi);
                else
                    RIPMSG1(RIP_WARNING,
                        "DefWindowProc can not send WM_IME_message [%X] now",
                        message);
            }
            else {
                /*
                 * Review !!
                 * If this is the toplevel window, we pass messages to
                 * the default IME window...
                 */
                return SendMessageWorker(pwndDefIme, message, wParam, lParam, fAnsi);
            }
        }
        break;

    case WM_IME_NOTIFY:
        switch (wParam) {
        case IMN_OPENSTATUSWINDOW:
        case IMN_CLOSESTATUSWINDOW:
            goto dwpime_ToTopLevel_withchk;

        default:
            goto dwpime_ToIMEWnd_withchk;
        }
        break;

    case WM_IME_SYSTEM:
        if (wParam == IMS_SETACTIVECONTEXT) {
            RIPMSG0(RIP_WARNING, "DefWindowProc received unexpected WM_IME_SYSTEM");
            break;
        }

        /*
         * IMS_SETOPENSTATUS is depended on the activated input context.
         * It needs to be sent to only the activated system window.
         */
        if (wParam == IMS_SETOPENSTATUS)
            goto dwpime_ToIMEWnd_withchk;

        /*
         * Fall through to send to Default IME Window.
         */

    case WM_IME_SETCONTEXT:
        //if (TestCF(pwnd, CFIME))
        //    break;

        hwndDefIme = ImmGetDefaultIMEWnd(hwnd);

        if (hwndDefIme == hwnd) {
            /*
             * VC++ 1.51 TLW0NCL.DLL subclass IME class window
             * and pass IME messages to DefWindowProc().
             */
            RIPMSG1(RIP_WARNING,
                "IME Class window is hooked and IME message [%X] are sent to DefWindowProc",
                message);
            ImeWndProcWorker(pwnd, message, wParam, lParam, fAnsi);
            break;
        }

        if ((pwndDefIme = ValidateHwndNoRip(hwndDefIme)) != NULL)
            return SendMessageWorker(pwndDefIme, message, wParam, lParam, fAnsi);

        break;

    case WM_IME_SELECT:
        RIPMSG0(RIP_WARNING, "DefWindowProc should not receive WM_IME_SELECT");
        break;

    case WM_IME_COMPOSITIONFULL:
        //if (TestCF(pwnd, CFIME))
        //    break;

        if (GetAppVer(NULL) < VER40) {
            /*
             * This is a temporary solution for win31app.
             * FEREVIEW: For M5 this will call WINNLS message mapping logic
             *           -yutakan
             */
            return SendMessageWorker(pwnd, WM_IME_REPORT,
                             IR_FULLCONVERT, (LPARAM)0L, fAnsi);
        }
        break;
#endif

    }

    return 0;

#ifdef DEBUG
    } // gfTurboDWP
#endif
}
#endif
