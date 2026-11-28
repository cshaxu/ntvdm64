/****************************** Module Header ******************************\
* Module Name: winmgrc.c
*
* Copyright (c) 1985 - 1999, Microsoft Corporation
*
* This module contains
*
* History:
* 20-Feb-1992 DarrinM   Pulled functions from user\server.
\***************************************************************************/

#include "precomp.h"
#pragma hdrstop

#define CONSOLE_WINDOW_CLASS (L"ConsoleWindowClass")
#line 54
BOOL FChildVisible(
    HWND hwnd)
{
    PWND pwnd;

    pwnd = ValidateHwnd(hwnd);

    if (pwnd == NULL)
        return 0;

    return (_FChildVisible(pwnd));
}

#line 545

/***************************************************************************\
* GetAppCompatFlags
*
* Compatibility flags for < Win 3.1 apps running on 3.1
*
* History:
* 01-Apr-1992 ScottLu   Created.
* 04-May-1992 DarrinM   Moved to USERRTL.DLL.
\***************************************************************************/


FUNCLOG1(LOG_GENERAL, DWORD, DUMMYCALLINGTYPE, GetAppCompatFlags, PTHREADINFO, pti)
DWORD GetAppCompatFlags(
    PWOW_CLIENT_THREADINFO pti)
{
    UNREFERENCED_PARAMETER(pti);

    ConnectIfNecessary(0);

    return GetClientInfo()->dwCompatFlags;
}

/***************************************************************************\
* GetAppCompatFlags2
*
* Compatibility flags for <= wVer apps.  Newer apps will get no hacks
* from this DWORD.
*
* History:
* 06-29-98 MCostea      Created.
\***************************************************************************/


FUNCLOG1(LOG_GENERAL, DWORD, DUMMYCALLINGTYPE, GetAppCompatFlags2, WORD, wVer)
DWORD GetAppCompatFlags2(
    WORD wVer)
{
    ConnectIfNecessary(0);
    /*
     * Newer apps should behave, so they get no hacks
     */
    if (wVer < GETAPPVER()) {
        return 0;
    }
    return GetClientInfo()->dwCompatFlags2;
}
#line 682
/***************************************************************************\
* _AdjustWindowRectEx (API)
*
*
*
* History:
* 10-24-90 darrinm      Ported from Win 3.0.
\***************************************************************************/

BOOL _AdjustWindowRectEx(
    LPRECT lprc,
    DWORD style,
    BOOL fMenu,
    DWORD dwExStyle)
{
    return RealAdjustWindowRectEx(lprc, style, fMenu, dwExStyle);
#line 704
}


BOOL RealAdjustWindowRectEx(
    LPRECT lprc,
    DWORD style,
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

    if (NeedsWindowEdge(style, dwExStyle, GETAPPVER() >= VER40))
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

#line 898
/***************************************************************************\
* TakeWindowSnapshot
*
* Helper routine to grab the visual appearance of a window to a bitmap.
*
\***************************************************************************/
HBITMAP TakeWindowSnapshot(HWND hwnd, HDC hdcWindow, HDC hdcSnapshot)
{
    PWND pwnd;
    int cx;
    int cy;
    HBITMAP hbmOld, hbmSnapshot;
    BOOL fOK = FALSE;

    pwnd = ValidateHwnd(hwnd);
    if (pwnd == NULL)
        return NULL;
    cx = pwnd->rcWindow.right - pwnd->rcWindow.left;
    cy = pwnd->rcWindow.bottom - pwnd->rcWindow.top;

    hbmSnapshot = CreateCompatibleBitmap(hdcWindow, cx, cy);
    if (hbmSnapshot == NULL) {
        return NULL;
    }

    hbmOld = SelectObject(hdcSnapshot, hbmSnapshot);

    /*
     * Try redirection first.
     */
    /*
    if (NtUserPrintWindow(hwnd, hdcSnapshot, 0)) {
        fOK = TRUE;
    } else */ {
        /*
         * We failed to redirect the window!  This can be caused by windows
         * with class or parent DCs.  Maybe other reasons as well.  Revert to
         * the old way of sending a WM_PRINT to the window.
         */
        
        UINT uBounds;
        RECT rcBounds;
        DWORD dwOldLayout = GDI_ERROR;
        BOOL fError = TRUE;

        /*
         * The WM_PRINT message expects a "normal" layout setting on the DC.
         * The message will handle RTL stuff itself.
         */
        dwOldLayout = SetLayout(hdcSnapshot, 0);

        /*
         * Clear the dirty bounds so we can tell if anything was painted.
         */
        SetBoundsRect(hdcSnapshot, NULL, DCB_RESET | DCB_ENABLE);

        /*
         * Get the actual image. The windows participating here must implement
         * WM_PRINTCLIENT or they will look ugly.
         */
        SendMessage(hwnd, WM_PRINT, (WPARAM)hdcSnapshot, PRF_CLIENT | PRF_NONCLIENT | PRF_CHILDREN | PRF_ERASEBKGND);

        /*
         * Check to see if the app painted in our DC.  We do this by checking to
         * see if the bounding rect of operations performed on the DC is set.
         */
        uBounds = GetBoundsRect(hdcSnapshot, &rcBounds, 0);
        if ((uBounds & DCB_RESET) && (!(uBounds & DCB_ACCUMULATE))) {
            goto Cleanup;
        }
    
        fOK = TRUE;

Cleanup:
        SetLayout(hdcSnapshot, dwOldLayout);
    }

    SelectObject(hdcSnapshot, hbmOld);

    if (!fOK) {
        DeleteObject(hbmSnapshot);
        hbmSnapshot = NULL;
    }

    return hbmSnapshot;
}

/***************************************************************************\
* AnimateWindow (API)
*
* Hide animations are done by updating a la full-drag.  Uses window's window
* region to do some of the magic.
*
* We have to put in the CLIPCHILDREN hack to work around a bug with the
* DC cache resetting attributes even if DCX_USESTYLE is not used whe
* the DC cache is invalidated.
*
* History:
* 9-Sep-1996    vadimg      created
\***************************************************************************/

#define AW_HOR          (AW_HOR_POSITIVE | AW_HOR_NEGATIVE | AW_CENTER)
#define AW_VER          (AW_VER_POSITIVE | AW_VER_NEGATIVE | AW_CENTER)

__inline int AnimInc(int x, int y, int z)
{
    return MultDiv(x, y, z);
}

__inline int AnimDec(int x, int y, int z)
{
    return x - AnimInc(x, y, z);
}


