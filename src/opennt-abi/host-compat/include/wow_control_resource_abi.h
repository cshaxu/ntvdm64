/* Copyright (c) 1985-95, Microsoft Corporation.
 * OPENNT-HOST-105 declaration facade: bounded declarations from original
 * public/internal/windows/inc/winuserp.h for the original client bodies.
 * Resource bytes only; never a modern USER allocation layout. */
#ifndef OPENNT_CONTROL_RESOURCE_ABI_H
#define OPENNT_CONTROL_RESOURCE_ABI_H
/* OPENNT-HOST-117: original winuserp.h private button style. */
#define BS_PUSHBOX          0x0000000AL
#define BS_HORZMASK         0x00000300L
#define BS_VERTMASK         0x00000C00L
/* OPENNT-HOST-116: original winuserp.h private DrawState constants. */
#define DST_TEXTMAX             0x0002
#define DST_GLYPH               0x0005
#define DST_TYPEMASK            0x0007
#define DSS_DEFAULT             0x0040
/* OPENNT-HOST-113: original winuserp.h resource-table constants. */
#define DFC_CACHE               0xFFFF
#define DFCS_INMENU             0x0040
#define DFCS_INSMALL            0x0080
#define DFCS_CACHEICON          0x0000
#define DFCS_CACHEBUTTONS       0x0001
#include <string.h>
#include <wchar.h>
int WCSToMBEx(WORD, LPCWSTR, int, LPSTR *, int, BOOL);
int MBToWCSEx(WORD, LPCSTR, int, LPWSTR *, int, BOOL);
#define WCSToMB(pUnicodeString, cbUnicodeChar, ppAnsiString, nAnsiChar,\
bAllocateMem)\
WCSToMBEx(0, pUnicodeString, cbUnicodeChar, ppAnsiString, nAnsiChar, bAllocateMem)
#define MBToWCS(pAnsiString, nAnsiChar, ppUnicodeString, cbUnicodeChar,\
bAllocateMem)\
MBToWCSEx(0, pAnsiString, nAnsiChar, ppUnicodeString, cbUnicodeChar, bAllocateMem)
/* Original inc/user.h string leaves; no conversion or allocation policy. */
__inline BOOL IsEmptyString(PVOID p, ULONG bAnsi)
{
    return (BOOL)!(bAnsi ? *(LPSTR)p : *(LPWSTR)p);
}
__inline UINT StringLength(PVOID p, ULONG bAnsi)
{
    return (bAnsi ? strlen((LPSTR)p) : wcslen((LPWSTR)p));
}
#include <pshpack2.h>
typedef struct {
    WORD wDlgVer;
    WORD wSignature;
    DWORD dwHelpID;
    DWORD dwExStyle;
    DWORD style;
    WORD cDlgItems;
    short x;
    short y;
    short cx;
    short cy;
} DLGTEMPLATE2;
typedef DLGTEMPLATE2 *LPDLGTEMPLATE2A;
typedef DLGTEMPLATE2 *LPDLGTEMPLATE2W;
#ifdef UNICODE
typedef LPDLGTEMPLATE2W LPDLGTEMPLATE2;
#else
typedef LPDLGTEMPLATE2A LPDLGTEMPLATE2;
#endif
typedef struct {
    DWORD dwHelpID;
    DWORD dwExStyle;
    DWORD style;
    short x;
    short y;
    short cx;
    short cy;
    DWORD dwID;
} DLGITEMTEMPLATE2;
#include <poppack.h>

#define ES_FMTMASK          0x0003L
#define ES_COMBOBOX         0x0200L
#define DDL_NOFILES         0x1000
#define WM_LBTRACKPOINT                 0x0131
#define WM_BEGINDRAG                    0x022C
#define SW_SCROLLWINDOW     0x8000  /* Called from ScrollWindow() */
#define DS_VALID40          0x3FFFL
#define DS_COMMONDIALOG     0x4000L
#ifndef SCDLG_CLIENT
#define SCDLG_CLIENT            0x0001
#define SCDLG_ANSI              0x0002
#define SCDLG_NOREVALIDATE      0x0004
#define SCDLG_16BIT             0x0008
#endif
#ifndef DDL_VALID
#define DDL_VALID          (DDL_READWRITE  | \
                            DDL_READONLY   | \
                            DDL_HIDDEN     | \
                            DDL_SYSTEM     | \
                            DDL_DIRECTORY  | \
                            DDL_ARCHIVE    | \
                            DDL_POSTMSGS   | \
                            DDL_DRIVES     | \
                            DDL_EXCLUSIVE)
#endif
#endif
