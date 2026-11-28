/****************************** Module Header ******************************\
* Module Name: text.c
*
* Copyright (c) 1985 - 1999, Microsoft Corporation
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

#ifdef _USERK_
#error Excluded unsupported original kernel USER text measurement
#else
#line 73

BOOL PSMGetTextExtent(
    HDC hdc,
    LPCWSTR lpstr,
    int cch,
    PSIZE psize)
{
    int result;
    WCHAR szTemp[255], *pchOut;

    if (cch > sizeof(szTemp)/sizeof(WCHAR)) {
        pchOut = (WCHAR*)UserLocalAlloc(0, (cch+1) * sizeof(WCHAR));
        if (pchOut == NULL) {
            psize->cx = psize->cy = 0;
            return FALSE;
        }
    } else {
        pchOut = szTemp;
    }

    result = HIWORD(GetPrefixCount(lpstr, cch, pchOut, cch));

    if (result) {
        lpstr = pchOut;
        cch -= result;
    }

    UserGetTextExtentPointW(hdc, lpstr, cch, psize);

    if (pchOut != szTemp)
        UserLocalFree(pchOut);

    /*
     * IanJa everyone seems to ignore the ret val
     */
    return TRUE;
}

#endif // _USERK_
