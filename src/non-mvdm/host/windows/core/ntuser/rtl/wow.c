/* Copyright (c) Microsoft Corporation.
 * Original windows/core/ntuser/rtl/wow.c true subset.
 * DIVERGENCE(OPENNT-HOST-078): retain required client-view bodies unchanged;
 * target-private precomp replaces the original build environment.
 * Unselected until the complete EDIT dependency group is wired. */
#include "precomp.h"

/* OPENNT-HOST-106: complete original relationship/coordinate leaves. */
BOOL _IsChild(
    PWND pwndParent,
    PWND pwnd)
{
    /*
     * Don't need a test to get out of the loop because the
     * desktop is not a child.
     */
    while (pwnd != NULL) {
        if (!TestwndChild(pwnd))
            return FALSE;

        pwnd = REBASEPWND(pwnd, spwndParent);
        if (pwndParent == pwnd)
            return TRUE;
    }
    return FALSE;
}

BOOL _ClientToScreen(
    PWND pwnd,
    PPOINT ppt)
{
    ppt->x += pwnd->rcClient.left;
    ppt->y += pwnd->rcClient.top;

    return TRUE;
}

BOOL _ScreenToClient(
    PWND pwnd,
    PPOINT ppt)
{
    ppt->x -= pwnd->rcClient.left;
    ppt->y -= pwnd->rcClient.top;

    return TRUE;
}

BOOL _IsWindowVisible(
    PWND pwnd)
{
    /*
     * Check if this is the iconic window being moved around with a mouse
     * If so, return a TRUE, though, strictly speaking, it is hidden.
     * This helps the Tracer guys from going crazy!
     * Fix for Bug #57 -- SANKAR -- 08-08-89 --
     */
    if (pwnd == NULL)
        return TRUE;

    for (;;) {
        if (!TestWF(pwnd, WFVISIBLE))
            return FALSE;
        if (GETFNID(pwnd) == FNID_DESKTOP)
            break;
        pwnd = REBASEPWND(pwnd, spwndParent);
    }

    return TRUE;
}

BOOL _GetClientRect(
    PWND   pwnd,
    LPRECT prc)
{
    /*
     * If this is a 3.1 app, and it's minimized, then we need to return
     * a rectangle other than the real-client-rect.  This is necessary since
     * there is no client-rect-size in Win4.0.  Apps such as PackRat 1.0
     * will GPF if returned a empty-rect.
     */
    if (TestWF(pwnd, WFMINIMIZED) && !TestWF(pwnd, WFWIN40COMPAT)) {

#ifdef WOW
        *((LPDWORD)prc)    = 0l;
        *(((LPWORD)prc)+2) = (WORD)ClientSharedInfo()->psi->aiSysMet[SM_CXMINIMIZED];
        *(((LPWORD)prc)+3) = (WORD)ClientSharedInfo()->psi->aiSysMet[SM_CYMINIMIZED];
#else
        prc->left   = 0;
        prc->top    = 0;
        prc->right  = SYSMET(CXMINIMIZED);
        prc->bottom = SYSMET(CYMINIMIZED);
#endif

    } else {

#ifdef WOW
        *((LPDWORD)prc)    = 0l;
        *(((LPWORD)prc)+2) = (WORD)(pwnd->rcClient.right - pwnd->rcClient.left);
        *(((LPWORD)prc)+3) = (WORD)(pwnd->rcClient.bottom - pwnd->rcClient.top);
#else
        *prc = pwnd->rcClient;
        OffsetRect(prc, -pwnd->rcClient.left, -pwnd->rcClient.top);
#endif

    }

    return TRUE;
}

