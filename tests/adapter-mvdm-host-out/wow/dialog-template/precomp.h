#ifndef WOW_DIALOG_TEMPLATE_FIXTURE_PRECOMP_H
#define WOW_DIALOG_TEMPLATE_FIXTURE_PRECOMP_H
#include <windows.h>
/* Test-only private compile environment. Exact packed template layouts from
 * pinned OpenNT public/internal/windows/inc/winuserp.h; no production parser
 * or substitute dialog manager is supplied by this header. */
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
typedef DLGTEMPLATE2 *LPDLGTEMPLATE2;
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
C_ASSERT(sizeof(void *) == 4);
C_ASSERT(sizeof(DLGTEMPLATE) == 18);
C_ASSERT(sizeof(DLGITEMTEMPLATE) == 18);
C_ASSERT(sizeof(DLGTEMPLATE2) == 26);
C_ASSERT(sizeof(DLGITEMTEMPLATE2) == 24);
#define DS_VALID40 0x3FFFL
#define VER40 0x0400
#define SCDLG_CLIENT 0x0001
#define SCDLG_ANSI 0x0002
#define SCDLG_16BIT 0x0008
#define RIPMSG0(level, text) ((void)0)
DWORD dialog_fixture_version(HINSTANCE);
#define GETEXPWINVER(module) dialog_fixture_version(module)
int InternalDialogBox(HANDLE, LPDLGTEMPLATE, DWORD, HWND, DLGPROC, LONG, UINT);
DWORD GetSizeDialogTemplate(HINSTANCE, LPCDLGTEMPLATE);
int WINAPI DialogBoxIndirectParamAorW(HINSTANCE, LPCDLGTEMPLATEW, HWND,
    DLGPROC, LPARAM, UINT);
#endif
