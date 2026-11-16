/****************************** Module Header ******************************\
* Module Name: wow.c
*
* Copyright (c) 1985-92, Microsoft Corporation
*
* This module contains shared code between USER32 and USER16
* No New CODE should be added to this file, unless its shared
* with USER16.
*
* History:
* 29-DEC-93 NanduriR      shared user32/user16 code.
\***************************************************************************/

#include "precomp.h"
#ifndef WOW
#pragma hdrstop
#endif

#ifdef WOW
#error
#line 119
#else // WOW

//**************************************************************************
// Stuff used when building for USER32
//
//**************************************************************************

#define ClientSharedInfo()  (&gSharedInfo)

//**************************************************************************
// These definitions get resolved differently for USER32 and USER16
//
//**************************************************************************
#define LPPOINTWOW                LPPOINT
#define NORMALIZEDSHORTVALUE(x)   (x)
#define WOWlstrlenA(x)            lstrlenA(x)
#define WOWRtlCopyMemory(lpDest, lpSrc, cb) RtlCopyMemory(lpDest, lpSrc, cb)

#endif // !WOW

/*
 * Undef ptCursor so we will always go through gSharedInfo->psi
 */
#undef ptCursor

#line 249


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


#line 304
#ifdef WOW
#error
#line 307
#else
#line 309
#endif
#line 313
#ifdef WOW
#error
#line 322
#endif
#line 435
#ifdef WOW
#error
#line 442
#else
#line 460
#endif
#line 519
#ifdef ENABLE_CLIENTSIDE_ACCESSCHECK
#error
#line 526
#endif
#line 719
#ifdef WOW
#error
#line 726
#else
#line 728
#endif
#line 752
#ifdef ENABLE_CLIENTSIDE_ACCESSCHECK
#error
#line 758
#endif
#line 776
#ifdef WOW
#error
#line 779
#else
#line 781
#endif
#line 795
#ifdef WOW
#error
#line 802
#else
#line 804
#endif
#line 835
#ifdef WOW
#error
#else
#line 839
#endif
#line 890
#ifndef WOW
#line 893
#endif
#line 911
#ifdef WOW
#error
#else
#line 1062
#endif
#line 1067
#ifdef WOW
#error
#line 1076
#endif
