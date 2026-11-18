/* Copyright (c) 1985-95, Microsoft Corporation.
 * OpenNT windows/core/ntuser/kernel/miscutil.c:24-37.
 * Revision 5c5b979ec08c17d3ca2eb70e8aad62d26515d01c.
 */
#include "precomp.h"

VOID SetDialogPointer(PWND pwnd, LONG lPtr) {

    ((PDIALOG)pwnd)->pdlg = (PDLG)lPtr;

    if (lPtr == 0) {
        pwnd->fnid |= FNID_CLEANEDUP_BIT;
        ClrWF(pwnd, WFDIALOGWINDOW);
    } else {
        if (pwnd->fnid == 0) {
            pwnd->fnid = FNID_DIALOG;
        }
        SetWF(pwnd, WFDIALOGWINDOW);
    }
}
