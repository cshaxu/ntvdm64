/****************************************************************************\
* Module Name: minmax.c
*
* Copyright (c) 1985-95, Microsoft Corporation
*
* Misc util functions
*
* 10-25-90 MikeHar      Ported from Windows.
* 14-Feb-1991 mikeke    Added Revalidation code (None)
\****************************************************************************/

/* DIVERGENCE(OPENNT-HOST-083): true subset, SetDialogPointer only.
 * Original body unchanged; retained host WND/DIALOG, not modern USER storage.
 * No queue/desktop/shell/server implementation is imported. */
#include "precomp.h"
#pragma hdrstop

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
