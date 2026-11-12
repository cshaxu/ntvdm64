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
#line 432
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
#line 456
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

#endif

#if defined(WOW_ORIGINAL_DEFAULT_GEOMETRY)
#include "wow_default_window_bindings.h"
#define gpsi (&wow_default_current->server)
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
#line 501
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
#line 613
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

/***************************************************************************\
* GetTopLevelWindow
*
* History:
* 10-19-90 darrinm      Ported from Win 3.0 sources.
\***************************************************************************/
#line 703
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
