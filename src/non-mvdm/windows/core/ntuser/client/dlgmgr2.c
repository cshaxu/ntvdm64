/***************************************************************************\
*
*  DLGMGR2.C
*
*      Dialog Management Routines
*
* ??-???-???? mikeke    Ported from Win 3.0 sources
* 12-Feb-1991 mikeke    Added Revalidation code
\***************************************************************************/

#line 11
#include "precomp.h"
#pragma hdrstop

/***************************************************************************\
* xxxRemoveDefaultButton
*
* Scan through all the controls in the dialog box and remove the default
* button style from any button that has it.  This is done since at times we
* do not know who has the default button.
*
* History:
\***************************************************************************/

#line 24
void xxxRemoveDefaultButton(
    PWND pwndRoot,
    PWND pwndStart)
{
    UINT code;
    PWND pwnd;
    TL tlpwnd;

    CheckLock(pwndRoot);
    CheckLock(pwndStart);

    if (!pwndStart || TestWF(pwndStart, WEFCONTROLPARENT))
        pwndStart = _NextControl(pwndRoot, NULL, CWP_SKIPINVISIBLE | CWP_SKIPDISABLED);
    else
        pwndStart = _GetChildControl(pwndRoot, pwndStart);

    if (!pwndStart)
        return;

    pwnd = pwndStart;
    do {
        ThreadLock(pwnd, &tlpwnd);

        code = (UINT)SendMessage(HWq(pwnd), WM_GETDLGCODE, 0, 0L);

        if (code & DLGC_DEFPUSHBUTTON) {
            SendMessage(HWq(pwnd), BM_SETSTYLE, BS_PUSHBUTTON, (LONG)TRUE);
        }

        pwnd = _NextControl(pwndRoot, pwnd, 0);

        ThreadUnlock(&tlpwnd);

    } while (pwnd && (pwnd != pwndStart));
}


/***************************************************************************\
* xxxCheckDefPushButton
*
* History:
\***************************************************************************/

void xxxCheckDefPushButton(
    PWND pwndDlg,
    HWND hwndOldFocus,
    HWND hwndNewFocus)
{
    PWND pwndNewFocus;
    PWND pwndOldFocus;
    TL tlpwndT;
    PWND pwndT;
    UINT codeNewFocus = 0;
    UINT styleT;
    LONG lT;
    int id;

    if (hwndNewFocus)
        pwndNewFocus = ValidateHwnd(hwndNewFocus);
    else
        pwndNewFocus = NULL;

     if (hwndOldFocus)
         pwndOldFocus = ValidateHwnd(hwndOldFocus);
     else
         pwndOldFocus = NULL;

    CheckLock(pwndDlg);
    CheckLock(pwndNewFocus);
    CheckLock(pwndOldFocus);

    if (pwndNewFocus)
    {
        // Do nothing if clicking on dialog background or recursive dialog
        // background.
        if (TestWF(pwndNewFocus, WEFCONTROLPARENT))
            return;

        codeNewFocus = SendMessage(hwndNewFocus, WM_GETDLGCODE, 0, 0L);
    }

    if (SAMEWOWHANDLE(hwndOldFocus, hwndNewFocus)) {
        //
        // NEW FOR 4.0:
        //
        // There is a very common frustrating scenario for ISVs who try to
        // set the default ID.  Our dialog manager assumes that if a push
        // button has the focus, it is the default button also.  As such
        // it passes in the focus window to this routine.  If someone tries
        // to change the focus or set the def ID such that they reside with
        // two different push buttons, the double-default-push button case
        // will result shortly.
        //
        // As such, for 4.0 dialogs, we will go check the def ID and see if
        // is the same as hwndOldFocus' ID.  If not, then we will find IT
        // and use that dude as hwndOldFocus
        //
        if (codeNewFocus & DLGC_UNDEFPUSHBUTTON)
        {
           if (TestWF(pwndDlg, WFWIN40COMPAT) && hwndOldFocus)
           {
               lT = SendMessage(HWq(pwndDlg), DM_GETDEFID, 0, 0L);
               id = (HIWORD(lT) == DC_HASDEFID ? LOWORD(lT) : IDOK);
               lT = MAKELONG(id, 0);

               if (lT != (LONG)(DWORD)pwndNewFocus->spmenu)
               {
                   if (pwndOldFocus = _FindDlgItem(pwndDlg, lT))
                   {
                       hwndOldFocus = HW(pwndOldFocus);
                       if (SendMessage(hwndOldFocus, WM_GETDLGCODE, 0, 0L) & DLGC_DEFPUSHBUTTON)
                       {
                           xxxRemoveDefaultButton(pwndDlg, pwndOldFocus);
                           goto SetNewDefault;
                       }
                   }
               }
           }

           SendMessage(hwndNewFocus, BM_SETSTYLE, BS_DEFPUSHBUTTON, (LONG)TRUE);
        }
        return;
    }

    /*
     * If the focus is changing to or from a pushbutton, then remove the
     * default style from the current default button
     */
    if ((hwndOldFocus != NULL && (SendMessage(hwndOldFocus, WM_GETDLGCODE,
                0, 0) & (DLGC_DEFPUSHBUTTON | DLGC_UNDEFPUSHBUTTON))) ||
            (hwndNewFocus != NULL &&
                (codeNewFocus & (DLGC_DEFPUSHBUTTON | DLGC_UNDEFPUSHBUTTON)))) {
        xxxRemoveDefaultButton(pwndDlg, pwndNewFocus);
    }

SetNewDefault:
    /*
     * If moving to a button, make that button the default.
     */
    if (codeNewFocus & DLGC_UNDEFPUSHBUTTON) {
        SendMessage(hwndNewFocus, BM_SETSTYLE, BS_DEFPUSHBUTTON, (LONG)TRUE);
    } else {

        /*
         * Otherwise, make sure the original default button is default
         * and no others.
         */

        /*
         * Get the original default button handle
         */
        lT = SendMessage(HWq(pwndDlg), DM_GETDEFID, 0, 0L);
        id = (HIWORD(lT) == DC_HASDEFID ? LOWORD(lT) : IDOK);
        pwndT = _FindDlgItem(pwndDlg, id);

        if (pwndT == NULL)
            return;
        ThreadLockAlways(pwndT, &tlpwndT);

        /*
         * If it already has the default button style, do nothing.
         */
        if ((styleT = (UINT)SendMessage(HWq(pwndT), WM_GETDLGCODE, 0, 0L)) & DLGC_DEFPUSHBUTTON) {
            ThreadUnlock(&tlpwndT);
            return;
        }

        /*
         * Also check to make sure it is really a button.
         */
        if (!(styleT & DLGC_UNDEFPUSHBUTTON)) {
            ThreadUnlock(&tlpwndT);
            return;
        }

        if (!TestWF(pwndT, WFDISABLED)) {
            SendMessage(HWq(pwndT), BM_SETSTYLE, BS_DEFPUSHBUTTON, (LONG)TRUE);
        }
        ThreadUnlock(&tlpwndT);
    }
}


#ifdef WOW_ORIGINAL_DIALOG_KEYBOARD
#line 207
/***************************************************************************\
* IsDialogMessage (API)
*
* History:
\***************************************************************************/

#line 213
#line 213
BOOL IsDialogMessageA(
    HWND hwndDlg,
    LPMSG lpmsg)
{
    WPARAM wParamSaved = lpmsg->wParam;
    BOOL bRet;

    switch (lpmsg->message) {
#endif /* WOW_ORIGINAL_DIALOG_KEYBOARD */
#line 221
#ifdef FE_SB // IsDialogMessageA()
#line 222
#error DIVERGENCE OPENNT-HOST-132: FE_SB branch is not selected; see mirror register.
#line 231
#else
#ifdef WOW_ORIGINAL_DIALOG_KEYBOARD
#line 232
#line 222
    case WM_CHAR:
    case EM_SETPASSWORDCHAR:
#endif /* WOW_ORIGINAL_DIALOG_KEYBOARD */
#line 234
#endif // FE_SB
#ifdef WOW_ORIGINAL_DIALOG_KEYBOARD
#line 235
    case WM_CHARTOITEM:
    case WM_DEADCHAR:
    case WM_SYSCHAR:
    case WM_SYSDEADCHAR:
    case WM_MENUCHAR:
#endif /* WOW_ORIGINAL_DIALOG_KEYBOARD */
#line 240
#ifdef FE_IME // IsDialogMessageA()
#line 241
#error DIVERGENCE OPENNT-HOST-132: FE_IME branch is not selected; see mirror register.
#line 243
#endif // FE_IME
#ifdef WOW_ORIGINAL_DIALOG_KEYBOARD
#line 243

#line 245
        RtlMBMessageWParamCharToWCS(lpmsg->message, (PDWORD)&lpmsg->wParam);
    }

#line 248
    bRet = IsDialogMessageW(hwndDlg, lpmsg);

    /*
     * Restore the original ANSI char.
     */
    lpmsg->wParam = wParamSaved;
#line 254
    return bRet;
}

BOOL IsDialogMessageW(
    HWND hwndDlg,
    LPMSG lpMsg)
{
    PWND pwndDlg;
    PWND pwnd;
    PWND pwnd2;
    HWND hwnd2;
    HWND hwndFocus;
    int iOK;
    BOOL fBack;
    UINT code;
    LONG lT;
    TL tlpwnd;
    TL tlpwndDlg;
    BOOL fLockDlg = FALSE;
    TL tlpwnd2;

    if ((pwndDlg = ValidateHwndNoRip(hwndDlg)) == NULL) {
        return FALSE;
    }

    CheckLock(pwndDlg);

#endif /* WOW_ORIGINAL_DIALOG_KEYBOARD */
#line 281
#ifdef LATER // JerrySh - we shouldn't need this on the client-side
#line 282
#error DIVERGENCE OPENNT-HOST-131: LATER branch is not selected; see mirror register.
#line 308
#endif
#ifdef WOW_ORIGINAL_DIALOG_KEYBOARD
#line 309

    if (CallMsgFilter(lpMsg, MSGF_DIALOGBOX))
        return TRUE;

#line 313
    if (lpMsg->hwnd == NULL) {
        return FALSE;
    }

    pwnd = ValidateHwnd(lpMsg->hwnd);
#line 318
    //
    // THIS IS FOR MFC.
    //
    // This solves many problems with apps that use MFC but want to take
    // advantage of DS_CONTROL.  MFC blindly passes in child dialogs sometimes
    // to IsDialogMessage, which can screw up tabbing etc.
    //
#line 325
    if (TestWF(pwndDlg, WEFCONTROLPARENT) && TestWF(pwndDlg, WFCHILD)) {
        pwndDlg = GetParentDialog(pwndDlg);
        ThreadLock(pwndDlg, &tlpwndDlg);
        fLockDlg = TRUE;
        hwndDlg = HWq(pwndDlg);
    }

    if (pwnd != pwndDlg && !_IsChild(pwndDlg, pwnd)) {
        if (fLockDlg)
            ThreadUnlock(&tlpwndDlg);
        return FALSE;
    }
    ThreadLock(pwnd, &tlpwnd);

    fBack = FALSE;
    iOK = IDCANCEL;
    switch (lpMsg->message) {
    case WM_LBUTTONDOWN:

        /*
         * Move the default button styles around on button clicks in the
         * same way as TABs.
         */
        if ((pwnd != pwndDlg) && ((hwndFocus = GetFocus()) != NULL)) {
            xxxCheckDefPushButton(pwndDlg, hwndFocus, lpMsg->hwnd);
        }
        break;

    case WM_SYSCHAR:

        /*
         * If no control has focus, and Alt not down, then ignore.
         */
        if ((GetFocus() == NULL) && (GetKeyState(VK_MENU) >= 0)) {
            if (lpMsg->wParam == VK_RETURN && TestWF(pwnd, WFMINIMIZED)) {

                /*
                 * If this is an iconic dialog box window and the user hits
                 * return, send the message off to DefWindowProc so that it
                 * can be restored.  Especially useful for apps whose top
                 * level window is a dialog box.
                 */
                goto CallDefWindowProcAndReturnTrue;
            } else {
                NtUserMessageBeep(0);
            }

            ThreadUnlock(&tlpwnd);
            if (fLockDlg)
                ThreadUnlock(&tlpwndDlg);
            return TRUE;
        }

        /*
         * If alt+menuchar, process as menu.
         */
        if (lpMsg->wParam == MENUSYSMENU) {
            DefWindowProcWorker(pwndDlg, lpMsg->message, lpMsg->wParam,
                    lpMsg->lParam, FALSE);
            ThreadUnlock(&tlpwnd);
            if (fLockDlg)
                ThreadUnlock(&tlpwndDlg);
            return TRUE;
        }

    /*
     *** FALL THRU **
     */

    case WM_CHAR:

        /*
         * Ignore chars sent to the dialog box (rather than the control).
         */
        if (pwnd == pwndDlg) {
            ThreadUnlock(&tlpwnd);
            if (fLockDlg)
                ThreadUnlock(&tlpwndDlg);
            return TRUE;
        }

        code = (UINT)SendMessage(lpMsg->hwnd, WM_GETDLGCODE, lpMsg->wParam,
                (DWORD)lpMsg);

        /*
         * If the control wants to process the message, then don't check for
         * possible mnemonic key.
         */
#line 413
        if ((lpMsg->message == WM_CHAR) && (code & (DLGC_WANTCHARS | DLGC_WANTMESSAGE)))
            break;

        /* If the control wants tabs, then don't let tab fall thru here
         */
        if ((lpMsg->wParam == VK_TAB) && (code & DLGC_WANTTAB))
            break;


        /*
         * HACK ALERT
         *
         * If ALT is held down (i.e., SYSCHARs), then ALWAYS do mnemonic
         * processing.  If we do away with SYSCHARS, then we should
         * check key state of ALT instead.
         */

        /*
         * Space is not a valid mnemonic, but it IS the char that toggles
         * button states.  Don't look for it as a mnemonic or we will
         * beep when it is typed....
         */
        if (lpMsg->wParam == VK_SPACE) {
            ThreadUnlock(&tlpwnd);
            if (fLockDlg)
                ThreadUnlock(&tlpwndDlg);
            return TRUE;
        }

        if (!(pwnd2 = xxxGotoNextMnem(pwndDlg, pwnd, (WCHAR)lpMsg->wParam))) {

            if (code & DLGC_WANTMESSAGE)
                break;

            /*
             * No mnemonic could be found so we will send the sys char over
             * to xxxDefWindowProc so that any menu bar on the dialog box is
             * handled properly.
             */
            if (lpMsg->message == WM_SYSCHAR) {
CallDefWindowProcAndReturnTrue:
                DefWindowProcWorker(pwndDlg, lpMsg->message, lpMsg->wParam,
                        lpMsg->lParam, FALSE);

                ThreadUnlock(&tlpwnd);
                if (fLockDlg)
                    ThreadUnlock(&tlpwndDlg);
                return TRUE;
            }
            NtUserMessageBeep(0);
        } else {

            /*
             * pwnd2 is 1 if the mnemonic took us to a pushbutton.  We
             * don't change the default button status here since doing this
             * doesn't change the focus.
             */
            if (pwnd2 != (PWND)1) {
                ThreadLockAlways(pwnd2, &tlpwnd2);
                xxxCheckDefPushButton(pwndDlg, lpMsg->hwnd, HWq(pwnd2));
                ThreadUnlock(&tlpwnd2);
            }
        }

        ThreadUnlock(&tlpwnd);
        if (fLockDlg)
            ThreadUnlock(&tlpwndDlg);
        return TRUE;

    case WM_KEYDOWN:
        code = (UINT)SendMessage(lpMsg->hwnd, WM_GETDLGCODE, lpMsg->wParam,
                (DWORD)lpMsg);
#line 485
        if (code & (DLGC_WANTALLKEYS | DLGC_WANTMESSAGE))
            break;

        switch (lpMsg->wParam) {
        case VK_TAB:
            if (code & DLGC_WANTTAB)
                break;
            pwnd2 = _GetNextDlgTabItem(pwndDlg, pwnd,
                    (GetKeyState(VK_SHIFT) & 0x8000));
            if (pwnd2 != NULL) {
                hwnd2 = HWq(pwnd2);
                ThreadLockAlways(pwnd2, &tlpwnd2);
                DlgSetFocus(hwnd2);
#line 498
                xxxCheckDefPushButton(pwndDlg, lpMsg->hwnd, hwnd2);
#line 499
                ThreadUnlock(&tlpwnd2);
            }

            ThreadUnlock(&tlpwnd);
            if (fLockDlg)
                ThreadUnlock(&tlpwndDlg);
            return TRUE;

        case VK_LEFT:
        case VK_UP:
            fBack = TRUE;

        /*
         *** FALL THRU **
         */
        case VK_RIGHT:
        case VK_DOWN:
            if (code & DLGC_WANTARROWS)
                break;

            pwnd2 = _GetNextDlgGroupItem(pwndDlg, pwnd, fBack);
            if (pwnd2 == NULL) {
                ThreadUnlock(&tlpwnd);
                if (fLockDlg)
                    ThreadUnlock(&tlpwndDlg);
                return TRUE;
            }

            hwnd2 = HWq(pwnd2);
            ThreadLockAlways(pwnd2, &tlpwnd2);

            code = (UINT)SendMessage(hwnd2, WM_GETDLGCODE, lpMsg->wParam,
                    (DWORD)lpMsg);

            /* DIVERGENCE(OPENNT-HOST-119): do not borrow cleaned Button data. */
            if (pwnd2->fnid & FNID_STATUS_BITS) {
                ThreadUnlock(&tlpwnd2);
                goto ReturnProcessed;
            }

#line 533
            /*
             * We are just moving the focus rect around! So, do not send
             * BN_CLICK messages, when WM_SETFOCUSing.  Fix for Bug
             * #4358.
             */
#line 538
#line 538
            if (code & (DLGC_UNDEFPUSHBUTTON | DLGC_DEFPUSHBUTTON)) {
                PBUTN pbutn;
                BOOL fIsNTButton = (GETFNID(pwnd2) == FNID_BUTTON);
                /* DIVERGENCE(OPENNT-HOST-119): dialog focus also borrows BUTN. */
                wow_button_scope buttonScope;
                if (fIsNTButton) wow_button_scope_enter(&buttonScope, pwnd2);
                __try {
#line 541
                if (fIsNTButton) {
                    pbutn = ((PBUTNWND)pwnd2)->pbutn;
                    BUTTONSTATE(pbutn) |= BST_DONTCLICK;
                }
                DlgSetFocus(hwnd2);
                /* DIVERGENCE(OPENNT-HOST-119): restore before releasing BUTN. */
                } __finally {
#line 546
                if (fIsNTButton) {
                    BUTTONSTATE(pbutn) &= ~BST_DONTCLICK;
                }
                    if (fIsNTButton) wow_button_scope_leave(&buttonScope);
#line 548
                }
#line 549
#line 549
                xxxCheckDefPushButton(pwndDlg, lpMsg->hwnd, hwnd2);
#line 550
#line 550
            } else if (code & DLGC_RADIOBUTTON) {
                DlgSetFocus(hwnd2);
                xxxCheckDefPushButton(pwndDlg, lpMsg->hwnd, hwnd2);
                if (TestWF(pwnd2, BFTYPEMASK) == LOBYTE(BS_AUTORADIOBUTTON)) {

                    /*
                     * So that auto radio buttons get clicked on
                     */
                    if (!SendMessage(hwnd2, BM_GETCHECK, 0, 0L)) {
                        SendMessage(hwnd2, BM_CLICK, TRUE, 0L);
                    }
                }
            } else if (!(code & DLGC_STATIC)) {
                DlgSetFocus(hwnd2);
                xxxCheckDefPushButton(pwndDlg, lpMsg->hwnd, hwnd2);
            }
            ThreadUnlock(&tlpwnd2);
            ThreadUnlock(&tlpwnd);
            if (fLockDlg)
                ThreadUnlock(&tlpwndDlg);
            return TRUE;

        case VK_EXECUTE:
        case VK_RETURN:

            /*
             * Guy pressed return - if button with focus is
             * defpushbutton, return its ID.  Otherwise, return id
             * of original defpushbutton.
             */
            if (!(hwndFocus = GetFocus()))
                code = 0;
            else
            {
                code = (WORD)(DWORD)SendMessage(hwndFocus, WM_GETDLGCODE,
                    0, 0L);
            }

            if (code & DLGC_DEFPUSHBUTTON)
            {
                iOK = GetDlgCtrlID(hwndFocus);
                pwnd2 = ValidateHwnd(hwndFocus);
                goto HaveWindow;
            }
            else
            {
                lT = (LONG)SendMessage(hwndDlg, DM_GETDEFID, 0, 0L);
                iOK = MAKELONG(
                    (HIWORD(lT)==DC_HASDEFID ? LOWORD(lT) : IDOK),
                    0);
            }
            // FALL THRU

        case VK_ESCAPE:
        case VK_CANCEL:

            /*
             * Make sure button is not disabled.
             */
            pwnd2 = _FindDlgItem(pwndDlg, iOK);
HaveWindow:
            if (pwnd2 != NULL && TestWF(pwnd2, WFDISABLED)) {
                NtUserMessageBeep(0);
            } else {
                SendMessage(hwndDlg, WM_COMMAND,
                        MAKELONG(iOK, BN_CLICKED), (LONG)HW(pwnd2));
            }

ReturnProcessed: /* DIVERGENCE(OPENNT-HOST-119): shared existing unlock exit. */
#line 618
            ThreadUnlock(&tlpwnd);
            if (fLockDlg)
                ThreadUnlock(&tlpwndDlg);
            return TRUE;
        }
        break;
    }

    ThreadUnlock(&tlpwnd);
    if (fLockDlg)
        ThreadUnlock(&tlpwndDlg);

    TranslateMessage(lpMsg);
    DispatchMessage(lpMsg);

    return TRUE;
}
#endif

#line 636
/***************************************************************************\
*
* FindDlgItem32()
*
* Given a dialog, finds the window with the given ID anywhere w/in the
* descendant chain.
*
\***************************************************************************/

#line 645
#line 645
PWND _FindDlgItem(PWND pwndParent, DWORD id)
{
    PWND    pwndChild;
    PWND    pwndOrig;

    // QUICK TRY:
    pwndChild = _GetDlgItem(pwndParent, id);
    if (pwndChild || !TestWF(pwndParent, WFWIN40COMPAT))
        return(pwndChild);

    pwndOrig = _NextControl(pwndParent, NULL, CWP_SKIPINVISIBLE);
    if (pwndOrig == pwndParent)
        return(NULL);

    pwndChild = pwndOrig;

//    VerifyWindow(pwndChild);

    do
    {
        if ((DWORD)pwndChild->spmenu == id)
            return(pwndChild);

        pwndChild = _NextControl(pwndParent, pwndChild, CWP_SKIPINVISIBLE);
    }
    while (pwndChild && (pwndChild != pwndOrig));

    return(NULL);
}
