/****************************** Module Header ******************************\
* Module Name: drawtext.c
*
* Copyright (c) 1985-91, Microsoft Corporation
*
* This module contains common text drawing functions.
*
* History:
* 02-12-92 mikeke   Moved Drawtext to the client side
\***************************************************************************/
#include "precomp.h"

#line 22
#ifdef FE_SB
#line 23
#error Excluded unsupported original FE_SB branch
#line 25
#endif
#line 26

#line 59
#ifdef FE_SB
#line 60
#error Excluded unsupported original FE_SB branch
#line 640
#endif
#line 641

#line 677
#ifdef FE_SB // GetNextWordbreak()
#line 678
#error Excluded unsupported original FE_SB branch
#line 711
#endif // FE_SB
#line 712

#line 719
#ifdef FE_SB // GetNextWordbreak()
#line 720
#error Excluded unsupported original FE_SB branch
#line 720
#else
#line 721
#line 722
#endif // FE_SB
#line 723

/***************************************************************************\
* GetPrefixCount
*
* This routine returns the count of accelerator mnemonics and the
* character location (starting at 0) of the character to underline.
* A single CH_PREFIX character will be striped and the following character
* underlined, all double CH_PREFIX character sequences will be replaced by
* a single CH_PREFIX (this is done by PSMTextOut). This routine is used
* to determine the actual character length of the string that will be
* printed, and the location the underline should be placed. Only
* cch characters from the input string will be processed. If the lpstrCopy
* parameter is non-NULL, this routine will make a printable copy of the
* string with all single prefix characters removed and all double prefix
* characters collapsed to a single character. If copying, a maximum
* character count must be specified which will limit the number of
* characters copied.
*
* The location of the single CH_PREFIX is returned in the low order
* word, and the count of CH_PREFIX characters that will be striped
* from the string during printing is in the hi order word. If the
* high order word is 0, the low order word is meaningless. If there
* were no single prefix characters (i.e. nothing to underline), the
* low order word will be -1 (to distinguish from location 0).
*
* These routines assume that there is only one single CH_PREFIX character
* in the string.
*
* WARNING! this rountine returns information in BYTE count not CHAR count
* (so it can easily be passed onto GreExtTextOutW which takes byte
* counts as well)
*
* History:
* 11-13-90 JimA         Ported to NT
* 30-Nov-1992 mikeke    Client side version
\***************************************************************************/
#line 761
LONG GetPrefixCount(
    LPCWSTR lpstr,
    int cch,
    LPWSTR lpstrCopy,
    int charcopycount)
{
    int chprintpos = 0;         /* Num of chars that will be printed */
    int chcount = 0;            /* Num of prefix chars that will be removed */
    int chprefixloc = -1;       /* Pos (in printed chars) of the prefix */
    WCHAR ch;

    /*
     * If not copying, use a large bogus count...
     */
    if (lpstrCopy == NULL)
        charcopycount = 32767;

    while ((cch-- > 0) && *lpstr && charcopycount-- != 0) {

        /*
         * Is this guy a prefix character ?
         */
        if ((ch = *lpstr++) == CH_PREFIX) {

            /*
             * Yup - increment the count of characters removed during print.
             */
            chcount++;

            /*
             * Is the next also a prefix char?
             */
            if (*lpstr != CH_PREFIX) {

                /*
                 * Nope - this is a real one, mark its location.
                 */
                chprefixloc = chprintpos;

            } else {

                /*
                 * yup - simply copy it if copying.
                 */
                if (lpstrCopy != NULL)
                    *(lpstrCopy++) = CH_PREFIX;
                cch--;
                lpstr++;
                chprintpos++;
            }
#line 811
#ifdef FE_SB // GetPrefixCount()
#line 812
#error DIVERGENCE OPENNT-HOST-132: FE_SB branch is not selected; see mirror register.
#line 841
#endif // FE_SB
#line 842
        } else {

            /*
             * Nope - just inc count of char.  that will be printed
             */
            chprintpos++;
            if (lpstrCopy != NULL)
                *(lpstrCopy++) = ch;
        }
    }

    if (lpstrCopy != NULL)
        *lpstrCopy = 0;

    /*
     * Return the character counts
     */
    return MAKELONG(chprefixloc, chcount);
}

#line 875
#ifdef FE_SB // DT_GetExtentMinusPrefixes()
#line 876
#error Excluded unsupported original FE_SB branch
#line 881
#else
#line 882
#line 885
#endif // FE_SB
#line 886

#line 1213
#ifdef FE_SB // DT_GetLineBreak()
#line 1214
#error Excluded unsupported original FE_SB branch
#endif // FE_SB
#line 1216

#line 1224
#ifdef FE_SB // DT_GetLineBreak()
#line 1225
#error Excluded unsupported original FE_SB branch
#else
#line 1227
#line 1228
#endif // FE_SB
#line 1229

#line 1598
#ifdef DEBUG
#line 1599
#error Unsupported DEBUG original branch
#line 1601
#endif
#line 1602

#line 1622
#ifdef LATER
#line 1623
#error Excluded unsupported original LATER branch
#line 1634
#endif
#line 1635

#line 1657
#ifdef _USERK_
#line 1658
#error Unsupported _USERK_ original branch
#else
#line 1660
#line 1661
#endif
#line 1662

#line 1673
#ifdef LATER
#line 1674
#error Excluded unsupported original LATER branch
#line 1685
#endif
#line 1686

/***************************************************************************\
* PSMTextOut
*
* Outputs the text and puts and _ below the character with an &
* before it. Note that this routine isn't used for menus since menus
* have their own special one so that it is specialized and faster...
*
* History:
* 11-13-90 JimA         Ported to NT.
* 30-Nov-1992 mikeke    Client side version
\***************************************************************************/
#line 1867
void PSMTextOut(
    HDC hdc,
    int xLeft,
    int yTop,
    LPWSTR lpsz,
    int cch)
{
    int cx;
    LONG textsize, result;
    WCHAR achWorkBuffer[255];
    WCHAR *pchOut = achWorkBuffer;
    TEXTMETRICW textMetric;
    SIZE size;
    RECT rc;
    COLORREF color;
    BOOL restoreColor = FALSE;

    /* DIVERGENCE(OPENNT-HOST-115): prefix copy appends a terminator.
     * Per-call scratch and finally cleanup also cover reentry/exceptions. */
    if (!lpsz || cch < 0 || (SIZE_T)cch > ((SIZE_T)-1) / sizeof(WCHAR) - 1) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return;
    }
    if (cch >= sizeof(achWorkBuffer)/sizeof(WCHAR)) {
        pchOut = (WCHAR*)UserRtlAllocMem(((SIZE_T)cch+1) * sizeof(WCHAR));
#line 1885
        if (pchOut == NULL)
            return;
    }

    __try {
#line 1889
    result = GetPrefixCount(lpsz, cch, pchOut, cch);

    UserTextOutW(hdc, xLeft, yTop, pchOut,
                      cch - HIWORD(result));

    /*
     * Any true prefix characters to underline?
     */
    if (LOWORD(result) == 0xFFFF) {
#line 1900
        return;
    }

    if (!UserGetTextMetricsW(hdc, &textMetric)) {
        textMetric.tmOverhang = 0;
        textMetric.tmAscent = 0;
    }

    /*
     * For proportional fonts, find starting point of underline.
     */
    if (LOWORD(result) != 0) {

        /*
         * How far in does underline start (if not at 0th byte.).
         */
        UserGetTextExtentPointW(hdc, pchOut, LOWORD(result), &size);
        xLeft += size.cx;

        /*
         * Adjust starting point of underline if not at first char and there is
         * an overhang.  (Italics or bold fonts.)
         */
        xLeft = xLeft - textMetric.tmOverhang;
    }

    /*
     * Adjust for proportional font when setting the length of the underline and
     * height of text.
     */
    UserGetTextExtentPointW(hdc, pchOut + LOWORD(result), 1, &size);
    textsize = size.cx;

    /*
     * Find the width of the underline character.  Just subtract out the overhang
     * divided by two so that we look better with italic fonts.  This is not
     * going to effect embolded fonts since their overhang is 1.
     */
    cx = LOWORD(textsize) - textMetric.tmOverhang / 2;

    /*
     * Get height of text so that underline is at bottom.
     */
    yTop += textMetric.tmAscent + 1;

    /*
     * Draw the underline using the foreground color.
     */
    SetRect(&rc, xLeft, yTop, xLeft+cx, yTop+1);
    color = UserSetBkColor(hdc, UserGetTextColor(hdc));
    restoreColor = TRUE;
#line 1950
    UserExtTextOutW(hdc, xLeft, yTop, ETO_OPAQUE, &rc, TEXT(""), 0, NULL);
    UserSetBkColor(hdc, color);
    restoreColor = FALSE;

    } __finally {
        DWORD error = GetLastError();
        if (restoreColor) UserSetBkColor(hdc, color);
        if (pchOut != achWorkBuffer)
            UserRtlFreeMem(pchOut);
        SetLastError(error);
    }
#line 1956
}

/***************************************************************************\
*
* IsSysFontAndDefaultMode()
*
* Returns TRUE if font selected into DC is the system font AND the current
* mapping mode of the DC is MM_TEXT (Default mode); else returns FALSE. This
* is called by interrupt time code so it needs to be in the fixed code
* segment.
*
* History:
* 07-Jul-95 BradG   Ported from Win95
\***************************************************************************/
#line 1971
BOOL IsSysFontAndDefaultMode(HDC hdc)
{
    return((UserGetHFONT(hdc) == ghFontSys) && (UserGetMapMode(hdc) == MM_TEXT));
}
