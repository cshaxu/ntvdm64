/* Copyright (c) 1985-1995, Microsoft Corporation.
 * Derived from OpenNT windows/core/ntuser/client/client.c, revision
 * 5c5b979ec08c17d3ca2eb70e8aad62d26515d01c.
 * T483 S6 P24 owner-approved extraction: DispatchClientMessage.
 * Original function bodies and existing host bindings are preserved.
 */
#define WOW_ORIGINAL_CLIENT_DISPATCH
#include "wow_window_dispatch_bindings.h"
#define WNDPROC_WOW WOW_WINDOW_PROC_TAG


/***************************************************************************\
* DispatchMessage
*
* !!! Warning if this function becomes more complicated then look at the
* server code for WrapCallProc
*
* pwnd is threadlocked in the kernel and thus always valid.
*
* 19-Aug-1992 mikeke   created
\***************************************************************************/

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
