/* Copyright (c) Microsoft Corporation.
 * Original windows/core/ntuser/kernel/ntstubs.c true subset.
 * DIVERGENCE(OPENNT-HOST-076): retain NtUserSetWindowFNID unchanged;
 * scoped original EDITWND storage replaces the unavailable receive shell. */
#define WOW_ORIGINAL_EDIT_STATE
#include "wow_edit_state_bindings.h"

BOOL NtUserSetWindowFNID(
    IN HWND hwnd,
    IN WORD fnid)
{
    BEGINRECV_HWND(BOOL, FALSE, hwnd);

    /*
     * Remember what window class this window belongs to.  Can't use
     * the real class because any app can call CallWindowProc()
     * directly no matter what the class is!
     */
    if ((GETFNID(pwnd) == 0) || (fnid == FNID_CLEANEDUP_BIT)) {
        pwnd->fnid |= fnid;
        retval = TRUE;
    }

    TRACE("NtUserSetWindowFNID");
    ENDRECV_HWND();
}
