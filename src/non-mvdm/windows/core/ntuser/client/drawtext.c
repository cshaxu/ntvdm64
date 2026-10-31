/* Copyright (c) 1985-91, Microsoft Corporation.
 * Original windows/core/ntuser drawtext.c function subset.
 * DIVERGENCE(OPENNT-HOST-097): unrelated text engines excluded;
 * selected function body unchanged. See the S3 source ledger.
 */
#include "precomp.h"

LONG TabTextOut(
    HDC hdc,
    int x,
    int y,
    LPCWSTR lpstring,
    int nCount,
    int nTabPositions,
    LPINT lpTabPositions,
    int iTabOrigin,
    BOOL fDrawTheText)
{
    SIZE textextent, viewextent, windowextent;
    int     initialx = x;
    int     cch;
    LPCWSTR  lp;
    int     cxCharWidth;
    int     cyCharHeight = 0;
    int     iOneTab = 0;
    RECT rc;
    UINT uOpaque = (GetBkMode(hdc) == OPAQUE) ? ETO_OPAQUE : 0;
    BOOL    fStrStart = TRUE;
    int     ySign = 1; //Assume y increases in down direction.

    if (!lpstring || !nCount || nTabPositions < 0)
        return 0;


    // Check if it is SysFont AND the mapping mode is MM_TEXT;
    // Fix made in connection with Bug #8717 --02-01-90  --SANKAR--
    if (IsSysFontAndDefaultMode(hdc))
    {
        cxCharWidth  = gpsi->cxSysFontChar;
        cyCharHeight = gpsi->cySysFontChar;
    } else {
        cxCharWidth  = GdiGetCharDimensions(hdc, NULL, &cyCharHeight);
        if (cxCharWidth == 0) {
            RIPMSG0(RIP_WARNING, "TabTextOut: GdiGetCharDimensions failed");
            return 0;
        }
    }

    /*
     * If no tabstop positions are specified, then use a default of 8 system
     * font ave char widths or use the single fixed tab stop.
     */
    if (!lpTabPositions) {
       // no tab stops specified -- default to a tab stop every 8 characters
        iOneTab = 8 * cxCharWidth;
    } else if (nTabPositions == 1) {
        // one tab stop specified -- treat value as the tab increment, one
        // tab stop every increment
            iOneTab = lpTabPositions[0];

        if (!iOneTab)
             iOneTab = 1;
    }

    // Calculate if the y increases or decreases in the down direction using
    // the ViewPortExtent and WindowExtents.
    // If this call fails, hdc must be invalid
    if (!GetViewportExtEx(hdc, &viewextent))
        return 0;
    GetWindowExtEx(hdc, &windowextent);
    if ((viewextent.cy ^ windowextent.cy) & 0x80000000)
         ySign = -1;

    rc.left = initialx;
    rc.top = y;
    rc.bottom = rc.top + (ySign * cyCharHeight);

    while (TRUE) {
        // count the number of characters until the next tab character
        // this set of characters (substring) will be the working set for
        // each iteration of this loop
        for (cch = nCount, lp = lpstring; cch && (*lp != TEXT('\t')); lp++, cch--)
        {
        }

        // Compute the number of characters to be drawn with textout.
        cch = nCount - cch;

        // Compute the number of characters remaining.
        nCount -= cch + 1;

        // get height and width of substring
        if (cch == 0) {
            textextent.cx = 0;
            textextent.cy = cyCharHeight;
        } else
            GetTextExtentPointW(hdc, lpstring, cch, &textextent);

        if (fStrStart)
            // first iteration should just spit out the first substring
            // no tabbing occurs until the first tab character is encountered
            fStrStart = FALSE;
        else
        {
           // not the first iteration -- tab accordingly

            int xTab;
            int i;

            if (!iOneTab)
            {
                // look thru tab stop array for next tab stop after existing
                // text to put this substring
                for (i = 0; i < nTabPositions; i++)
                {
                    xTab = lpTabPositions[i];

                    if (xTab < 0)
                        // calc length needed to use this right justified tab
                        xTab = (iTabOrigin - xTab) - textextent.cx;
                    else
                        // calc length needed to use this left  justified tab
                        xTab = iTabOrigin + xTab;

                    if (x < xTab)
                    {
                        // we found a tab with enough room -- let's use it
                        x = xTab;
                        break;
                    }
                }

                if (i == nTabPositions)
                    // we've exhausted all of the given tab positions
                    // go back to default of a tab stop every 8 characters
                    iOneTab = 8 * cxCharWidth;
            }

            // we have to recheck iOneTab here (instead of just saying "else")
            // because iOneTab will be set if we've run out of tab stops
            if (iOneTab)
            {
                if (iOneTab < 0)
                {
                    // calc next available right justified tab stop
                    xTab = x + textextent.cx - iTabOrigin;
                    xTab = ((xTab / iOneTab) * iOneTab) - iOneTab - textextent.cx + iTabOrigin;
                }
                else
                {
                    // calc next available left justified tab stop
                    xTab = x - iTabOrigin;
                    xTab = ((xTab / iOneTab) * iOneTab) + iOneTab + iTabOrigin;
                }
                x = xTab;
            }
        }

        if (fDrawTheText) {

            /*
             * Output all text up to the tab (or end of string) and get its
             * extent.
             */
            rc.right = x + textextent.cx;
            ExtTextOutW(
                    hdc, x, y, uOpaque, &rc, (LPWSTR)lpstring,
                    cch, NULL);
            rc.left = rc.right;
        }

        // Skip over the tab and the characters we just drew.
        x += textextent.cx;

        // Skip over the characters we just drew.
        lpstring += cch;

        // See if we have more to draw OR see if this string ends in
        // a tab character that needs to be drawn.
        if((nCount > 0) || ((nCount == 0) && (*lpstring == TEXT('\t'))))
        {

            lpstring++;  // Skip over the tab
            continue;
        }
        else
            break;        // Break from the loop.
    }
    return MAKELONG((x - initialx), (short)textextent.cy);
}
