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
#if defined(WOW_ORIGINAL_DEF_WINDOW_BITS)
/* DIVERGENCE(OPENNT-HOST-094): retain the complete original bitmap merge;
 * only the shared-table environment and compilation subset are adapted. */
#include "wow_default_message_bindings.h"
#include <assert.h>
#define gSharedInfo wow_default_message_state
#define UserAssert(expression) assert(expression)

#line 2203
WORD WowGetDefWindowProcBits(
    PBYTE    pDefWindowProcBits,
    WORD     cbDefWindowProcBits)
{
    WORD  wMaxMsg;
    PBYTE pbSrc, pbDst, pbDstEnd;

    /*
     * Merge the bits from gpsi->gabDefWindowMsgs and
     * gpsi->gabDefWindowSpecMsgs into WOW's DefWindowProcBits.  These two
     * indicate which messages must go directly to the server and which
     * can be handled with some special code in DefWindowProcWorker.
     * Bitwise OR'ing the two gives a bit array with 1 in the bit field
     * for each message that must go to user32's DefWindowProc, and 0
     * for those that can be returned back to the client immediately.
     *
     * For speed we assume WOW has zeroed the buffer, in fact it's in
     * USER.EXE's code segment and is zeroed in the image.
     */

    wMaxMsg = max(gSharedInfo.DefWindowMsgs.maxMsgs,
            gSharedInfo.DefWindowSpecMsgs.maxMsgs);

    UserAssert((wMaxMsg / 8 + 1) <= cbDefWindowProcBits);

    //
    // If the above assertion fires, the size of the DWPBits array in
    // \nt\private\mvdm\wow16\user\user.asm needs to be increased.
    //

    /* First copy the bits from DefWindowMsgs */

    RtlCopyMemory(
        pDefWindowProcBits,
        gSharedInfo.DefWindowMsgs.abMsgs,
        gSharedInfo.DefWindowMsgs.maxMsgs / 8 + 1
        );

    /* Next OR in the bits from DefWindowSpecMsgs */

    pbSrc = gSharedInfo.DefWindowSpecMsgs.abMsgs;
    pbDst = pDefWindowProcBits;
    pbDstEnd = pbDst + (gSharedInfo.DefWindowSpecMsgs.maxMsgs / 8 + 1);

    while (pbDst < pbDstEnd)
    {
        *pbDst++ |= *pbSrc++;
    }

    return wMaxMsg;
}

#elif defined(WOW_ORIGINAL_CONTROL_STATE)
/* DIVERGENCE(OPENNT-HOST-088): unchanged original state guards; the scoped
 * call seam binds symbolic services to current exports and refreshes WND. */
#define WOW_ORIGINAL_CONTROL_STATE_CALLS
#include "wow_edit_call_bindings.h"

#line 2826
void SetWindowState(
    PWND pwnd,
    UINT flags)
{
    if (TestWF(pwnd, flags) != LOBYTE(flags))
        NtUserCallHwndParam(HWq(pwnd), flags, SFI_SETWINDOWSTATE);
}

#line 2834
void ClearWindowState(
    PWND pwnd,
    UINT flags)
{
    if (TestWF(pwnd, flags))
        NtUserCallHwndParam(HWq(pwnd), flags, SFI_CLEARWINDOWSTATE);
}

#elif defined(WOW_ORIGINAL_DIALOG_EDIT)
/* DIVERGENCE(OPENNT-HOST-071): retain original callback-only dialog EDIT
 * allocation/release bodies. The existing WOW registration supplies inputs;
 * this separately selected subset does not import USER server transport. */
#include "wow_edit_memory_bindings.h"

/* DIVERGENCE(OPENNT-HOST-081): complete original registration-based
 * dialog initializer; no direct private WOW entry replaces this callback. */
#line 2326
DWORD WOWDlgInit(
HWND hwndDlg,
LONG lParam)
{
    UserAssert(pfnInitDlgCallback != NULL);

    return (*pfnInitDlgCallback)(hwndDlg, lParam);
}

/***************************************************************************\
* GetEditDS
*
* This is a callback to WOW used to allocate a segment for DS_LOCALEDIT
* edit controls.  The segment is disguised to look like a WOW hInstance.
*
* 06-19-92 sanfords Created
\***************************************************************************/
#line 2345
HANDLE GetEditDS()
{
    UserAssert(pfn16GlobalAlloc != NULL);

    return((HANDLE)((*pfn16GlobalAlloc)(GHND | GMEM_SHARE, 256)));
}



/***************************************************************************\
* ReleaseEditDS
*
* This is a callback to WOW used to free a segment for DS_LOCALEDIT
* edit controls.
*
* 06-19-92 sanfords Created
\***************************************************************************/
#line 2362
VOID ReleaseEditDS(
HANDLE h)
{
    UserAssert(pfn16GlobalFree != NULL);

    (*pfn16GlobalFree)(LOWORD(h));
}

#elif defined(WOW_ORIGINAL_CLASS_CLIENT)
/* DIVERGENCE(OPENNT-HOST-046): separately compiled original ANSI class
 * client closure; bindings supply version/context and native publication. */
#include "wow_class_client_bindings.h"
#include "../inc/ntcftxt.h"
#line 2181
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

#line 2383
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



#line 3494
#ifdef FE_IME
#line 3495
#error Unsupported FE_IME original branch
#line 3502
#endif
#line 3503

#line 642
#ifdef DEBUG
#line 643
#error Unsupported DEBUG original branch
#endif
#line 645

#line 2288
#ifdef DEBUG
#line 2289
#error Unsupported DEBUG original branch
#else
#line 2291
#line 2292
#endif
#line 2293
