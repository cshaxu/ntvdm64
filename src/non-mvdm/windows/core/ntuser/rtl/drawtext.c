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
#line 11

/***************************************************************************\
* Stuff used in DrawText code
\***************************************************************************/

#define CR 13
#define LF 10
#define DT_HFMTMASK 0x03
#define DT_VFMTMASK 0x0C
#define ETO_OPAQUEFGND 0x0A

#ifdef FE_SB // CH_ENGLISHPREFIX and CH_KANJIPREFIX.
#error Excluded unsupported original FE_SB branch
#line 25
#endif // FE_SB
#line 26

typedef void (FAR *LPFNTEXTDRAW)(HDC, int, int, LPWSTR, int);

typedef  struct   {
       RECT     rcFormat;       // Format rectangle.
       int      cxTabLength;    // Tab length in pixels.
       int      iXSign;
       int      iYSign;
       int      cyLineHeight;   // Height of a line based on DT_EXTERNALLEADING
       int      cxMaxWidth;     // Width of the format rectangle.
       int      cxMaxExtent;    // Width of the longest line drawn.
//       int      cxLeftMargin;   // Left margin in pixels (with proper sign)
       int      cxRightMargin;  // Right margin in pixels (with proper sign)
               // pointer to PSTextOut or PSMTextOut based
               // on DT_NOPREFIX flag.
       LPFNTEXTDRAW  lpfnTextDraw;
       int      cxOverhang;     // Character overhang.
     }  DRAWTEXTDATA;
typedef DRAWTEXTDATA FAR *LPDRAWTEXTDATA;

static  WCHAR szEllipsis[] = TEXT("...");

// The number of characters in the ellipsis string.
#define CCHELLIPSIS   (sizeof(szEllipsis)/sizeof(WCHAR) - 1)

/* Max length of a full path is around 260. But, most of the time, it will
 * be less than 128. So, we alloc only this much on stack. If the string is
 * longer, we alloc from local heap (which is slower).
 *
 * BOGUS: For international versions, we need to give some more margin here.
 */
#define MAXBUFFSIZE     128

#ifdef FE_SB // UserIsFELineBreak() and ...
#error Excluded unsupported original FE_SB branch
#line 640
#endif // FE_SB
#line 641

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*  GetNextWordbreak() -                                                    */
/*    From Chicago ctlmgr.c  FritzS                                         */
/*                                                                          */
/*--------------------------------------------------------------------------*/

LPCWSTR GetNextWordbreak(DWORD dwCodePage,
                         LPCWSTR lpch,
                         LPCWSTR lpchEnd,
                         BOOL  fBreakSpace)

{
    int ichNonWhite;

  /* ichNonWhite is used to make sure we always make progress. */
  ichNonWhite = 1;

  // We must terminate this loop before lpch == lpchEnd, otherwise, we
  // may gp fault during *lpch.
  while (lpch < lpchEnd) {
      switch (*lpch) {
          case CR:
          case LF:
              return lpch;

          case '\t':
          case ' ':
              if (fBreakSpace)
                  return (lpch + ichNonWhite);

              /*** FALL THRU ***/

          default:

#ifdef FE_SB // GetNextWordbreak()
#error Excluded unsupported original FE_SB branch
#line 711
#endif // FE_SB
#line 712
              lpch++;
              ichNonWhite = 0;
        }
    }

  return lpch;

#ifdef FE_SB // GetNextWordbreak()
#error Excluded unsupported original FE_SB branch
#line 720
#else
#line 721
  UNREFERENCED_PARAMETER(dwCodePage);
#endif // FE_SB
#line 723
}

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

/***************************************************************************\
*  DT_GetExtentMinusPrefixes
\***************************************************************************/

int DT_GetExtentMinusPrefixes(HDC hdc, LPCWSTR lpchStr, int cchCount, UINT wFormat, int iOverhang)
{
  int  iPrefixCount;
  int  cxPrefixes = 0;
  WCHAR PrefixChar = CH_PREFIX;
  SIZE size;

  if((!(wFormat & DT_NOPREFIX)) &&
      (iPrefixCount = HIWORD(GetPrefixCount(lpchStr, cchCount, NULL, 0)))) {
#ifdef FE_SB // DT_GetExtentMinusPrefixes()
#error Excluded unsupported original FE_SB branch
#line 881
#else
#line 882
      cxPrefixes = UserGetTextExtentPointW(hdc, &PrefixChar, 1, &size);
      cxPrefixes = size.cx - iOverhang;
      cxPrefixes *=  iPrefixCount;
#endif // FE_SB
#line 886
    }
  UserGetTextExtentPointW(hdc, lpchStr, cchCount, &size);
  return (size.cx - cxPrefixes);
}

/***************************************************************************\
*   DT_DrawStr
*      This will draw the given string in the given location without worrying
*  about the left/right justification. Gets the extent and returns it.
*  If fDraw is TRUE and if NOT DT_CALCRECT, this draws the text.
*        NOTE: This returns the extent minus Overhang.
*
*   From Chicago ctlmgr.c  FritzS
\***************************************************************************/
int DT_DrawStr(HDC hdc, int  xLeft, int yTop, LPCWSTR lpchStr,
                                 int cchCount, BOOL fDraw, UINT wFormat,
                                 LPDRAWTEXTDATA  lpDrawInfo)
{
  LPCWSTR        lpch;
  int   iLen;
  int   cxExtent;
  int   xOldLeft = xLeft;  // Save the xLeft given to compute the extent later
  int   xTabLength = lpDrawInfo->cxTabLength;
  int   iTabOrigin = lpDrawInfo->rcFormat.left;

  // Check if the tabs need to be expanded
  if(wFormat & DT_EXPANDTABS) {
      while(cchCount) {
          // Look for a tab
          for(iLen = 0, lpch = lpchStr; iLen < cchCount; iLen++)
                if(*lpch++ == TEXT('\t'))
                  break;

          // Draw text, if any, upto the tab
          if(iLen) {
              // Draw the substring taking care of the prefixes.
              if(fDraw && (!(wFormat & DT_CALCRECT))) { // Only if we need to draw text
                  (*(lpDrawInfo->lpfnTextDraw))(hdc, xLeft, yTop, (LPWSTR)lpchStr, iLen);
              }
              // Get the extent of this sub string and add it to xLeft.
              xLeft += DT_GetExtentMinusPrefixes(hdc, lpchStr, iLen, wFormat, lpDrawInfo->cxOverhang) - lpDrawInfo->cxOverhang;
            }

          //if a TAB was found earlier, calculate the start of next sub-string.
          if(iLen < cchCount) {
              iLen++;  // Skip the tab
              if(xTabLength) // Tab length could be zero
                  xLeft = (((xLeft - iTabOrigin)/xTabLength) + 1)*xTabLength + iTabOrigin;
            }

          // Calculate the details of the string that remains to be drawn.
          cchCount -= iLen;
          lpchStr = lpch;
        }
      cxExtent = xLeft - xOldLeft;
    } else {
      // If required, draw the text (with either PSMTextOut or PSTextOut)
      if(fDraw && (!(wFormat & DT_CALCRECT)))   {
          (*(lpDrawInfo->lpfnTextDraw))(hdc, xLeft, yTop, (LPWSTR)lpchStr, cchCount);
      }
      // Compute the extent of the text.
      cxExtent = DT_GetExtentMinusPrefixes(hdc, lpchStr, cchCount, wFormat,
                        lpDrawInfo->cxOverhang) - lpDrawInfo->cxOverhang;
    }
  return cxExtent;
}

/***************************************************************************\
*  DT_DrawJustifiedLine
*      This function draws one complete line with proper justification
*
*   from Chicago ctlmgr.c  FritzS
\***************************************************************************/

void DT_DrawJustifiedLine(HDC  hdc, int yTop, LPCWSTR lpchLineSt,
                                 int cchCount, UINT wFormat,
                                 LPDRAWTEXTDATA lpDrawInfo)
{
  LPRECT lprc;
  int   cxExtent;
  int   xLeft;

  lprc = &(lpDrawInfo->rcFormat);
  xLeft = lprc->left;

  // Handle the special justifications (right or centered) properly.
  if(wFormat & (DT_CENTER | DT_RIGHT)) {
      cxExtent = DT_DrawStr(hdc, xLeft, yTop, lpchLineSt, cchCount, FALSE,
                     wFormat, lpDrawInfo) + lpDrawInfo->cxOverhang;
      if(wFormat & DT_CENTER)
          xLeft = lprc->left + (((lprc->right - lprc->left) - cxExtent) >> 1);
      else
          xLeft = lprc->right - cxExtent;
    } else
      xLeft = lprc->left;

  // Draw the whole line.
  cxExtent = DT_DrawStr(hdc, xLeft, yTop, lpchLineSt, cchCount, TRUE, wFormat,
                        lpDrawInfo) +lpDrawInfo->cxOverhang;
  if(cxExtent > lpDrawInfo->cxMaxExtent)
      lpDrawInfo->cxMaxExtent = cxExtent;
}

/***************************************************************************\
* DT_InitDrawTextInfo
*      This is called at the begining of DrawText(); This initializes the
* DRAWTEXTDATA structure passed to this function with all the required info.
*
*  from Chicago ctlmgr.c  FritzS
\***************************************************************************/

BOOL DT_InitDrawTextInfo(
    HDC hdc,
    LPRECT lprc,
    UINT wFormat,
    LPDRAWTEXTDATA lpDrawInfo,
    LPDRAWTEXTPARAMS lpDTparams)
{
  SIZE   sizeViewPortExt,sizeWindowExt;
  TEXTMETRICW tm;
  LPRECT      lprcDest;
  int         iTabLength = 8;   // Default Tab length is 8 characters.
  int         iLeftMargin;
  int         iRightMargin;
  BOOL        fUseSystemFont;

  if (lpDTparams) {
      /*
       *  Only if DT_TABSTOP flag is mentioned, we must use the iTabLength field.
       */
      if (wFormat & DT_TABSTOP)
          iTabLength = lpDTparams->iTabLength;
      iLeftMargin = lpDTparams->iLeftMargin;
      iRightMargin = lpDTparams->iRightMargin;
  } else {
      iLeftMargin = iRightMargin = 0;
  }

  /*
   *  Get the View port and Window extents for the given DC
   *  If this call fails, hdc must be invalid
   */
  if (!UserGetViewportExtEx(hdc,&sizeViewPortExt))
    return FALSE;
  UserGetWindowExtEx(hdc, &sizeWindowExt);

  /*
   *  For the current mapping mode,  find out the sign of x from left to right.
   */
  lpDrawInfo->iXSign =
      (((sizeViewPortExt.cx ^ sizeWindowExt.cx) & 0x80000000) ? -1 : 1);

  /*
   *  For the current mapping mode,  find out the sign of y from top to bottom.
   */
  lpDrawInfo->iYSign =
      (((sizeViewPortExt.cy ^ sizeWindowExt.cy) & 0x80000000) ? -1 : 1);

  /*
   *  Calculate the dimensions of the current font in this DC.
   * (If it is SysFont AND the mapping mode is MM_TEXT, use system font's data)
   */
  fUseSystemFont = ((wFormat & DT_INTERNAL) || IsSysFontAndDefaultMode(hdc));
  if (!fUseSystemFont) {
      /*
       *  Edit controls have their own way of calculating the aveCharWidth.
       */
      if (wFormat & DT_EDITCONTROL) {
          tm.tmAveCharWidth = UserGetCharDimensionsW(hdc, &tm, NULL);
          if (tm.tmAveCharWidth == 0) {
              fUseSystemFont = TRUE;
          }
      } else if (!UserGetTextMetricsW(hdc, &tm)) {
          /*
           * This can fail in a hard error popup during logon or logoff
           * because UpdatePerUserSystemParameters destroys the server-side
           * font handle for the DC, and a repaint occurs before we switch
           * desktops (the switch recreates the popup from scratch with the
           * new font OK). ChrisWil's changes to move system-wide attributes
           * into desktops should take care of this in Kernel-mode.  This is
           * just a horrible, horrible hack for now.
           */
          RIPMSG0(RIP_WARNING, "UserGetTextMetricsW failed: only in logon/off?\n");
          tm.tmOverhang = 0;

          /*
           * We should probably set fUseSystemFont to TRUE here. But I
           *  assume that this "horrible hack" works fine plus it has been
           *  here for good. So I'll leave it alone. 6/3/96
           */
      }
  }

  if (fUseSystemFont) {
      /*
       *  Avoid GetTextMetrics for internal calls since they use sys font.
       */
      tm.tmHeight = gpsi->cySysFontChar;
      tm.tmExternalLeading = gpsi->tmSysFont.tmExternalLeading;
      tm.tmAveCharWidth = gpsi->tmSysFont.tmAveCharWidth;
      tm.tmOverhang = gpsi->tmSysFont.tmOverhang;
  }


  // cyLineHeight is in pixels (This will be signed).
  lpDrawInfo->cyLineHeight = (tm.tmHeight +
            ((wFormat & DT_EXTERNALLEADING) ? tm.tmExternalLeading : 0)) *
            lpDrawInfo->iYSign;

  // cxTabLength is the tab length in pixels (This will not be signed)
  lpDrawInfo->cxTabLength = tm.tmAveCharWidth * iTabLength;

  // Set the cxOverhang
  lpDrawInfo->cxOverhang = tm.tmOverhang;

  // Pick up the proper TextOut function based on the prefix processing reqd.
  lpDrawInfo->lpfnTextDraw = ((wFormat & DT_NOPREFIX) ? (LPFNTEXTDRAW)UserTextOutW : PSMTextOut);

  // Set up the format rectangle based on the margins.
//  LCopyStruct(lprc, lprcDest = (LPRECT)&(lpDrawInfo->rcFormat), sizeof(RECT));
  lprcDest = &(lpDrawInfo->rcFormat);
  *lprcDest = *lprc;

  // We need to do the following only if the margins are given
  if(iLeftMargin | iRightMargin) {
      lprcDest->left += iLeftMargin * lpDrawInfo->iXSign;
      lprcDest->right -= (lpDrawInfo->cxRightMargin = iRightMargin * lpDrawInfo->iXSign);
    } else
      lpDrawInfo->cxRightMargin = 0;  // Initialize to zero.

  // cxMaxWidth is unsigned.
  lpDrawInfo->cxMaxWidth = (lprcDest->right - lprcDest->left) * lpDrawInfo->iXSign;
  lpDrawInfo->cxMaxExtent = 0;  // Initialize this to zero.

  return TRUE;
}

/***************************************************************************\
* DT_AdjustWhiteSpaces
*      In the case of WORDWRAP, we need to treat the white spaces at the
* begining/end of each line specially. This function does that.
*  lpStNext = points to the begining of next line.
*  lpiCount = points to the count of characters in the current line.
\***************************************************************************/

LPCWSTR  DT_AdjustWhiteSpaces(LPCWSTR  lpStNext, LPINT lpiCount, UINT wFormat)
{
  switch(wFormat & DT_HFMTMASK) {
      case DT_LEFT:
        // Prevent a white space at the begining of a left justfied text.
        // Is there a white space at the begining of next line......
        if((*lpStNext == TEXT(' ')) || (*lpStNext == TEXT('\t'))) {
            // ...then, exclude it from next line.
            lpStNext++;
          }
        break;

      case DT_RIGHT:
        // Prevent a white space at the end of a RIGHT justified text.
        // Is there a white space at the end of current line,.......
        if((*(lpStNext-1) == TEXT(' ')) || (*(lpStNext - 1) == TEXT('\t'))) {
            // .....then, Skip the white space from the current line.
            (*lpiCount)--;
          }
        break;

      case DT_CENTER:
        // Exclude white spaces from the begining and end of CENTERed lines.
        // If there is a white space at the end of current line.......
        if((*(lpStNext-1) == TEXT(' ')) || (*(lpStNext - 1) == TEXT('\t')))
            (*lpiCount)--;    //...., don't count it for justification.
        // If there is a white space at the begining of next line.......
        if((*lpStNext == TEXT(' ')) || (*lpStNext == TEXT('\t')))
            lpStNext++;       //...., exclude it from next line.
        break;
    }
  return lpStNext;
}

/***************************************************************************\
*  DT_BreakAWord
*      A word needs to be broken across lines and this finds out where to
*  break it.
\***************************************************************************/
LPCWSTR  DT_BreakAWord(HDC  hdc, LPCWSTR lpchText,
              int iLength, int iWidth, UINT wFormat, int iOverhang)
{
  int  iLow = 0, iHigh = iLength;
  int  iNew;


  while((iHigh - iLow) > 1) {
      iNew = iLow + (iHigh - iLow)/2;
      if(DT_GetExtentMinusPrefixes(hdc, lpchText, iNew, wFormat, iOverhang) > iWidth)
          iHigh = iNew;
      else
          iLow = iNew;
    }
  // If the width is too low, we must print atleast one char per line.
  // Else, we will be in an infinite loop.
  if(!iLow && iLength)
      iLow = 1;
  return (lpchText+iLow);
}

/***************************************************************************\
* DT_GetLineBreak
*      This finds out the location where we can break a line.
* Returns LPCSTR to the begining of next line.
* Also returns via lpiLineLength, the length of the current line.
* NOTE: (lpstNextLineStart - lpstCurrentLineStart) is not equal to the
* line length; This is because, we exclude some white spaces at the begining
* and/or end of lines; Also, CR/LF is excluded from the line length.
\***************************************************************************/

LPWSTR DT_GetLineBreak(
    HDC  hdc,
    LPCWSTR lpchLineStart,
    int   cchCount,
    DWORD dwFormat,
    LPINT lpiLineLength,
    LPDRAWTEXTDATA  lpDrawInfo)
{
  LPCWSTR lpchText, lpchEnd, lpch, lpchLineEnd;
  int   cxStart, cxExtent, cxNewExtent;
  BOOL  fAdjustWhiteSpaces = FALSE;
  WCHAR  ch;
#ifdef FE_SB // DT_GetLineBreak()
#error Excluded unsupported original FE_SB branch
#endif // FE_SB
#line 1216

  cxStart = lpDrawInfo->rcFormat.left;
  cxExtent = cxNewExtent = 0;
  lpchText = lpchLineStart;
  lpchEnd = lpchLineStart + cchCount;


  while(lpchText < lpchEnd) {
#ifdef FE_SB // DT_GetLineBreak()
#error Excluded unsupported original FE_SB branch
#else
#line 1227
      lpchLineEnd = lpch = GetNextWordbreak(0,lpchText, lpchEnd, (dwFormat & DT_WORDBREAK));
#endif // FE_SB
#line 1229
      // DT_DrawStr does not return the overhang; Otherwise we will end up
      // adding one overhang for every word in the string.

      // For simulated Bold fonts, the summation of extents of individual
      // words in a line is greater than the extent of the whole line. So,
      // always calculate extent from the LineStart.
      // BUGTAG: #6054 -- Win95B -- SANKAR -- 3/9/95 --
      cxNewExtent = DT_DrawStr(hdc, cxStart, 0, lpchLineStart, (int)(((PBYTE)lpch - (PBYTE)lpchLineStart)/sizeof(WCHAR)), FALSE,
                 dwFormat, lpDrawInfo);

      if ((dwFormat & DT_WORDBREAK) && ((cxNewExtent + lpDrawInfo->cxOverhang) > lpDrawInfo->cxMaxWidth)) {
          // Are there more than one word in this line?
          if (lpchText != lpchLineStart)  {
              lpchLineEnd = lpch = lpchText;
              fAdjustWhiteSpaces = TRUE;
          } else {
              //One word is longer than the maximum width permissible.
              //See if we are allowed to break that single word.
              if((dwFormat & DT_EDITCONTROL) && (!(dwFormat & DT_WORD_ELLIPSIS))) {
                  lpchLineEnd = lpch = DT_BreakAWord(hdc, lpchText, (int)(((PBYTE)lpch - (PBYTE)lpchText)/sizeof(WCHAR)),
                        lpDrawInfo->cxMaxWidth - cxExtent,
                        dwFormat,
                        lpDrawInfo->cxOverhang); //Break that word
                  //Note: Since we broke in the middle of a word, no need to
                  // adjust for white spaces.
              } else {
                  fAdjustWhiteSpaces = TRUE;
                  // Check if we need to end this line with ellipsis
                  if(dwFormat & DT_WORD_ELLIPSIS)
                    {
                      // Don't do this if already at the end of the string.
                      if (lpch < lpchEnd)
                        {
                          // If there are CR/LF at the end, skip them.
                          if ((ch = *lpch) == CR || ch == LF)
                            {
                              if ((++lpch < lpchEnd) && (*lpch == (WCHAR)(ch ^ (LF ^ CR))))
                                  lpch++;
                              fAdjustWhiteSpaces = FALSE;
                            }
                        }
                    }
              }
          }
          // Well! We found a place to break the line. Let us break from this
          // loop;
          break;
      } else {
          // Don't do this if already at the end of the string.
          if (lpch < lpchEnd) {
              if ((ch = *lpch) == CR || ch == LF) {
                  if ((++lpch < lpchEnd) && (*lpch == (WCHAR)(ch ^ (LF ^ CR))))
                      lpch++;
                  fAdjustWhiteSpaces = FALSE;
                  break;
              }
          }
      }

      // Point at the beginning of the next word.
      lpchText = lpch;
      cxExtent = cxNewExtent;
  }

  // Calculate the length of current line.
  *lpiLineLength = ((PBYTE)lpchLineEnd - (PBYTE)lpchLineStart)/sizeof(WCHAR);

  // Adjust the line length and lpch to take care of spaces.
  if(fAdjustWhiteSpaces && (lpch < lpchEnd))
      lpch = DT_AdjustWhiteSpaces(lpch, lpiLineLength, dwFormat);

  // return the begining of next line;
  return (LPWSTR)lpch;
}

/***************************************************************************\
*  NeedsEndEllipsis()
*      This function checks whether the given string fits within the given
*      width or we need to add end-ellipse. If it required end-ellipses, it
*      returns TRUE and it returns the number of characters that are saved
*      in the given string via lpCount.
\***************************************************************************/
BOOL  NeedsEndEllipsis(HDC        hdc,
                                     LPCWSTR     lpchText,
                                     LPINT      lpCount,
                                     LPDRAWTEXTDATA  lpDTdata,
                                     UINT       wFormat)
{
    int   cchText;
    int   ichMin, ichMax, ichMid;
    int   cxMaxWidth;
    int   iOverhang;
    int   cxExtent;
    SIZE size;
    cchText = *lpCount;  // Get the current count.

    if (cchText == 0)
        return FALSE;

    cxMaxWidth  = lpDTdata->cxMaxWidth;
    iOverhang   = lpDTdata->cxOverhang;

    cxExtent = DT_GetExtentMinusPrefixes(hdc, lpchText, cchText, wFormat, iOverhang);

    if (cxExtent <= cxMaxWidth)
        return FALSE;
    // Reserve room for the "..." ellipses;
    // (Assumption: The ellipses don't have any prefixes!)
    UserGetTextExtentPointW(hdc, szEllipsis, wcslen(szEllipsis), &size);
    cxMaxWidth -= size.cx - iOverhang;

    // If no room for ellipses, always show first character.
    //
    ichMax = 1;
    if (cxMaxWidth > 0) {
        // Binary search to find characters that will fit.
        ichMin = 0;
        ichMax = cchText;
        while (ichMin < ichMax) {
            // Be sure to round up, to make sure we make progress in
            // the loop if ichMax == ichMin + 1.
            //
            ichMid = (ichMin + ichMax + 1) / 2;

            cxExtent = DT_GetExtentMinusPrefixes(hdc, lpchText, ichMid, wFormat, iOverhang);

            if (cxExtent < cxMaxWidth)
                ichMin = ichMid;
            else {
                if (cxExtent > cxMaxWidth)
                    ichMax = ichMid - 1;
                else {
                    // Exact match up up to ichMid: just exit.
                    //
                    ichMax = ichMid;
                    break;
                  }
              }
          }

        // Make sure we always show at least the first character...
        //
        if (ichMax < 1)
            ichMax = 1;
      }

    *lpCount = ichMax;
    return TRUE;
}

/***************************************************************************\
* BOGUS: The same function is available in SHELL2.DLL also.
* We need to remove from one of the places.
\***************************************************************************/
// Returns a pointer to the last component of a path string.
//
// in:
//      path name, either fully qualified or not
//
// returns:
//      pointer into the path where the path is.  if none is found
//      returns a poiter to the start of the path
//
//  c:\foo\bar  -> bar
//  c:\foo      -> foo
//  c:\foo\     -> c:\foo\      (REVIEW: is this case busted?)
//  c:\         -> c:\          (REVIEW: this case is strange)
//  c:          -> c:
//  foo         -> foo
/***************************************************************************\
\***************************************************************************/


LPWSTR PathFindFileName(LPCWSTR pPath, int cchText)
{
    LPCWSTR pT;

    for (pT = pPath; cchText > 0 && *pPath; pPath++, cchText--) {
        if ((pPath[0] == TEXT('\\') || pPath[0] == TEXT(':')) && pPath[1])
            pT = pPath + 1;
    }

    return (LPWSTR)pT;   // REVIEW, should this be const?
}

/***************************************************************************\
* AddPathEllipse():
*      This adds a path ellipse to the given path name.
*      Returns TRUE if the resultant string's extent is less the the
* cxMaxWidth. FALSE, if otherwise.
\***************************************************************************/
int AddPathEllipsis(
    HDC    hDC,
    LPWSTR lpszPath,
    int    cchText,
    UINT   wFormat,
    int    cxMaxWidth,
    int    iOverhang)
{
  int    iLen;
  UINT   dxFixed, dxEllipsis;
  LPWSTR lpEnd;          /* end of the unfixed string */
  LPWSTR lpFixed;        /* start of text that we always display */
  BOOL   bEllipsisIn;
  int    iLenFixed;
  SIZE   size;

  lpFixed = PathFindFileName(lpszPath, cchText);
  if (lpFixed != lpszPath)
      lpFixed--;  // point at the slash
  else
      return cchText;

  lpEnd = lpFixed;
  bEllipsisIn = FALSE;
  iLenFixed = cchText - (lpFixed - lpszPath);
  dxFixed = DT_GetExtentMinusPrefixes(hDC, lpFixed, iLenFixed, wFormat, iOverhang);

  // It is assumed that the "..." string does not have any prefixes ('&').
  UserGetTextExtentPointW(hDC, szEllipsis, wcslen(szEllipsis), &size);
  dxEllipsis = size.cx - iOverhang;

  while (TRUE) {
      iLen = dxFixed + DT_GetExtentMinusPrefixes(hDC, lpszPath, ((PBYTE)lpEnd - (PBYTE)lpszPath)/sizeof(WCHAR),
                                       wFormat, iOverhang) - iOverhang;

      if (bEllipsisIn)
          iLen += dxEllipsis;

      if (iLen <= cxMaxWidth)
          break;

      bEllipsisIn = TRUE;

      if (lpEnd <= lpszPath) {
          /* Things didn't fit. */
          lpEnd = lpszPath;
          break;
      }

      /* Step back a character. */
      lpEnd--;
  }

  if (bEllipsisIn && (lpEnd + CCHELLIPSIS < lpFixed)) {
      // NOTE: the strings could over lap here. So, we use LCopyStruct.

      RtlMoveMemory((lpEnd + CCHELLIPSIS), lpFixed, iLenFixed * sizeof(WCHAR));
      *(lpEnd+CCHELLIPSIS+iLenFixed) = TEXT('\0');
      RtlMoveMemory(lpEnd, szEllipsis, wcslen(szEllipsis)* sizeof(WCHAR));

      cchText = (int)(lpEnd - lpszPath) + CCHELLIPSIS + iLenFixed;

      // now we can NULL terminate the string
      *(lpszPath + cchText) = TEXT('\0');
  }

  return cchText;
}

//-----------------------------------------------------------------------
// This function returns the number of characters actually drawn.
//-----------------------------------------------------------------------
int AddEllipsisAndDrawLine(
    HDC            hdc,
    int            yLine,
    LPCWSTR        lpchText,
    int            cchText,
    DWORD          dwDTformat,
    LPDRAWTEXTDATA lpDrawInfo)
{
    LPWSTR pEllipsis = NULL;
    WCHAR  szTempBuff[MAXBUFFSIZE];
    LPWSTR lpDest;
    BOOL   fAlreadyCopied = FALSE;

    // Check if this is a filename with a path AND
    // Check if the width is too narrow to hold all the text.
    if((dwDTformat & DT_PATH_ELLIPSIS) &&
        ((DT_GetExtentMinusPrefixes(hdc, lpchText, cchText,
                   dwDTformat, lpDrawInfo->cxOverhang)) > lpDrawInfo->cxMaxWidth)) {
        // We need to add Path-Ellipsis. See if we can do it in-place.
        if(!(dwDTformat & DT_MODIFYSTRING)) {
            // NOTE: When you add Path-Ellipsis, the string could grow by
            // CCHELLIPSIS bytes.
            if((cchText + CCHELLIPSIS + 1) <= MAXBUFFSIZE)
                lpDest = szTempBuff;
            else {   // Alloc from local heap.
                // Alloc the buffer from local heap.
                if(!(pEllipsis = (LPWSTR)UserRtlAllocMem(
                        (cchText+CCHELLIPSIS+1)*sizeof(WCHAR))))
                    return 0;
                lpDest = (LPWSTR)pEllipsis;
            }
            // Source String may not be NULL terminated. So, copy just
            // the given number of characters.
            RtlCopyMemory(lpDest, lpchText, cchText*sizeof(WCHAR));
            lpchText = lpDest;        // lpchText points to the copied buff.
            fAlreadyCopied = TRUE;    // Local copy has been made.
        }
        // Add the path ellipsis now!
        cchText = AddPathEllipsis(hdc, (LPWSTR)lpchText, cchText, dwDTformat,
            lpDrawInfo->cxMaxWidth, lpDrawInfo->cxOverhang);
    }

    // Check if end-ellipsis are to be added.
    if((dwDTformat & (DT_END_ELLIPSIS | DT_WORD_ELLIPSIS)) &&
        NeedsEndEllipsis(hdc, lpchText, &cchText, lpDrawInfo, dwDTformat)) {
        // We need to add end-ellipsis; See if we can do it in-place.
        if((!(dwDTformat & DT_MODIFYSTRING)) && (!fAlreadyCopied))   {
            // See if the string is small enough for the buff on stack.
            if((cchText+CCHELLIPSIS+1) <= MAXBUFFSIZE)
                lpDest = szTempBuff;  // If so, use it.
            else {
                // Alloc the buffer from local heap.
                if(!(pEllipsis = (LPWSTR)UserRtlAllocMem(
                        (cchText+CCHELLIPSIS+1)*sizeof(WCHAR))))
                    return 0;
                lpDest = pEllipsis;
            }
            // Make a copy of the string in the local buff.
//          LCopyStruct(lpchText, lpDest, cchText);
            RtlCopyMemory(lpDest, lpchText, cchText*sizeof(WCHAR));
            lpchText = lpDest;
        }
        // Add an end-ellipsis at the proper place.
        wcscpy((LPWSTR)(lpchText+cchText), szEllipsis);
        cchText = wcslen(lpchText);
    }

    // Draw the line that we just formed.
    DT_DrawJustifiedLine(hdc, yLine, lpchText, cchText, dwDTformat, lpDrawInfo);

    // Free the block allocated for End-Ellipsis.
    if(pEllipsis)
        UserRtlFreeMem(pEllipsis);

    return cchText;
}


/***************************************************************************\
* IDrawTextEx
*      This is the new DrawText API
\***************************************************************************/

#line 1598
#ifdef DEBUG
#error Unsupported DEBUG original branch
#line 1601
#endif
#line 1602
#line 1622
#ifdef LATER
#error Excluded unsupported original LATER branch
#line 1634
#endif
#line 1635
#line 1657
#ifdef _USERK_
#error Unsupported _USERK_ original branch
#else
#line 1660
#line 1661
#endif
#line 1662
#line 1673
#ifdef LATER
#error Excluded unsupported original LATER branch
#line 1685
#endif
#line 1686
#line 1853


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
    static WCHAR achWorkBuffer[255];
    WCHAR *pchOut = achWorkBuffer;
    TEXTMETRICW textMetric;
    SIZE size;
    RECT rc;
    COLORREF color;

    if (cch >= sizeof(achWorkBuffer)/sizeof(WCHAR)) {
        pchOut = (WCHAR*)UserRtlAllocMem((cch+1) * sizeof(WCHAR));
        if (pchOut == NULL)
            return;
    }

    result = GetPrefixCount(lpsz, cch, pchOut, cch);

    UserTextOutW(hdc, xLeft, yTop, pchOut,
                      cch - HIWORD(result));

    /*
     * Any true prefix characters to underline?
     */
    if (LOWORD(result) == 0xFFFF) {
        if (pchOut != achWorkBuffer)
            UserRtlFreeMem(pchOut);
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
    UserExtTextOutW(hdc, xLeft, yTop, ETO_OPAQUE, &rc, TEXT(""), 0, NULL);
    UserSetBkColor(hdc, color);

    if (pchOut != achWorkBuffer) {
        UserRtlFreeMem(pchOut);
    }
}

#line 1958
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

BOOL IsSysFontAndDefaultMode(HDC hdc)
{
    return((UserGetHFONT(hdc) == ghFontSys) && (UserGetMapMode(hdc) == MM_TEXT));
}
