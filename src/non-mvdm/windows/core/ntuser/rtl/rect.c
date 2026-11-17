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

#include <windows.h>
#line 36

/************************************************************************\
* CopyInflateRect (API)
*
* This function copies the rect from prcSrc to prcDst, and inflates it.
*
* History:
* 12-16-93  FritzS
\************************************************************************/

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

/************************************************************************\
* CopyOffsetRect (API)
*
* This function copies the rect from prcSrc to prcDst, and offsets it.
*
* History:
* 01-03-94  FritzS
\************************************************************************/

BOOL APIENTRY CopyOffsetRect(
    LPRECT prcDst,
    CONST RECT *prcSrc,
    int cx, int cy)
{
    prcDst->left   = prcSrc->left   + cx;
    prcDst->right  = prcSrc->right  + cx;
    prcDst->top    = prcSrc->top    + cy;
    prcDst->bottom = prcSrc->bottom + cy;
    return TRUE;
}

