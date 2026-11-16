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

#include "precomp.h"
#pragma hdrstop


#line 23

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

