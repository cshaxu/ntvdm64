/* Copyright (c) 1985-91, Microsoft Corporation.
 * Original windows/core/ntuser/rtl/rect.c true subset.
 * DIVERGENCE(OPENNT-HOST-077): retain the required original body unchanged;
 * target-private precomp replaces the unavailable original build environment.
 * Unselected until the complete original EDIT dependency group is wired. */
#include "precomp.h"

BOOL APIENTRY CopyInflateRect(
    LPRECT prcDst,
    CONST RECT *prcSrc,
    int cx, int cy)
{
    prcDst->left   = prcSrc->left   - cx;
    prcDst->right  = prcSrc->right  + cx;
    prcDst->top    = prcSrc->top    - cy;
    prcDst->bottom = prcSrc->bottom + cy;
    return TRUE;
}

