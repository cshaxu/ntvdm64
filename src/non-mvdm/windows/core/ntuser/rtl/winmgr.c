/****************************** Module Header ******************************\
* Module Name: winmgr.c
*
* Copyright (c) 1985-92, Microsoft Corporation
*
* This module contains routines common to client and kernel.
*
* History:
* 02-20-92 DarrinM      Pulled functions from user\server.
* 11-11-94 JimA         Separated from client.
\***************************************************************************/
#include "precomp.h"

#if !defined(WOW_ORIGINAL_DEFAULT_GEOMETRY)
#line 258
BOOL _FChildVisible(
    PWND pwnd)
{
    while (TestwndChild(pwnd)) {
        pwnd = REBASEPWND(pwnd, spwndParent);
        if (pwnd == NULL)
            break;
        if (!TestWF(pwnd, WFVISIBLE))
            return FALSE;
    }

    return TRUE;
}

/***************************************************************************\
* _MapWindowPoints
*
*
* History:
* 03-03-92 JimA             Ported from Win 3.1 sources.
\***************************************************************************/

int _MapWindowPoints(
    PWND pwndFrom,
    PWND pwndTo,
    LPPOINT lppt,
    DWORD cpt)
{
    int dx, dy;

    /*
     * If a window is NULL, use the desktop window
     */
    if (pwndFrom == NULL)
        pwndFrom = _GetDesktopWindow();
    if (pwndTo == NULL)
        pwndTo = _GetDesktopWindow();

    /*
     * Compute deltas
     */
    dx = pwndFrom->rcClient.left - pwndTo->rcClient.left;
    dy = pwndFrom->rcClient.top - pwndTo->rcClient.top;

    /*
     * Map the points
     */
    while (cpt--) {
        lppt->x += dx;
        lppt->y += dy;
        ++lppt;
    }
    return MAKELONG(dx, dy);
}


/***************************************************************************\
*
*  GetRealClientRect()
*
*  Gets real client rectangle, inc. scrolls and excl. one row or column
*  of minimized windows.
*
\***************************************************************************/

void GetRealClientRect(
    PWND   pwnd,
    LPRECT prc,
    UINT   uFlags)
{
    if (GETFNID(pwnd) == FNID_DESKTOP) {

        *prc = (uFlags & GRC_FULLSCREEN) ? pwnd->rcWindow : gpsi->rcWork;

    } else {
        _GetClientRect(pwnd, prc);
        if (uFlags & GRC_SCROLLS) {
            if (TestWF(pwnd, WFHPRESENT))
                prc->bottom += SYSMET(CYHSCROLL);
            if (TestWF(pwnd, WFVPRESENT))
                prc->right += SYSMET(CXVSCROLL);
        }
    }

    if (uFlags & GRC_MINWNDS) {
        switch (SYSMET(ARRANGE) & ~ARW_HIDE) {
            case ARW_TOPLEFT | ARW_RIGHT:
            case ARW_TOPRIGHT | ARW_LEFT:
                //
                // Leave space on top for one row of min windows
                //
                prc->top += SYSMET(CYMINSPACING);
                break;

            case ARW_TOPLEFT | ARW_DOWN:
            case ARW_BOTTOMLEFT | ARW_UP:
                //
                // Leave space on left for one column of min windows
                //
                prc->left += SYSMET(CXMINSPACING);
                break;

            case ARW_TOPRIGHT | ARW_DOWN:
            case ARW_BOTTOMRIGHT | ARW_UP:
                //
                // Leave space on right for one column of min windows
                //
                prc->right -= SYSMET(CXMINSPACING);
                break;

            case ARW_BOTTOMLEFT | ARW_RIGHT:
            case ARW_BOTTOMRIGHT | ARW_LEFT:
                //
                // Leave space on bottom for one row of min windows
                //
                prc->bottom -= SYSMET(CYMINSPACING);
                break;
        }
    }
}


/***************************************************************************\
* _GetLastActivePopup (API)
*
*
*
* History:
* 11-27-90 darrinm      Ported from Win 3.0 sources.
* 02-19-91 JimA         Added enum access check
\***************************************************************************/

PWND _GetLastActivePopup(
    PWND pwnd)
{
    if (pwnd->spwndLastActive == NULL)
        return pwnd;

    return REBASEPWND(pwnd, spwndLastActive);
}


/****************************************************************************\
* GetTopLevelTiled
*
* This function returns the window handle of the top level "tiled"
* parent/owner of the window specified.
*
* 10-23-90 MikeHar      Ported from Windows.
\****************************************************************************/

PWND GetTopLevelTiled(
    PWND pwnd)
{
    while (TestWF(pwnd, WFCHILD) || pwnd->spwndOwner != NULL)
        pwnd = GetWindowCreator(pwnd);

    return pwnd;
}


/***************************************************************************\
* IsDescendant
*
* Internal version if IsChild that is a bit faster and ignores the WFCHILD
* business.
*
* Returns TRUE if pwndChild == pwndParent (IsChild doesn't).
*
* History:
* 07-22-91 darrinm      Translated from Win 3.1 ASM code.
* 03-03-94 Johnl        Moved from server
\***************************************************************************/

BOOL _IsDescendant(
    PWND pwndParent,
    PWND pwndChild)
{
    while (1) {
        if (pwndParent == pwndChild)
            return TRUE;
        if (GETFNID(pwndChild) == FNID_DESKTOP)
            break;
        pwndChild = REBASEPWND(pwndChild, spwndParent);
    }

    return FALSE;
}

/***************************************************************************\
* IsVisible
*
* Return whether or not a given window can be drawn in or not.
*
* History:
* 07-22-91 darrinm      Translated from Win 3.1 ASM code.
\***************************************************************************/

BOOL IsVisible(
    PWND pwnd)
{
    PWND pwndT;

    for (pwndT = pwnd; pwndT; pwndT = REBASEPWND(pwndT, spwndParent)) {

        /*
         * Invisible windows are always invisible
         */
        if (!TestWF(pwndT, WFVISIBLE))
            return FALSE;

        if (TestWF(pwndT, WFMINIMIZED)) {

            /*
             * Children of minimized windows are always invisible.
             */
            if (pwndT != pwnd)
                return FALSE;
        }

        /*
         * If we're at the desktop, then we don't want to go any further.
         */
        if (GETFNID(pwndT) == FNID_DESKTOP)
            break;
    }

    return TRUE;
}



#else
#include "wow_default_window_bindings.h"
#define gpsi (&wow_default_current->server)
#line 489
/***************************************************************************\
*
*  Function:       GetWindowBorders
*
*  Synopsis:       Calculates # of borders around window
*
*  Algorithm:      Calculate # of window borders and # of client borders
*
*   This routine is ported from Chicago wmclient.c -- FritzS
*
\***************************************************************************/

int GetWindowBorders(LONG lStyle, DWORD dwExStyle, BOOL fWindow, BOOL fClient)
{
    int cBorders = 0;

    if (fWindow) {
        //
        // Is there a 3D border around the window?
        //
        if (dwExStyle & WS_EX_WINDOWEDGE)
            cBorders += 2;
        else if (dwExStyle & WS_EX_STATICEDGE)
            ++cBorders;

        //
        // Is there a single flat border around the window?  This is true for
        // WS_BORDER, WS_DLGFRAME, and WS_EX_DLGMODALFRAME windows.
        //
        if ( (lStyle & WS_CAPTION) || (dwExStyle & WS_EX_DLGMODALFRAME) )
                ++cBorders;

        //
        // Is there a sizing flat border around the window?
        //
        if (lStyle & WS_SIZEBOX)
                cBorders += gpsi->gclBorder;
    }

    if (fClient) {
            //
            // Is there a 3D border around the client?
            //
            if (dwExStyle & WS_EX_CLIENTEDGE)
            cBorders += 2;
    }

    return(cBorders);
}

#endif

#if !defined(WOW_ORIGINAL_DEFAULT_GEOMETRY)
#line 539


/***************************************************************************\
*  SizeBoxHwnd()
*
*  Returns the HWND that will be sized if the user drags in the given window's
*  sizebox -- If NULL, then the sizebox is not needed
*
*  Criteria for choosing what window will be sized:
*  find first sizeable parent; if that parent is not maximized and the child's
*  bottom, right corner is within a scroll bar height and width of the parent's
*  bottom, right corner, that parent will be sized.
*
*   From Chicago
\***************************************************************************/

PWND SizeBoxHwnd(
    PWND pwnd)
{
    int xbrChild = pwnd->rcWindow.right;
    int ybrChild = pwnd->rcWindow.bottom;

    while (GETFNID(pwnd) != FNID_DESKTOP) {
        if (TestWF(pwnd, WFSIZEBOX)) {
            // First sizeable parent found
            int xbrParent;
            int ybrParent;

            if (TestWF(pwnd, WFMAXIMIZED))
                return(NULL);

            xbrParent = pwnd->rcClient.right;
            ybrParent = pwnd->rcClient.bottom;

            /*  If the sizebox dude is within an EDGE of the client's bottom
             *  right corner, let this succeed.  That way people who draw
             *  their own sunken clients will be happy.
             */
            if ((xbrChild + SYSMET(CXEDGE) < xbrParent) || (ybrChild + SYSMET(CYEDGE) < ybrParent)) {
                //
                // Child's bottom, right corner of SIZEBOX isn't close enough
                // to bottom right of parent's client.
                //
                return(NULL);
            }

            return(pwnd);
        }

        if (!TestWF(pwnd, WFCHILD) || TestWF(pwnd, WFCPRESENT))
            break;

        pwnd = REBASEPWND(pwnd, spwndParent);
    }
    return(NULL);
}



// --------------------------------------------------------------------------
//
//  NeedsWindowEdge()
//
//  Modifies style/extended style to enforce WS_EX_WINDOWEDGE when we want
//  it.
//
//
// When do we want WS_EX_WINDOWEDGE on a window?
//      (1) If the window has a caption
//      (2) If the window has the WS_DLGFRAME or WS_EX_DLGFRAME style (note
//          that this takes care of (1))
//      (3) If the window has WS_THICKFRAME
//
// --------------------------------------------------------------------------
BOOL NeedsWindowEdge(DWORD dwStyle, DWORD dwExStyle, BOOL fNewApp)
{
    BOOL    fGetsWindowEdge;

    fGetsWindowEdge = FALSE;

    if (dwExStyle & WS_EX_DLGMODALFRAME)
        fGetsWindowEdge = TRUE;
    else if (dwExStyle & WS_EX_STATICEDGE)
        fGetsWindowEdge = FALSE;
    else if (dwStyle & WS_THICKFRAME)
        fGetsWindowEdge = TRUE;
    else switch (dwStyle & WS_CAPTION)
    {
        case WS_DLGFRAME:
            fGetsWindowEdge = TRUE;
            break;

        case WS_CAPTION:
            fGetsWindowEdge = fNewApp;
            break;
    }

    return(fGetsWindowEdge);
}


#line 696
/***************************************************************************\
* GetTopLevelWindow
*
* History:
* 10-19-90 darrinm      Ported from Win 3.0 sources.
\***************************************************************************/

PWND GetTopLevelWindow(
    PWND pwnd)
{
    if (pwnd != NULL) {
        while (TestwndChild(pwnd))
            pwnd = REBASEPWND(pwnd, spwndParent);
    }

    return pwnd;
}
#endif
