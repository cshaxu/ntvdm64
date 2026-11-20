#include "non-mvdm/public/sdk/inc/nt.h"
/* Copyright (c) 1985-1995, Microsoft Corporation.
 * Derived from OpenNT windows/core/ntuser/client/client.c, revision
 * 5c5b979ec08c17d3ca2eb70e8aad62d26515d01c.
 * T483 S6 P24 owner-approved extraction: RegisterClassWOWA.
 * Original function bodies and existing host bindings are preserved.
 */
#define WOW_ORIGINAL_CLASS_CLIENT
#include "wow_class_client_bindings.h"
#include "non-mvdm/windows/core/ntuser/inc/ntcftxt.h"


/***************************************************************************\
* RegisterClassWOW(API)
*
* History:
* 28-Jul-1992 ChandanC Created.
\***************************************************************************/
ATOM
WINAPI
RegisterClassWOWA(
    WNDCLASSA *lpWndClass,
    LPDWORD pdwWOWstuff)
{
    WNDCLASSEXA wc;

    memcpy(&(wc.style), lpWndClass, sizeof(WNDCLASSA));
    wc.hIconSm = NULL;
    wc.cbSize = sizeof(WNDCLASSEXA);

    return RegisterClassExWOWA(&wc, pdwWOWstuff, NULL, 0);
}
