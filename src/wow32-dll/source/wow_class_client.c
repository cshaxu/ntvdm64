#include "non-mvdm/public/sdk/inc/nt.h"
/* Copyright (c) 1985-1995, Microsoft Corporation.
 * Derived from OpenNT windows/core/ntuser/client/client.c, revision
 * 5c5b979ec08c17d3ca2eb70e8aad62d26515d01c.
 * T483 S6 P24 owner-approved extraction: RegisterClassWOWA.
 * Original function bodies and existing host bindings are preserved.
 */
#define WOW_ORIGINAL_CLASS_CLIENT
#include "wow32-dll/include/wow_original_class_client_bindings.h"
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

/* Scoped class-name adapter: original capture owns conversion and allocation. */
BOOL WINAPI wow_original_class_capture(LPCSTR source, PUNICODE_STRING output)
{
    IN_STRING captured = {0};
    if (!RtlCaptureAnsiString(&captured, source, TRUE)) return FALSE;
    *output = *captured.pstr;
    return TRUE;
}
