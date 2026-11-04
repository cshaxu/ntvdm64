/****************************** Module Header ******************************\
* Module Name: chartran.c
*
* Copyright (c) 1985-91, Microsoft Corporation
*
* This module contains the routines for translating ACP characters
* to Unicode and translating Unicode characters to ACP characters.
* NOTE: The ACP is the currently installed 8-bit code page.
*
*
* History:
* 08-01-91 GregoryW      Created.
* 05-14-92 GregoryW      Modified to use the Rtl translation routines.
\***************************************************************************/

#include "compat/include/wow_char_conversion_abi.h"
/* DIVERGENCE(OPENNT-HOST-119): current layout input, no native CLIENTINFO cast. */
#line 17
#ifdef _USERK_
#line 18
#error DIVERGENCE OPENNT-HOST-145: kernel USER profile is not selected
#else
#line 20
#define THREAD_CODEPAGE() OpenNtUserThreadCodePage()
#endif // _USERK_
#line 22
#define IS_ACP(cp) (((cp) == NlsAnsiCodePage) || ((cp) == CP_ACP))

#line 363
#define NWCH 2
#line 441
/* DIVERGENCE(OPENNT-HOST-119): restore the original message converter. */
BOOL RtlMBMessageWParamCharToWCS(DWORD msg, PDWORD pWParam)
{
    DWORD dwUni;
    NTSTATUS Status;
#line 446
#ifdef FE_SB // RtlMBMessageWParamCharToWCS()
#line 447
#error DIVERGENCE OPENNT-HOST-132: FE_SB branch is not selected; see mirror register.
#endif // FE_SB
#line 450
    WORD CodePage = THREAD_CODEPAGE();

    /*
     * Only these messages have CHARs: others are passed through
     */

    switch(msg) {
#line 457
#ifdef FE_SB // RtlMBMessageWParamCharToWCS()
#line 458
#error DIVERGENCE OPENNT-HOST-132: FE_SB branch is not selected; see mirror register.
#else
#line 497
    case WM_CHAR:
#endif // FE_SB
#line 499
    case WM_CHARTOITEM:
    case EM_SETPASSWORDCHAR:
    case WM_DEADCHAR:
    case WM_SYSCHAR:
    case WM_SYSDEADCHAR:
    case WM_MENUCHAR:

        dwUni = 0;

        if (IS_ACP(CodePage)) {
            Status = RtlMultiByteToUnicodeN((LPWSTR)&dwUni, sizeof(dwUni),
                    NULL, (LPSTR)pWParam, 2 * sizeof(CHAR));
            if (!NT_SUCCESS(Status))
                return FALSE;
        } else {
            int cwch;
#line 515
#ifdef _USERK_
#line 516
#error DIVERGENCE OPENNT-HOST-136: _USERK_ branch is not selected; see mirror register.
#else
#line 520
            cwch = MultiByteToWideChar(CodePage, 0,
                    (LPSTR)pWParam, 2,
                    (LPWSTR)&dwUni, sizeof(dwUni) / sizeof(WCHAR));
#endif // _USERK_
#line 524
            // KdPrint(("0x%02x -> 0x%04x (%d)\n", *pWParam, dwUni, CodePage));
            if (cwch == 0) {
                return FALSE;
            }
        }

#line 530
#ifdef FE_SB // RtlMBMessageWParamCharToWCS
#line 531
#error DIVERGENCE OPENNT-HOST-132: FE_SB branch is not selected; see mirror register.
#else
#line 537
#ifdef DEBUG
#line 538
#error Excluded unsupported original DEBUG branch
#endif
#line 542
#endif // FE_SB
#line 543
        *pWParam = dwUni;
        break;
    }

    return TRUE;
}

/* DIVERGENCE(OPENNT-HOST-118): recover unchanged companion ANSI constructor. */
#line 559
VOID RtlInitLargeAnsiString(
    PLARGE_ANSI_STRING plstr,
    LPCSTR psz,
    UINT cchLimit)
{
    ULONG Length;

    plstr->Buffer = (PSTR)psz;
    plstr->bAnsi = TRUE;
    if (ARGUMENT_PRESENT( psz )) {
        Length = strlen( psz );
        plstr->Length = min(Length, cchLimit);
        plstr->MaximumLength = min((Length + 1), cchLimit);
    } else {
        plstr->MaximumLength = 0;
        plstr->Length = 0;
    }
}

/* DIVERGENCE(OPENNT-HOST-105): restore this complete original leaf from
 * the same source unit; the previously selected conversion subset omitted it. */
VOID RtlInitLargeUnicodeString(
    PLARGE_UNICODE_STRING plstr,
    LPCWSTR psz,
    UINT cchLimit)
{
    ULONG Length;

    plstr->Buffer = (PWSTR)psz;
    plstr->bAnsi = FALSE;
    if (ARGUMENT_PRESENT( psz )) {
        Length = wcslen( psz ) * sizeof( WCHAR );
        plstr->Length = min(Length, cchLimit);
        plstr->MaximumLength = min((Length + sizeof(UNICODE_NULL)), cchLimit);
    } else {
        plstr->MaximumLength = 0;
        plstr->Length = 0;
    }
}

/***************************************************************************\
* WCSToMBEx (API)
*
* Convert a wide-character (Unicode) string to MBCS (ANSI) string.
*
* nAnsiChar > 0 indicates the number of bytes to allocate to store the
*    ANSI string (if bAllocateMem == TRUE) or the size of the buffer
*    pointed to by *pAnsiString (bAllocateMem == FALSE).
*
* nAnsiChar == -1 indicates that the necessary number of bytes be allocated
*    to hold the translated string.  bAllocateMem must be set to TRUE in
*    this case.
*
* Return value
*   Success: number of characters in the output string
*        If bAllocateMem was TRUE, then FreeAnsiString() may be
*        used to free the allocated memory at *ppAnsiString.
*   Failure: 0 means failure
*        (Any buffers allocated by this routine are freed)
*
* History:
*  1992-??-?? GregoryW   Created
*  1993-01-07 IanJa      fix memory leak on error case.
\***************************************************************************/

int
WCSToMBEx(
    WORD wCodePage,
    LPCWSTR pUnicodeString,
    int cchUnicodeString,
    LPSTR *ppAnsiString,
    int nAnsiChar,
    BOOL bAllocateMem)
{
    ULONG nCharsInAnsiString;
#line 59
#ifdef _USERK_
#line 60
#error DIVERGENCE OPENNT-HOST-136: _USERK_ branch is not selected; see mirror register.
#endif // _USERK_
#line 62

    if (nAnsiChar == 0 || cchUnicodeString == 0) {
        return 0;      // nothing to translate or nowhere to put it
    }

    /*
     * Adjust the cchUnicodeString value.  If cchUnicodeString == -1 then the
     * string pointed to by pUnicodeString is NUL terminated so we
     * count the number of bytes.  If cchUnicodeString < -1 this is an
     * illegal value so we return FALSE.  Otherwise, cchUnicodeString is
     * set and requires no adjustment.
     */
    if (cchUnicodeString == -1) {
        cchUnicodeString = (wcslen(pUnicodeString) + 1);
    } else if (cchUnicodeString < -1) {
        return 0;     // illegal value
    }

    /*
     * Adjust the nAnsiChar value.  If nAnsiChar == -1 then we pick a
     * value based on cchUnicodeString to hold the converted string.  If
     * nAnsiChar < -1 this is an illegal value so we return FALSE.
     * Otherwise, nAnsiChar is set and requires no adjustment.
     */
    if (nAnsiChar == -1) {
        if (bAllocateMem == FALSE) {
            return 0;  // no destination
        }
        nAnsiChar = cchUnicodeString * sizeof(USHORT);
    } else if (nAnsiChar < -1) {
        return 0;     // illegal value
    }

    if (bAllocateMem) {
        /*
         * We need to allocate memory to hold the translated string.
         */
        *ppAnsiString = (LPSTR)UserRtlAllocMem(nAnsiChar);
        if (*ppAnsiString == NULL) {
            return 0;
        }
    }

    /*
     * translate Unicode string pointed to by pUnicodeString into
     * ANSI and store in location pointed to by pAnsiString.  We
     * stop translating when we fill up the ANSI buffer or reach
     * the end of the Unicode string.
     */

    /*
     * if the target multibyte codepage is eqaul to ACP, Call faster Rtl function.
     */
    if (IS_ACP(wCodePage)) {

        NTSTATUS Status;

        Status = RtlUnicodeToMultiByteN(
                        (PCH)*ppAnsiString,
                        nAnsiChar,
                        &nCharsInAnsiString,
                        (PWCH)pUnicodeString,
                        cchUnicodeString * sizeof(WCHAR));
        /*
         * If the ansi buffer is too small, RtlUnicodeToMultiByteN()
         * returns STATUS_BUFFER_OVERFLOW. In this case, the function
         * put as many ansi characters as specified in the buffer and
         *  returns the number by chacacters(in bytes) written. We would
         * like to return the actual byte  count written in the ansi
         * buffer rather than returnning 0 since callers of this function
         * don't expect to be returned 0 in most case.
         */

        if (!NT_SUCCESS(Status) && Status != STATUS_BUFFER_OVERFLOW) {
            if (bAllocateMem) {
                UserRtlFreeMem(*ppAnsiString);
            }
            return 0;   // translation failed
        }

        return (int)nCharsInAnsiString;

    } else {

#line 146
#ifdef _USERK_
#line 147
#error DIVERGENCE OPENNT-HOST-136: _USERK_ branch is not selected; see mirror register.
#else
#line 166
        /*
         * Call NLS API (Kernel32) to convert string to Unicode. (User mode)
         */
        nCharsInAnsiString = WideCharToMultiByte(
                                 (UINT)wCodePage, 0,
                                 (LPCWSTR)pUnicodeString,
                                 cchUnicodeString,
                                 (LPSTR)*ppAnsiString,
                                 nAnsiChar,
                                 NULL, NULL);
#endif // _USERK_
#line 177

        if (nCharsInAnsiString == 0) {
            if (bAllocateMem) {
                UserRtlFreeMem(*ppAnsiString);
            }
        }

        return (int)nCharsInAnsiString;
    }
}

// Returns number of character converted

int MBToWCSEx(
    WORD wCodePage,
    LPCSTR pAnsiString,
    int nAnsiChar,
    LPWSTR *ppUnicodeString,
    int cchUnicodeString,
    BOOL bAllocateMem)
{
    ULONG nBytesInUnicodeString;

    if (nAnsiChar == 0 || cchUnicodeString == 0) {
        return 0;      // nothing to translate or nowhere to put it
    }

    /*
     * Adjust the nAnsiChar value.  If nAnsiChar == -1 then the
     * string pointed to by pAnsiString is NUL terminated so we
     * count the number of bytes.  If nAnsiChar < -1 this is an
     * illegal value so we return FALSE.  Otherwise, nAnsiChar is
     * set and requires no adjustment.
     */
    if (nAnsiChar == -1) {
        nAnsiChar = strlen(pAnsiString) + 1;   // don't forget the NUL
    } else if (nAnsiChar < -1) {
        return 0;     // illegal value
    }

    /*
     * Adjust the cchUnicodeString value.  If cchUnicodeString == -1 then we
     * pick a value based on nAnsiChar to hold the converted string.  If
     * cchUnicodeString < -1 this is an illegal value so we return FALSE.
     * Otherwise, cchUnicodeString is set and requires no adjustment.
     */
    if (cchUnicodeString == -1) {
        if (bAllocateMem == FALSE) {
            return 0;    // no destination
        }
        cchUnicodeString = nAnsiChar;
    } else if (cchUnicodeString < -1) {
        return 0;     // illegal value
    }

    if (bAllocateMem) {
        *ppUnicodeString = (LPWSTR)UserRtlAllocMem(cchUnicodeString*sizeof(WCHAR));
        if (*ppUnicodeString == NULL) {
            return 0;    // allocation failed
        }
    }

    /*
     * if codepage is CP_ACP, We will call faster RtlXXX function.
     */
    if (IS_ACP(wCodePage)) {
        /*
         * translate ANSI string pointed to by pAnsiString into Unicode
         * and store in location pointed to by pUnicodeString.  We
         * stop translating when we fill up the Unicode buffer or reach
         * the end of the ANSI string.
         */
        if (!NT_SUCCESS(RtlMultiByteToUnicodeN(
                            (PWCH)*ppUnicodeString,
                            cchUnicodeString * sizeof(WCHAR),
                            &nBytesInUnicodeString,
                            (PCH)pAnsiString,
                            nAnsiChar
                            ))) {
            if (bAllocateMem) {
                UserRtlFreeMem(*ppUnicodeString);
            }
            return 0;   // translation failed
        }

        return (int)(nBytesInUnicodeString / sizeof(WCHAR));

    } else {
        /*
         * if wCodePage is not ACP, Call NLS API.
         */
        ULONG nCharsInUnicodeString;

#line 270
#ifdef _USERK_
#line 271
#error DIVERGENCE OPENNT-HOST-136: _USERK_ branch is not selected; see mirror register.
#else
#line 291
        /*
         * Call NLS API (Kernel32) to convert string to Unicode. (User mode)
         */
        nCharsInUnicodeString = MultiByteToWideChar(
                                    (UINT)wCodePage, 0,
                                    (LPCSTR)pAnsiString,
                                    (int)nAnsiChar,
                                    (LPWSTR)*ppUnicodeString,
                                    (int)cchUnicodeString);
#endif // _USERK_
#line 301

        if (nCharsInUnicodeString == 0) {
            if (bAllocateMem) {
                UserRtlFreeMem(*ppUnicodeString);
            }
        }

        return (int)nCharsInUnicodeString;
    }

}




#line 326
#ifdef FE_SB
#line 327
#error Excluded unsupported original FE_SB branch
#endif
#line 329


#line 351
#ifdef FE_SB // RtlWCSMessageWParamCharToMB()
#line 352
#error Excluded unsupported original FE_SB branch
#else
#line 363
#endif // FE_SB
#line 365

#line 378
#ifdef _USERK_
#line 379
#error Excluded unsupported original _USERK_ branch
#else
#line 383
#endif // _USERK_
#line 387

#line 393
#ifdef FE_SB // RtlWCSMessageWParamCharToMB()
#line 394
#error Excluded unsupported original FE_SB branch
#else
#line 418
#ifdef DEBUG
#line 419
#error Excluded unsupported original DEBUG branch
#endif
#line 423
#endif // FE_SB
#line 425


#line 337
#ifdef FE_IME // RtlWCSMessageWParamCharToMB()
#line 338
#error Unsupported FE_IME original branch
#endif // FE_IME
#line 341
