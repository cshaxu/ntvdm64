/****************************** Module Header ******************************\
* Module Name: clres.c
*
* Copyright (c) 1985 - 1999, Microsoft Corporation
*
* Resource Loading/Creation Routines
*
* History:
* 24-Sep-1990 MikeKe    From win30
* 19-Sep-1995 ChrisWil  Win95/NT merge.
\***************************************************************************/

#if defined(WOW_ORIGINAL_DIALOG_TEMPLATE)
#line 13
#include "precomp.h"
#pragma hdrstop
#else
#include "non-mvdm/windows/core/w32inc/newres.h"
#ifndef WOW_BITMAP_FIXTURE_BINDINGS
#include "wow_bitmap_bindings.h"
#include "wow_menu_resource_bindings.h"
#define WCSToMB(text, count, output, capacity, allocate) \
    WCSToMBEx(0, text, count, output, capacity, allocate)
#endif
#endif

#if !defined(WOW_ORIGINAL_DIALOG_TEMPLATE)
#line 16
/*
 * Constants.
 */
#define BPP01_MAXCOLORS     2
#define BPP04_MAXCOLORS    16
#define BPP08_MAXCOLORS   256
#define ICON_DEFAULTDEPTH   8
#define ICON_MAXWIDTH     256
#define ICON_MAXHEIGHT    256

#define RESCLR_BLACK      0x00000000
#define RESCLR_WHITE      0x00FFFFFF

typedef struct {
    ACCEL accel;
    WORD  padding;
} RESOURCE_ACCEL, *PRESOURCE_ACCEL;

/*
 * Bitmap resource IDs
 */
#define BMR_ICON    1
#define BMR_BITMAP  2
#define BMR_CURSOR  3

typedef struct _OLDCURSOR {
    BYTE bType;
    BYTE bFormat;
    WORD xHotSpot;  // 0 for icons
    WORD yHotSpot;  // 0 for icons
    WORD cx;
    WORD cy;
    WORD cxBytes;
    WORD wReserved2;
    BYTE abBitmap[1];
} OLDCURSOR, *POLDCURSOR;
typedef OLDCURSOR UNALIGNED *UPOLDCURSOR;

/*
 * Local Macros.
 */
#line 63
#define ISRIFFFORMAT(p) \
    (((UNALIGNED RTAG *)(p))->ckID == FOURCC_RIFF)

#define MR_FAILFOR40    0x01
#define MR_MONOCHROME   0x02


typedef struct tagMAPRES {
    WORD idDisp;                // display driver ID
    WORD idUser;                // USER ID
    BYTE bFlags;                // Flags
    BYTE bReserved;             // unused
} MAPRES, *LPMAPRES, *PMAPRES;


HBITMAP CopyBmp(HBITMAP hbmpOrg, int cxNew, int cyNew, UINT LR_flags);

#endif

#if defined(WOW_ORIGINAL_DIALOG_TEMPLATE)
#line 118


/***********************************************************************\
* WowGetModuleFileName
*
* This converts a WOW or non-WOW module handle to a string form that
* can be restored even for WOW handles.
*
* Returns: fSuccess
*
* 29-Nov-1995 SanfordS  Created.
\***********************************************************************/

BOOL WowGetModuleFileName(
    HMODULE hModule,
    LPWSTR  pwsz,
    DWORD   cchMax)
{
    if (!GetModuleFileName(hModule, pwsz, cchMax)) {

        if (cchMax < 10) {
            RIPMSG0(RIP_WARNING, "WowGetModuleFileName: exceeded Char-Max");
            return FALSE;
        }

        wsprintf(pwsz, TEXT("\001%08lx"), hModule);
    }

    return TRUE;
}

/***********************************************************************\
* WowGetModuleHandle
*
* This restores the string form of a module handle created by
* WowGetModuleFileName to the original handle.
*
* Returns: fSuccess
*
* 29-Nov-1995 Created   SanfordS
\***********************************************************************/

HMODULE WowGetModuleHandle(
    LPCWSTR pwsz)
{
    HMODULE hMod = NULL;
    DWORD   digit;

    if (pwsz[0] == TEXT('\001')) {

        /*
         * Cant seem to link to swscanf without CRT0 problems so just
         * do it by hand.
         */
        while (*(++pwsz)) {

            if (*pwsz == TEXT(' '))
                continue;

            digit = *pwsz - TEXT('0');

            if (digit > 9)
                digit += (DWORD)(TEXT('0') - TEXT('a') + 10);

            (ULONG_PTR)hMod <<= 4;
            (ULONG_PTR)hMod += digit;
        }

    } else {

        hMod = GetModuleHandle(pwsz);
    }

    return hMod;
}
#line 597
/***************************************************************************\
* SkipIDorString
*
* Skips string (or ID) and returns the next aligned WORD.
*
\***************************************************************************/

PBYTE SkipIDorString(
    LPBYTE pb)
{
    if (*((LPWORD)pb) == 0xFFFF)
        return (pb + 4);

    while (*((PWCHAR)pb)++ != 0);

    return pb;
}

/***************************************************************************\
* GetSizeDialogTemplate
*
* This gets called by thank produced stubs. It returns the size of a
* dialog template.
*
* 07-Apr-1991 ScottLu   Created.
\***************************************************************************/

DWORD GetSizeDialogTemplate(
    HINSTANCE      hmod,
    LPCDLGTEMPLATE pdt)
{
    UINT           cdit;
    LPBYTE         pb;
    BOOL           fChicago;
    LPDLGTEMPLATE2 pdt2;

    if (HIWORD(pdt->style) == 0xFFFF) {

        pdt2 = (LPDLGTEMPLATE2)pdt;
        fChicago = TRUE;

        /*
         * Fail if the app is passing invalid style bits.
         */
        if (pdt2->style & ~(DS_VALID40 | 0xffff0000)) {
            RIPMSG0(RIP_WARNING, "Bad dialog style bits - please remove");
            return 0;
        }

        pb = (LPBYTE)(((LPDLGTEMPLATE2)pdt) + 1);

    } else {

        fChicago = FALSE;

        /*
         * Check if invalid style bits are being passed. Fail if the app
         * is a new app ( >= VER40).
         * This is to ensure that we are compatible with Chicago.
         */
        if ((pdt->style & ~(DS_VALID40 | 0xffff0000)) &&
                (GETEXPWINVER(hmod) >= VER40)) {

            /*
             * It's a new app with invalid style bits - fail.
             */
            RIPMSG0(RIP_WARNING, "Bad dialog style bits - please remove");
            return 0;
        }

        pb = (LPBYTE)(pdt + 1);
    }

    /*
     * If there is a menu ordinal, add 4 bytes skip it. Otherwise it is a
     * string or just a 0.
     */
    pb = SkipIDorString(pb);

    /*
     * Skip window class and window text, adjust to next word boundary.
     */
    pb = SkipIDorString(pb);
    pb = SkipIDorString(pb);

    /*
     * Skip font type, size and name, adjust to next dword boundary.
     */
    if ((fChicago ? pdt2->style : pdt->style) & DS_SETFONT) {
        pb += fChicago ? sizeof(DWORD) + sizeof(WORD): sizeof(WORD);
        pb = SkipIDorString(pb);
    }
    pb = (LPBYTE)(((ULONG_PTR)pb + 3) & ~3);

    /*
     * Loop through dialog items now...
     */
    cdit = fChicago ? pdt2->cDlgItems : pdt->cdit;

    while (cdit-- != 0) {

        UINT cbCreateParams;

        pb += fChicago ? sizeof(DLGITEMTEMPLATE2) : sizeof(DLGITEMTEMPLATE);

        /*
         * Skip the dialog control class name.
         */
        pb = SkipIDorString(pb);

        /*
         * Look at window text now.
         */
        pb = SkipIDorString(pb);

        cbCreateParams = *((LPWORD)pb);

        /*
         * skip any CreateParams which include the generated size WORD.
         */
        if (cbCreateParams)
            pb += cbCreateParams;

        pb += sizeof(WORD);

        /*
         * Point at the next dialog item. (DWORD aligned)
         */
        pb = (LPBYTE)(((ULONG_PTR)pb + 3) & ~3);
    }

    /*
     * Return template size.
     */
    return (DWORD)(pb - (LPBYTE)pdt);
}
#line 775



FUNCLOG6(LOG_GENERAL, INT_PTR, WINAPI, DialogBoxIndirectParamAorW, HINSTANCE, hmod, LPCDLGTEMPLATEW, lpDlgTemplate, HWND, hwndOwner, DLGPROC, lpDialogFunc, LPARAM, dwInitParam, UINT, fAnsiFlags)
INT_PTR WINAPI DialogBoxIndirectParamAorW(
    HINSTANCE       hmod,
    LPCDLGTEMPLATEW lpDlgTemplate,
    HWND            hwndOwner,
    DLGPROC         lpDialogFunc,
    LPARAM          dwInitParam,
    UINT            fAnsiFlags)
{
    DWORD cb;

    /*
     * The server routine destroys the menu if it fails.
     */
    cb = GetSizeDialogTemplate(hmod, lpDlgTemplate);

    if (!cb) {
        RIPMSG0(RIP_WARNING, "DialogBoxIndirectParam: Invalid Paramter");
        return -1;
    }

    return InternalDialogBox(hmod,
                            (LPDLGTEMPLATE)lpDlgTemplate,
                            hwndOwner,
                            lpDialogFunc,
                            dwInitParam,
                            SCDLG_CLIENT | (fAnsiFlags & (SCDLG_ANSI | SCDLG_16BIT)));
}
#endif

#if !defined(WOW_ORIGINAL_DIALOG_TEMPLATE)
#line 1543
/***************************************************************************\
* _FreeResource   (API)
* _LockResource   (API)
* _UnlockResource (API)
*
* These are dummy routines that need to exist for the apfnResCallNative
* array, which is used when calling the run-time libraries.
*
\***************************************************************************/

BOOL WINAPI _FreeResource(
    HANDLE    hResData,
    HINSTANCE hModule)
{
    UNREFERENCED_PARAMETER(hResData);
    UNREFERENCED_PARAMETER(hModule);

    return FALSE;
}

LPSTR WINAPI _LockResource(
    HANDLE    hResData,
    HINSTANCE hModule)
{
    UNREFERENCED_PARAMETER(hModule);

    return (LPSTR)(hResData);
}

BOOL WINAPI _UnlockResource(
    HANDLE    hResData,
    HINSTANCE hModule)
{
    UNREFERENCED_PARAMETER(hResData);
    UNREFERENCED_PARAMETER(hModule);

    return TRUE;
}
#line 1728
/***************************************************************************\
* Convert1BppToMonoBitmap
*
* This routine converts a 1bpp bitmap to a true monochrome surface.  This
* is done for bitmaps which need to do foreground/background color matching
* at output time.  Otherwise, a 1bpp will just match to its palette.
*
* NOTE: This routine deletes the original bitmap if successful.  If failure
*       we'll return the original bitmap.
*
* History:
* 17-Apr-1996 ChrisWil  Created
\***************************************************************************/

HBITMAP Convert1BppToMonoBitmap(
    HDC     hdcSrc,
    HBITMAP hbm1Bpp)
{
    HBITMAP hbmMono = hbm1Bpp;
    HBITMAP hbmDst;
    HBITMAP hbmS;
    HBITMAP hbmD;
    HDC     hdcDst;
    BITMAP  bm;

    if (hdcDst = CreateCompatibleDC(hdcSrc)) {

        GetObject(hbm1Bpp, sizeof(BITMAP), &bm);

        if (hbmDst = CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL)) {

            hbmS = SelectBitmap(hdcSrc, hbm1Bpp);
            hbmD = SelectBitmap(hdcDst, hbmDst);

            BitBlt(hdcDst,
                   0,
                   0,
                   bm.bmWidth,
                   bm.bmHeight,
                   hdcSrc,
                   0,
                   0,
                   SRCCOPY);

            SelectBitmap(hdcSrc, hbmS);
            SelectBitmap(hdcDst, hbmD);

            hbmMono = hbmDst;
            DeleteObject(hbm1Bpp);
        }

        DeleteDC(hdcDst);
    }

    return hbmMono;
}

/***************************************************************************\
* CreateScreenBitmap
*
* This routine creates a screen bitmap.  We use the CreateDIBitmap call
* to do compatible color-matching with Win95.  Also, note that this
* routine takes in WORD aligned bits.
*
\***************************************************************************/

HBITMAP CreateScreenBitmap(
    int    cx,
    int    cy,
    UINT   planes,
    UINT   bpp,
    LPSTR  lpBits,
    LPBOOL pf1Bpp)
{
    HDC     hdcScreen;
    HBITMAP hbm = NULL;
    DWORD   dwCount;

    static struct {
        BITMAPINFOHEADER bi;
        DWORD            ct[16];
    } dib4Vga = {{sizeof(BITMAPINFOHEADER),
                  0,
                  0,
                  1,
                  4,
                  BI_RGB,
                  0,
                  0,
                  0,
                  16,
                  0
                 },
                 {0x00000000,
                  0x00800000,
                  0x00008000,
                  0x00808000,
                  0x00000080,
                  0x00800080,
                  0x00008080,
                  0x00C0C0C0,
                  0x00808080,
                  0x00FF0000,
                  0x0000FF00,
                  0x00FFFF00,
                  0x000000FF,
                  0x00FF00FF,
                  0x0000FFFF,
                  0x00FFFFFF
                 }
                };

    static struct {
        BITMAPINFOHEADER bi;
        DWORD            ct[2];
    } dib1Vga = {{sizeof(BITMAPINFOHEADER),
                  0,
                  0,
                  1,
                  1,
                  BI_RGB,
                  0,
                  0,
                  0,
                  2,
                  0
                 },
                 {0x00000000,
                  0x00FFFFFF
                 }
                };


    /*
     * Create the surface.
     */
    if (hdcScreen = GETINITDC()) {

        /*
         * This appears to mess up color to mono conversion by losing all
         * the data and forcing all non-forground colors to black.
         * (try copyimage with IDC_WARNING_DEFAULT)
         * This is what win95 does but their system works.  The scary thing
         * (according to marke) is that win95 may have changed GDI to make
         * this work.
         *
         * In order to get nearest-color-matching compatible with Win95,
         * we're going to need to use the CreateDIBitmap() for mono-surfaces.
         * This code-path will do nearest-color, rather than color-matching.
         */
        if ((bpp == 1) && (planes == 1)) {

            dib1Vga.bi.biWidth  = cx;
            dib1Vga.bi.biHeight = cy;

            hbm = CreateDIBitmap(hdcScreen,
                                 (LPBITMAPINFOHEADER)&dib1Vga,
                                 CBM_CREATEDIB,
                                 NULL,
                                 (LPBITMAPINFO)&dib1Vga,
                                 DIB_RGB_COLORS);

            *pf1Bpp = TRUE;

        } else {

            if (((planes == 0) || (planes == gpsi->Planes)) &&
                ((bpp == 0) || (bpp == gpsi->BitsPixel))) {

                hbm = CreateCompatibleBitmap(hdcScreen, cx, cy);

            } else {

                dib4Vga.bi.biBitCount = planes * bpp ? planes * bpp : gpsi->BitCount;

#if 0 // We use to do the dib-section create, but this breaks icons
#error Unsupported original resource branch
#line 1920
#else
                dib4Vga.bi.biWidth  = cx;
                dib4Vga.bi.biHeight = cy;

                hbm = CreateDIBitmap(hdcScreen,
                                     (LPBITMAPINFOHEADER)&dib4Vga,
                                     CBM_CREATEDIB,
                                     NULL,
                                     (LPBITMAPINFO)&dib4Vga,
                                     DIB_RGB_COLORS);
#endif
            }
        }

        RELEASEINITDC(hdcScreen);
    }

    if (hbm && lpBits) {

        BITMAP bm;

        GetObject(hbm, sizeof(BITMAP), &bm);
        dwCount = (DWORD)(UINT)(bm.bmWidthBytes * bm.bmPlanes) * (DWORD)(UINT)cy;
        SetBitmapBits(hbm, dwCount, lpBits);
    }

    return hbm;
}
#line 3019
/***************************************************************************\
* BitmapFromDIB
*
* Creates a bitmap-handle from a DIB-Spec.  This function supports the
* LR_CREATEDIBSECTION flag, sets proper color depth, and stretches the
* DIBs as requested.
*
\***************************************************************************/

HBITMAP BitmapFromDIB(
    int          cxNew,
    int          cyNew,
    WORD         bPlanesNew,
    WORD         bBitsPixelNew,
    UINT         LR_flags,
    int          cxOld,
    int          cyOld,
    LPSTR        lpBits,
    LPBITMAPINFO lpbi,
    HPALETTE     hpal)
{
    HBITMAP hbmpNew = NULL;
    BOOL    fStretch;
    BOOL    f1Bpp = FALSE;

    RtlEnterCriticalSection(&gcsHdc);

    if (cxNew == 0)
        cxNew = cxOld;

    if (cyNew == 0)
        cyNew = cyOld;

    fStretch = ((cxNew != cxOld) || (cyNew != cyOld));

    /*
     * If LR_flags indicate DIB-Section, then return that as the
     * bitmap handle.
     */
    if (LR_flags & (LR_CREATEDIBSECTION | LR_CREATEREALDIB)) {

        int   cxTemp;
        int   cyTemp;
        BOOL  fOldFormat;
        LPVOID dwDummy;
        DWORD dwTemp;

#define lpbch ((LPBITMAPCOREHEADER)lpbi)

        fOldFormat = ((WORD)lpbi->bmiHeader.biSize == sizeof(BITMAPCOREHEADER));

        if (fOldFormat) {

            cxTemp = lpbch->bcWidth;
            cyTemp = lpbch->bcHeight;

            lpbch->bcWidth  = (WORD)cxNew;
            lpbch->bcHeight = (WORD)cyNew;

        } else {

            cxTemp = lpbi->bmiHeader.biWidth;
            cyTemp = lpbi->bmiHeader.biHeight;
            dwTemp = lpbi->bmiHeader.biCompression;

            lpbi->bmiHeader.biWidth  = cxNew;
            lpbi->bmiHeader.biHeight = cyNew;

            if (dwTemp != BI_BITFIELDS)
                lpbi->bmiHeader.biCompression = BI_RGB;
        }

        if (LR_flags & LR_CREATEREALDIB) {
            hbmpNew = CreateDIBitmap(ghdcBits2,
                                     (LPBITMAPINFOHEADER)lpbi,
                                     CBM_CREATEDIB,
                                     NULL,
                                     lpbi,
                                     DIB_RGB_COLORS);
        } else {
            hbmpNew = CreateDIBSection(ghdcBits2,
                                       lpbi,
                                       DIB_RGB_COLORS,
                                       &dwDummy,
                                       0,
                                       0);
        }

        if (fOldFormat) {
            lpbch->bcWidth  = (WORD)cxTemp;
            lpbch->bcHeight = (WORD)cyTemp;
        } else {
            lpbi->bmiHeader.biWidth       = cxTemp;
            lpbi->bmiHeader.biHeight      = cyTemp;
            lpbi->bmiHeader.biCompression = dwTemp;
        }
#undef lpbch
    }

    if (hbmpNew == NULL) {

        hbmpNew = CreateScreenBitmap(cxNew,
                                     cyNew,
                                     bPlanesNew,
                                     bBitsPixelNew,
                                     NULL,
                                     &f1Bpp);
    }

    if (hbmpNew) {

        int     nStretchMode;
        DWORD   rgbBk;
        DWORD   rgbText;
        HBITMAP hbmpT;
        BOOL    fFail;

        /*
         * We need to select in appropriate bitmap immediately!  That way,
         * if we need to handle palette realization, the color matching
         * will work properly.
         */
        hbmpT = SelectBitmap(ghdcBits2, hbmpNew);

        /*
         * Setup for stretching
         */
        if (fStretch) {
            nStretchMode = SetBestStretchMode(ghdcBits2,
                                              bPlanesNew,
                                              bBitsPixelNew);
        }

        rgbBk   = SetBkColor(ghdcBits2, RESCLR_WHITE);
        rgbText = SetTextColor(ghdcBits2, RESCLR_BLACK);

        /*
         * Realize the palette.
         */
        if (hpal) {
#if DBG
#error Unsupported original resource branch
#line 3161
#endif // DBG

            hpal = SelectPalette(ghdcBits2, hpal, FALSE);
            RealizePalette(ghdcBits2);
        }

        if (fStretch) {

            fFail = SmartStretchDIBits(ghdcBits2,
                               0,
                               0,
                               cxNew,
                               cyNew,
                               0,
                               0,
                               cxOld,
                               cyOld,
                               lpBits,
                               lpbi,
                               DIB_RGB_COLORS,
                               SRCCOPY) <= 0;
        } else {

            fFail = SetDIBits(ghdcBits2,
                      hbmpNew,
                      0,
                      cyNew,
                      lpBits,
                      lpbi,
                      DIB_RGB_COLORS) <= 0;
        }

        /*
         * Unrealize the palette
         */
        if (hpal) {
            SelectPalette(ghdcBits2, hpal, TRUE);
            RealizePalette(ghdcBits2);
        }

        /*
         * Cleanup after stretching
         */
        SetTextColor(ghdcBits2, rgbText);
        SetBkColor(ghdcBits2, rgbBk);
        if (fStretch)
            SetStretchBltMode(ghdcBits2, nStretchMode);

        SelectBitmap(ghdcBits2, hbmpT);

        /*
         * If the SetDIBits() of StretchDIBits() failed, it is probably because
         * GDI or the driver did not like the DIB format.  This may happen if
         * the file is truncated and we are using a memory mapped file to read
         * the DIB in.  In this case, an exception gets thrown in GDI, that it
         * traps and will return failure from the GDI call.
         */

        if (fFail) {
            DeleteObject(hbmpNew);
            hbmpNew = NULL;
        }
    }

    /*
     * If the bitmap was created as a 1bpp, we need to convert to a
     * true mono-bitmap.  GDI performs different color-matching depending
     * upon this case.
     */
    if (f1Bpp && hbmpNew)
        hbmpNew = Convert1BppToMonoBitmap(ghdcBits2, hbmpNew);

    RtlLeaveCriticalSection(&gcsHdc);
    return hbmpNew;
}

/***************************************************************************\
* HowManyColors
*
* Aug-01-2000 DwayneN - Fixed high color support
\***************************************************************************/

DWORD HowManyColors(
    IN  UPBITMAPINFOHEADER upbih,
    IN  BOOL               fOldFormat,
    OUT OPTIONAL LPBYTE    *ppColorTable)
{
#define upbch ((UPBITMAPCOREHEADER)upbih)

    if (fOldFormat) {
        if (ppColorTable != NULL) {
            *ppColorTable = (LPBYTE)(upbch + 1);
        }
        if (upbch->bcBitCount <= 8)
            return (1 << upbch->bcBitCount);
    } else {
        if (ppColorTable != NULL) {
            *ppColorTable = (LPBYTE)(upbih + 1);
        }

        if (upbih->biClrUsed) {
            /*
             * If the bitmap header explicitly provides the number of colors
             * in the color table, use it.
             */
            return (DWORD)upbih->biClrUsed;
        } else if (upbih->biBitCount <= 8) {
            /*
             * If the bitmap header describes a pallete-bassed bitmap
             * (8bpp or less) then the color table must be big enough
             * to hold all palette indecies.
             */
            return (1 << upbih->biBitCount);
        } else {
            /*
             * Finally, if the bitmap header describes a highcolor+ bitmap
             * (anything over 8bpp, but typically 16bpp, 24bpp, or 32bpp)
             * there isn't a need for a color table.  However, 16bpp and
             * 32bpp bitmaps are special cases since it is not obvious how
             * they should be split into triplets (Red, Green, and Blue).  For
             * instance, 16bpp bitmaps can have 6 red bits, 5 green bits, and
             * 5 blue bits.  Or some other combination.  To specify this, the
             * 16bpp and 32bpp bitmaps provide BI_BITFIELDS in the 
             * biCompression field.  This indicates that the color table will
             * have 3 DWORDS that describe the masks for the red, green, and
             * blue components of entry in the bitmap.  If this isn't 
             * specified, then GDI assumes "555" for 16bpp and "XBGR" for
             * 32bpp.  24bpp bitmaps are always "XBGR" and consume a 32-bit
             * entry in the form of an RGBQUAD.
             */
            if (upbih->biCompression == BI_BITFIELDS) {
                if ((upbih->biBitCount != 16) && (upbih->biBitCount != 32)) {
                    RIPMSG1(RIP_WARNING, "HowManyColors: biCompression==BI_BITFIELDS on a %dbpp bitmap!", upbih->biBitCount);
                }

                return 3;
            }
        }
    }

    /*
     * Evidently, there is no color table.
     */
    return 0;

#undef upbch
}

/***************************************************************************\
* ChangeDibColors
*
* Given a DIB, processes LR_MONOCHROME, LR_LOADTRANSPARENT and
* LR_LOADMAP3DCOLORS flags on the given header and colortable.
*
*
\***************************************************************************/

VOID ChangeDibColors(
    IN LPBITMAPINFOHEADER lpbih,
    IN UINT               LR_flags)
{
    LPDWORD lpColorTable;
    DWORD  rgb;
    UINT   iColor;
    UINT   cColors;

    cColors = HowManyColors(lpbih, FALSE, &(LPBYTE)lpColorTable);

    /*
     * NT Bug 366661: Don't check the color count here b/c we will do different
     * things depending on what type of change we are performing.  For example,
     * when loading hi-color/true-color icons, we always need to do the
     * monochrome conversion in order to properly get an icon-mask.
     */

    /*
     * LR_MONOCHROME is the only option that handles PM dibs.
     */
    if (LR_flags & LR_MONOCHROME) {
        /*
         * LR_MONOCHROME is the only option that handles PM dibs.
         *
         * DO THIS NO MATTER WHETHER WE HAVE A COLOR TABLE!  We need
         * to do this for mono conversion and for > 8 BPP
         * icons/cursors.  In CopyDibHdr, we already made a copy of
         * the header big enough to hold 2 colors even on high-color+
         * bpp images.
         */
        lpbih->biBitCount = lpbih->biPlanes = 1;
        lpbih->biCompression = 0;
        lpColorTable[0] = RESCLR_BLACK;
        lpColorTable[1] = RESCLR_WHITE;
    } else if (LR_flags & LR_LOADTRANSPARENT) {

        LPBYTE pb;

        /*
         * No color table!  Do nothing.
         */
        if (cColors == 0) {
            RIPMSG0(RIP_WARNING, "ChangeDibColors: DIB doesn't have a color table");
            return;
        }

        pb = (LPBYTE)(lpColorTable + cColors);

        /*
         * Change the first pixel's color table entry to RGB_WINDOW
         * Gosh, I love small-endian
         */
        if (lpbih->biCompression == 0)
            iColor = (UINT)pb[0];
        else
            /*
             * RLE bitmap, will start with cnt,clr  or  0,cnt,clr
             */
            iColor = (UINT)(pb[0] == 0 ? pb[2] : pb[1]);

        switch (cColors) {
        case BPP01_MAXCOLORS:
            iColor &= 0x01;
            break;

        case BPP04_MAXCOLORS:
            iColor &= 0x0F;
            break;

        case BPP08_MAXCOLORS:
            iColor &= 0xFF;
            break;
        }

        rgb = (LR_flags & LR_LOADMAP3DCOLORS ? SYSRGB(3DFACE) : SYSRGB(WINDOW));

        lpColorTable[iColor] = RGBX(rgb);

    } else  if (LR_flags & LR_LOADMAP3DCOLORS) {

        /*
         * Fix up the color table, mapping shades of grey to the current
         * 3D colors.
         */
        for (iColor = 0; iColor < cColors; iColor++) {

            switch (*lpColorTable & 0x00FFFFFF) {

            case RGBX(RGB(223, 223, 223)):
                rgb = SYSRGB(3DLIGHT);
                goto ChangeColor;

            case RGBX(RGB(192, 192, 192)):
                rgb = SYSRGB(3DFACE);
                goto ChangeColor;

            case RGBX(RGB(128, 128, 128)):
                rgb = SYSRGB(3DSHADOW);

                /*
                 * NOTE: byte-order is different in DIBs than in RGBs
                 */
ChangeColor:
                *lpColorTable = RGBX(rgb);
                break;
            }
            lpColorTable++;
        }
    }
}
#line 3607
/***************************************************************************\
* TrulyMonochrome
*
* Checks to see if a DIB colro table is truly monochrome.  ie: the color
* table has black & white entries only.
*
\***************************************************************************/

BOOL TrulyMonochrome(
    LPVOID lpColorTable,
    BOOL   fOldFormat)
{
    #define lpRGB  ((UNALIGNED LONG *)lpColorTable)
    #define lpRGBw ((UNALIGNED WORD *)lpColorTable)

    if (fOldFormat) {

        /*
         * Honey - its triplets.
         */
        if (lpRGBw[0] == 0x0000)
            return (lpRGBw[1] == 0xFF00) && (lpRGBw[2] == 0xFFFF);
        else if (lpRGBw[0] == 0xFFFF)
            return (lpRGBw[1] == 0x00FF) && (lpRGBw[2] == 0x0000);

    } else {

        /*
         * Honey - its quadruplets!
         */
        if (lpRGB[0] == RESCLR_BLACK)
            return (lpRGB[1] == RESCLR_WHITE);
        else if (lpRGB[0] == RESCLR_WHITE)
            return (lpRGB[1] == RESCLR_BLACK);
    }

    #undef lpRGB
    #undef lpRGBw

    return FALSE;
}

/***************************************************************************\
* CopyDibHdr
*
* Copies and converts a DIB resource header
*
* Handles conversion of OLDICON, OLDCURSOR and BITMAPCOREHEADER
* structures to BITMAPINFOHEADER headers.
*
* Note: fSingleHeightMasks is set for OLDICON and OLDCURSOR formats.
*       This identifies that a monochrome AND/Color mask
*       is NOT double height as it is in the newer formats.
*
* NOTE:  On the off chance that LR_LOADTRANSPARENT is used, we want to
*     copy a DWORD of the bits.  Since DIB bits are DWORD aligned, we know
*     at least a DWORD is there, even if the thing is a 1x1 mono bmp.
*
* The returned buffer is allocated in this function and needs to be
* freed by the caller.
*
* 22-Oct-1995 SanfordS  Revised
\***************************************************************************/

LPBITMAPINFOHEADER CopyDibHdr(
    IN  UPBITMAPINFOHEADER upbih,
    OUT LPSTR             *lplpBits,
    OUT LPBOOL             lpfMono)
{

#define upbch ((UPBITMAPCOREHEADER)upbih)
    DWORD              cColors;
    DWORD              cMinColors;
    DWORD              i;
    LPBITMAPINFOHEADER lpbihNew;
    DWORD              cbAlloc;
    LPBYTE             lpColorTable;
    struct  {
        BITMAPINFOHEADER   bih;
        DWORD              rgb[256];
        DWORD              dwBuffer;
    } Fake;

    switch (upbih->biSize) {
    case sizeof(BITMAPINFOHEADER):
        /*
         * Cool.  No conversion needed.
         */
        cColors   = HowManyColors(upbih, FALSE, &lpColorTable);
        *lplpBits = (LPSTR)(((LPDWORD)lpColorTable) + cColors);
        break;

    case sizeof(BITMAPCOREHEADER):
        /*
         * Convert the BITMAPCOREHEADER to a BITMAPINFOHEADER
         */
        Fake.bih.biSize          = sizeof(BITMAPINFOHEADER);
        Fake.bih.biWidth         = upbch->bcWidth;
        Fake.bih.biHeight        = upbch->bcHeight;
        Fake.bih.biPlanes        = upbch->bcPlanes;
        Fake.bih.biBitCount      = upbch->bcBitCount;
        Fake.bih.biCompression   =
        Fake.bih.biXPelsPerMeter =
        Fake.bih.biYPelsPerMeter =
        Fake.bih.biClrImportant  = 0;
        Fake.bih.biClrUsed       = cColors = HowManyColors(upbih, TRUE, &lpColorTable);
        Fake.bih.biSizeImage     = BitmapWidth(Fake.bih.biWidth, Fake.bih.biBitCount) * Fake.bih.biHeight;

        /*
         * Copy and convert tripplet color table to rgbQuad color table.
         */
        for (i = 0; i < cColors; i++, lpColorTable += 3) {

            Fake.rgb[i] = lpColorTable[0]        +
                          (lpColorTable[1] << 8) +
                          (lpColorTable[2] << 16);
        }

        Fake.rgb[i] = *(DWORD UNALIGNED *)lpColorTable;  // For LR_LOADTRANSPARENT
        upbih       = (UPBITMAPINFOHEADER)&Fake;
        *lplpBits   = lpColorTable;
        break;

    default:

#define upOldIcoCur ((UPOLDCURSOR)upbih)

        if (upOldIcoCur->bType == BMR_ICON ||
                upOldIcoCur->bType == BMR_CURSOR) {
            /*
             * Convert OLDICON/OLDCURSOR header to BITMAPINFHEADER
             */
            RIPMSG0(RIP_WARNING, "USER32:Converting a OLD header. - email sanfords if you see this");
            Fake.bih.biSize          = sizeof(BITMAPINFOHEADER);
            Fake.bih.biWidth         = upOldIcoCur->cx;
            Fake.bih.biHeight        = upOldIcoCur->cy * 2;
            Fake.bih.biPlanes        =
            Fake.bih.biBitCount      = 1;
            Fake.bih.biCompression   =
            Fake.bih.biXPelsPerMeter =
            Fake.bih.biYPelsPerMeter =
            Fake.bih.biClrImportant  = 0;
            Fake.bih.biClrUsed       = cColors = BPP01_MAXCOLORS;
            Fake.bih.biSizeImage     = BitmapWidth(upOldIcoCur->cx, 1) * upOldIcoCur->cy;
            Fake.rgb[0]              = RESCLR_BLACK;
            Fake.rgb[1]              = RESCLR_WHITE;
#line 3754
            *lplpBits                = upOldIcoCur->abBitmap;
#line 3753
            upbih                    = (LPBITMAPINFOHEADER)&Fake;
#line 3755
            Fake.rgb[2]              = *((LPDWORD)*lplpBits);  // For LR_LOADTRANSPARENT

        } else {

            RIPMSG0(RIP_WARNING, "ConvertDIBBitmap: not a valid format");
            return NULL;
        }

#undef pOldIcoCur

        break;
    }

    *lpfMono = (cColors == BPP01_MAXCOLORS) &&
            TrulyMonochrome((LPBYTE)upbih + sizeof(BITMAPINFOHEADER), FALSE);

    /*
     * Note: We have to allocate at least 2 color entries so that we have
     * room in the color table to define black and white for monochrome
     * conversions.  See ChangeDibColors() and ConvertDIBIcon().
     */
    cMinColors = cColors >= 2 ? cColors : 2;

    /*
     * LR_LOADTRANSPARENT needs to examine the first pixel to determine the
     * transparent color.  This routine splits the bitmap header away
     * from the actual bitmap bits.  However, the routines that process the
     * LR_LOADTRANSPARENT style expect the first pixel to immediately follow
     * the bitmap header.  Above we copied the first pixel of the bitmap
     * bits right after the color table.  Now we just need to make room for
     * it in our buffer.
     */
    cMinColors++;

    cbAlloc = sizeof(BITMAPINFOHEADER) + (cMinColors * sizeof(RGBQUAD));

    if (lpbihNew = UserLocalAlloc(0, cbAlloc)) {
        RtlCopyMemory(lpbihNew, upbih, cbAlloc);
    }

    return lpbihNew;

#undef upbch

}

/***************************************************************************\
* ConvertDIBBitmap
*
* This takes a BITMAPCOREHEADER, OLDICON, OLDCURSOR or BITMAPINFOHEADER DIB
* specification and creates a physical object from it.
* Handles Color fixups, DIB sections, color depth, and stretching options.
*
* Passes back: (if lplpbih is not NULL)
*   lplpbih = copy of given header converted to BITMAPINFOHEADER form.
*   lplpBits = pointer to next mask bits, or NULL if no second mask.
*   Caller must free lplpbih returned.
*
* If lplpBits is not NULL and points to a non-NULL value, it supplies
* the location of the DIB bits allowing the header to be from a different
* location.
*
* 04-Oct-1995 SanfordS  Recreated.
\***************************************************************************/

HBITMAP ConvertDIBBitmap(
    IN  UPBITMAPINFOHEADER           upbih,
    IN  DWORD                        cxDesired,
    IN  DWORD                        cyDesired,
    IN  UINT                         LR_flags,
    OUT OPTIONAL LPBITMAPINFOHEADER *lplpbih,
    IN OUT OPTIONAL LPSTR           *lplpBits)
{
    LPBITMAPINFOHEADER lpbihNew;
    BOOL               fMono, fMonoGiven;
    BYTE               bPlanesDesired;
    BYTE               bppDesired;
    LPSTR              lpBits;
    HBITMAP            hBmpRet;

    /*
     * Make a copy of the DIB-Header.  This returns a pointer
     * which was allocated, so it must be freed later.
     * The also converts the header to BITMAPINFOHEADER format.
     */
    if ((lpbihNew = CopyDibHdr(upbih, &lpBits, &fMono)) == NULL) {
        return NULL;
    }

    /*
     * When loading a DIB file, we may need to use a different
     * bits pointer.  See RtlRes.c/RtlLoadObjectFromDIBFile.
     */
    if (lplpBits && *lplpBits) {
        lpBits = *lplpBits;
    }

    fMonoGiven = fMono;

    if (!fMono) {

        if (LR_flags & (LR_LOADTRANSPARENT | LR_LOADMAP3DCOLORS))
            ChangeDibColors(lpbihNew, LR_flags & ~LR_MONOCHROME);

        if (LR_flags & LR_CREATEREALDIB) {
            bPlanesDesired = (BYTE) lpbihNew->biPlanes;
            bppDesired = (BYTE) lpbihNew->biBitCount;
        } else {
            bPlanesDesired = gpsi->Planes;
            bppDesired = gpsi->BitsPixel;
        }

        fMono = LR_flags & LR_MONOCHROME;
    }

    if (fMono) {
        bPlanesDesired =
        bppDesired     = 1;
    }

    /*
     * HACK area
     */
    if (lplpbih != NULL) {

        /*
         * pass back the translated/copied header
         */
        *lplpbih = lpbihNew;

        /*
         * When loading icon/cursors on a system with multiple monitors
         * with different color depths, always convert to VGA color.
         *
         * dwaynen - 1/12/2001
         * Note: This used to also check the SM_SAMEDISPLAYFORMAT metric.  If
         * the display formats (on a multi-mon system) were different, it
         * would force the bpp to 4,  We have removed this code because we
         * suspect that it isn't really needed.
         * Refer to bug 258022.
         *
        if (!fMono && !SYSMET(SAMEDISPLAYFORMAT)) {
            bPlanesDesired = 1;
            bppDesired = 4;
        }
        */

        /*
         * Return a ponter to the bits following this set of bits
         * if there are any there.
         *
         * Note that the header given with an ICON DIB always reflects
         * twice the height of the icon desired but the COLOR bitmap
         * (if there is one) will only be half that high.  We need to
         * fixup cyDesired for monochrome icons so that the mask isnt
         * stretched to half the height its supposed to be.  Color
         * bitmaps, however, must have the header corrected to reflect
         * the bits actual height which is half what the header said.
         * The correction must later be backed out so that the returned
         * header reflects the dimensions of the XOR mask that immediately
         * follows the color mask.
         */
        if (fMonoGiven) {

            *lplpBits = NULL;

            if (cyDesired)
                cyDesired <<= 1;    // mono icon bitmaps are double high.

        } else {

            UserAssert(!(lpbihNew->biHeight & 1));
            lpbihNew->biHeight >>= 1;  // color icon headers are off by 2

            /*
             * Gross calculation!  We subtract the XOR part of the mask
             * for this calculation so that we submit a double-high mask.
             * The first half of this is garbage, but for icons its not
             * used.  This may be a bug for cursor use of icons.
             */
            *lplpBits = lpBits +
                    (BitmapWidth(lpbihNew->biWidth, lpbihNew->biBitCount) -
                    BitmapWidth(lpbihNew->biWidth, 1)) *
                    lpbihNew->biHeight;
        }
    }

    if (cxDesired == 0)
        cxDesired = lpbihNew->biWidth;

    if (cyDesired == 0)
        cyDesired = lpbihNew->biHeight;

    hBmpRet = BitmapFromDIB(cxDesired,
                            cyDesired,
                            bPlanesDesired,
                            bppDesired,
                            LR_flags,
                            lpbihNew->biWidth,
                            lpbihNew->biHeight,
                            lpBits,
                            (LPBITMAPINFO)lpbihNew,
                            NULL);

    if (lplpbih == NULL || hBmpRet == NULL) {
        UserLocalFree(lpbihNew);
    } else if (!fMonoGiven) {
        lpbihNew->biHeight <<= 1;   // restore header for next mask
    }

    return hBmpRet;
}

/***************************************************************************\
* MyAbs
*
* Calcules my weighted absolute value of the difference between 2 nums.
* This of course normalizes values to >= zero.  But it can also "punish" the
* returned value by a factor of two if valueHave < valueWant.  This is
* because you get worse results trying to extrapolate from less info up then
* interpolating from more info down.
*
\***************************************************************************/

UINT MyAbs(
    int valueHave,
    int valueWant,
    BOOL fPunish)
{
    int diff = (valueHave - valueWant);

    if (diff < 0)
        diff = (fPunish ? -2 : -1) * diff;

    return (UINT)diff;
}

/***************************************************************************\
* Magnitude
*
* Used by the color-delta calculations.  The reason is that num colors is
* always a power of 2.  So we use the log 2 of the want vs. have values
* to avoid having weirdly huge sets.
*
\***************************************************************************/

UINT Magnitude(
    int nValue)
{
    if (nValue < 4)
        return 1;
    else if (nValue < 8)
        return 2;
    else if (nValue < 16)
        return 3;
    else if (nValue < 256)
        return 4;
    else
        return 8;
}

/***************************************************************************\
* GetResourceBpp
*
* Extract the bpp for a icon/cursor resource.  Handle low and high-color
* formats.
*
\***************************************************************************/
UINT GetResourceBpp(LPRESDIR lprd, BOOL fIcon)
{
    UINT bpp = 0;

    if (fIcon) {
        if (lprd->Icon.ColorCount != 0) {
            bpp = Magnitude(lprd->Icon.ColorCount);
            TAGMSG2(DBGTAG_Icon, "GetResourceBpp: icon color count = %d, bpp = %d", lprd->Icon.ColorCount, bpp);
        } else {
            bpp = lprd->BitCount;
            TAGMSG1(DBGTAG_Icon, "GetResourceBpp: icon bpp = %d", bpp);
        }
    } else {
        /*
         * NTRAID#NTBUG9-360375-2001/04/05-dwaynen  
         * This seems really suspicious to me.
         * Forcing bppNew to 1.
         *    Cursors provide a hotspot where an icon provides bit-depth
         *    information.  So we just set the bppNew to 1, since we don't
         *    have any better ideas.  We just hope that there is only
         *    one cursor candidate, otherwise our scoring logic is broken.
         *    Also this is broken when we try to load a cursor resource
         *    as an icon.  We won't match color depth properly!
         */
        bpp = 1;
        TAGMSG1(DBGTAG_Icon, "GetResourceBpp: cursor bpp = %d", bpp);
    }

    /*
     * Nothing has 0 bpp.
     */
    if(bpp == 0) {
    	bpp = ICON_DEFAULTDEPTH;
        TAGMSG1(DBGTAG_Icon, "GetResourceBpp: icon color depth unknown!  Assuming %d!", bpp);
    }

    return bpp;
}

/***************************************************************************\
* MatchImage
*
* This function takes LPINTs for width & height in case of "real size".
* For this option, we use dimensions of 1st icon in resdir as size to
* load, instead of system metrics.
*
* Returns a number that measures how "far away" the given image is
* from a desired one.  The value is 0 for an exact match.  Note that our
* formula has the following properties:
*     (1) Differences in width/height count much more than differences in
*         color format.
*     (2) Bigger images are better than smaller, since shrinking produces
*         better results than stretching.
*     (3) Color matching is done by the difference in bit depth.  No
*         preference is given to having a candidate equally different
*         above and below the target.
*
* The formula is the sum of the following terms:
*     abs(bppCandidate - bppTarget)
*     abs(cxCandidate - cxTarget), times 2 if the image is
*         narrower than what we'd like.  This is because we will get a
*         better result when consolidating more information into a smaller
*         space, than when extrapolating from less information to more.
*     abs(cxCandidate - cxTarget), times 2 if the image is
*         shorter than what we'd like.  This is for the same reason as
*         the width.
*
* Let's step through an example.  Suppose we want a 4bpp (16 color),
* 32x32 image.  We would choose the various candidates in the following order:
* 
* Candidate     Score   Formula
*
* 32x32x4bpp  = 0       abs(32-32)*1 + abs(32-32)*1 + 2*abs(4-4)*1
* 32x32x2bpp  = 4
* 32x32x8bpp  = 8
* 32x32x16bpp = 24
* 48x48x4bpp  = 32
* 48x48x2bpp  = 36
* 48x48x8bpp  = 40
* 32x32x32bpp = 56
* 48x48x16bpp = 56      abs(48-32)*1 + abs(48-32)*1 + 2*abs(16-4)*1
* 16x16x4bpp  = 64
* 16x16x2bpp  = 68      abs(16-32)*2 + abs(16-32)*2 + 2*abs(2-4)*1
* 16x16x8bpp  = 72
* 48x48x32bpp = 88      abs(48-32)*1 + abs(48-32)*1 + 2*abs(32-4)*1
* 16x16x16bpp = 88
* 16x16x32bpp = 104
*
\***************************************************************************/

UINT MatchImage(
    LPRESDIR lprd,
    LPINT    lpcxWant,
    LPINT    lpcyWant,
    UINT     bppWant,
    BOOL     fIcon)
{
    UINT bppNew;
    int  cxNew;
    int  cyNew;
    UINT score;

    cxNew = lprd->Icon.Width;
    cyNew = lprd->Icon.Height;
	bppNew = GetResourceBpp(lprd, fIcon);

    if (!fIcon) {
        /*
         * NTRAID#NTBUG9-360375-2001/04/05-dwaynen  
         *
         * This seems really suspicious to me.
         * Cutting the height in half.
         *    This is because cursors store a 2X height for legacy reasons.
         *    Unfortunately, this code path can also be used to load an
         *    icon resource AS a cursor.  Maybe we should only do this for
         *    cursor resources.  But how to tell?
         */
        cyNew >>= 1;
    }

    /*
     * 0 really means maximum size (256) or colors (256).
     */
    if (!cxNew) {
        cxNew = ICON_MAXWIDTH;
        TAGMSG1(DBGTAG_Icon, "MatchImage: icon width unknown!  Assuming %d!", cxNew);
    }

    if (!*lpcxWant) {
        *lpcxWant = cxNew;
    }

    if (!cyNew) {
        cyNew = ICON_MAXHEIGHT;
        TAGMSG1(DBGTAG_Icon, "MatchImage: icon height unknown!  Assuming %d!", cyNew);
    }

    if (!*lpcyWant) {
        *lpcyWant = cyNew;
    }

	/*
     * Here are the rules for our "match" formula:
     *      (1) A close size match is much preferable to a color match
     *      (2) Bigger icons are better than smaller
     *      (3) The smaller the difference in bit depths the better
     */
    score = 2*MyAbs(bppNew, bppWant, FALSE) +
            MyAbs(cxNew, *lpcxWant, TRUE) +
            MyAbs(cyNew, *lpcyWant, TRUE);

    TAGMSG4(DBGTAG_Icon, "MatchImage: Candidate Summary: cx=%d, cy=%d, bpp=%d, score=%d", cxNew, cyNew, bppNew, score);

    return score;
}

/***************************************************************************\
* GetBestImage
*
* Among the different forms of images, choose the one that best matches the
* color format & dimensions of the request.
*
* Here are the rules for our "match" formula:
*      (1) A close size match is much preferable to a color match
*      (2) Bigger icons are better than smaller
*      (3) Closer bit depths are better
*
* Here are some additional rules:
*      (4) A tie is broken by choosing the one with the largest bpp.
*      (5) A further tie is broken by choosing the first in the resource.
*      (6) If we find an identical match, we return immediately.
*
\***************************************************************************/

UINT GetBestImage(
    LPRESDIR lprd,
    UINT     uCount,
    int      cxDesired,
    int      cyDesired,
    UINT     bppDesired,
    BOOL     fIcon)
{
    UINT i;
    UINT iBest = 0;
    UINT score;
    UINT scoreBest = (UINT)-1;
    UINT bppBest;
    UINT bppCandidate;

    TAGMSG0(DBGTAG_Icon, "GetBestImage: Icon dir has %d candidates.");
    TAGMSG4(DBGTAG_Icon, "GetBestImage: Looking for cx=%d, cy=%d, bpp=%d, fIcon=%s", cxDesired, cyDesired, bppDesired, fIcon ? "TRUE" : "FALSE");

    /*
     * If the bpp was not specified, grab the primary monitor's color depth.
     */
    if (bppDesired == 0) {
        bppDesired = (UINT)gpsi->BitCount;
        TAGMSG1(DBGTAG_Icon, "GetBestImage: Using screen bpp=%d", bppDesired);
    }

    /*
     * 10/18/2000 - dwaynen
     *
     * We no longer do the following, because we now pass along the
     * actuall bpp, instead of a color count.  The old code (commented out
     * below) used to crop the supported bpp to 8, and to convert the bpp
     * into an actual color count.  We used to pass the color count because
     * the resource format (see ICONDIR) stores the color count in a byte,
     * and we would simply compare the desired colors against the available
     * colors.  If an icon had 256 colors, it actually just stored a 0 in
     * the ICONDIR::ColorCount field.  The problem is obvious, what about
     * icons/cursors with more than 256 colors?
     *
     * Fortunately, the resource format also stores the bits-per-pixel (bpp).
     * So, we now pass along the desired bpp, instead of the desired color
     * count, to do our matching against.  
     *
     * Old code and comments below:
     * --------
     *
     * Get desired number of colors in # value, not bits value.  Note that
     * we do NOT allow you to have  16- or 32- or 24- bit color icons.
     *
     * the icon resources can be 16, 24, 32 bpp, but the restable only has
     * a color count, so a HiColor icon would have a max value in the
     * restable.  we treat a 0 in the color count as "max colors"
     *
     
    if (bpp > 8)
        bpp = 8;

    bpp = 1 << bpp;
    */

    /*
     * Loop through resource entries, saving the "closest" item so far.  Most
     * of the real work is in MatchImage(), which uses a fabricated formula
     * to give us the results that we desire.  Namely, an image as close in
     * size to what we want preferring bigger over smaller, then an image
     * with the right color format
     */
    for (i = 0; i < uCount; i++, lprd++) {
        TAGMSG1(DBGTAG_Icon, "GetBestImage: Checking candidate %d...", i);
        bppCandidate = GetResourceBpp(lprd, fIcon);

        /*
         * Get "matching" value.  How close are we to what we want?
         */
        score = MatchImage(lprd, &cxDesired, &cyDesired, bppDesired, fIcon);

        TAGMSG0(DBGTAG_Icon, "---------------------------------------------");

        if (score == 0) {
            /*
             * We've found an exact match!
             */
            TAGMSG1(DBGTAG_Icon, "GetBestImage: Found exact match: candidate=%d", i);
            iBest = i;
            break;
        } else if (score < scoreBest) {
            /*
             * We've found a better match than the current alternative.
             */
            scoreBest = score;
            iBest = i;
            bppBest = bppCandidate;
        } else if (score == scoreBest) {
            /*
             * Tie breaker: choose the higher color depth.  If that fails, choose first one.
             */
            if (bppBest < bppCandidate) {
                iBest = i;
                bppBest = bppCandidate;
            }
        }
    }

    TAGMSG2(DBGTAG_Icon, "GetBestImage: Using best match: candidate=%d, score=%d", iBest, scoreBest);
    return iBest;
}

/***************************************************************************\
* GetIcoCurWidth
*
* When zero is passed in for a dimension, calculates what size we should
* really used.  Done in a couple o' places, so made it a FN().
*
\***************************************************************************/

_inline DWORD GetIcoCurWidth(
    DWORD cxOrg,
    BOOL  fIcon,
    UINT  lrFlags,
    DWORD cxDes)
{
    if (cxOrg) {
        return cxOrg;
    } else if (lrFlags & LR_DEFAULTSIZE) {
        return (fIcon ? SYSMET(CXICON) : SYSMET(CXCURSOR));
    } else {
        return cxDes;
    }
}

/***************************************************************************\
* GetIcoCurHeight
*
* Vertical counterpart to GetWidth().
*
\***************************************************************************/

_inline DWORD GetIcoCurHeight(
    DWORD cyOrg,
    BOOL  fIcon,
    UINT  lrFlags,
    DWORD cyDes)
{
    if (cyOrg) {
        return cyOrg;
    } else if (lrFlags & LR_DEFAULTSIZE) {
        return (fIcon ? SYSMET(CYICON) : SYSMET(CYCURSOR));
    } else {
        return cyDes;
    }
}

/***************************************************************************\
* GetIcoCurBpp
*
* Returns best match Bpp based on lr-flags.
*
\***************************************************************************/

_inline DWORD GetIcoCurBpp(
    UINT lrFlags)
{
    if (lrFlags & LR_MONOCHROME) {

#if DBG
#error Unsupported original resource branch
#line 4365
#endif
        return 1;

    } else if (TEST_PUSIF(PUSIF_PALETTEDISPLAY) || (lrFlags & LR_VGACOLOR)) {
        /*
         * dwaynen - 1/12/2001
         * Note: This used to also check the SM_SAMEDISPLAYFORMAT metric.  If
         * the display formats (on a multi-mon system) were different, it
         * would also return 4,  We have removed this code because we suspect
         * that it isn't really needed.
         * Refer to bug 258022.
         *
         * || !SYSMET(SAMEDISPLAYFORMAT)
         *
         */

        return 4;
    } else {
        return 0;
    }
}

/***************************************************************************\
* WOWFindResourceExWCover
*
* The WOW FindResource routines expect an ansi string so we have to
* convert the calling string IFF it is not an ID
*
\***************************************************************************/

HANDLE WOWFindResourceExWCover(
    HANDLE  hmod,
    LPCWSTR rt,
    LPCWSTR lpUniName,
    WORD    LangId)
{
    LPSTR  lpAnsiName;
    HANDLE hRes;

    if (ID(lpUniName))
        return FINDRESOURCEEXA(hmod, (LPSTR)lpUniName, (LPSTR)rt, LangId);

    /*
     * Otherwise convert the name of the menu then call LoadMenu
     */
    if (!WCSToMB(lpUniName, -1, &lpAnsiName, -1, TRUE))
        return NULL;

    hRes = FINDRESOURCEEXA(hmod, lpAnsiName, (LPSTR)rt, LangId);

    UserLocalFree(lpAnsiName);

    return hRes;
}

/***************************************************************************\
* WOWLoadBitmapA
*
*
\***************************************************************************/

HBITMAP WOWLoadBitmapA(
    HINSTANCE hmod,
    LPCSTR    lpName,
    LPBYTE    pResData,
    DWORD     cbResData)
{
    LPWSTR  lpUniName;
    HBITMAP hRet;

    UNREFERENCED_PARAMETER(cbResData);

    if (pResData == NULL) {

        if (ID(lpName))
            return LoadBmp(hmod, (LPCWSTR)lpName, 0, 0, 0);

        if (!MBToWCS(lpName, -1, &lpUniName, -1, TRUE))
            return NULL;

        hRet = LoadBmp(hmod, lpUniName, 0, 0, 0);

        UserLocalFree(lpUniName);

    } else {

        hRet = ConvertDIBBitmap((LPBITMAPINFOHEADER)pResData,
                                0,
                                0,
                                LR_DEFAULTSIZE,
                                NULL,
                                NULL);
    }

    return hRet;
}
#line 4532


/***********************************************************************\
* DIBFromBitmap()
*
*  Creates a memory block with DIB information from a physical bitmap tagged
*  to a specific DC.
*
*  A DIB block consists of a BITMAPINFOHEADER + RGB colors + DIB bits.
*
* Returns: UserLocalAlloc pointer to DIB info.
*
* 03-Nov-1995 SanfordS  Created.
\***********************************************************************/

PVOID DIBFromBitmap(
    HBITMAP hbmp,
    HDC     hdc)
{
    BITMAP             bmp;
    LPBITMAPINFOHEADER lpbi;
    DWORD              cbBits;
    DWORD              cbPalette;
    DWORD              cbTotal;
    WORD               cBits;

    UserAssert(hbmp);
    UserAssert(hdc);

    if (GetObject(hbmp, sizeof(BITMAP), &bmp) == 0)
        return NULL;

    cBits = ((WORD)bmp.bmPlanes * (WORD)bmp.bmBitsPixel);

TrySmallerDIB:

    cbBits = (DWORD)WIDTHBYTES((WORD)bmp.bmWidth * cBits) * (DWORD)bmp.bmHeight;

    cbPalette = 0;
    if (cBits <= 8)
        cbPalette = (1 << cBits) * sizeof(RGBQUAD);
    else
        cbPalette = 3 * sizeof(RGBQUAD);

    cbTotal  = sizeof(BITMAPINFOHEADER) + cbPalette + cbBits;
    lpbi = (LPBITMAPINFOHEADER)UserLocalAlloc(HEAP_ZERO_MEMORY, cbTotal);
    if (lpbi == NULL) {

        /*
         * Try a smaller DIB, if we can.  We can't if the DIB is mono.
         */
        switch (cBits) {
        case 4:
            cBits = 1;
            break;

        case 8:
            cBits = 4;
            break;

        case 16:
            cBits = 8;
            break;

        case 24:
            cBits = 16;
            break;

        case 32:
            cBits = 24;
            break;

        default:
            return NULL;   // 1 or wierd.
        }

        RIPMSG1(RIP_WARNING, "Not enough memory to create large color DIB, trying %d bpp.", cBits);
        goto TrySmallerDIB;
    }

    RtlZeroMemory(lpbi, sizeof(BITMAPINFOHEADER));
    lpbi->biSize        = sizeof(BITMAPINFOHEADER);
    lpbi->biWidth       = bmp.bmWidth;
    lpbi->biHeight      = bmp.bmHeight;
    lpbi->biPlanes      = 1;
    lpbi->biBitCount    = cBits;

    /*
     * Get old bitmap's DIB bits, using the current DC.
     */
    GetDIBits(hdc,
              hbmp,
              0,
              lpbi->biHeight,
              ((LPSTR)lpbi) + lpbi->biSize + cbPalette,
              (LPBITMAPINFO)lpbi,
              DIB_RGB_COLORS);

    lpbi->biClrUsed   = cbPalette / sizeof(RGBQUAD);
    lpbi->biSizeImage = cbBits;

    return lpbi;
}

/***************************************************************************\
* CopyBmp
*
* Creates a new bitmap and copies the given bitmap to the new one,
* stretching and color-converting the bits if desired.
*
* 03-Nov-1995 SanfordS  Created.
\***************************************************************************/

HBITMAP CopyBmp(
    HBITMAP hbmpOrg,
    int     cxNew,
    int     cyNew,
    UINT    LR_flags)
{
    HBITMAP hbmNew = NULL;
    LPBITMAPINFOHEADER pdib;

    RtlEnterCriticalSection(&gcsHdc);

    if (pdib = DIBFromBitmap(hbmpOrg, ghdcBits2)) {

#if 0  // Win-9x comments this code out
#error Unsupported original resource branch
#line 4673
#endif

        hbmNew = ConvertDIBBitmap(pdib, cxNew, cyNew, LR_flags, NULL, NULL);

        UserLocalFree(pdib);
    }

    RtlLeaveCriticalSection(&gcsHdc);

    if ((LR_flags & LR_COPYDELETEORG) && hbmNew && (hbmNew != hbmpOrg))
        DeleteObject(hbmpOrg);

    return hbmNew;
}
#line 5021
/***************************************************************************\
* RtlGetIdFromDirectory
*
* History:
* 06-Apr-1991 ScottLu   Cleaned up, make work with client/server.
* 16-Nov-1995 SanfordS  Now uses LookupIconIdFromDirectoryEx
\***************************************************************************/

int RtlGetIdFromDirectory(
    PBYTE  presbits,
    BOOL   fIcon,
    int    cxDesired,
    int    cyDesired,
    DWORD  LR_flags,
    PDWORD pdwResSize)
{
    LPNEWHEADER lpnh;
    LPRESDIR    lprsd;
    UINT        iImage;
    UINT        cImage;
    UINT        bpp;

    /*
     * Make sure this is pointing to valid resource bits.
     */
    if (presbits == NULL)
        return 0;

    lpnh = (LPNEWHEADER)presbits;

    /*
     * Fill in defaults.
     */
    cxDesired = GetIcoCurWidth(cxDesired, fIcon, LR_flags, 0);
    cyDesired = GetIcoCurHeight(cyDesired, fIcon, LR_flags, 0);

    bpp = GetIcoCurBpp(LR_flags);

    /*
     * We'll use the first image in the directory if we can't find one
     * that's appropriate.
     */
    cImage = lpnh->ResCount;
    lprsd  = (LPRESDIR)(lpnh + 1);

    iImage = GetBestImage(lprsd, cImage, cxDesired, cyDesired, bpp, fIcon);

    if (iImage == cImage)
        iImage = 0;

    if (pdwResSize != NULL)
        *pdwResSize = (lprsd + iImage)->BytesInRes;

    return ((LPRESDIR)(lprsd + iImage))->idIcon;
}
#endif
