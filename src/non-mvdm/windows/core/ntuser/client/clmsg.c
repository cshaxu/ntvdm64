/****************************** Module Header ******************************\
* Module Name: ClMsg.c
*
* Copyright (c) 1985-91, Microsoft Corporation
*
* Includes the mapping table for messages when calling the server.
*
* 04-11-91 ScottLu Created.
\***************************************************************************/

#line 23
#ifdef DEBUG
#line 24
#error Unsupported original DEBUG branch
#endif
#line 26
#line 11
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

#line 1916
DWORD GetCPD(
    PVOID pWndOrCls,
    DWORD options,
    DWORD dwData)
{
    return NtUserGetCPD(HW(pWndOrCls), options, dwData);
}
#endif

/***************************************************************************\
* DefWindowProcWorker
*
* Handles any messages that can be dealt with wholly on the client and
* passes the rest to the server.
*
* 03-31-92 DarrinM      Created.
\***************************************************************************/

/* OPENNT-HOST-089: complete original client default-window algorithm. */
#if defined(WOW_ORIGINAL_DEFAULT_WINDOW)
#include "precomp.h"
#pragma hdrstop
#include "wow_default_window_bindings.h"

#line 555
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
#endif /* WOW_ORIGINAL_DEFAULT_WINDOW */
#line 566
#ifdef FE_IME
#line 567
#error DIVERGENCE OPENNT-HOST-132: FE_IME branch is not selected; see mirror register.
#endif

#line 572
#ifdef DEBUG
#line 573
#error Excluded unsupported original DEBUG default-window path
#endif

#if defined(WOW_ORIGINAL_DEFAULT_WINDOW)
#line 580
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
#line 647
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

#endif /* WOW_ORIGINAL_DEFAULT_WINDOW */
#line 905
#if defined(FE_IME) // DefWindowProcWorker()
#line 906
#error DIVERGENCE OPENNT-HOST-132: FE_IME branch is not selected; see mirror register.
#endif

#if defined(WOW_ORIGINAL_DEFAULT_WINDOW)
#line 1210
    }

    return 0;

#endif /* WOW_ORIGINAL_DEFAULT_WINDOW */
#line 1214
#ifdef DEBUG
#line 1215
#error Excluded unsupported original DEBUG default-window close
#endif
#if defined(WOW_ORIGINAL_DEFAULT_WINDOW)
#line 1217
}
#endif

#line 1650
#if 0
#line 1651
#error Excluded original disabled WMCTLCOLOR handler
#endif
#line 1677


#line 327
#ifdef FE_IME // SendMessageWorker()
#line 328
#error Excluded unsupported original FE_IME branch
#endif // FE_IME
#line 331


#line 270
#ifdef FE_SB // SendMessageWorker()
#line 271
#error Unsupported FE_SB original branch
#endif // FE_SB
#line 274

#line 1719
#ifdef FE_SB // DispatchMessageWorker()
#line 1720
#error Unsupported FE_SB original branch
#endif // FE_SB
#line 1722

#line 331
#ifdef FE_SB // SendMessageWorker()
#line 332
#error Unsupported FE_SB original branch
#else
#line 356
#endif // FE_SB
#line 364

#line 453
#ifdef FE_SB // SendMessageWorker()
#line 454
#error Unsupported FE_SB original branch
#else
#line 467
#endif // FE_SB
#line 470

#line 1777
#ifdef FE_SB // DispatchMessageWorker()
#line 1778
#error Unsupported FE_SB original branch
#else
#line 1790
#endif // FE_SB
#line 1793

#line 1803
#ifdef FE_SB // DispatchMessageWorker()
#line 1804
#error Unsupported FE_SB original branch
#else
#line 1841
#endif // FE_SB
#line 1853

#line 373
#ifndef LATER
#line 374
#line 400
#ifdef FE_SB // SendMessageWorker()
#line 401
#error Excluded unsupported original FE_SB branch
#else
#line 413
#endif // FE_SB
#line 415
#line 416
#ifdef FE_SB // SendMessageWorker()
#line 417
#error Excluded unsupported original FE_SB branch
#else
#line 429
#endif // FE_SB
#line 431
#line 433
#ifdef FE_SB // SendMessageWorker()
#line 434
#error Excluded unsupported original FE_SB branch
#else
#line 447
#endif // FE_SB
#line 449
#endif /* !LATER */
#line 452

