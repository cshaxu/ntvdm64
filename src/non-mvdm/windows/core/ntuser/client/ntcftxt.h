/***************************** Module Header ******************************\
* Module Name: ntcftxt.h
*
* Copyright (c) 1985 - 1999, Microsoft Corporation
*
* Kernel call forward stubs with text arguments
*
* Each function will be created with two flavors Ansi and Unicode
*
* 06-Jan-1992 IanJa      Moved from cf.h
* 18-Mar-1995 JimA       Ported from cftxt.h
\**************************************************************************/

#ifdef UNICODE
#line 15
#error DIVERGENCE: original Unicode client profile is not selected.
#else
#line 20
#define IS_ANSI TRUE
#line 21
#undef _UNICODE
#endif
#include <tchar.h>
#include "ntsend.h"
#line 88
extern WNDPROC mpPfnAddress[];

BOOL GetClassInfo(
    HINSTANCE hmod OPTIONAL,
    LPCTSTR pszClassName,
    LPWNDCLASS pwc)
{
    IN_STRING strClassName;
    LPWSTR pszMenuName;
    WNDCLASSEXW WndClass;

    /*
     * Make sure cleanup will work successfully
     */
    strClassName.fAllocated = FALSE;

    BEGINCALL_CLASSV()

        FIRSTCOPYLPTSTRID(&strClassName, lpClassNameVer);

        retval = (DWORD)NtUserGetClassInfo(
                hmod,
                strClassName.pstr,
                &WndClass,
                &pszMenuName,
                IS_ANSI);

        if (retval) {

            /*
             * Move the info from the WNDCLASSEX to the WNDCLASS structure
             * On 64-bit plaforms we'll have 32 bits of padding between style and
             * lpfnWndProc in WNDCLASS, so start the copy from the first 64-bit
             * aligned field and hand copy the rest.
             */
            RtlCopyMemory(&(pwc->lpfnWndProc), &(WndClass.lpfnWndProc), sizeof(WNDCLASS) - FIELD_OFFSET(WNDCLASS, lpfnWndProc));
            pwc->style = WndClass.style;

            /*
             * Update these pointers so they point to something real.
             * pszMenuName is actually just the pointer the app originally
             * passed to us.
             */
            pwc->lpszMenuName = (LPTSTR)pszMenuName;
            pwc->lpszClassName = pszClassName;
        }

    ERRORTRAP(0);
    CLEANUPLPTSTR(strClassName);
    ENDCALL(BOOL);
}

BOOL GetClassInfoEx(
    HINSTANCE hmod OPTIONAL,
    LPCTSTR pszClassName,
    LPWNDCLASSEX pwc)
{
    IN_STRING strClassName;
    LPWSTR pszMenuName;

    /*
     * Make sure cleanup will work successfully
     */
    strClassName.fAllocated = FALSE;

    BEGINCALL_CLASSV()

        FIRSTCOPYLPTSTRID(&strClassName, lpClassNameVer);

        retval = (DWORD)NtUserGetClassInfo(
                hmod,
                strClassName.pstr,
                (LPWNDCLASSEXW)pwc,
                &pszMenuName,
                IS_ANSI);

        if (retval) {

            /*
             * Update these pointers so they point to something real.
             * pszMenuName is actually just the pointer the app originally
             * passed to us.
             */
            pwc->lpszMenuName = (LPTSTR)pszMenuName;
            pwc->lpszClassName = pszClassName;
        }

    ERRORTRAP(0);
    CLEANUPLPTSTR(strClassName);
    ENDCALL(BOOL);
}
#line 582
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
    if (IS_PTR(lpszMenuName)) {
#ifdef UNICODE
#line 612
#error DIVERGENCE: original Unicode client profile is not selected.
#else
#line 620
        if ((pcmn->pszClientAnsiMenuName = StringDuplicate(lpszMenuName)) == NULL) {
            return FALSE;
        }

        if (!MBToWCS(lpszMenuName, -1, &(pcmn->pwszClientUnicodeMenuName), -1, TRUE)) {
            pcmn->pwszClientUnicodeMenuName = NULL;
        }
#endif // UNICODE
#line 628
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
    LocalFree(pcmn->pszClientAnsiMenuName);
    LocalFree(pcmn->pwszClientUnicodeMenuName);
#line 641
   return FALSE;
}
#line 722
/**************************************************************************\
* RegisterClassExWOW
*
* 03-22-96 GerardoB      Added Header
\**************************************************************************/
ATOM TEXT_FN(RegisterClassExWOW)(
    WNDCLASSEX *lpWndClass,
    LPDWORD pdwWOWstuff,
    WORD fnid,
    DWORD dwFlags)
{
    WNDCLASSEX WndClass;
    IN_STRING strClassName;
    IN_STRING strClassNameVer;
    IN_STRING strMenuName;
    DWORD dwExpWinVer;
    CLSMENUNAME cmn;
    TCHAR ClassNameVer[MAX_ATOM_LEN];
    LPTSTR lpClassNameVer; 

    strClassName.fAllocated = 0;
    strClassNameVer.fAllocated = 0;
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
         dwExpWinVer = VER40;
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
            RIPMSG1(RIP_VERBOSE, "RegisterClass: fixing up NULL hmodule to %#p",
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
        if (lpWndClass->hbrBackground > (HBRUSH)COLOR_MAX
                && !GdiValidateHandle(lpWndClass->hbrBackground)) {

            RIPMSG1(RIP_WARNING, "RegisterClass: Invalid class brush:%#p", lpWndClass->hbrBackground);
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

#ifndef UNICODE
        dwFlags |= CSF_ANSIPROC;
#endif // UNICODE

        if (dwExpWinVer > VER31) {
            dwFlags |= CSF_WIN40COMPAT;
        }
        
        if(GetClientInfo()->dwTIFlags & TIF_16BIT) {
          /*
           * No Fusion redirection for 16BIT apps
           */
           if(!(GetAppCompatFlags2(VERMAX) & GACF2_FORCEFUSION)) {
              dwFlags &= ~CW_FLAGS_VERSIONCLASS;
           }
        }
                 
        if (dwFlags & CSF_VERSIONCLASS) {
            lpClassNameVer = (LPTSTR)ClassNameToVersion((LPCWSTR)lpWndClass->lpszClassName, (LPWSTR)ClassNameVer, NULL, IS_ANSI);

            if (lpClassNameVer == NULL) {
                RIPMSG0(RIP_WARNING, "RegisterClass: Couldn't resolve class name");
                MSGERROR();
            }
        } else {
            lpClassNameVer = (LPTSTR)lpWndClass->lpszClassName;
        }

        COPYLPTSTRID(&strClassName, (LPTSTR)lpWndClass->lpszClassName);
        COPYLPTSTRID(&strClassNameVer, (LPTSTR)lpClassNameVer);

        retval = NtUserRegisterClassExWOW(
                &WndClass,
                strClassName.pstr,
                strClassNameVer.pstr,
                &cmn,
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
    CLEANUPLPTSTR(strClassNameVer);

    if (!retval) {
        if (IS_PTR(cmn.pszClientAnsiMenuName)) {
            LocalFree(KPVOID_TO_PVOID(cmn.pszClientAnsiMenuName));
        }
        if (IS_PTR(cmn.pwszClientUnicodeMenuName)) {
            LocalFree(KPVOID_TO_PVOID(cmn.pwszClientUnicodeMenuName));
        }
    }
    ENDCALL(BOOL);

BadParameter:
    RIPERR0(ERROR_INVALID_PARAMETER, RIP_VERBOSE, "RegisterClass: Invalid Parameter");
    return FALSE;

}
#line 1045

#ifdef UNICODE
FUNCLOG2(LOG_GENERAL, BOOL, DUMMYCALLINGTYPE, UnregisterClassW, LPCTSTR, pszClassName, HINSTANCE, hModule)
#else
FUNCLOG2(LOG_GENERAL, BOOL, DUMMYCALLINGTYPE, UnregisterClassA, LPCTSTR, pszClassName, HINSTANCE, hModule)
#endif // UNICODE
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

    BEGINCALL_CLASSV()

        FIRSTCOPYLPTSTRID(&strClassName, lpClassNameVer);

        retval = (DWORD)NtUserUnregisterClass(
                strClassName.pstr,
                hModule,
                &cmn);


        /*
         * Check explicity for TRUE so we don't get a !FALSE when
         * converttogui fails and the NtUser returns a status code intead of bool.
         */
        if (retval == TRUE) {
            /*
             * Free the menu strings if they are not resource IDs
             */
            if (IS_PTR(cmn.pszClientAnsiMenuName)) {
                LocalFree(KPVOID_TO_PVOID(cmn.pszClientAnsiMenuName));
            }
            if (IS_PTR(cmn.pwszClientUnicodeMenuName)) {
                LocalFree(KPVOID_TO_PVOID(cmn.pwszClientUnicodeMenuName));
            }
        }

    ERRORTRAP(0);
    CLEANUPLPTSTR(strClassName);
    ENDCALL(BOOL);
}
#line 1297
#ifdef UNICODE
FUNCLOG3(LOG_GENERAL, BOOL, DUMMYCALLINGTYPE, EnumDisplaySettingsW , LPCTSTR, lpszDeviceName, DWORD, iModeNum, LPDEVMODE, lpDevMode)
#else
FUNCLOG3(LOG_GENERAL, BOOL, DUMMYCALLINGTYPE, EnumDisplaySettingsA, LPCTSTR, lpszDeviceName, DWORD, iModeNum, LPDEVMODE, lpDevMode)
#endif // UNICODE
BOOL EnumDisplaySettings(
    LPCTSTR   lpszDeviceName,
    DWORD     iModeNum,
    LPDEVMODE lpDevMode)
{

    //
    // Work-around Win95 problem which does not require the caller
    // to initialize these two fields.
    //

    lpDevMode->dmDriverExtra = 0;
    lpDevMode->dmSize = FIELD_OFFSET(DEVMODE, dmICMMethod);

    return EnumDisplaySettingsEx(lpszDeviceName, iModeNum, lpDevMode, 0);
}
#line 1485
#ifdef UNICODE
FUNCLOG2(LOG_GENERAL, LONG, DUMMYCALLINGTYPE, ChangeDisplaySettingsW, LPDEVMODE, lpDevMode, DWORD, dwFlags)
#else
FUNCLOG2(LOG_GENERAL, LONG, DUMMYCALLINGTYPE, ChangeDisplaySettingsA, LPDEVMODE, lpDevMode, DWORD, dwFlags)
#endif // UNICODE
LONG ChangeDisplaySettings(
    LPDEVMODE lpDevMode,
    DWORD     dwFlags)
{

    /*
     * Compatibility
     */
    if (lpDevMode)
        lpDevMode->dmDriverExtra = 0;

    return ChangeDisplaySettingsEx(NULL, lpDevMode, NULL, dwFlags, NULL);
}
