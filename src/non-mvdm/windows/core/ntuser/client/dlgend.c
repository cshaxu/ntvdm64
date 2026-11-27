/***************************************************************************\
*
*  DLGEND.C -
*
* Copyright (c) 1985 - 1999, Microsoft Corporation
*
*      Dialog Destruction Routines
*
* ??-???-???? mikeke    Ported from Win 3.0 sources
* 12-Feb-1991 mikeke    Added Revalidation code
\***************************************************************************/

#include "precomp.h"
#pragma hdrstop


/***************************************************************************\
* EndDialog
*
* History:
* 11-Dec-1990 mikeke  ported from win30
\***************************************************************************/


FUNCLOG2(LOG_GENERAL, BOOL, DUMMYCALLINGTYPE, EndDialog, HWND, hwnd, INT_PTR, result)
BOOL EndDialog(
    HWND hwnd,
    INT_PTR result)
{
    PWND pwnd;
    PWND pwndOwner;
    HWND hwndOwner;
    BOOL fWasActive = FALSE;
#ifdef SYSMODALWINDOWS
#line 35
#error DIVERGENCE OPENNT-HOST-133: SYSMODALWINDOWS branch is not selected; see mirror register.
#endif
#line 37

    if ((pwnd = ValidateHwnd(hwnd)) == NULL) {
        return (0L);
    }

    CheckLock(pwnd);

    /*
     * Must do special validation here to make sure pwnd is a dialog window.
     */
    if (!ValidateDialogPwnd(pwnd))
        return 0;

    if (SAMEWOWHANDLE(hwnd, GetActiveWindow())) {
        fWasActive = TRUE;
    }

    /*
     * GetWindowCreator returns either a kernel address or NULL.
     */
    pwndOwner = GetWindowCreator(pwnd);

    if (pwndOwner != NULL) {

        /*
         * Hide the window.
         */
        pwndOwner = REBASEPTR(pwnd, pwndOwner);
        hwndOwner = HWq(pwndOwner);
        if (!PDLG(pwnd)->fDisabled) {
            NtUserEnableWindow(hwndOwner, TRUE);
        }
    } else {
        hwndOwner = NULL;
    }

    /*
     * Terminate Mode Loop.
     */
    if (TestWF(pwnd, WFDESTROYED) || (pwnd->fnid & FNID_STATUS_BITS))
        return FALSE;
#line 76
    PDLG(pwnd)->fEnd = TRUE;
    PDLG(pwnd)->result = result;

    if (fWasActive && IsChild(hwnd, GetFocus())) {

        /*
         * Set focus to the dialog box so that any control which has the focus
         * can do an kill focus processing.  Most useful for combo boxes so that
         * they can popup their dropdowns before destroying/hiding the dialog
         * box window.  Note that we only do this if the focus is currently at a
         * child of this dialog box.  We also need to make sure we are the active
         * window because this may be happening while we are in a funny state.
         * ie.  the activation is in the middle of changing but the focus hasn't
         * changed yet.  This happens with TaskMan (or maybe with other apps that
         * change the focus/activation at funny times).
         */
        NtUserSetFocus(hwnd);
    }

    NtUserSetWindowPos(hwnd, NULL, 0, 0, 0, 0,
                       SWP_HIDEWINDOW | SWP_NOACTIVATE | SWP_NOMOVE |
                       SWP_NOSIZE | SWP_NOZORDER);

#ifdef SYSMODALWINDOWS
#line 100
#error DIVERGENCE OPENNT-HOST-133: SYSMODALWINDOWS branch is not selected; see mirror register.
#endif
#line 120

    /*
     * Don't do any activation unless we were previously active.
     */
    if (fWasActive && hwndOwner) {
        NtUserSetActiveWindow(hwndOwner);
    } else {

        /*
         * If at this point we are still the active window it means that
         * we have fallen into the black hole of being the only visible
         * window in the system when we hid ourselves.  This is a bug and
         * needs to be fixed better later on.  For now, though, just
         * set the active and focus window to NULL.
         */
        if (SAMEWOWHANDLE(hwnd, GetActiveWindow())) {
//     The next two lines are *not* the equivalent of the two Unlock
//      statements that were in Daytona server-side dlgend.c.  So, we
//      need to go over to server/kernel and do it right.  This fixes
//      a problem in Visual Slick, which had the MDI window lose focus
//      when a message box was dismissed.  FritzS
//            SetActiveWindow(NULL);
//            SetFocus(NULL);
            NtUserCallNoParam(SFI_ZAPACTIVEANDFOCUS);
        }
    }

#ifdef SYSMODALWINDOWS
#line 148
#error DIVERGENCE OPENNT-HOST-133: SYSMODALWINDOWS branch is not selected; see mirror register.
#endif
#line 166

    /*
     * Make sure the dialog loop will wake and destroy the window.
     * The dialog loop is waiting on posted events (WaitMessage). If
     * EndDialog is called due to a sent message from another thread the
     * dialog loop will keep waiting for posted events and not destroy
     * the window. This happens when the dialog is obscured.
     * This is a problem with winfile and its copy/move dialog.
     */
    PostMessage(hwnd, WM_NULL, 0, 0);

    return TRUE;
}
