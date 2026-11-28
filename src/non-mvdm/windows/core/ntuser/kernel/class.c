/****************************** Module Header ******************************\
* Module Name: class.c
*
* Copyright (c) 1985 - 1999, Microsoft Corporation
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
#include "wow_user_client_view_layout.h"
#line 18
/*
 * These arrays are used by Get/SetClassWord/Long.
 *
 * INDEX_OFFSET must refer to the first entry of afClassDWord[]
 */
#define INDEX_OFFSET GCLP_HICONSM
CONST BYTE afClassDWord[-INDEX_OFFSET] = {
    FIELD_SIZE(CLS, spicnSm),          // GCL_HICONSM       (-34)
    0,
    FIELD_SIZE(CLS, atomNVClassName),  // GCW_ATOM          (-32)
    0,
    0,
    0,
    0,
    0,
    FIELD_SIZE(CLS, style),            // GCL_STYLE         (-26)
    0,
    FIELD_SIZE(CLS, lpfnWndProc),      // GCL_WNDPROC       (-24)
    0,
    0,
    0,
    FIELD_SIZE(CLS, cbclsExtra),       // GCL_CBCLSEXTRA    (-20)
    0,
    FIELD_SIZE(CLS, cbwndExtra),       // GCL_CBWNDEXTRA    (-18)
    0,
    FIELD_SIZE(CLS, hModule),          // GCL_HMODULE       (-16)
    0,
    FIELD_SIZE(CLS, spicn),            // GCL_HICON         (-14)
    0,
    FIELD_SIZE(CLS, spcur),            // GCL_HCURSOR       (-12)
    0,
    FIELD_SIZE(CLS, hbrBackground),    // GCL_HBRBACKGROUND (-10)
    0,
    FIELD_SIZE(CLS, lpszMenuName),     // GCL_HMENUNAME      (-8)
    0,
    0,
    0,
    0,
    0,
    0,
    0
};

CONST BYTE aiClassOffset[-INDEX_OFFSET] = {
    FIELD_OFFSET(CLS, spicnSm),         // GCL_HICONSM
    0,
    FIELD_OFFSET(CLS, atomNVClassName),   // GCW_ATOM
    0,
    0,
    0,
    0,
    0,
    FIELD_OFFSET(CLS, style),           // GCL_STYLE
    0,
    FIELD_OFFSET(CLS, lpfnWndProc),     // GCL_WNDPROC
    0,
    0,
    0,
    FIELD_OFFSET(CLS, cbclsExtra),      // GCL_CBCLSEXTRA
    0,
    FIELD_OFFSET(CLS, cbwndExtra),      // GCL_CBWNDEXTRA
    0,
    FIELD_OFFSET(CLS, hModule),         // GCL_HMODULE
    0,
    FIELD_OFFSET(CLS, spicn),           // GCL_HICON
    0,
    FIELD_OFFSET(CLS, spcur),           // GCL_HCURSOR
    0,
    FIELD_OFFSET(CLS, hbrBackground),   // GCL_HBRBACKGROUND
    0,
    FIELD_OFFSET(CLS, lpszMenuName),    // GCL_MENUNAME
    0,
    0,
    0,
    0,
    0,
    0,
    0
};

#include "wow_class_client_bindings.h"
VOID WINAPI wow_original_class_initialize_identity(struct tagCLS *identity, ATOM atom)
{
    identity->atomClassName = atom;
    identity->atomNVClassName = atom;
}
VOID WINAPI wow_original_class_capture_menu(const CLSMENUNAME *menu,
    wow_class_lookup_entry *entry)
{
    entry->lpszClientAnsiMenuName = menu->pszClientAnsiMenuName;
    entry->lpszClientUnicodeMenuName = menu->pwszClientUnicodeMenuName;
}
BOOL WINAPI wow_original_class_unregister_committed(LPCWSTR name, HANDLE module,
    PCLSMENUNAME menu, wow_class_remove_context *context)
{
    CLSMENUNAME result;
    if (!_UnregisterClass(name, module, &result, context)) return FALSE;
    if (menu) *menu = result;
    return TRUE;
}

#line 98
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

ATOM xxxRegisterClassEx(
    WNDCLASSEXA *cczpwc,
    PCLSMENUNAME pcmn,
    WORD fnid,
    DWORD dwFlags,
    LPDWORD pdwWOW, wow_class_register_context *context)
#line 116
{
    PCLS pcls;
    PTHREADINFO  ptiCurrent = PtiCurrent();

    /*
     * NOTE -- lpszClassName and lpszMenuName in the wndclass may be client-side
     *         pointers. Use of those fields must be protected in try blocks.
     */

    /*
     * Convert a possible CallProc Handle into a real address. They may
     * have kept the CallProc Handle from some previous mixed GetClassinfo
     * or SetWindowLong.
     */
    if (ISCPDTAG(cczpwc->lpfnWndProc)) {
        PCALLPROCDATA pCPD;
        if (pCPD = HMValidateHandleNoRip((HANDLE)cczpwc->lpfnWndProc, TYPE_CALLPROC)) {
            cczpwc->lpfnWndProc = (WNDPROC)pCPD->pfnClientPrevious;
        }
    }

    pcls = InternalRegisterClassEx(cczpwc, fnid, dwFlags | ((ptiCurrent->TIF_flags & TIF_16BIT)? CSF_WOWCLASS : 0));
    if (pcls != NULL) {

        pcls->lpszClientUnicodeMenuName = pcmn->pwszClientUnicodeMenuName;
        pcls->lpszClientAnsiMenuName = pcmn->pszClientAnsiMenuName;

        /*
         * copy 5 WOW dwords.
         */
        if (pdwWOW && (ptiCurrent->TIF_flags & TIF_16BIT)) {
            RtlCopyMemory (PWCFromPCLS(pcls), pdwWOW, sizeof(WC));
        }

        if ((ptiCurrent->TIF_flags & TIF_16BIT) && ptiCurrent->ptdb) {
            pcls->hTaskWow = ptiCurrent->ptdb->hTaskWow;
        } else {
            pcls->hTaskWow = 0;
        }

        /*
         * For some (presumably good) reason Win 3.1 changed RegisterClass
         * to return the classes classname atom.
         */
        return pcls->atomNVClassName;
    } else {
        return 0;
    }
}


#elif defined(WOW_ORIGINAL_CLASS_REMOVAL)
#include <winternl.h>
#include "wow_user_client_view_layout.h"
#include "wow_class_remove_bindings.h"
#line 550
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
    LPCWSTR ccxlpszClassName,
    HANDLE hModule,
    PCLSMENUNAME pcmn, wow_class_remove_context *context)
#line 576
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
    atomT = FindClassAtom(ccxlpszClassName);
    ppcls = _InnerGetClassPtr(atomT, &ptiCurrent->ppi->pclsPrivateList, hModule);
    if (ppcls == NULL) {
        /*
         * Maybe this is a public class.
         */
        ppcls = _InnerGetClassPtr(atomT, &ptiCurrent->ppi->pclsPublicList, NULL);
        if (ppcls == NULL) {
            RIPERR1(ERROR_CLASS_DOES_NOT_EXIST, RIP_WARNING, "UnregisterClass: Class does not exist; atom=%lX", (DWORD)atomT);
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
    if (!DestroyClass(ppcls)) return FALSE;

    return TRUE;
}


#else
#define WOW_ORIGINAL_CLASS_LOOKUP
#include "wow_class_lookup_bindings.h"
#line 1016
PPCLS _InnerGetClassPtr(
    ATOM atom,
    PPCLS ppcls,
    HANDLE hModule)
{
    if (atom == 0)
        return NULL;

    while (*ppcls != NULL) {
        if ((*ppcls)->atomClassName == atom &&
                (hModule == NULL || HIWORD((ULONG_PTR)(*ppcls)->hModule) == HIWORD((ULONG_PTR)hModule)) &&
                !((*ppcls)->CSF_flags & CSF_WOWDEFERDESTROY)) {
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
* Scan the passed-in class list for the specified class. Return NULL if
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
     * we may only need to check for hModClient. Check this out.
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

#line 436
#ifdef FE_SB // InternalRegisterClassEx()
#line 437
#error Excluded unsupported kernel DBCS menu allocation
#else
#line 439
#endif // FE_SB
#line 965
#ifdef _WIN64
#line 966
#error Excluded unsupported native x64 USER class owner
#endif
#line 1207
#if DBG
#line 1208
#error Excluded unsupported kernel USER debug diagnostics
#endif
