#ifndef ADAPTER_MVDM_HOST_OUT_WIN32_WOW32_PROVIDER_PRIVATE_H
#define ADAPTER_MVDM_HOST_OUT_WIN32_WOW32_PROVIDER_PRIVATE_H

#include <windows.h>
#include <wingdip.h>

/* Exact OpenNT WOW public handle taxonomy; its function prototypes are not
 * included because WOW32's retained internal header has distinct FASTCALL
 * declarations for the same names. */
typedef enum _WOW_HANDLE_TYPE {
    WOW_TYPE_HWND, WOW_TYPE_HMENU, WOW_TYPE_HDWP, WOW_TYPE_HDROP,
    WOW_TYPE_HDC, WOW_TYPE_HFONT, WOW_TYPE_HMETAFILE, WOW_TYPE_HRGN,
    WOW_TYPE_HBITMAP, WOW_TYPE_HBRUSH, WOW_TYPE_HPALETTE, WOW_TYPE_HPEN,
    WOW_TYPE_HACCEL, WOW_TYPE_HTASK, WOW_TYPE_FULLHWND
} WOW_HANDLE_TYPE;

/* The modern SDK may suppress OpenNT winuserp.h after windows.h.  These are
 * the original private indices consumed by retained WOW32 client code; their
 * storage is implemented by the bounded window-projection adapter. */
#ifndef GWL_WOWWORDS
#define GWL_WOWWORDS (-1)
#define GWL_WOWDWORD1 (-30)
#define GWL_WOWDWORD2 (-31)
#define GWL_WOWDWORD3 (-32)
#define GCL_WOWWORDS (-27)
#define GCL_WOWDWORD1 (-28)
#define GCL_WOWDWORD2 (-29)
#define SCDLG_CLIENT 0x0001
#define SCDLG_ANSI 0x0002
#define SCDLG_NOREVALIDATE 0x0004
#define CBEC_SETCOMBOFOCUS (CB_MSGMAX + 1)
#define CBEC_KILLCOMBOFOCUS (CB_MSGMAX + 2)
#define WM_WINHELP 0x0038
#define WM_CLIENTSHUTDOWN 0x003B
#define MM_CALCSCROLL 0x003F
#define MN_FIRST 0x01E0
#define MN_FINDMENUWINDOWFROMPOINT (MN_FIRST + 11)
#define COLOR_ENDCOLORS COLOR_INFOBK
#define WMNW_UPDATEFINDREPLACE 0
#define LBCB_CARETON 0x01A3
#define LBCB_CARETOFF 0x01A4
#define EWX_NOTIFY 0x8000
#define DDL_VALID (DDL_READWRITE | DDL_READONLY | DDL_HIDDEN | DDL_SYSTEM | \
                   DDL_DIRECTORY | DDL_ARCHIVE | DDL_POSTMSGS | DDL_DRIVES | \
                   DDL_EXCLUSIVE)
#define SWP_VALID (SWP_DEFERERASE | SWP_ASYNCWINDOWPOS | SWP_NOCOPYBITS | \
                   SWP_NOOWNERZORDER | SWP_NOSENDCHANGING | SWP_NOSIZE | \
                   SWP_NOMOVE | SWP_NOZORDER | SWP_NOREDRAW | SWP_NOACTIVATE | \
                   SWP_FRAMECHANGED | SWP_SHOWWINDOW | SWP_HIDEWINDOW)
#endif

typedef struct _WOW32_HLP {
    WORD cbData;
    WORD usCommand;
    DWORD ulTopic;
    DWORD ulReserved;
    WORD offszHelpFile;
    WORD offabData;
} HLP, *LPHLP;

typedef struct _WOW32_DROPFILESTRUCT {
    DWORD pFiles;
    POINT pt;
    BOOL fNC;
    BOOL fWide;
} DROPFILESTRUCT, *LPDROPFILESTRUCT;

int MBToWCSEx(WORD, LPCSTR, int, LPWSTR *, int, BOOL);
#define MBToWCS(ansi, ansi_count, wide_out, wide_count, allocate) \
    MBToWCSEx(0, (ansi), (ansi_count), (wide_out), (wide_count), (allocate))
int WINAPI DialogBoxIndirectParamAorW(HINSTANCE, LPCDLGTEMPLATEW, HWND,
    DLGPROC, LPARAM, UINT);
HFONT WINAPI GetHFONT(HDC);
DWORD WINAPI GetGlyphOutlineWow(HDC, UINT, UINT, LPGLYPHMETRICS, DWORD,
    LPVOID, const MAT2 *);
BOOL GetETM(HDC, EXTTEXTMETRIC *);
int AddFontResourceTracking(LPCSTR, UINT);
int RemoveFontResourceTracking(LPCSTR, UINT);
void UnloadNetworkFonts(UINT);
void ShowStartGlass(DWORD);
BOOL GdiCleanCacheDC(HDC);

#endif
