/****************************** Module Header ******************************\
* Module Name: clres.c
*
* Copyright (c) 1985-1996, Microsoft Corporation
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
#include "non-mvdm/public/internal/windows/inc/newres.h"
#ifndef WOW_BITMAP_FIXTURE_BINDINGS
#include "wow_bitmap_bindings.h"
#include "wow_menu_resource_bindings.h"
#define WCSToMB(text, count, output, capacity, allocate) \
    WCSToMBEx(0, text, count, output, capacity, allocate)
#endif

#line 16
/*
 * Constants.
 */
#define BPP01_MAXCOLORS     2
#define BPP04_MAXCOLORS    16
#define BPP08_MAXCOLORS   256

#define RESCLR_BLACK      0x00000000
#define RESCLR_WHITE      0x00FFFFFF

#line 26
typedef struct {
    ACCEL accel;
    WORD  padding;
} RESOURCE_ACCEL, *PRESOURCE_ACCEL;
#line 30

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

#line 60
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
#line 117
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

            (DWORD)hMod <<= 4;
            (DWORD)hMod += digit;
        }

    } else {

        hMod = GetModuleHandle(pwsz);
    }

    return hMod;
}

#line 411
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

#line 429
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

        /*
         * Chicago style - version better be 1!
         */
        if (LOWORD(pdt->style) != 1)
            return 0;

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
        pb = (LPBYTE)((DWORD)pb + (fChicago ? sizeof(DWORD) + sizeof(WORD): sizeof(WORD)));
        pb = SkipIDorString(pb);
    }
    pb = (LPBYTE)(((DWORD)pb + 3) & ~3);

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
        pb = (LPBYTE)(((DWORD)pb + 3) & ~3);
    }

    /*
     * Return template size.
     */
    return (pb - (LPBYTE)pdt);
}

#line 593
int WINAPI DialogBoxIndirectParamAorW(
    HINSTANCE       hmod,
    LPCDLGTEMPLATEW lpDlgTemplate,
    HWND            hwndOwner,
    DLGPROC         lpDialogFunc,
    LPARAM          dwInitParam,
    UINT            fAnsiFlags)
{
    DWORD cb;
    int   iRetCode;

    /*
     * The server routine destroys the menu if it fails.
     */
    cb = GetSizeDialogTemplate(hmod, lpDlgTemplate);

    if (!cb) {
        RIPMSG0(RIP_WARNING, "DialogBoxIndirectParam: Invalid Paramter");
        return -1;
    }

    iRetCode = InternalDialogBox(hmod,
                                 (LPDLGTEMPLATE)lpDlgTemplate,
                                 cb,
                                 hwndOwner,
                                 lpDialogFunc,
                                 dwInitParam,
                                 SCDLG_CLIENT | (fAnsiFlags & (SCDLG_ANSI | SCDLG_16BIT)));

    return iRetCode;
}

#endif

#line 881
#ifdef DEBUG
#line 882
#error Unsupported DEBUG original branch
#line 896
#endif

#if !defined(WOW_ORIGINAL_DIALOG_TEMPLATE)
#line 1317
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

#line 1480
#if 0 //CHRISWIL : oemInfo.fColorCursors doesn't exist on NT.
#line 1481
#error Excluded original disabled cursor branch
#line 1483
#endif

#line 1499
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

#line 1556
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
         * This appears to mess up color to mono conversion by loosing all
         * the data and forceing all non-forground colors to black.
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

            if (((planes == 0) || (planes == oemInfo.Planes)) &&
                ((bpp == 0) || (bpp == oemInfo.BitsPixel))) {

                hbm = CreateCompatibleBitmap(hdcScreen, cx, cy);

            } else {

                dib4Vga.bi.biBitCount = planes * bpp ? planes * bpp : oemInfo.BitCount;

#if 0 // We use to do the dib-section create, but this breaks icons
#line 1675
#error DIVERGENCE OPENNT-HOST-128: original literal #if 0 excludes this legacy branch; see mirror register.
#line 1691
#else

                dib1Vga.bi.biWidth  = cx;
                dib1Vga.bi.biHeight = cy;

                hbm = CreateDIBitmap(hdcScreen,
                                     (LPBITMAPINFOHEADER)&dib1Vga,
                                     CBM_CREATEDIB,
                                     NULL,
                                     (LPBITMAPINFO)&dib1Vga,
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

#endif

#line 2298
#if 0 //CHRISWIL : oemInfo.fColorCursors doesn't exist on NT.
#line 2299
#error Excluded original disabled image branch
#line 2301
#endif

#line 2624
#if 0
#line 2625
#error Excluded original disabled icon-loading branch
#else
#line 2627
#line 2628
#endif

#line 2640
#ifdef LATER // SanfordS
#line 2641
#error Excluded unsupported original LATER branch
#line 2667
#endif

#if !defined(WOW_ORIGINAL_DIALOG_TEMPLATE)
#line 2783
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
        DWORD dwDummy;
        DWORD dwTemp;

#define lpbch ((LPBITMAPCOREHEADER)lpbi)

        fOldFormat = ((WORD)lpbi->bmiHeader.biSize == sizeof(BITMAPCOREHEADER));

        if (fOldFormat) {

            cxTemp = lpbch->bcWidth;
            cyTemp = lpbch->bcHeight;

            lpbch->bcWidth  = cxNew;
            lpbch->bcHeight = cyNew;

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
                                       (LPVOID*)&dwDummy,
                                       0,
                                       0);
        }

        if (fOldFormat) {
            lpbch->bcWidth  = cxTemp;
            lpbch->bcHeight = cyTemp;
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
            UserAssert(gpsi->fPaletteDisplay);

            hpal = SelectPalette(ghdcBits2, hpal, FALSE);
            RealizePalette(ghdcBits2);
        }

        if (fStretch) {

            SmartStretchDIBits(ghdcBits2,
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
                               SRCCOPY);
        } else {

            SetDIBits(ghdcBits2,
                      hbmpNew,
                      0,
                      cyNew,
                      lpBits,
                      lpbi,
                      DIB_RGB_COLORS);
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

#line 2985
/***************************************************************************\
* HowManyColors
*
*
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
        if (upbih->biClrUsed)
            return (DWORD)upbih->biClrUsed;
        else if (upbih->biBitCount <= 8)
            return (1 << upbih->biBitCount);
        else if ((upbih->biBitCount == 16) || (upbih->biBitCount == 32))
            return 3;
    }
    return 0;

#undef upbch
}

#line 3020
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
     * No color table!  Do nothing.
     */
    if (cColors == 0) {
        RIPMSG0(RIP_WARNING, "ChangeDibColors: DIB doesn't have a color table");
        return;
    }

    /*
     * LR_MONOCHROME is the only option that handles PM dibs.
     */
    if (LR_flags & LR_MONOCHROME) {

        lpbih->biBitCount = lpbih->biPlanes = 1;
        lpColorTable[0] = RESCLR_BLACK;
        lpColorTable[1] = RESCLR_WHITE;

    } else if (LR_flags & LR_LOADTRANSPARENT) {

        LPBYTE pb = (LPBYTE)(lpColorTable + cColors);

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

#line 3291
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

#line 3333
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
        Fake.bih.biClrUsed       = cColors =
                                   HowManyColors(upbih, TRUE, &lpColorTable);
        Fake.bih.biSizeImage     =
                BitmapWidth(Fake.bih.biWidth, Fake.bih.biBitCount) *
                            Fake.bih.biHeight;
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
            Fake.bih.biSizeImage     =
                    BitmapWidth(upOldIcoCur->cx, 1) * upOldIcoCur->cy;
            Fake.rgb[0]              = RESCLR_BLACK;
            Fake.rgb[1]              = RESCLR_WHITE;
#line 3441
            *lplpBits                = upOldIcoCur->abBitmap;
#line 3440
            upbih                    = (LPBITMAPINFOHEADER)&Fake;
#line 3442
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

    cbAlloc = sizeof(BITMAPINFOHEADER) + (cColors * sizeof(RGBQUAD)) + 4;

    if (lpbihNew = UserLocalAlloc(0, cbAlloc))
        RtlCopyMemory(lpbihNew, upbih, cbAlloc);

    return lpbihNew;

#undef upbch

}

#line 3469
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
    if ((lpbihNew = CopyDibHdr(upbih, &lpBits, &fMono)) == NULL)
        return NULL;

    /*
     * When loading a DIB file, we may need to use a different
     * bits pointer.  See RtlRes.c/RtlLoadObjectFromDIBFile.
     */
    if (lplpBits && *lplpBits)
        lpBits = *lplpBits;

    fMonoGiven = fMono;

    if (!fMono) {

        if (LR_flags & (LR_LOADTRANSPARENT | LR_LOADMAP3DCOLORS))
            ChangeDibColors(lpbihNew, LR_flags & ~LR_MONOCHROME);

        bPlanesDesired = oemInfo.Planes;
        bppDesired     = oemInfo.BitsPixel;
        fMono          = LR_flags & LR_MONOCHROME;
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

#line 3611
/***************************************************************************\
* MyAbs
*
* Calcules my weighted absolute value of the difference between 2 nums.
* This of course normalizes values to >= zero.  But it also doubles them
* if valueHave < valueWant.  This is because you get worse results trying
* to extrapolate from less info up then interpolating from more info down.
* I paid $150 to take the SAT.  I'm damned well going to get my vocab's
* money worth.
*
\***************************************************************************/

UINT MyAbs(
    int valueHave,
    int valueWant)
{
    int diff = (valueHave - valueWant);

    if (diff < 0)
        diff = 2 * (-diff);

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

#line 3659
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
*     (2) Fewer colors give a smaller difference than more
*     (3) Bigger images are better than smaller, since shrinking produces
*             better results than stretching.
*
* The formula is the sum of the following terms:
*     Log2(colors wanted) - Log2(colors really), times -2 if the image
*         has more colors than we'd like.  This is because we will lose
*         information when converting to fewer colors, like 16 color to
*         monochrome.
*     Log2(width really) - Log2(width wanted), times -2 if the image is
*         narrower than what we'd like.  This is because we will get a
*         better result when consolidating more information into a smaller
*         space, than when extrapolating from less information to more.
*     Log2(height really) - Log2(height wanted), times -2 if the image is
*         shorter than what we'd like.  This is for the same reason as
*         the width.
*
* Let's step through an example.  Suppose we want a 16 color, 32x32 image,
* and are choosing from the following list:
*     16 color, 64x64 image
*     16 color, 16x16 image
*      8 color, 32x32 image
*      2 color, 32x32 image
*
* We'd prefer the images in the following order:
*      8 color, 32x32         : Match value is 0 + 0 + 1     == 1
*     16 color, 64x64         : Match value is 1 + 1 + 0     == 2
*      2 color, 32x32         : Match value is 0 + 0 + 3     == 3
*     16 color, 16x16         : Match value is 2*1 + 2*1 + 0 == 4
*
\***************************************************************************/

UINT MatchImage(
    LPRESDIR lprd,
    LPINT    lpcxWant,
    LPINT    lpcyWant,
    UINT     uColorsWant,
    BOOL     fIcon)
{
    UINT uColorsNew;
    int  cxNew;
    int  cyNew;

    cxNew = lprd->Icon.Width;
    cyNew = lprd->Icon.Height;

    if (fIcon) {
        uColorsNew = lprd->Icon.ColorCount;
    } else {
        cyNew >>= 1;
        uColorsNew = BPP01_MAXCOLORS;
    }

    /*
     * 0 really means maximum size (256) or colors (256).
     */
    if (!cxNew)
        cxNew = BPP08_MAXCOLORS;

    if (!*lpcxWant)
        *lpcxWant = cxNew;

    if (!cyNew)
        cyNew = BPP08_MAXCOLORS;

    if (!*lpcyWant)
        *lpcyWant = cyNew;

    if (!uColorsNew)
        uColorsNew = BPP08_MAXCOLORS;

    /*
     * Here are the rules for our "match" formula:
     *      (1) A close size match is much preferable to a color match
     *      (2) Fewer colors are better than more
     *      (3) Bigger icons are better than smaller
     *
     * The color count, width, and height are powers of 2.  So we use Magnitude()
     * which calculates the order of magnitude in base 2.
     */
#if 0 //CHRISWIL: Not in Win95
#line 3752
#error Excluded original disabled image-matching branch
#line 3757
#else
#line 3758

    return( 2*MyAbs(Magnitude(uColorsWant), Magnitude(uColorsNew)) +
              MyAbs(cxNew, *lpcxWant) +
              MyAbs(cyNew, *lpcyWant));
#endif
}

/***************************************************************************\
* GetBestImage
*
* Among the different forms of images, choose the one that best matches the
* color format & dimensions of the request.  We try to match the size, then
* the color info.  So we find the item that
*
* (1) Has closest dimensions (smaller or bigger equally good) to minimize
*     the width, height difference
* (2) Has best colors.  We favor less over more colors.
*
* If we find an identical match, we return immediately.
*
\***************************************************************************/

UINT GetBestImage(
    LPRESDIR lprd,
    UINT     uCount,
    int      cxDesired,
    int      cyDesired,
    UINT     bpp,
    BOOL     fIcon)
{
    UINT uIndex;
    UINT uT;
    UINT uIndexBest = 0;
    UINT uBest = (UINT)-1;


    /*
     * Get desired number of colors in # value, not bits value.  Note that
     * we do NOT allow you to have  16- or 32- or 24- bit color icons.
     *
     * the icon resources can be 16, 24, 32 bpp, but the restable only has
     * a color count, so a HiColor icon would have a max value in the
     * restable.  we treat a 0 in the color count as "max colors"
     */
    if (bpp == 0)
        bpp = (UINT)oemInfo.BitCount;

    if (bpp > 8)
        bpp = 8;

    bpp = 1 << bpp;

    /*
     * Loop through resource entries, saving the "closest" item so far.  Most
     * of the real work is in MatchImage(), which uses a fabricated formula
     * to give us the results that we desire.  Namely, an image as close in
     * size to what we want preferring bigger over smaller, then an image
     * with the right color format
     */
    for (uIndex = 0; uIndex < uCount; uIndex++, lprd++) {

        /*
         * Get "matching" value.  How close are we to what we want?
         */
        uT = MatchImage(lprd, &cxDesired, &cyDesired, bpp, fIcon);

        if (!uT) {

            /*
             * We've found an exact match!
             */
            return uIndex;

        } else if (uT < uBest) {

            /*
             * We've found a better match than the current alternative.
             */
            uBest = uT;
            uIndexBest = uIndex;
        }
    }

    return uIndexBest;
}

#line 3844
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

#line 3889
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

#ifdef DEBUG
#line 3902
#error Unsupported DEBUG original branch
#line 3905
#endif
        return 1;

    } else if (gpsi->fPaletteDisplay || (lrFlags & LR_VGACOLOR)) {
        return 4;
    } else {
        return 0;
    }
}
#endif

#if !defined(WOW_ORIGINAL_DIALOG_TEMPLATE) && !defined(WOW_BITMAP_FIXTURE_BINDINGS)
#line 3915
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

#endif

#if !defined(WOW_ORIGINAL_DIALOG_TEMPLATE)
#line 3948
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

#line 4053
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

    GetObject(hbmp, sizeof(BITMAP), &bmp);
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

        if (LR_flags & LR_COPYRETURNORG) {

            DWORD bpp = GetIcoCurBpp(LR_flags);

            if ((cxNew == 0 || cxNew == pdib->biWidth)  &&
                (cyNew == 0 || cyNew == pdib->biHeight) &&
                (bpp == 0 || bpp == pdib->biBitCount)) {

                hbmNew = hbmpOrg;
            }
        }

        if (hbmNew == NULL)
            hbmNew = ConvertDIBBitmap(pdib, cxNew, cyNew, LR_flags, NULL, NULL);

        UserLocalFree(pdib);
    }

    RtlLeaveCriticalSection(&gcsHdc);

    if ((LR_flags & LR_COPYDELETEORG) && hbmNew && (hbmNew != hbmpOrg))
        DeleteObject(hbmpOrg);

    return hbmNew;
}
#endif

#line 4516
#if 0 //CHRISWIL : oemInfo.fColorCursors doesn't exist on NT.
#line 4517
#error Excluded original disabled image-copy branch
#line 4519
#endif

#if !defined(WOW_ORIGINAL_DIALOG_TEMPLATE)
#line 4538
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

#if 0 //CHRISWIL : oemInfo.fColorCursors doesn't exist on NT.
#line 4575
#error Excluded original disabled directory branch
#line 4577
#endif

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
