/* Copyright (c) Microsoft Corporation.
 * Original windows/core/ntuser/client/wow.c true subset.
 * DIVERGENCE(OPENNT-HOST-108): retain only the unchanged _GetDlgItem
 * direct-child lookup. Other WOW client services have separate owners.
 * Standalone precomp supplies the retained, same-address WND graph. */
#include "precomp.h"

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
