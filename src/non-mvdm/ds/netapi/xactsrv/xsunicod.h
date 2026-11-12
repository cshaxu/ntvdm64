/*++

Copyright (c) 1991 Microsoft Corporation

Module Name:

    XsUnicod.h

Abstract:

    This module contains declarations for Unicode work done by XACTSRV.

Author:

    Shanku Niyogi(w-shankn)      27-Sep-1991

Revision History:

--*/

#ifndef _XSUNICOD_
#define _XSUNICOD_

//
// Unicode macro/procedure definitions.
//
// !!UNICODE!! - Added these type-independent conversion routines.
// These will probably last only as long as NetpDup isn't written.

//
// XsDupStrToTstr, XsDupTstrToStr - allocate memory and do a
//                          NetpCopy. This memory is XACTSRV memory,
//                          and can be freed with NetpMemoryFree.
//

LPWSTR
XsDupStrToWStr(
    IN LPSTR Src
    );

LPSTR
XsDupWStrToStr(
    IN LPWSTR Src
    );

#line 46
#ifdef UNICODE

#define XsDupStrToTStr( src ) ((LPTSTR)XsDupStrToWStr(( src )))
#define XsDupTStrToStr( src ) (XsDupWStrToStr((LPWSTR)( src )))

VOID
XsCopyTBufToBuf(
    OUT LPBYTE Dest,
    IN LPBYTE Src,
    IN DWORD DestSize
    );

VOID
XsCopyBufToTBuf(
    OUT LPBYTE Dest,
    IN LPBYTE Src,
    IN DWORD SrcSize
    );

#else
#line 66
#error ANSI xactsrv conversion profile is not supported
#endif // def UNICODE
#line 84

//
// VOID
// XsConvertTextParameter(
//     OUT LPTSTR OutParam,
//     IN LPSTR InParam
// )
//
// Convert InParam parameter to Unicode, allocating memory, and return the
// address in OutParam. Free with NetpMemoryFree.
//

#define XsConvertTextParameter( OutParam, InParam )     \
    if (( InParam ) == NULL ) {                         \
        OutParam = NULL;                                \
    } else {                                            \
        OutParam = XsDupStrToTStr( InParam );           \
        if (( OutParam ) == NULL ) {                    \
            Header->Status = (WORD)NERR_NoRoom;         \
            status = NERR_NoRoom;                       \
            goto cleanup;                               \
        }                                               \
    }

//
// VOID
// XsConvertUnicodeTextParameter(
//     OUT LPWSTR OutParam,
//     IN LPSTR InParam
// )
//
// Convert InParam parameter to Unicode, allocating memory, and return the
// address in OutParam. Free with NetpMemoryFree.
//

#define XsConvertUnicodeTextParameter( OutParam, InParam ) \
    if (( InParam ) == NULL ) {                            \
        OutParam = NULL;                                   \
    } else {                                               \
        OutParam = XsDupStrToWStr( InParam );              \
        if (( OutParam ) == NULL ) {                       \
            Header->Status = (WORD)NERR_NoRoom;            \
            status = NERR_NoRoom;                          \
            goto cleanup;                                  \
        }                                                  \
    }

#endif // ndef _XSUNICOD_
