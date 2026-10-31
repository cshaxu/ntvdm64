/* Copyright (c) 1985-1995, Microsoft Corporation.
 * DIVERGENCE(OPENNT-HOST-102): retain only xxxGetControlBrush from the
 * original random.c. The facade supplies lifetime and native color service;
 * no original recipient-selection algorithm is changed. */
#define WOW_ORIGINAL_CONTROL_BRUSH
#include "wow_control_brush_bindings.h"

HBRUSH xxxGetControlBrush(
    PWND pwnd,
    HDC hdc,
    UINT message)
{
    DWORD dw;
    PWND pwndSend;
    TL tlpwndSend;

    CheckLock(pwnd);

    if ((pwndSend = (TestwndPopup(pwnd) ? pwnd->spwndOwner : pwnd->spwndParent))
         == NULL)
        pwndSend = pwnd;

    ThreadLock(pwndSend, &tlpwndSend);

    /*
     * Last parameter changes the message into a ctlcolor id.
     */
    dw = (DWORD)xxxGetControlColor(pwndSend, pwnd, hdc, message);
    ThreadUnlock(&tlpwndSend);

    return (HBRUSH)dw;
}
