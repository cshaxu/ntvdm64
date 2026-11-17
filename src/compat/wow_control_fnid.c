/* Copyright (c) 1985-95, Microsoft Corporation.
 * Derived from OpenNT windows/core/ntuser/kernel/ntstubs.c,
 * NtUserSetWindowFNID, lines 6170-6188, revision
 * 5c5b979ec08c17d3ca2eb70e8aad62d26515d01c.
 * Owner-approved T483 S6 P19 extraction; uses the existing local HWND binding.
 */
#include "wow_edit_state_bindings.h"

BOOL WINAPI wow_edit_set_window_fnid(HWND hwnd, WORD fnid)
{
    BOOL retval = FALSE;
    PWND pwnd = wow_window_client_validate_hwnd(hwnd);
    if (!pwnd) return FALSE;

    /* Remember the control kind even when CallWindowProc bypasses its class. */
    if ((GETFNID(pwnd) == 0) || (fnid == FNID_CLEANEDUP_BIT)) {
        pwnd->fnid |= fnid;
        retval = TRUE;
    }
    return retval;
}
