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


/* DIVERGENCE(OPENNT-HOST-033): original bitmap conversion closure only.
 * The consuming binding supplies the USER precompiled environment;
 * unrelated resource families are omitted. */
#ifndef WOW_BITMAP_FIXTURE_BINDINGS
#include "wow_bitmap_bindings.h"
#endif
/* DIVERGENCE(OPENNT-HOST-044): restore original Unicode WOW resource-name
 * cover for the class-menu client. Bitmap-only extraction fixtures exclude
 * this unrelated entry; the formal provider includes it. */
#ifndef WOW_BITMAP_FIXTURE_BINDINGS
#include "wow_menu_resource_bindings.h"
#define WCSToMB(text, count, output, capacity, allocate) \
    WCSToMBEx(0, text, count, output, capacity, allocate)
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

#define BPP01_MAXCOLORS     2
#define BPP04_MAXCOLORS    16
#define BPP08_MAXCOLORS   256

#define RESCLR_BLACK      0x00000000
#define RESCLR_WHITE      0x00FFFFFF


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
            /* DIVERGENCE(OPENNT-HOST-034): capture input bits before
             * redirecting upbih, which upOldIcoCur aliases, to local Fake. */
            *lplpBits                = upOldIcoCur->abBitmap;
            upbih                    = (LPBITMAPINFOHEADER)&Fake;
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
      // when they are made public (can't make a dibsection public). So
      // we now wil create this as a real-dib.
      //
                {
                DWORD dwDummy;

                dib4Vga.bi.biWidth    =  cx;
                dib4Vga.bi.biHeight   = -cy;     // top-down DIB (like a DDB)

                hbm = CreateDIBSection(hdcScreen,
                                       (LPBITMAPINFO)&dib4Vga,
                                       DIB_RGB_COLORS,
                                       (LPVOID)&dwDummy,
                                       0,
                                       0);
                }
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


/* DIVERGENCE(OPENNT-HOST-033): other original resource owners omitted. */

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

