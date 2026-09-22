/***************************** Module Header ******************************\
* Module Name: ntcftxt.h
*
* Copyright (c) 1985-95, Microsoft Corporation
*
* Kernel call forward stubs with text arguments
*
* Each function will be created with two flavors Ansi and Unicode
*
* 06-Jan-1992 IanJa      Moved from cf.h
* 18-Mar-1995 JimA       Ported from cftxt.h
\**************************************************************************/

/* DIVERGENCE(OPENNT-HOST-036): retain the complete ANSI class client
 * closure: duplication, menu capture, registration and unregistration.
 * Other text APIs and the private USER precompiled environment are cropped.
 * The consuming translation unit supplies the original-shaped declarations,
 * call/capture macros and finite host bindings; no server engine is imported. */

/**************************************************************************\
* StringDuplicate
*
* 03-25-96 GerardoB         Added Header.
\**************************************************************************/
#define StringDuplicate TEXT_FN(StringDuplicate)
LPTSTR StringDuplicate(LPCTSTR ptszDup) {
    LPTSTR ptsz;
    ULONG cb;

    cb = (_tcslen(ptszDup) + 1) * sizeof(TCHAR);
    ptsz = (LPTSTR)LocalAlloc(NONZEROLPTR, cb);
    if (ptsz != NULL) {
        RtlCopyMemory(ptsz, ptszDup, cb);
    }
    return ptsz;
}
/**************************************************************************\
* InitClsMenuName
*
* 03-22-96 GerardoB         Created.
\**************************************************************************/
#define InitClsMenuName TEXT_FN(InitClsMenuName)
BOOL InitClsMenuName (PCLSMENUNAME pcmn, LPCTSTR lpszMenuName, PIN_STRING pstrMenuName)
{
    /*
     * We check the high-word because this may be a resource-ID.
     */
    if (HIWORD(lpszMenuName) != 0) {
#ifdef UNICODE
        if ((pcmn->pwszClientUnicodeMenuName = StringDuplicate(lpszMenuName)) == NULL) {
            return FALSE;
        }

        if (!WCSToMB(lpszMenuName, -1, &(pcmn->pszClientAnsiMenuName), -1, TRUE)) {
            pcmn->pszClientAnsiMenuName = NULL;
        }
#else
        if ((pcmn->pszClientAnsiMenuName = StringDuplicate(lpszMenuName)) == NULL) {
            return FALSE;
        }

        if (!MBToWCS(lpszMenuName, -1, &(pcmn->pwszClientUnicodeMenuName), -1, TRUE)) {
            pcmn->pwszClientUnicodeMenuName = NULL;
        }
#endif // UNICODE
    } else {
        /* Copy the ID */
        pcmn->pszClientAnsiMenuName = (LPSTR)lpszMenuName;
        pcmn->pwszClientUnicodeMenuName = (LPWSTR)lpszMenuName;
    }

    COPYLPTSTRID(pstrMenuName, lpszMenuName);
    pcmn->pusMenuName = pstrMenuName->pstr;

    return TRUE;

    goto errorexit; /* Keep the compiler happy */
errorexit: /* Used by COPYLPTSTRID */
    /* DIVERGENCE(OPENNT-HOST-037): capture can fail after both client
     * menu names have been allocated. No class owns them on this path. */
    LocalFree(pcmn->pszClientAnsiMenuName);
    LocalFree(pcmn->pwszClientUnicodeMenuName);
   return FALSE;
}

/* DIVERGENCE(OPENNT-HOST-036): SetClassLong is outside this client slice. */
/**************************************************************************\
* RegisterClassExWOW
*
* 03-22-96 GerardoB      Added Header
\**************************************************************************/
ATOM TEXT_FN(RegisterClassExWOW)(
    WNDCLASSEX *lpWndClass,
    LPDWORD pdwWOWstuff,
    PROC lpfnWorker,
    WORD fnid)
{
    WNDCLASSEX WndClass;
    IN_STRING strClassName;
    IN_STRING strMenuName;
    DWORD dwFlags, dwExpWinVer;
    CLSMENUNAME cmn;

    strClassName.fAllocated = 0;
    strMenuName.fAllocated  = 0;

    /*
     * Skip validation for our classes
     */
    if (fnid !=0) {
        /*
         * This is a hack to bypass validation for DDE classes
         * specifically, allow them to use hmodUser.
         */
         if (fnid == FNID_DDE_BIT) {
             fnid = 0;
         }
    } else {
        if (lpWndClass->cbSize != sizeof(WNDCLASSEX)) {
            RIPMSG0(RIP_WARNING, "RegisterClass: Invalid cbSize");
        }

        if (lpWndClass->cbClsExtra < 0 ||
                lpWndClass->cbWndExtra < 0) {
            RIPMSG0(RIP_WARNING, "RegisterClass: invalid cb*Extra");
            goto BadParameter;
        }

        /*
         * Validate hInstance
         * Don't allow 4.0 apps to use hmodUser
         */
         if ((lpWndClass->hInstance == hmodUser)
                && (GetClientInfo()->dwExpWinVer >= VER40)) {
             RIPMSG0(RIP_WARNING, "RegisterClass: Cannot use USER's hInstance");
             goto BadParameter;
         } else if (lpWndClass->hInstance == NULL) {
            /*
             * For 32 bit apps we need to fix up the hInstance because Win 95 does
             * this in their thunk MapHInstLS
             */

            lpWndClass->hInstance = GetModuleHandle(NULL);
            RIPMSG1(RIP_WARNING, "RegisterClass: fixing up NULL hmodule to %lX",
                    lpWndClass->hInstance);
        }

        dwExpWinVer = GETEXPWINVER(lpWndClass->hInstance);


        /*
         * Check for valid style bits and strip if appropriate
         */
        if (lpWndClass->style & ~CS_VALID40) {

            if (dwExpWinVer > VER31) {
                RIPMSG0(RIP_WARNING, "RegisterClass: Invalid class style");
                goto BadParameter;
            }

            /*
             * Old application - strip bogus bits and pass through
             */
            RIPMSG0(RIP_WARNING, "RegisterClass: Invalid class style, stripping bad styles");
            lpWndClass->style &= CS_VALID40;
        }

        /*
         * Validate hbrBackground
         */
        if ((UINT) lpWndClass->hbrBackground > COLOR_MAX
                && !GdiValidateHandle(lpWndClass->hbrBackground)) {

            RIPMSG1(RIP_WARNING, "RegisterClass: Invalid class brush:%#lx", lpWndClass->hbrBackground);
            if (dwExpWinVer > VER30) {
                goto BadParameter;
            }

            lpWndClass->hbrBackground = NULL;
        }

    } /* if (fnid !=0) */


    if (!InitClsMenuName(&cmn, lpWndClass->lpszMenuName, &strMenuName)) {
        return FALSE;
    }

    BEGINCALL()

        WndClass = *lpWndClass;

#ifdef UNICODE
        dwFlags = 0;
#else
        dwFlags = CSF_ANSIPROC;
#endif // UNICODE

        COPYLPTSTRID(&strClassName, (LPTSTR)lpWndClass->lpszClassName);

        retval = NtUserRegisterClassExWOW(
                &WndClass,
                strClassName.pstr,
                &cmn,
                lpfnWorker,
                fnid,
                dwFlags,
                pdwWOWstuff);

        /*
         * Return the atom associated with this class or if earlier
         * than Win 3.1 convert it to a strict BOOL (some apps check)
         */
        if (GETEXPWINVER(lpWndClass->hInstance) < VER31)
            retval = !!retval;

    ERRORTRAP(0);
    CLEANUPLPTSTR(strMenuName);     /* Initialized by InitClsMenuName */
    CLEANUPLPTSTR(strClassName);

    if (!retval) {
        if (HIWORD(cmn.pszClientAnsiMenuName) != 0) {
            LocalFree(cmn.pszClientAnsiMenuName);
        }
        if (HIWORD(cmn.pwszClientUnicodeMenuName) != 0) {
            LocalFree(cmn.pwszClientUnicodeMenuName);
        }
    }
    ENDCALL(BOOL);

BadParameter:
    RIPERR0(ERROR_INVALID_PARAMETER, RIP_VERBOSE, "RegisterClass: Invalid Parameter");
    return FALSE;

}

/* DIVERGENCE(OPENNT-HOST-036): unrelated text APIs between class operations
 * are excluded; retain the original unregister/cleanup body. */
BOOL UnregisterClass(
    LPCTSTR pszClassName,
    HINSTANCE hModule)
{
    IN_STRING strClassName;
    CLSMENUNAME cmn;

    /*
     * Make sure cleanup will work successfully
     */
    strClassName.fAllocated = FALSE;

    BEGINCALL()

        FIRSTCOPYLPTSTRID(&strClassName, pszClassName);

        retval = (DWORD)NtUserUnregisterClass(
                strClassName.pstr,
                hModule,
                &cmn);

        if (retval) {
            /*
             * Free the menu strings if they are not resource IDs
             */
            if (HIWORD(cmn.pszClientAnsiMenuName) != 0) {
                LocalFree(cmn.pszClientAnsiMenuName);
            }
            if (HIWORD(cmn.pwszClientUnicodeMenuName) != 0) {
                LocalFree(cmn.pwszClientUnicodeMenuName);
            }
        }

    ERRORTRAP(0);
    CLEANUPLPTSTR(strClassName);
    ENDCALL(BOOL);
}

/* DIVERGENCE(OPENNT-HOST-036): remaining text API families are excluded. */


