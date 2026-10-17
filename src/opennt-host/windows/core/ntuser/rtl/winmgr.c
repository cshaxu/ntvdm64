/* Copyright (c) Microsoft Corporation.
 * Original windows/core/ntuser/rtl/winmgr.c true subset.
 * DIVERGENCE(OPENNT-HOST-078): retain required client-view bodies unchanged;
 * target-private precomp replaces the original build environment.
 * Unselected until the complete EDIT dependency group is wired. */
#include "precomp.h"

#if !defined(WOW_ORIGINAL_DEFAULT_GEOMETRY)
/* OPENNT-HOST-108: original style/version predicate, unchanged. */
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
#endif

#if defined(WOW_ORIGINAL_DEFAULT_GEOMETRY)
/* OPENNT-HOST-091: unchanged original border arithmetic. */
#include "wow_default_window_bindings.h"
#define gpsi (&wow_default_current->server)
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
/* OPENNT-HOST-106: original traversal, no fresh HWND sampling. */
PWND GetTopLevelWindow(
    PWND pwnd)
{
    if (pwnd != NULL) {
        while (TestwndChild(pwnd))
            pwnd = REBASEPWND(pwnd, spwndParent);
    }

    return pwnd;
}

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

/* OPENNT-HOST-106: original graph predicate used by Tab traversal. */
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
#endif

