/****************************** Module Header ******************************\
* Module Name: text.c
*
* Copyright (c) 1985-95, Microsoft Corporation
*
* This module contains the MessageBox API and related functions.
*
* History:
* 10-01-90 EricK        Created.
* 11-20-90 DarrinM      Merged in User text APIs.
* 02-07-91 DarrinM      Removed TextOut, ExtTextOut, and GetTextExtentPoint stubs.
\***************************************************************************/
#include "precomp.h"
/***************************************************************************\
* PSMGetTextExtent
*
* NOTE: This routine should only be called with the system font since having
* to realize a new font would cause memory to move...
*
* LATER: Can't this be eliminated altogether?  Nothing should be moving
*        anymore.
*
* History:
* 11-13-90  JimA        Ported.
\***************************************************************************/
#line 27

BOOL PSMGetTextExtent(
    HDC hdc,
    LPWSTR lpstr,
    int cch,
    PSIZE psize)
{
    int result;
    WCHAR szTemp[255];
    WCHAR *pchOut = szTemp;
#line 36

    if (!psize || !lpstr || cch < 0 ||
            (SIZE_T)cch > ((SIZE_T)-1) / sizeof(WCHAR) - 1) {
        if (psize) psize->cx = psize->cy = 0;
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    if ((SIZE_T)cch >= sizeof(szTemp) / sizeof(WCHAR)) {
        pchOut = (WCHAR *)UserRtlAllocMem(((SIZE_T)cch + 1) * sizeof(WCHAR));
        if (!pchOut) {
            psize->cx = psize->cy = 0;
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            return FALSE;
        }
    }
    __try {
        result = HIWORD(GetPrefixCount(lpstr, cch, pchOut, cch));
#line 38

        if (!result)
            UserGetTextExtentPointW(hdc, lpstr, cch, psize);
        else
            UserGetTextExtentPointW(hdc, pchOut, cch - result, psize);
    } __finally {
        DWORD error = GetLastError();
        if (pchOut != szTemp) UserRtlFreeMem(pchOut);
        SetLastError(error);
    }
    /*
     * IanJa everyone seems to ignore the ret val
     */
#line 48
    return TRUE;
}
