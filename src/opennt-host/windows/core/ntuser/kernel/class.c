/****************************** Module Header ******************************\
* Module Name: class.c
*
* Copyright (c) 1985-96, Microsoft Corporation
*
* This module contains RegisterClass and the related window class management
* functions.
*
* History:
* 10-16-90 DarrinM      Ported functions from Win 3.0 sources.
* 02-01-91 mikeke       Added Revalidation code (None)
* 04-08-91 DarrinM      C-S-ized and removed global/public class support.
\***************************************************************************/

/* DIVERGENCE(OPENNT-HOST-039/059/060): original class-list lookup and separately
 * compiled server registration wrapper. Native storage remains external; no USER
 * server, kernel allocation, process, desktop or handle engine is imported.
 * The binding supplies only their finite caller-owned metadata view. */
#ifdef WOW_ORIGINAL_CLASS_REGISTRATION
/* DIVERGENCE(OPENNT-HOST-059): original server registration wrapper;
 * explicit current-thread and required registration/CPD boundary bindings. */
#include "wow_class_register_bindings.h"

ATOM _RegisterClassEx(
    LPWNDCLASSEX pwc,
    PROC lpfnWorker,
    PCLSMENUNAME pcmn,
    WORD fnid,
    DWORD dwFlags,
    LPDWORD pdwWOW,
    wow_class_register_context *context )
{
    PCLS pcls;

    /*
     * Convert a possible CallProc Handle into a real address.  They may
     * have kept the CallProc Handle from some previous mixed GetClassinfo
     * or SetWindowLong.
     */
    if (ISCPDTAG(pwc->lpfnWndProc)) {
        PCALLPROCDATA pCPD;
        if  (pCPD = HMValidateHandleNoRip((HANDLE)pwc->lpfnWndProc, TYPE_CALLPROC)) {
            pwc->lpfnWndProc = (WNDPROC)pCPD->pfnClientPrevious;
        }
    }

    pcls = InternalRegisterClassEx(pwc, fnid, dwFlags);

    if (pcls != NULL) {
        PTHREADINFO ptiCurrent = PtiCurrent();

        pcls->lpfnWorker = lpfnWorker;

        pcls->lpszClientUnicodeMenuName = pcmn->pwszClientUnicodeMenuName;
        pcls->lpszClientAnsiMenuName = pcmn->pszClientAnsiMenuName;

        /*
         * copy 5 WOW dwords.
         */
        if (pdwWOW)
            RtlCopyMemory (pcls->adwWOW, pdwWOW, sizeof(pcls->adwWOW));
        if ((ptiCurrent->TIF_flags & TIF_16BIT) && ptiCurrent->ptdb) {
            pcls->hTaskWow = ptiCurrent->ptdb->hTaskWow;
        } else {
            pcls->hTaskWow = 0;
        }

        /*
         * For some (presumably good) reason Win 3.1 changed RegisterClass
         * to return the classes classname atom.
         */
        return pcls->atomClassName;
    } else {
        return 0;
    }
}
#elif defined(WOW_ORIGINAL_CLASS_REMOVAL)
/* DIVERGENCE(OPENNT-HOST-060): original removal policy with explicit context
 * and native destruction failure propagation. */
#include "wow_class_remove_bindings.h"

BOOL _UnregisterClass(
    LPWSTR lpszClassName,
    HANDLE hModule,
    PCLSMENUNAME pcmn,
    wow_class_remove_context *context)
{
    ATOM atomT;
    PPCLS ppcls;
    PTHREADINFO ptiCurrent;

    CheckCritIn();

    ptiCurrent = PtiCurrent();

    /*
     * Check whether the given ClassName is already registered by the
     * Application with the given handle.
     * Return error, if either the Class does not exist or it does not
     * belong to the calling process.
     */

    /*
     * bradg (3/9/95) - Must first check to see if an ATOM has been passed
     */
    atomT = FindClassAtom(lpszClassName);

    ppcls = _InnerGetClassPtr(atomT, &ptiCurrent->ppi->pclsPrivateList, hModule);
    if (ppcls == NULL) {
        /*
         * Maybe this is a public class.
         */
        ppcls = _InnerGetClassPtr(atomT, &ptiCurrent->ppi->pclsPublicList, NULL);
        if (ppcls == NULL) {
            RIPERR0(ERROR_CLASS_DOES_NOT_EXIST, RIP_VERBOSE, "");
            return FALSE;
        }
    }

    /*
     * If any windows created with this class still exist return an error.
     */
    if ((*ppcls)->cWndReferenceCount != 0) {
        RIPERR0(ERROR_CLASS_HAS_WINDOWS, RIP_WARNING, "UnregisterClass: Class still has window");
        return FALSE;
    }

    /*
     * Return client side pointers for cleanup
     */
    pcmn->pszClientAnsiMenuName = (*ppcls)->lpszClientAnsiMenuName;
    pcmn->pwszClientUnicodeMenuName = (*ppcls)->lpszClientUnicodeMenuName;
    pcmn->pusMenuName = NULL;

    /*
     * Release the Window class and related information.
     */
    /* DIVERGENCE(OPENNT-HOST-060): propagate native class-release failure. */
    if (!DestroyClass(ppcls))
        return FALSE;

    return TRUE;
}
#else
#define WOW_ORIGINAL_CLASS_LOOKUP
#include "wow_class_lookup_bindings.h"

PPCLS _InnerGetClassPtr(
    ATOM atom,
    PPCLS ppcls,
    HANDLE hModule)
{
    if (atom == 0)
        return NULL;

    while (*ppcls != NULL) {
        if ((*ppcls)->atomClassName == atom &&
                (hModule == NULL || HIWORD((*ppcls)->hModule) == HIWORD(hModule)) &&
                !((*ppcls)->flags & CSF_WOWDEFERDESTROY)) {
            return ppcls;
        }

        ppcls = (PPCLS)*ppcls;
    }

    return NULL;
}


/***************************************************************************\
* GetClassPtr
*
* Note: This returns a "pointer-to-PCLS" and not "PCLS".
*
* Scan the passed-in class list for the specified class.  Return NULL if
* the class isn't in the list.
*
* History:
* 10-16-90 darrinm      Ported this puppy.
* 04-08-91 DarrinM      Rewrote to remove global classes.
* 08-14-92 FritzS     Changed check to HIWORD only to allow Wow apps to
*                     share window classes between instances of an app.
                      (For Wow apps, HiWord of hInstance is 16-bit module,
                       and LoWord is 16-bit hInstance
\***************************************************************************/

PPCLS GetClassPtr(
    ATOM atom,
    PPROCESSINFO ppi,
    HANDLE hModule)
{
    PPCLS ppcls;

    /*
     * First search public then private then usersrv registered classes
     */
    ppcls = _InnerGetClassPtr(atom, &ppi->pclsPrivateList, hModule);
    if (ppcls)
        return ppcls;

    ppcls = _InnerGetClassPtr(atom, &ppi->pclsPublicList, NULL);
    if (ppcls)
        return ppcls;

    /*
     * Next seach public and private classes and override hmodule;
     * some apps (bunny) do a GetClassInfo(dialog) and RegisterClass
     * and only change the wndproc which set the hmodule to be just
     * like usersrv created it even though it is in the app's public
     * or private class list
     */

    /*
     * Later -- since we are no longer returning hModuleWin to any app,
     * we may only need to check for hModClient.  Check this out.
     *      FritzS
     */

    ppcls = _InnerGetClassPtr(atom, &ppi->pclsPrivateList, hModClient);
    if (ppcls)
        return ppcls;

    ppcls = _InnerGetClassPtr(atom, &ppi->pclsPublicList, hModClient);
    if (ppcls)
        return ppcls;

    /*
     * Search the system class list
     */
    ppcls = _InnerGetClassPtr(atom, &gpclsList, NULL);
    return ppcls;
}
#endif
