/****************************** Module Header ******************************\
* Module Name: wow.c
*
* Copyright (c) 1985-92, Microsoft Corporation
*
* This module contains shared code between USER32 and USER16
* No New CODE should be added to this file, unless its shared
* with USER16.
*
* History:
* 29-DEC-93 NanduriR      shared user32/user16 code.
\***************************************************************************/
#include "precomp.h"

#line 15
#ifdef WOW
#line 16
#error Excluded unsupported original 16-bit WOW source branch
#line 108
#else
#line 109

//**************************************************************************
// Stuff used when building for USER32
//
//**************************************************************************


//**************************************************************************
// These definitions get resolved differently for USER32 and USER16
//
//**************************************************************************

#endif  // WOW
#line 128

#line 135
#ifdef WOW
#line 136
#error Excluded unsupported original 16-bit WOW source branch
#line 144
#else
#line 145
#line 146
#ifdef _USERK_
#line 147
#error Excluded unsupported original _USERK_ desktop validation
#line 150
#else
#line 151

/* !!! LATER BUG 14263 remove 0xc000000 check */

#endif
#line 170
#endif


#line 244
#ifndef WOW
#line 245
#endif
#line 247

#line 262
#if defined(DEBUG) && !defined(_USERK_) && !defined(WOW)
#line 263
#error Unsupported DEBUG original branch
#line 274
#endif
#line 275

#line 344
#ifndef WOW
#line 345
#endif
#line 347

/***************************************************************************\
* _IsChild (API)
*
*
*
* History:
* 11-07-90 darrinm      Translated from Win 3.0 ASM code.
\***************************************************************************/

#line 682
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

/***************************************************************************\
* _IsWindowVisible (API)
*
* IsWindowVisible returns the TRUEVIS state of a window, rather than just
* the state of its WFVISIBLE flag.  According to this routine, a window is
* considered visible when it and all the windows on its parent chain are
* visible (WFVISIBLE flag set).  A special case hack was put in that causes
* any icon window being dragged to be considered as visible.
*
* History:
* 11-12-90 darrinm      Ported.
\***************************************************************************/

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

/***************************************************************************\
* _ClientToScreen (API)
*
* Map a point from client to screen-relative coordinates.
*
* History:
* 11-12-90 darrinm      Translated from Win 3.0 ASM code.
\***************************************************************************/

#line 749
BOOL _ClientToScreen(
    PWND pwnd,
    PPOINT ppt)
{
    ppt->x += pwnd->rcClient.left;
    ppt->y += pwnd->rcClient.top;

    return TRUE;
}

/***************************************************************************\
* _GetClientRect (API)
*
*
*
* History:
* 26-Oct-1990 DarrinM   Implemented.
\***************************************************************************/

#line 769
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
#line 785
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
#line 798
#else
#line 799
        *prc = pwnd->rcClient;
        OffsetRect(prc, -pwnd->rcClient.left, -pwnd->rcClient.top);
#endif
#line 802

    }

    return TRUE;
}

/***************************************************************************\
* _ScreenToClient (API)
*
* Map a point from screen to client-relative coordinates.
*
* History:
* 11-12-90 darrinm      Translated from Win 3.0 ASM code.
\***************************************************************************/

#line 836
BOOL _ScreenToClient(
    PWND pwnd,
    PPOINT ppt)
{
    ppt->x -= pwnd->rcClient.left;
    ppt->y -= pwnd->rcClient.top;

    return TRUE;
}
