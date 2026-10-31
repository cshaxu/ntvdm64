/* Copyright (c) 1985-95, Microsoft Corporation.
 * Original windows/core/ntuser/rtl/text.c PSMGetTextExtent.
 * DIVERGENCE(OPENNT-HOST-112): retain prefix/extent logic; repair temporary
 * capacity using the sibling PSMTextOut allocation pattern and finally free.
 * Full provenance and pending acceptance are in the S3 source ledger. */
#include "precomp.h"

BOOL PSMGetTextExtent(
    HDC hdc,
    LPWSTR lpstr,
    int cch,
    PSIZE psize)
{
    int result;
    WCHAR szTemp[255];
    WCHAR *pchOut = szTemp;

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

        if (!result)
            UserGetTextExtentPointW(hdc, lpstr, cch, psize);
        else
            UserGetTextExtentPointW(hdc, pchOut, cch - result, psize);
    } __finally {
        DWORD error = GetLastError();
        if (pchOut != szTemp) UserRtlFreeMem(pchOut);
        SetLastError(error);
    }
    /* Original callers and return contract do not propagate GDI's BOOL. */
    return TRUE;
}
