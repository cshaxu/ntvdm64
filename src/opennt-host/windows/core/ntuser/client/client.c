/**************************************************************************\
* Module Name: client.c
*
* Client/Server call related routines.
*
* Copyright (c) 1985-1995, Microsoft Corporation
*
* History:
* 04-Dec-1990 SMeans    Created.
\**************************************************************************/

/* DIVERGENCE(OPENNT-HOST-038): retain original DispatchClientMessage;
 * other USER client entry points and the private precompiled environment
 * are excluded. The synchronous native view supplies HWND, original WW
 * storage and the registered WOW callback, not an NT4 kernel PWND. */
#ifdef WOW_ORIGINAL_CLASS_CLIENT
/* DIVERGENCE(OPENNT-HOST-046): separately compiled original ANSI class
 * client closure; bindings supply version/context and native publication. */
#include "wow_class_client_bindings.h"
#include "../inc/ntcftxt.h"
ATOM
WINAPI
RegisterClassWOWA(
    WNDCLASSA *lpWndClass,
    LPDWORD pdwWOWstuff)
{
    WNDCLASSEXA wc;

    memcpy(&(wc.style), lpWndClass, sizeof(WNDCLASSA));
    wc.hIconSm = NULL;
    wc.cbSize = sizeof(WNDCLASSEXA);

    return RegisterClassExWOWA(&wc, pdwWOWstuff, NULL, 0);
}
#else
#define WOW_ORIGINAL_CLIENT_DISPATCH
#include "wow_window_dispatch_bindings.h"

LONG DispatchClientMessage(
    PWND pwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam,
    DWORD pfn)
{

    /*
     * Add assert to catch dispatching messages to a thread not associated
     * with a desktop.
     */
    UserAssert(GetClientInfo()->ulClientDelta != 0);

    /*
     * More complicate then regular CALLPROC_WOWCHECK() we want to get the
     * PWW so wow doesn't have to
     */
    if (WNDPROC_WOW & (DWORD)pfn) {
        return (*pfnWowWndProcEx)(HW(pwnd), message, wParam, lParam, (DWORD)pfn, pwnd->adwWOW);
    } else {
        return ((WNDPROC)pfn)(HW(pwnd), message, wParam, lParam);
    }
}

/* DIVERGENCE(OPENNT-HOST-038): remaining client entry points are excluded. */
#endif

