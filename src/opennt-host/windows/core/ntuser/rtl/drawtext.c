/* Copyright (c) 1985-91, Microsoft Corporation.
 * Original windows/core/ntuser drawtext.c function subset.
 * DIVERGENCE(OPENNT-HOST-097): unrelated text engines excluded;
 * selected function body unchanged. See the S3 source ledger.
 */
#include "precomp.h"

BOOL IsSysFontAndDefaultMode(HDC hdc)
{
    return((UserGetHFONT(hdc) == ghFontSys) && (UserGetMapMode(hdc) == MM_TEXT));
}

/* DIVERGENCE(OPENNT-HOST-110): additional unchanged original body;
 * unrelated text engines remain excluded. */
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
#ifdef FE_SB // GetPrefixCount()
        } else if (ch == CH_ENGLISHPREFIX) {

            /*
             * Yup - increment the count of characters removed during print.
             */
            chcount++;

            /*
             * Next character is a real one, mark its location.
             */
            chprefixloc = chprintpos;

        } else if (ch == CH_KANJIPREFIX) {

            /*
             * We only support Alpha Numeric(CH_ENGLISHPREFIX).
             * no support for Kana(CH_KANJIPREFIX).
             */
            /*
             * Yup - increment the count of characters removed during print.
             */
            chcount++;

            if(cch) {
                /* don't copy the character */
                chcount++;
                lpstr++;
                cch--;
            }
#endif // FE_SB
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

#ifdef WOW_ORIGINAL_MNEMONIC_DRAWING
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
        if (pchOut == NULL)
            return;
    }

    __try {
    result = GetPrefixCount(lpsz, cch, pchOut, cch);

    UserTextOutW(hdc, xLeft, yTop, pchOut,
                      cch - HIWORD(result));

    /*
     * Any true prefix characters to underline?
     */
    if (LOWORD(result) == 0xFFFF) {
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
}
#endif
