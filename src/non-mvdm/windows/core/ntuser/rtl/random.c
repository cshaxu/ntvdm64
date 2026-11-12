/****************************** Module Header ******************************\
* Module Name: random.c
*
* Copyright (c) 1985-91, Microsoft Corporation
*
* This module contains a random collection of support routines for the User
* API functions.  Many of these functions will be moved to more appropriate
* files once we get our act together.
*
* History:
* 10-17-90 DarrinM      Created.
* 02-06-91 IanJa        HWND revalidation added (none required)
\***************************************************************************//* DIVERGENCE(OPENNT-HOST-105): complete original FindCharPosition,
 * TextCopy and wcsncpycch bodies only. Explicit standalone includes replace
 * the historical enclosing RTL build; no window/server dependencies. */
#include <windows.h>
#define WOW_ORIGINAL_LARGE_STRING_LAYOUT
#include "compat/include/opennt_user_multipass.h"
#undef WOW_ORIGINAL_LARGE_STRING_LAYOUT
/***************************************************************************\
* FindCharPosition
*
* Finds position of character ch in lpString.  If not found, the length
* of the string is returned.
*
* History:
*   11-13-90 JimA                Created.
\***************************************************************************/
#line 79

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
/***************************************************************************\
* TextCopy
*
* Returns: number of characters copied not including the NULL
*
* History:
* 10-25-90 MikeHar      Wrote.
* 11-09-90 DarrinM      Rewrote with a radically new algorithm.
* 01-25-91 MikeHar      Fixed the radically new algorithm.
* 02-01-91 DarrinM      Bite me.
* 11-26-91 DarrinM      Ok, this time it's perfect (except NLS, probably).
* 01-13-92 GregoryW     Now it's okay for Unicode.
\***************************************************************************/
#line 107

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

/***************************************************************************\
* DWORD wcsncpycch(dest, source, count) - copy no more than n wide chars
*
* Purpose:
*       Copies no more than count characters from the source string to the
*       destination.  If count is less than the length of source,
*       NO NULL CHARACTER is put onto the end of the copied string.
*       If count is greater than the length of sources, dest is NOT padded
*       with more than 1 null character.
*
*
* Entry:
*       LPWSTR dest - pointer to destination
*       LPWSTR source - source string for copy
*       DWORD count - max number of characters to copy
*
* Exit:
*       returns number of characters copied into dest, including the null
*   terminator, if any.
*
* Exceptions:
*
****************************************************************************/
#line 146
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
