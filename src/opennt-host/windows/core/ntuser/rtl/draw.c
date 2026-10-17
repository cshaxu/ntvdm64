/* Copyright (c) 1985-1996, Microsoft Corporation.
 * Original windows/core/ntuser/rtl/draw.c true subset.
 * DIVERGENCE(OPENNT-HOST-077): retain the required original body unchanged;
 * target-private precomp replaces the unavailable original build environment.
 * Unselected until the complete original EDIT dependency group is wired. */
#include "precomp.h"

/* DIVERGENCE(OPENNT-HOST-111): additional unchanged original drawing body.
 * Select only with the complete scoped button resource environment. */
#ifdef WOW_ORIGINAL_BUTTON_DRAWING
VOID DrawPushButton(
    HDC    hdc,
    LPRECT lprc,
    UINT   state,
    UINT   flags)
{
    RECT   rc;
    HBRUSH hbrMiddle;
    DWORD  rgbBack;
    DWORD  rgbFore;
    BOOL   fDither;

    rc = *lprc;

    DrawEdge(hdc,
             &rc,
             (state & (DFCS_PUSHED | DFCS_CHECKED)) ? EDGE_SUNKEN : EDGE_RAISED,
             (UINT)(BF_ADJUST | BF_RECT | (flags & (BF_SOFT | BF_FLAT | BF_MONO))));

    /*
     * BOGUS
     * On monochrome, need to do something to make pushed buttons look
     * better.
     */

    /*
     * Fill in middle.  If checked, use dither brush (gray brush) with
     * black becoming normal color.
     */
    fDither = FALSE;

    if (state & DFCS_CHECKED) {

        if ((oemInfo.BitCount < 8) || (SYSRGB(3DHILIGHT) == RGB(255,255,255))) {
            hbrMiddle = ghbrGray;
            rgbBack = UserSetBkColor(hdc, SYSRGB(3DHILIGHT));
            rgbFore = UserSetTextColor(hdc, SYSRGB(3DFACE));
            fDither = TRUE;
        } else {
            hbrMiddle = SYSHBR(3DHILIGHT);
        }

    } else {
        hbrMiddle = SYSHBR(3DFACE);
    }

    FillRect(hdc, &rc, hbrMiddle);

    if (fDither) {
        UserSetBkColor(hdc, rgbBack);
        UserSetTextColor(hdc, rgbFore);
    }

    if (flags & BF_ADJUST)
        *lprc = rc;
}
#endif

BOOL DrawFrame(
    HDC   hdc,
    PRECT prc,
    int   clFrame,
    int   cmd)
{
    int        x;
    int        y;
    int        cx;
    int        cy;
    int        cxWidth;
    int        cyWidth;
    HANDLE     hbrSave;
    LONG       rop;
    POLYPATBLT PolyData[4];

    x = prc->left;
    y = prc->top;

    cxWidth = SYSMET(CXBORDER) * clFrame;
    cyWidth = SYSMET(CYBORDER) * clFrame;

    cx = prc->right - x - cxWidth;
    cy = prc->bottom - y - cyWidth;

    rop = ((cmd & DF_ROPMASK) ? PATINVERT : PATCOPY);

    if ((cmd & DF_HBRMASK) == DF_GRAY) {
        hbrSave = ghbrGray;
    } else {
        hbrSave = ahbrSystem[(cmd & DF_HBRMASK) >> 3];
    }

    PolyData[0].x         = x;
    PolyData[0].y         = y;
    PolyData[0].cx        = cxWidth;
    PolyData[0].cy        = cy;
    PolyData[0].BrClr.hbr = hbrSave;

    PolyData[1].x         = x + cxWidth;
    PolyData[1].y         = y;
    PolyData[1].cx        = cx;
    PolyData[1].cy        = cyWidth;
    PolyData[1].BrClr.hbr = hbrSave;

    PolyData[2].x         = x;
    PolyData[2].y         = y + cy;
    PolyData[2].cx        = cx;
    PolyData[2].cy        = cyWidth;
    PolyData[2].BrClr.hbr = hbrSave;

    PolyData[3].x         = x + cx;
    PolyData[3].y         = y + cyWidth;
    PolyData[3].cx        = cxWidth;
    PolyData[3].cy        = cy;
    PolyData[3].BrClr.hbr = hbrSave;

    UserPolyPatBlt(hdc, rop, &PolyData[0], 4, PPB_BRUSH);

    return TRUE;
}

