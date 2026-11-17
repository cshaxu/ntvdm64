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

#define ClientSharedInfo()  (&gSharedInfo)
#define ServerInfo()  (gpsi)

//**************************************************************************
// These definitions get resolved differently for USER32 and USER16
//
//**************************************************************************
#define LPPOINTWOW                LPPOINT
#define NORMALIZEDSHORTVALUE(x)   (x)
#define WOWlstrlenA(x)            lstrlenA(x)
#define WOWRtlCopyMemory(lpDest, lpSrc, cb) RtlCopyMemory(lpDest, lpSrc, cb)

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
* MNLookUpItem
*
* Return a pointer to the menu item specified by wCmd and wFlags
*
* History:
*   10-11-90 JimA       Translated from ASM
*   01-07-93 FritzS     Ported from Chicago
\***************************************************************************/

PITEM MNLookUpItem(
    PMENU pMenu,
    UINT wCmd,
    BOOL fByPosition,
    PMENU *ppMenuItemIsOn)
{
    PITEM pItem;
    PITEM pItemRet = NULL;
    PITEM  pItemMaybe;
    PMENU   pMenuMaybe = NULL;
    int i;

    if (ppMenuItemIsOn != NULL)
        *ppMenuItemIsOn = NULL;

    if (pMenu == NULL || !pMenu->cItems || wCmd == MFMWFP_NOITEM) {
//      RIPERR0(ERROR_INVALID_PARAMETER, RIP_WARNING, "MNLookUpItem: invalid item");
        return NULL;
    }

    /*
     * dwFlags determines how we do the search
     */
    if (fByPosition) {
        if (wCmd < (UINT)pMenu->cItems) {
            pItemRet = &((PITEM)REBASEALWAYS(pMenu, rgItems))[wCmd];
            if (ppMenuItemIsOn != NULL)
                *ppMenuItemIsOn = pMenu;
            return (pItemRet);
        } else
            return NULL;
    }
    /*
     * Walk down the menu and try to find an item with an ID of wCmd.
     * The search procedes from the end of the menu (as was done in
     * assembler).
     */

/* this is the Chicago code, which walks from the front of the menu -- Fritz */


//        for (pItem = &pMenu->rgItems[i - 1]; pItemRet == NULL && i--; --pItem) {
    for (i = 0, pItem = REBASEALWAYS(pMenu, rgItems); i < (int)pMenu->cItems;
            i++, pItem++) {

        /*
         * If the item is a popup, recurse down the tree
         */
        if (pItem->spSubMenu != NULL) {
        //
        // COMPAT:
        // Allow apps to pass in menu handle as ID in menu APIs.  We
        // remember that this popup had a menu handle with the same ID
        // value.  This is a 2nd choice though.  We still want to see
        // if there's some actual command that has this ID value first.
        //
            if (pItem->wID == wCmd) {
                pMenuMaybe = pMenu;
                pItemMaybe = pItem;
            }

            pItemRet = MNLookUpItem((PMENU)REBASEPTR(pMenu, pItem->spSubMenu),
                    wCmd, FALSE, ppMenuItemIsOn);
            if (pItemRet != NULL)
                return pItemRet;
        } else if (pItem->wID == wCmd) {

                /*
                 * Found the item, now save things for later
                 */
                if (ppMenuItemIsOn != NULL)
                    *ppMenuItemIsOn = pMenu;
                return pItem;
        }
    }

    if (pMenuMaybe) {
        // no non popup menu match found -- use the 2nd choice popup menu
        // match
        if (ppMenuItemIsOn != NULL)
            *ppMenuItemIsOn = pMenuMaybe;
        return(pItemMaybe);
    }

    return(NULL);
}

/***************************************************************************\
* GetMenuState
*
* Either returns the state of a menu item or the state and item count
* of a popup.
*
* History:
* 10-11-90 JimA       Translated from ASM
\***************************************************************************/

UINT _GetMenuState(
    PMENU pMenu,
    UINT wId,
    UINT dwFlags)
{
    PITEM pItem;
    DWORD fFlags;

    /*
     * If the item does not exist, leave
     */
    if ((pItem = MNLookUpItem(pMenu, wId, (BOOL) (dwFlags & MF_BYPOSITION), NULL)) == NULL)
        return (UINT)-1;

    fFlags = pItem->fState | pItem->fType;

    if (pItem->spSubMenu != NULL) {
        /*
         * If the item is a popup, return item count in high byte and
         * popup flags in low byte
         */

        fFlags = ((fFlags | MF_POPUP) & 0x00FF) +
            (((PMENU)REBASEPTR(pMenu, pItem->spSubMenu))->cItems << 8);
    }

    return fFlags;
}


/***************************************************************************\
* GetPrevPwnd
*
*
*
* History:
* 11-05-90 darrinm      Ported from Win 3.0 sources.
\***************************************************************************/

PWND GetPrevPwnd(
    PWND pwndList,
    PWND pwndFind)
{
    PWND pwndFound, pwndNext;

    if (pwndList == NULL)
        return NULL;

    if (pwndList->spwndParent == NULL)
        return NULL;

    pwndNext = REBASEPWND(pwndList, spwndParent);
    pwndNext = REBASEPWND(pwndNext, spwndChild);
    pwndFound = NULL;

    while (pwndNext != NULL) {
        if (pwndNext == pwndFind)
            break;
        pwndFound = pwndNext;
        pwndNext = REBASEPWND(pwndNext, spwndNext);
    }

    return (pwndNext == pwndFind) ? pwndFound : NULL;
}


/***************************************************************************\
* _GetWindow (API)
*
*
* History:
* 11-05-90 darrinm      Ported from Win 3.0 sources.
* 02-19-91 JimA         Added enum access check
* 05-04-02 DarrinM      Removed enum access check and moved to USERRTL.DLL
\***************************************************************************/

PWND _GetWindow(
    PWND pwnd,
    UINT cmd)
{
    PWND pwndT;
    BOOL fRebase = FALSE;

    /*
     * If this is a desktop window, return NULL for sibling or
     * parent information.
     */
    if (GETFNID(pwnd) == FNID_DESKTOP) {
        switch (cmd) {
        case GW_CHILD:
            break;

        default:
            return NULL;
            break;
        }
    }

    /*
     * Rebase the returned window at the end of the routine
     * to avoid multiple test for pwndT == NULL.
     */
    pwndT = NULL;
    switch (cmd) {
    case GW_HWNDNEXT:
        pwndT = pwnd->spwndNext;
        fRebase = TRUE;
        break;

    case GW_HWNDFIRST:
        if (pwnd->spwndParent) {
            pwndT = REBASEPWND(pwnd, spwndParent);
            pwndT = REBASEPWND(pwndT, spwndChild);
            if (GetAppCompatFlags(NULL) & GACF_IGNORETOPMOST) {
                while (pwndT != NULL) {
                    if (!TestWF(pwndT, WEFTOPMOST))
                        break;
                    pwndT = REBASEPWND(pwndT, spwndNext);
                }
            }
        }
        break;

    case GW_HWNDLAST:
        pwndT = GetPrevPwnd(pwnd, NULL);
        break;

    case GW_HWNDPREV:
        pwndT = GetPrevPwnd(pwnd, pwnd);
        break;

    case GW_OWNER:
        pwndT = pwnd->spwndOwner;
        fRebase = TRUE;
        break;

    case GW_CHILD:
        pwndT = pwnd->spwndChild;
        fRebase = TRUE;
        break;

    default:
        RIPERR0(ERROR_INVALID_GW_COMMAND, RIP_VERBOSE, "");
        return NULL;
    }

    if (pwndT != NULL && fRebase)
        pwndT = REBASEPTR(pwnd, pwndT);

    return pwndT;
}

/***************************************************************************\
* _GetParent (API)
*
*
*
* History:
* 11-12-90 darrinm      Ported.
* 02-19-91 JimA         Added enum access check
* 05-04-92 DarrinM      Removed enum access check and moved to USERRTL.DLL
\***************************************************************************/

PWND _GetParent(
    PWND pwnd)
{
    /*
     * For 1.03 compatibility reasons, we should return NULL
     * for top level "tiled" windows and owner for other popups.
     * pwndOwner is set to NULL in xxxCreateWindow for top level
     * "tiled" windows.
     */
    if (!(TestwndTiled(pwnd))) {
        if (TestwndChild(pwnd))
            pwnd = REBASEPWND(pwnd, spwndParent);
        else
            pwnd = REBASEPWND(pwnd, spwndOwner);
        return pwnd;
    }

    /*
     * The window was not a child window; they may have been just testing
     * if it was
     */
    return NULL;
}


/***************************************************************************\
* GetSubMenu
*
* Return the handle of a popup menu.
*
* History:
* 10-11-90 JimA       Translated from ASM
\***************************************************************************/

PMENU _GetSubMenu(
    PMENU pMenu,
    int nPos)
{
    PITEM pItem;
    PMENU pPopup = NULL;

    /*
     * Make sure nPos refers to a valid popup
     */
    if ((UINT)nPos < (UINT)((PMENU)pMenu)->cItems) {
        pItem = &((PITEM)REBASEALWAYS(pMenu, rgItems))[nPos];
        if (pItem->spSubMenu != NULL)
            pPopup = (PMENU)REBASEPTR(pMenu, pItem->spSubMenu);

    }

    return (PVOID)pPopup;
}


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

#line 807


/***************************************************************************\
* _GetWindowRect (API)
*
*
*
* History:
* 26-Oct-1990 DarrinM   Implemented.
\***************************************************************************/

BOOL _GetWindowRect(
    PWND   pwnd,
    LPRECT prc)
{
    *prc = pwnd->rcWindow;

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
