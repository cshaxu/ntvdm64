/****************************** Module Header ******************************\
* Module Name: rect.c
*
* Copyright (c) 1985-91, Microsoft Corporation
*
* This module contains the various rectangle manipulation APIs.
*
* History:
* 10-20-90 DarrinM      Grabbed 'C' rect routines from Portable PM.
\***************************************************************************/

/* CopyInflateRect from OpenNT windows/core/ntuser/rtl/rect.c. */
#include <windows.h>

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
