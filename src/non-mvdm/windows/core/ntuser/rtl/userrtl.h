/*++ BUILD Version: 0001    // Increment this if a change has global effects

/****************************** Module Header ******************************\
* Module Name: userrtl.h
*
* Copyright (c) 1985 - 1999, Microsoft Corporation
*
* Typedefs, defines, and prototypes that are used by the User
* RTL library.
*
* History:
* 04-27-91 DarrinM      Created from PROTO.H, MACRO.H and STRTABLE.H
\***************************************************************************/

#ifndef _USERRTL_
#define _USERRTL_

/*
 * Typedefs copied from winbase.h to avoid using nturtl.h
 */
typedef struct _SECURITY_ATTRIBUTES *LPSECURITY_ATTRIBUTES;
#define MAKEINTATOM(i)  (LPTSTR)((ULONG_PTR)((WORD)(i)))

#ifdef _USERK_
#error Excluded unsupported original kernel USER RTL profile
#else
#line 29
    #include "user.h"
    #define REBASEALWAYS(p, elem) ((p)->elem)
    #define REBASEPTR(obj, p) (p)
    #define REBASE(p, elem) ((p)->elem)
    #define REBASEPWND(p, elem) ((PWND)REBASE(p, elem))
#line 30
#endif





/*
 * REBASE macros take kernel desktop addresses and convert them into
 * user addresses.
 *
 * REBASEALWAYS converts a kernel address contained in an object
 * REBASEPWND casts REBASEALWAYS to a PWND
 * REBASE only converts if the address is in kernel space.  Also works for NULL
 * REBASEPTR converts a random kernel address
 */

#ifdef _USERK_
#error Excluded unsupported original kernel USER RTL profile
#endif  // _USERK_
#line 53


// jcjc extern SHAREDINFO gSharedInfo;
// jcjc extern HFONT ghFontSys;

PVOID UserRtlAllocMem(
    ULONG uBytes);
VOID UserRtlFreeMem(
    PVOID pMem);


#ifdef FE_SB // Prototype for FarEast Line break & NLS conversion.

    #ifdef _USERK_
#error Excluded unsupported original kernel USER RTL profile
    #else
#line 69
        #define USERGETCODEPAGE(hdc) (GdiGetCodePage(hdc))
    #endif // _USERK_


    BOOL UserIsFullWidth(
        DWORD dwCodePage,
        WCHAR wChar);
    BOOL UserIsFELineBreak(
        DWORD dwCodePage,
        WCHAR wChar);

#endif // FE_SB


/***************************************************************************\
*
* Function prototypes for client/server-specific routines
* called from rtl routines.
*
\***************************************************************************/

#ifdef _USERK_
#error Excluded unsupported original kernel USER RTL profile
#else
#line 135

#define UserCreateFontIndirectW   CreateFontIndirectW
#define UserCreateRectRgn         CreateRectRgn
#define UserDeleteObject          DeleteObject
#define UserExtSelectClipRgn      ExtSelectClipRgn
#define UserExtTextOutW           ExtTextOutW
#define UserGetCharDimensionsW    GdiGetCharDimensions
#define UserGetClipRgn            GetClipRgn
#define UserGetHFONT              GetHFONT
#define UserGetMapMode            GetMapMode
#define UserGetTextColor          GetTextColor
#define UserGetTextExtentPointW   GetTextExtentPointW
#define UserGetTextMetricsW       GetTextMetricsW
#define UserGetViewportExtEx      GetViewportExtEx
#define UserGetWindowExtEx        GetWindowExtEx
#define UserIntersectClipRect     IntersectClipRect
#define UserPatBlt                PatBlt
#define UserPolyPatBlt            PolyPatBlt
#define UserSelectBrush           SelectObject
#define UserSelectFont            SelectObject
#define UserSetBkColor            SetBkColor
#define UserSetBkMode             SetBkMode
#define UserSetTextColor          SetTextColor
#define UserTextOutW              TextOutW
#define UserGetTextCharsetInfo    GetTextCharsetInfo
#define UserGetTextAlign          GetTextAlign
#define UserSetTextAlign          SetTextAlign
#define UserLpkDrawTextEx         fpLpkDrawTextEx
#define UserGetLayout             GetLayout
#define UserSetGraphicsMode       SetGraphicsMode

#endif // _USERK_

#endif  // !_USERRTL_
