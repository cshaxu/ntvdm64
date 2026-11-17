/* Copyright (c) 1985-92, Microsoft Corporation.
 * Derived from OpenNT windows/core/ntuser/client/wow.c:251-265, revision
 * 5c5b979ec08c17d3ca2eb70e8aad62d26515d01c.
 * Owner-approved T483 S6 P21 extraction; original child-list lookup.
 */
#include "control-client/precomp.h"

PWND _GetDlgItem(
    PWND pwnd,
    int id)
{
    if (pwnd != NULL) {
        pwnd = REBASEPWND(pwnd, spwndChild);
        while (pwnd != NULL) {
            if ((int)pwnd->spmenu == id)
                break;
            pwnd = REBASEPWND(pwnd, spwndNext);
        }
    }

    return pwnd;
}
