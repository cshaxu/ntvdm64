/****************************** Module Header ******************************\
* Module Name: random.c
*
* Copyright (c) 1985-95, Microsoft Corporation
*
* This module contains a random collection of support routines for the User
* API functions.  Many of these functions will be moved to more appropriate
* files once we get our act together.
*
* History:
* 10-17-90 DarrinM      Created.
* 02-06-91 IanJa        HWND revalidation added (none required)
\***************************************************************************/

/* DIVERGENCE(OPENNT-HOST-102): retain only xxxGetControlBrush from the
 * original random.c. The facade supplies lifetime and native color service;
 * no original recipient-selection algorithm is changed. */
#define WOW_ORIGINAL_CONTROL_BRUSH
#include "wow_control_brush_bindings.h"

/***************************************************************************\
* xxxGetControlBrush
*
* <brief description>
*
* History:
* 12-10-90 IanJa   type replaced with new 32-bit message
* 01-21-91 IanJa   Prefix '_' denoting exported function (although not API)
\***************************************************************************/

#line 208
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


#line 185
#ifdef DEBUG
#line 186
#error Unsupported DEBUG original branch
#endif
#line 190
