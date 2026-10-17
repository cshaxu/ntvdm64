/* Copyright (c) Microsoft Corporation.
 * Original windows/core/ntuser/client/winmgrc.c true subset.
 * DIVERGENCE(OPENNT-HOST-078): retain required client-view bodies unchanged;
 * target-private precomp replaces the original build environment.
 * Unselected until the complete EDIT dependency group is wired. */
#include "precomp.h"

#ifdef WOW_ORIGINAL_EDIT_CLIENT
/* OPENNT-HOST-108: original dialog geometry, unchanged. */
BOOL _AdjustWindowRectEx(
    LPRECT lprc,
    LONG style,
    BOOL fMenu,
    DWORD dwExStyle)
{
    //
    // Here we add on the appropriate 3D borders for old and new apps.
    //
    // Rules:
    //   (1) Do nothing for windows that have 3D border styles.
    //   (2) If the window has a dlgframe border (has a caption or is a
    //          a dialog), then add on the window edge style.
    //   (3) We NEVER add on the CLIENT STYLE.  New apps can create
    //          it if they want.  This is because it screws up alignment
    //          when the app doesn't know about it.
    //

    if (NeedsWindowEdge(style, dwExStyle, GetAppVer(PtiCurrent()) >= VER40))
        dwExStyle |= WS_EX_WINDOWEDGE;
    else
        dwExStyle &= ~WS_EX_WINDOWEDGE;

    //
    // Space for a menu bar
    //
    if (fMenu)
        lprc->top -= SYSMET(CYMENU);

    //
    // Space for a caption bar
    //
    if ((HIWORD(style) & HIWORD(WS_CAPTION)) == HIWORD(WS_CAPTION)) {
        lprc->top -= (dwExStyle & WS_EX_TOOLWINDOW) ? SYSMET(CYSMCAPTION) : SYSMET(CYCAPTION);
    }

    //
    // Space for borders (window AND client)
    //
    {
        int cBorders;

        //
        // Window AND Client borders
        //

        if (cBorders = GetWindowBorders(style, dwExStyle, TRUE, TRUE))
            InflateRect(lprc, cBorders*SYSMET(CXBORDER), cBorders*SYSMET(CYBORDER));
    }

    return TRUE;
}
#endif

BOOL FChildVisible(
    HWND hwnd)
{
    PWND pwnd;

    pwnd = ValidateHwnd(hwnd);

    if (pwnd == NULL)
        return 0;

    return (_FChildVisible(pwnd));
}

