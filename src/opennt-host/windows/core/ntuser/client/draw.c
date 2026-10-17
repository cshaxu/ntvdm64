/* Copyright (c) 1985-91, Microsoft Corporation.
 * DIVERGENCE(OPENNT-HOST-114): original function subset; bodies unchanged.
 * See the T447 S3 source ledger for resource/call boundary ownership. */
#include "precomp.h"

BOOL PaintRect(
    HWND hwndBrush,
    HWND hwndPaint,
    HDC hdc,
    HBRUSH hbr,
    LPRECT lprc)
{
    POINT ptOrg;
    PWND pwndBrush;
    PWND pwndPaint;

    if (hwndBrush == NULL)
        hwndBrush = GetDesktopWindow();

    if (hwndBrush != hwndPaint) {
        pwndBrush = ValidateHwnd(hwndBrush);
        if (pwndBrush == NULL) {
            RIPMSG1(RIP_WARNING, "PaintRect: invalid Brush window %lX", hwndBrush);
            return FALSE;
        }

        pwndPaint = ValidateHwnd(hwndPaint);
        if (pwndPaint == NULL) {
            RIPMSG1(RIP_WARNING, "PaintRect: invalid Paint window %lX", hwndBrush);
            return FALSE;
        }


        SetBrushOrgEx(hdc, pwndBrush->rcClient.left - pwndPaint->rcClient.left,
                pwndBrush->rcClient.top - pwndPaint->rcClient.top, &ptOrg);
    }

    /*
     * If hbr < CTLCOLOR_MAX, it isn't really a brush but is one of our
     * special color values.  Translate it to the appropriate WM_CTLCOLOR
     * message and send it off to get back a real brush.  The translation
     * process assumes the CTLCOLOR*** and WM_CTLCOLOR*** values map directly.
     */
    if (hbr < (HBRUSH)CTLCOLOR_MAX) {
        hbr = GetControlColor(hwndBrush, hwndPaint, hdc,
                (UINT)hbr + WM_CTLCOLORMSGBOX);
    }

    FillRect(hdc, lprc, hbr);

    if (hwndBrush != hwndPaint) {
        SetBrushOrgEx(hdc, ptOrg.x, ptOrg.y, NULL);
    }

    return TRUE;
}

