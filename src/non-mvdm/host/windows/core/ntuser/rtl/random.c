/****************************** Module Header ******************************\
* Module Name: random.c
*
* Copyright (c) 1985-91, Microsoft Corporation
*
* This module contains a random collection of support routines for the User
* API functions.
\***************************************************************************/
/* DIVERGENCE(OPENNT-HOST-105): complete original FindCharPosition,
 * TextCopy and wcsncpycch bodies only. Explicit standalone includes replace
 * the historical enclosing RTL build; no window/server dependencies. */
#include <windows.h>
#define WOW_ORIGINAL_LARGE_STRING_LAYOUT
#include "../inc/user.h"
#undef WOW_ORIGINAL_LARGE_STRING_LAYOUT

DWORD FindCharPosition(
    LPWSTR lpString,
    WCHAR ch)
{
    DWORD dwPos = 0L;

    while (*lpString && *lpString != ch) {
        ++lpString;
        ++dwPos;
    }
    return dwPos;
}

UINT TextCopy(
    PLARGE_UNICODE_STRING pstr,
    LPWSTR pszDst,
    UINT cchMax)
{
    if (cchMax != 0) {
        cchMax = min(pstr->Length / sizeof(WCHAR), cchMax - 1);
        RtlCopyMemory(pszDst, pstr->Buffer, cchMax * sizeof(WCHAR));
        pszDst[cchMax] = 0;
    }

    return cchMax;
}

DWORD wcsncpycch (
        LPWSTR dest,
        LPCWSTR source,
        DWORD count
        )
{
        LPWSTR start = dest;

        while (count && (*dest++ = *source++))    /* copy string */
                count--;

        return dest - start;
}
