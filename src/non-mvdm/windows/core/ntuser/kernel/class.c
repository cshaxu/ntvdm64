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

#line 16
#pragma hdrstop
#ifdef WOW_ORIGINAL_CLASS_REGISTRATION
#include <winternl.h>
#include "non-mvdm/windows/core/ntuser/inc/user.h"
#include "wow_user_client_view_layout.h"
#include "wow_class_client_bindings.h"

VOID WINAPI wow_original_class_initialize_identity(struct tagCLS *identity, ATOM atom)
{
    identity->atomClassName = atom;
}
VOID WINAPI wow_original_class_capture_menu(const CLSMENUNAME *menu,
    wow_class_lookup_entry *entry)
{
    entry->lpszClientAnsiMenuName = menu->pszClientAnsiMenuName;
    entry->lpszClientUnicodeMenuName = menu->pwszClientUnicodeMenuName;
}
BOOL WINAPI wow_original_class_unregister_committed(LPWSTR name, HANDLE module,
    PCLSMENUNAME menu, wow_class_remove_context *context)
{
    CLSMENUNAME result;
    if (!_UnregisterClass(name, module, &result, context)) return FALSE;
    if (menu) *menu = result;
    return TRUE;
}

#line 68
/***************************************************************************\
* _RegisterClass (API)
*
* This stub calls InternalRegisterClass to do its work and then does some
* additional work to save a pointer to the client-side menu name string.
* The menu string is returned by _GetClassInfo so the client can fix up
* a valid entry for the WNDCLASS lpszMenuName field.
*
* History:
* 04-26-91 DarrinM      Created.
\***************************************************************************/

ATOM _RegisterClassEx(
    LPWNDCLASSEX pwc,
    PROC lpfnWorker,
    PCLSMENUNAME pcmn,
    WORD fnid,
    DWORD dwFlags,
    LPDWORD pdwWOW, wow_class_register_context *context )
#line 87
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

    pcls = InternalRegisterClassEx(pwc, fnid, dwFlags |
        ((PtiCurrent()->TIF_flags & TIF_16BIT) ? CSF_WOWCLASS : 0));

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
#include <winternl.h>
#include "non-mvdm/windows/core/ntuser/inc/user.h"
#include "wow_class_remove_bindings.h"

#line 478
/***************************************************************************\
* _UnregisterClass (API)
*
* This API function is used to unregister a window class previously
* registered by the Application.
*
* Returns:
*     TRUE  if successful.
*     FALSE otherwise.
*
* NOTE:
*  1. The class name must have been registered earlier by this client
*     through RegisterClass().
*  2. The class name should not be one of the predefined control classes.
*  3. All windows created with this class must be destroyed before calling
*     this function.
*
* History:
* 10-15-90 DarrinM      Ported from Win 3.0 sources.
* 03-09-94 BradG        Fixed bug when ATOM was passed in
\***************************************************************************/

BOOL _UnregisterClass(
    LPWSTR lpszClassName,
    HANDLE hModule,
    PCLSMENUNAME pcmn, wow_class_remove_context *context)
#line 504
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
        if (!DestroyClass(ppcls))
        return FALSE;

#line 557
    return TRUE;
}
#else
#define WOW_ORIGINAL_CLASS_LOOKUP
#include "wow_class_lookup_bindings.h"

#line 891
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

#line 930
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


#line 1070
#ifdef DBG
#line 1071
#error
#endif

#line 375
#ifdef FE_SB // InternalRegisterClassEx()
#line 376
#error
#else
#line 378
#endif // FE_SB
