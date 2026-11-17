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
#line 15
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

BOOL PSMGetTextExtent(
    HDC hdc,
    LPWSTR lpstr,
    int cch,
    PSIZE psize)
{
    int result;
    WCHAR szTemp[255]; /* Strings w/prefix chars must be under 256 chars long.*/
    WCHAR *pchOut = szTemp;

    if (cch >= (int)(sizeof(szTemp)/sizeof(WCHAR))) {
        pchOut = (WCHAR *)UserRtlAllocMem(((SIZE_T)cch + 1) * sizeof(WCHAR));
        if (!pchOut) return FALSE;
    }
#line 36

    result = HIWORD(GetPrefixCount(lpstr, cch, pchOut, cch));

    if (!result)
        UserGetTextExtentPointW(hdc, lpstr, cch, psize);
    else
        UserGetTextExtentPointW(hdc, pchOut, cch - result, psize);

    if (pchOut != szTemp) UserRtlFreeMem(pchOut);
#line 43


    /*
     * IanJa everyone seems to ignore the ret val
     */
    return TRUE;
}
