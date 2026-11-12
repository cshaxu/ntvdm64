/****************************** Module Header ******************************\
*
* Module Name: clmenu.c
*
* Copyright (c) 1985-92, Microsoft Corporation
*
* Menu Loading Routines
*
* History:
* 24-Sep-1990 mikeke        From win30
* 29-Nov-1994 JimA          Moved from server.
\***************************************************************************/

/* DIVERGENCE(OPENNT-HOST-043/045): retain original menu resource clients
 * and parsers. Other menu APIs and private USER environment are excluded. */
#define WOW_ORIGINAL_MENU_CLIENT
#include "wow_menu_resource_bindings.h"


/***************************************************************************\
* MenuLoadWinTemplates
*
* Recursive routine that loads in the new style menu template and
* builds the menu. Assumes that the menu template header has already been
* read in and processed elsewhere...
*
* History:
* 28-Sep-1990 mikeke     from win30
\***************************************************************************/

/* DIVERGENCE(OPENNT-HOST-045): original menu parsers. The original
 * TIF_16BIT predicate is an explicit invocation parameter, not fabricated
 * private TEB state. Recursion carries the same value. */
#line 29
LPBYTE MenuLoadWinTemplates(
    LPBYTE lpMenuTemplate,
    HMENU *phMenu,
    BOOL wow16)
#line 32
{
    HMENU hMenu;
    UINT menuFlags = 0;
    long menuId = 0;
    LPWSTR lpmenuText;
    MENUITEMINFO    mii;
    UNICODE_STRING str;

    if (!(hMenu = NtUserCreateMenu()))
        goto memoryerror;

    do {

        /*
         * Get the menu flags.
         */
        menuFlags = (UINT)(*(WORD *)lpMenuTemplate);
        lpMenuTemplate += 2;

        if (menuFlags & ~MF_VALID) {
            RIPERR1(ERROR_INVALID_DATA, RIP_WARNING, "Menu Flags %lX are invalid", menuFlags);
            goto memoryerror;
        }


        if (!(menuFlags & MF_POPUP)) {
            menuId = *(WORD *)lpMenuTemplate;
            lpMenuTemplate += 2;
        }

        lpmenuText = (LPWSTR)lpMenuTemplate;

        if (*lpmenuText) {
            /*
             * Some Win3.1 and Win95 16 bit apps (chessmaster, mavis typing) know that
             * dwItemData for MFT_OWNERDRAW items is a pointer to a string in the resource data.
             * So WOW has given us the proper pointer from the 16 bit resource.
             */
            if ((menuFlags & MFT_OWNERDRAW)
                    && wow16) {
                lpmenuText = (LPWSTR)(*(DWORD UNALIGNED *)lpMenuTemplate);
                /*
                 * We'll skip one WCHAR later; so skip only the difference now.
                 */
                lpMenuTemplate += sizeof(DWORD) - sizeof(WCHAR);
            } else {
                /*
                 * If a string exists, then skip to the end of it.
                 */
                RtlInitUnicodeString(&str, lpmenuText);
                lpMenuTemplate = lpMenuTemplate + str.Length;
            }

        } else {
            lpmenuText = NULL;
        }

        /*
         * Skip over terminating NULL of the string (or the single NULL
         * if empty string).
         */
        lpMenuTemplate += sizeof(WCHAR);
        lpMenuTemplate = NextWordBoundary(lpMenuTemplate);

        RtlZeroMemory(&mii, sizeof(mii));
        mii.cbSize = sizeof(MENUITEMINFO);
#line 98
#ifdef MEMPHIS_MENUS
#line 99
#error DIVERGENCE OPENNT-HOST-132: MEMPHIS_MENUS branch is not selected; see mirror register.
#else
#line 103
        mii.fMask = MIIM_TYPE | MIIM_ID | MIIM_STATE;
#endif // MEMPHIS_MENUS
#line 105
        if (menuFlags & MF_POPUP) {
            mii.fMask |= MIIM_SUBMENU;
            lpMenuTemplate = MenuLoadWinTemplates(lpMenuTemplate,
                    (HMENU *)&menuId, wow16);
            if (!lpMenuTemplate)
                goto memoryerror;

            mii.hSubMenu = (HMENU)menuId;
        }

        /*
         * We have to take out MF_HILITE since that bit marks the end of a
         * menu in a resource file.  Since we shouldn't have any pre hilited
         * items in the menu anyway, this is no big deal.
         */
        if (menuFlags & MF_BITMAP) {

            /*
             * Don't allow bitmaps from the resource file.
             */
            menuFlags = (UINT)((menuFlags | MFT_RIGHTJUSTIFY) & ~MF_BITMAP);
        }

        // We have to take out MFS_HILITE since that bit marks the end of a menu in
        // a resource file.  Since we shouldn't have any pre hilited items in the
        // menu anyway, this is no big deal.
        mii.fState = (menuFlags & MFS_OLDAPI_MASK) & ~MFS_HILITE;
        mii.fType = (menuFlags & MFT_OLDAPI_MASK);
        if (menuFlags & MFT_OWNERDRAW)
        {
            mii.fMask |= MIIM_DATA;
            mii.dwItemData = (DWORD) lpmenuText;
            lpmenuText = 0;
        }
        mii.dwTypeData = (LPWSTR) lpmenuText;
        mii.cch = (UINT)-1;
        mii.wID = menuId;
#line 142
#ifdef MEMPHIS_MENUS
#line 143
#error DIVERGENCE OPENNT-HOST-132: MEMPHIS_MENUS branch is not selected; see mirror register.
#endif // MEMPHIS_MENUS
#line 145
        if (!NtUserThunkedMenuItemInfo(hMenu, MFMWFP_NOITEM, TRUE, TRUE,
                    &mii, lpmenuText ? &str : NULL, FALSE)) {
            if (menuFlags & MF_POPUP)
                NtUserDestroyMenu(mii.hSubMenu);
            goto memoryerror;
        }

    } while (!(menuFlags & MF_END));

    *phMenu = hMenu;
    return lpMenuTemplate;

memoryerror:
    if (hMenu != NULL)
        NtUserDestroyMenu(hMenu);
    *phMenu = NULL;
    return NULL;
}


/***************************************************************************\
* MenuLoadChicagoTemplates
*
* Recursive routine that loads in the new new style menu template and
* builds the menu. Assumes that the menu template header has already been
* read in and processed elsewhere...
*
* History:
* 15-Dec-93 SanfordS    Created
\***************************************************************************/

PMENUITEMTEMPLATE2 MenuLoadChicagoTemplates(
    PMENUITEMTEMPLATE2 lpMenuTemplate,
    HMENU *phMenu,
    WORD wResInfo)
{
    HMENU hMenu;
    HMENU hSubMenu;
    long menuId = 0;
    LPWSTR lpmenuText;
    MENUITEMINFO    mii;
    UNICODE_STRING str;
    DWORD           dwHelpID;

    if (!(hMenu = NtUserCreateMenu()))
        goto memoryerror;

    do {
        if (!(wResInfo & MFR_POPUP)) {
            /*
             * If the PREVIOUS wResInfo field was not a POPUP, the
             * dwHelpID field is not there.  Back up so things fit.
             */
            lpMenuTemplate = (PMENUITEMTEMPLATE2)(((LPBYTE)lpMenuTemplate) -
                    sizeof(lpMenuTemplate->dwHelpID));
            dwHelpID = 0;
        } else
            dwHelpID = lpMenuTemplate->dwHelpID;

        menuId = lpMenuTemplate->menuId;

        RtlZeroMemory(&mii, sizeof(mii));
        mii.cbSize = sizeof(MENUITEMINFO);
#line 208
#ifdef MEMPHIS_MENUS
#line 209
#error DIVERGENCE OPENNT-HOST-132: MEMPHIS_MENUS branch is not selected; see mirror register.
#else
#line 211
        mii.fMask = MIIM_TYPE | MIIM_ID | MIIM_STATE;
#endif // MEMPHIS_MENUS
#line 213

        mii.fType = lpMenuTemplate->fType;
        if (mii.fType & ~MFT_MASK) {
            RIPERR1(ERROR_INVALID_DATA, RIP_WARNING, "Menu Type flags %lX are invalid", mii.fType);
            goto memoryerror;
        }

        mii.fState  = lpMenuTemplate->fState;
        if (mii.fState & ~MFS_MASK) {
            RIPERR1(ERROR_INVALID_DATA, RIP_WARNING, "Menu State flags %lX are invalid", mii.fState);
            goto memoryerror;
        }

        wResInfo = lpMenuTemplate->wResInfo;
        if (wResInfo & ~(MF_END | MFR_POPUP)) {
            RIPERR1(ERROR_INVALID_DATA, RIP_WARNING, "Menu ResInfo flags %lX are invalid", wResInfo);
            goto memoryerror;
        }

        if (dwHelpID) {
            NtUserSetMenuContextHelpId(hMenu,dwHelpID);
        }
#line 235
#ifdef MEMPHIS_MENUS
#line 236
#error DIVERGENCE OPENNT-HOST-132: MEMPHIS_MENUS branch is not selected; see mirror register.
#else
#line 242
        lpmenuText = lpMenuTemplate->mtString[0] ?
                lpMenuTemplate->mtString : NULL;
#endif // MEMPHIS_MENUS
#line 245
        RtlInitUnicodeString(&str, lpmenuText);

        mii.dwTypeData = (LPWSTR) lpmenuText;

        /*
         * skip to next menu item template (DWORD boundary)
         */
        lpMenuTemplate = (PMENUITEMTEMPLATE2)
                (((LPBYTE)lpMenuTemplate) +
                sizeof(MENUITEMTEMPLATE2) +
                ((str.Length + 3) & ~3));

        if (mii.fType & MFT_OWNERDRAW)
        {
            mii.fMask |= MIIM_DATA;
            mii.dwItemData = (DWORD) mii.dwTypeData;
            mii.dwTypeData = 0;
        }

        if (wResInfo & MFR_POPUP) {
            mii.fMask |= MIIM_SUBMENU;
            lpMenuTemplate = MenuLoadChicagoTemplates(lpMenuTemplate,
                    &hSubMenu, MFR_POPUP);
            if (lpMenuTemplate == NULL)
                goto memoryerror;
            mii.hSubMenu = hSubMenu;
        }

        if (mii.fType & MFT_BITMAP) {

            /*
             * Don't allow bitmaps from the resource file.
             */
            mii.fType = (mii.fType | MFT_RIGHTJUSTIFY) & ~MFT_BITMAP;
        }

        mii.cch = (UINT)-1;
        mii.wID = menuId;
#line 283
#ifdef MEMPHIS_MENUS
#line 284
#error DIVERGENCE OPENNT-HOST-132: MEMPHIS_MENUS branch is not selected; see mirror register.
#endif // MEMPHIS_MENUS
#line 286
        if (!NtUserThunkedMenuItemInfo(hMenu, MFMWFP_NOITEM, TRUE, TRUE,
                    &mii, &str, FALSE)) {
            if (wResInfo & MFR_POPUP)
                NtUserDestroyMenu(mii.hSubMenu);
            goto memoryerror;
        }
        wResInfo &= ~MFR_POPUP;
    } while (!(wResInfo & MFR_END));

    *phMenu = hMenu;
    return lpMenuTemplate;

memoryerror:
    if (hMenu != NULL)
        NtUserDestroyMenu(hMenu);
    *phMenu = NULL;
    return NULL;
}


/***************************************************************************\
* CreateMenuFromResource
*
* Loads the menu resource named by the lpMenuTemplate parameter. The
* template specified by lpMenuTemplate is a collection of one or more
* MENUITEMTEMPLATE structures, each of which may contain one or more items
* and popup menus. If successful, returns a handle to the menu otherwise
* returns NULL.
*
* History:
* 28-Sep-1990 mikeke     from win30
\***************************************************************************/

HMENU CreateMenuFromResource(
    LPBYTE lpMenuTemplate,
    BOOL wow16)
#line 321
{
    HMENU hMenu = NULL;
    UINT menuTemplateVersion;
    UINT menuTemplateHeaderSize;

    /*
     * Win3 menu resource: First, strip version number word out of the menu
     * template.  This value should be 0 for Win3, 1 for win4.
     */
    /* DIVERGENCE(OPENNT-HOST-045): modern C pointer advancement. */
    menuTemplateVersion = *(WORD *)lpMenuTemplate;
    lpMenuTemplate += sizeof(WORD);
#line 331
    if (menuTemplateVersion > 1) {
        RIPMSG0(RIP_WARNING, "Menu Version number > 1");
        return NULL;
    }
    menuTemplateHeaderSize = *(WORD *)lpMenuTemplate;
    lpMenuTemplate += sizeof(WORD);
#line 336
    lpMenuTemplate += menuTemplateHeaderSize;
    switch (menuTemplateVersion) {
    case 0:
        MenuLoadWinTemplates(lpMenuTemplate, (HMENU *)&hMenu, wow16);
        break;

    case 1:
        MenuLoadChicagoTemplates((PMENUITEMTEMPLATE2)lpMenuTemplate, (HMENU *)&hMenu, 0);
        break;
    }
    return hMenu;
}

#line 378
HMENU CommonLoadMenu(
    HINSTANCE hmod,
    HANDLE hResInfo
    )
{
    HANDLE h;
    PVOID p;
    HMENU hMenu = NULL;

    if (h = LOADRESOURCE(hmod, hResInfo)) {

        if (p = LOCKRESOURCE(h, hmod)) {

            /* DIVERGENCE(OPENNT-HOST-045): WOW-only resource client. */
            hMenu = CreateMenuFromResource(p, TRUE);

#line 393
            UNLOCKRESOURCE(h, hmod);
        }
        /*
         * Win95 and Win3.1 do not free this resource; some 16 bit apps (chessmaster
         * and mavis typing) require this for their ownerdraw menu stuff.
         * For 32 bit apps, FreeResource is a nop anyway. For 16 bit apps,
         * Wow frees the 32 bit resource (returned by LockResource16)
         * in UnlockResource16; the actual 16 bit resource is freed when the task
         * goes away.
         *
         *   FREERESOURCE(h, hmod);
         */
    }

    return (hMenu);
}

HMENU WINAPI LoadMenuA(
    HINSTANCE hmod,
    LPCSTR lpName)
{
    HANDLE hRes;

    if (hRes = FINDRESOURCEA(hmod, (LPSTR)lpName, (LPSTR)RT_MENU))
        return CommonLoadMenu(hmod, hRes);
    else
        return NULL;
}

HMENU WINAPI LoadMenuW(
    HINSTANCE hmod,
    LPCWSTR lpName)
{
    HANDLE hRes;

    if (hRes = FINDRESOURCEW(hmod, (LPWSTR)lpName, RT_MENU))
        return CommonLoadMenu(hmod, hRes);
    else
        return NULL;
}

#line 434
#ifdef MEMPHIS_MENUS
#line 435
#error Excluded unsupported Memphis menu conversion profile
#endif // MEMPHIS_MENUS
#line 675

#line 676
#ifdef MEMPHIS_MENU_WATERMARKS
#line 677
#error Excluded unsupported Memphis menu watermark profile
#endif
#line 755
