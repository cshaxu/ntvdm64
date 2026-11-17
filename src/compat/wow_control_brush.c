/* Copyright (c) 1985-95, Microsoft Corporation.
 * OpenNT windows/core/ntuser/kernel/random.c:208-232.
 * Revision 5c5b979ec08c17d3ca2eb70e8aad62d26515d01c.
 */
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
