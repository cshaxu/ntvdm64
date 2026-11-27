/****************************** Module Header ******************************\
* Module Name: ClMsg.c
*
* Copyright (c) 1985 - 1999, Microsoft Corporation
*
* Includes the mapping table for messages when calling the server.
*
* 04-11-91 ScottLu Created.
\***************************************************************************/

#include "precomp.h"
#undef FindNCHit
#include "non-mvdm/windows/core/ntuser/rtl/userrtl.h"
#define FindNCHit wow_default_find_nc_hit
#line 12
#pragma hdrstop


#define fnINDESTROYCLIPBRD      fnDWORD
#define fnOUTDWORDDWORD         fnDWORD
#define fnPOWERBROADCAST        fnDWORD
#define fnLOGONNOTIFY           fnKERNELONLY
#define fnINLPKDRAWSWITCHWND    fnKERNELONLY

#define MSGFN(func) fn ## func
#define FNSCSENDMESSAGE CFNSCSENDMESSAGE

#line 23
#define PROTOS_ONLY
#include "messages.h"
#undef PROTOS_ONLY
#line 25

#if DBG
#error Unsupported original DBG branch
#endif

#define BEGIN_CALLWINPROC(fInsideHook, lRet)                            \
    PCLIENTTHREADINFO pcti = GetClientInfo()->pClientThreadInfo;        \
    BOOL fCallBack = ((pcti!=NULL) &&                                   \
             TEST_BOOL_FLAG(pcti->CTIF_flags, CTIF_INCALLBACKMESSAGE)); \
    RTL_CALLER_ALLOCATED_ACTIVATION_CONTEXT_STACK_FRAME ActivationFrame \
        = {                                                             \
            sizeof(ActivationFrame),                                    \
    RTL_CALLER_ALLOCATED_ACTIVATION_CONTEXT_STACK_FRAME_FORMAT_WHISTLER \
        };                                                              \
    fInsideHook = FALSE;                                                \
                                                                        \
    if (!fCallBack) {                                                   \
        RtlActivateActivationContextUnsafeFast(                         \
            &ActivationFrame,                                           \
            pActCtx);                                                   \
                                                                        \
        fInsideHook = _BeginIfHookedUserApiHook();                      \
    }                                                                   \
                                                                        \
    __try {                                                             \

#define END_CALLWINPROC(fInsideHook)                                    \
    } __finally {                                                       \
        if (!fCallBack) {                                               \
            if (fInsideHook) {                                          \
                _EndUserApiHook();                                      \
            }                                                           \
            RtlDeactivateActivationContextUnsafeFast(                   \
                &ActivationFrame);                                      \
        }                                                               \
    }


#if defined(WOW_ORIGINAL_DEFAULT_WINDOW)
#line 252
WORD
GetMouseKeyState(void)
{
    WORD keystate;

    /*
     * Note that it is more efficient to call GetKeyState for each
     * key than to call GetKeyboardState, since the keys we are testing
     * are cached and don't require a trip to the kernel to fetch.
     */

#define TESTANDSETKEYSTATE(x)            \
    if (GetKeyState(VK_##x) & 0x8000) {  \
        keystate |= MK_##x;              \
    }

    keystate = 0;
    TESTANDSETKEYSTATE(LBUTTON)
    TESTANDSETKEYSTATE(RBUTTON)
    TESTANDSETKEYSTATE(MBUTTON)
    TESTANDSETKEYSTATE(XBUTTON1)
    TESTANDSETKEYSTATE(XBUTTON2)
    TESTANDSETKEYSTATE(SHIFT)
    TESTANDSETKEYSTATE(CONTROL)

    return keystate;
}

#line 1271
LRESULT DefWindowProcWorker(
    PWND pwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam,
    DWORD fAnsi)
{
    if (ghmodUserApiHook) {
        if (fAnsi) {
            return DefWindowProcA(HWq(pwnd), message, wParam, lParam);
        } else {
            return DefWindowProcW(HWq(pwnd), message, wParam, lParam);
        }
    }
    return RealDefWindowProcWorker(pwnd, message, wParam, lParam, fAnsi);
}

LRESULT RealDefWindowProcWorker(
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
#line 1302

#if DBG
#error Unsupported original DBG branch
#endif
#line 1309

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
#ifdef LAME_BUTTON
#error Unsupported original LAME_BUTTON branch
#endif // LAME_BUTTON
#line 1349

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
        if (TestWF(pwnd, WEFLAYOUTRTL)) {
            lParam = MAKELONG(pwnd->rcClient.right - GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) + pwnd->rcClient.top);
        } else {
            lParam = MAKELONG(GET_X_LPARAM(lParam) + pwnd->rcClient.left, GET_Y_LPARAM(lParam) + pwnd->rcClient.top);
        }
        SendMessageWorker(pwnd, WM_CONTEXTMENU, (WPARAM)hwnd, lParam, fAnsi);
        break;

    case WM_APPCOMMAND:
        if (TestwndChild(pwnd)) {
            /*
             * Bubble the message to the parent
             */
            pwndParent = REBASEPWND(pwnd, spwndParent);
            return SendMessageW(HW(pwndParent), WM_APPCOMMAND, wParam, lParam);
        } else {
            /*
             * Call the server side to send the shell hook HSHELL_APPCOMMAND
             */
            return CsSendMessage(hwnd, WM_APPCOMMAND, wParam, lParam, 0L, FNID_DEFWINDOWPROC, fAnsi);
        }
        break;

    /*
     * Default handling for WM_APPCOMMAND support
     */
    case WM_NCXBUTTONUP:
    case WM_XBUTTONUP:
        {
            WORD cmd;
            WORD keystate;
            LPARAM lParamAppCommand;

            switch (GET_XBUTTON_WPARAM(wParam)) {
            case XBUTTON1:
                cmd = APPCOMMAND_BROWSER_BACKWARD;
                break;

            case XBUTTON2:
                cmd = APPCOMMAND_BROWSER_FORWARD;
                break;

            default:
                cmd = 0;
                break;
            }

            if (cmd == 0) {
                break;
            }

            cmd |= FAPPCOMMAND_MOUSE;
            if (message == WM_XBUTTONUP) {
                keystate = GET_KEYSTATE_WPARAM(wParam);
            } else {
                keystate = GetMouseKeyState();
            }

            lParamAppCommand = MAKELPARAM(keystate, cmd);
            SendMessageWorker(pwnd, WM_APPCOMMAND, (WPARAM)hwnd, lParamAppCommand, fAnsi);
            break;
        }

    case WM_WINDOWPOSCHANGED: {
        PWINDOWPOS ppos = (PWINDOWPOS)lParam;

        if (!(ppos->flags & SWP_NOCLIENTMOVE)) {
            POINT pt = {pwnd->rcClient.left, pwnd->rcClient.top};
            pwndParent = REBASEPWND(pwnd, spwndParent);

            if (pwndParent != _GetDesktopWindow()) {
                pt.x -= pwndParent->rcClient.left;
                pt.y -= pwndParent->rcClient.top;
            }

            SendMessageWorker(pwnd, WM_MOVE, FALSE, MAKELPARAM(pt.x, pt.y), fAnsi);
        }

        if (TestWF(pwnd, WFDESTROYED) || (pwnd->fnid & FNID_STATUS_BITS)) return 0;
#line 1464
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
        LRESULT lt;

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
        if ((gpsi->BitCount < 8) ||
            (SYSRGB(3DHILIGHT) != SYSRGB(SCROLLBAR)) ||
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
            return((LRESULT)gpsi->hbrGray);
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
      {
        SetBkColor((HDC)wParam, gpsi->argbSystem[icolBack]);
        SetTextColor((HDC)wParam, gpsi->argbSystem[icolFore]);
        return (LRESULT)(SYSHBRUSH(icolBack));
      }

    case WM_NCHITTEST:
        return FindNCHit(pwnd, (LONG)lParam);

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
                                     (UINT)(wParam - 1),
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

    case WM_QUERYDRAGICON:
        /*
         * If the window is WIN40COMPAT or has a kernel side procedure
         * do not attempt to look into the instance module
         */
        if (TestWF(pwnd, WFWIN40COMPAT) || TestWF(pwnd, WFSERVERSIDEPROC)) {
            return 0;
        }
        /*
         * For old apps, like the VB3 ones, try to load the icon from resources
         * This is how Win95 does.
         */
        return CsSendMessage(hwnd, message, wParam, lParam, 0L, FNID_DEFWINDOWPROC, fAnsi);

    case WM_QUERYOPEN:
    case WM_QUERYENDSESSION:
    case WM_DEVICECHANGE:
    case WM_POWERBROADCAST:
        return TRUE;

    case WM_KEYDOWN:
        if (wParam == VK_F10) {
            return CsSendMessage(hwnd, message, wParam, lParam, 0L,
                    FNID_DEFWINDOWPROC, fAnsi);
        }
        break;

    case WM_SYSKEYDOWN:
        if ((HIWORD(lParam) & SYS_ALTERNATE) || (wParam == VK_F10) ||
                (wParam == VK_ESCAPE))
            return CsSendMessage(hwnd, message, wParam, lParam, 0L,
                    FNID_DEFWINDOWPROC, fAnsi);
        break;

    case WM_UNICHAR:
        if (wParam == UNICODE_NOCHAR) {
            return FALSE;
        }
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
        shi.rc.left = MAKELONG(lprc->left, lprc->top);
        shi.rc.top = MAKELONG(lprc->right, lprc->bottom);

        if (gpsi->uiShellMsg == 0)
            SetTaskmanWindow(NULL);
        if (SendMessageWorker(pwnd, gpsi->uiShellMsg, HSHELL_GETMINRECT,
                (LPARAM)&shi, fAnsi)) {
            //
            // Now convert the RECT back from two POINTS structures into two POINT
            // structures.
            //
            lprc->left   = (SHORT)LOWORD(shi.rc.left);  // Sign extend
            lprc->top    = (SHORT)HIWORD(shi.rc.left);  // Sign extend
            lprc->right  = (SHORT)LOWORD(shi.rc.top);   // Sign extend
            lprc->bottom = (SHORT)HIWORD(shi.rc.top);   // Sign extend
        }
        break;
        }

    case WM_NOTIFYFORMAT:
        if (lParam == NF_QUERY)
            return(TestWF(pwnd, WFANSICREATOR) ? NFR_ANSI : NFR_UNICODE);
        break;

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

#line 2133
    }

    return 0;

#if DBG
#error Unsupported original DBG branch
#endif
}


#endif
#line 2260

/***************************************************************************\
* _ClientGetListboxString
*
* This special function exists because LB_GETTEXT and CB_GETLBTEXT don't have
* buffer counts in them anywhere. Because there is no buffer count we have
* no idea how much room to reserved in the shared memory stack for this
* string to be copied into. The solution is to get the string length ahead
* of time, and send the message with this buffer length. Since this buffer
* length isn't a part of the original message, this routine is used for
* just this purpose.
*
* This routine gets called from the server.
*
* 04-13-91 ScottLu Created.
\***************************************************************************/

DWORD WINAPI _ClientGetListboxString(
    PWND pwnd,
    UINT msg,
    WPARAM wParam,
    LPSTR lParam, // May be a unicode or ANSI string
    ULONG_PTR xParam,
    PROC xpfn)
{
    return ((DWORD)((GENERICPROC)xpfn)(pwnd, msg, wParam, (LPARAM)lParam, xParam));
}

#if defined(WOW_ORIGINAL_CLIENT_CPD)
#line 2588
ULONG_PTR GetCPD(
    KERNEL_PVOID pWndOrCls,
    DWORD options,
    ULONG_PTR dwData)
{
    return NtUserGetCPD(HW(pWndOrCls), options, dwData);
}

#endif
#line 2596
#ifdef BUILD_WOW6432
#error Unsupported original BUILD_WOW6432 branch
#endif
#line 2629
#ifdef GENERIC_INPUT
#error Unsupported original GENERIC_INPUT branch
#endif

