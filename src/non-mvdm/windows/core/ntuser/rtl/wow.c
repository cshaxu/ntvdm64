/* Copyright (c) Microsoft Corporation.
 * Original windows/core/ntuser/rtl/wow.c true subset.
 * DIVERGENCE(OPENNT-HOST-078): retain required client-view bodies unchanged;
 * target-private precomp replaces the original build environment.
 * Unselected until the complete EDIT dependency group is wired. */
#include "precomp.h"

#line 15
#ifdef WOW
#line 16
#error Excluded unsupported original 16-bit WOW source branch
#else
#line 109
#endif  // WOW
#line 128

#line 135
#ifdef WOW
#line 136
#error Excluded unsupported original 16-bit WOW source branch
#else
#line 145
#line 146
#ifdef _USERK_
#line 147
#error Excluded unsupported original _USERK_ desktop validation
#else
#line 151
#endif
#line 170
#endif
#line 679


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

#line 749
BOOL _ClientToScreen(
    PWND pwnd,
    PPOINT ppt)
{
    ppt->x += pwnd->rcClient.left;
    ppt->y += pwnd->rcClient.top;

    return TRUE;
}

#line 836
BOOL _ScreenToClient(
    PWND pwnd,
    PPOINT ppt)
{
    ppt->x -= pwnd->rcClient.left;
    ppt->y -= pwnd->rcClient.top;

    return TRUE;
}

#line 716
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

#line 781
#ifdef WOW
#line 782
#error DIVERGENCE OPENNT-HOST-135: 16-bit WOW rectangle layout is not selected here; see mirror register.
#else
#line 786
        prc->left   = 0;
        prc->top    = 0;
        prc->right  = SYSMET(CXMINIMIZED);
        prc->bottom = SYSMET(CYMINIMIZED);
#endif
#line 791

    } else {

#line 794
#ifdef WOW
#line 795
#error DIVERGENCE OPENNT-HOST-135: 16-bit WOW rectangle layout is not selected here; see mirror register.
#else
#line 799
        *prc = pwnd->rcClient;
        OffsetRect(prc, -pwnd->rcClient.left, -pwnd->rcClient.top);
#endif
#line 802

    }

    return TRUE;
}



#line 262
#if defined(DEBUG) && !defined(_USERK_) && !defined(WOW)
#line 263
#error Unsupported DEBUG original branch
#endif
#line 275

#line 244
#ifndef WOW
#line 245
#endif
#line 247

#line 344
#ifndef WOW
#line 345
#endif
#line 347

