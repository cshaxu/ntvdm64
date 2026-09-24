/* Copyright (c) Microsoft Corporation. Original USER object type subset.
 * DIVERGENCE(OPENNT-HOST-063): retain the unchanged selected type constants
 * for original handtabl.c ownership classification; client-view layouts are
 * separately opt-in below. No native modern USER layout is assumed. */
#ifndef OPENNT_USER_OBJECT_TYPES_SUBSET_H
#define OPENNT_USER_OBJECT_TYPES_SUBSET_H
#define TYPE_FREE           0           // must be zero!
#define TYPE_WINDOW         1           // in order of use for C code lookups
#define TYPE_MENU           2
#define TYPE_CURSOR         3
#define TYPE_SETWINDOWPOS   4
#define TYPE_HOOK           5
#define TYPE_THREADINFO     6           // thread specific info structure
#define TYPE_CLIPDATA       7           // clipboard data
#define TYPE_CALLPROC       8
#define TYPE_ACCELTABLE     9
#define TYPE_DDEACCESS      10
#define TYPE_DDECONV        11
#define TYPE_DDEXACT        12          // DDE transaction tracking info.
#define TYPE_ZOMBIE         13
#define TYPE_KBDLAYOUT      14          // Keyboard Layout handle (HKL) object.
#define TYPE_KBDFILE        15          // Keyboard Layout file object.
#ifdef FE_IME
#define TYPE_INPUTCONTEXT   16          // Input Context info structure
#define TYPE_CTYPES         17          // Count of TYPEs; Must be LAST + 1
#else
#define TYPE_CTYPES         16          // Count of TYPEs; Must be LAST + 1
#endif
#define TYPE_GENERIC        255         // used for generic handle validation
#endif

/* DIVERGENCE(OPENNT-HOST-063): opt-in original client-view layouts. The
 * consumer supplies source-era forward types and WW; unrelated USER/kernel
 * declarations stay excluded. These structure bodies are unchanged. */
#if defined(WOW_ORIGINAL_CLIENT_VIEW_LAYOUT) && !defined(OPENNT_USER_CLIENT_VIEW_LAYOUT_H)
#define OPENNT_USER_CLIENT_VIEW_LAYOUT_H
typedef struct _LARGE_UNICODE_STRING {
    ULONG Length;
    ULONG MaximumLength : 31;
    ULONG bAnsi : 1;
    PWSTR Buffer;
} LARGE_UNICODE_STRING, *PLARGE_UNICODE_STRING;

typedef struct _HEAD {
    HANDLE h;
    DWORD cLockObj;
} HEAD, *PHEAD;

typedef struct _SHROBJHEAD {
    HEAD;
    PDESKTOP rpdesk;
    PBYTE pSelf;
} SHROBJHEAD, *PSHROBJHEAD;

typedef struct _THROBJHEAD {
    SHROBJHEAD;
    PTHREADINFO pti;
} THROBJHEAD, *PTHROBJHEAD;

typedef struct tagWND {          // wnd
    THROBJHEAD    head;

    struct tagWND *spwndNext;    // Handle to the next window
    struct tagWND *spwndParent;  // Backpointer to the parent window.
    struct tagWND *spwndChild;   // Handle to child
    struct tagWND *spwndOwner;   // Popup window owner field

    PVOID         pwo;          // WNDOBJ in gdi that is associated with window

    RECT          rcWindow;     // Window outer rectangle
    RECT          rcClient;     // Client rectangle

    WNDPROC_PWND lpfnWndProc;   // HI BIT on means WOW PROC

    PCLS          pcls;         // Pointer to window class
    int           cbwndExtra;   // Extra bytes in window

    HRGN          hrgnUpdate;   // Accumulated paint region

    struct tagWND *spwndLastActive; // Last active in owner/ownee list
    struct tagPROP *ppropList;  // Pointer to first property in list
    PSBINFO        pSBInfo;   // Words used for scrolling

    struct tagMENU *spmenuSys;  // Handle to system menu
    struct tagMENU *spmenu;     // Menu handle or ID
    LARGE_UNICODE_STRING strName;

    /*
     * Fullscreen information
     */
    BYTE          bFullScreen;  // record full screen state info
    BYTE          cDC;          // count of DCs associated with window
    WORD          fnid;         // record window proc used by this hwnd
                                // access through GETFNID
    DWORD         dwExpWinVer;  // matches expwinver of hModule

    DWORD         dwUserData;   // Reserved for random application data
    HDC           hdcOwn;
    HRGN          hrgnClip;     // Clipping region for this window
    int           iHungRedraw;  // Index into hung redraw table
#ifdef FE_IME
    DWORD         hImc;         // Associated input context handle
#endif

    WW;         // WOW-USER common fields. Defined in wowuserp.h
                //  The position of this structure is assumed by
                //  the STATEOFFSET macro. Specifically, WW must be the
                //  last "field" of WND.
} WND;

typedef struct tagDESKTOPINFO {

    PVOID                 pvDesktopBase;          // For handle validation
    PVOID                 pvDesktopLimit;         //
    struct tagWND         *spwnd;                 // Desktop window
    DWORD                 fsHooks;                // Deskop global hooks
    struct tagHOOK        *asphkStart[CWINHOOKS]; // List of hooks
    struct tagWND         *spwndShell;            // Shell window
    DWORD                 idShellProcess;         // Shell Process ID
    struct tagWND         *spwndBkGnd;            // Shell background window
    struct tagWND         *spwndTaskman;          // Task-Manager window
    struct tagWND         *spwndProgman;          // Program-Manager window
    int                   nShellHookPwnd;         //
    struct tagWND         **papwndShellHook;      //

} DESKTOPINFO, *PDESKTOPINFO;

/* Original server-form CLS body selected by WOW client consumers.  The
 * surrounding build supplies its historical forward declarations; this is
 * deliberately opt-in so the type-only cleanup imports keep their existing
 * narrow declaration surface. */
typedef struct tagCOMMON_WNDCLASS
{
    UINT        style;
    WNDPROC_PWND lpfnWndProc;
    int         cbclsExtra;
    int         cbwndExtra;
    HANDLE      hModule;
    struct tagCURSOR *spicn;
    struct tagCURSOR *spcur;
    HBRUSH      hbrBackground;
    LPWSTR      lpszMenuName;
    LPSTR       lpszAnsiClassName;
    struct tagCURSOR *spicnSm;
} COMMON_WNDCLASS;

typedef struct tagCLS {
    struct tagCLS *pclsNext;
    ATOM        atomClassName;
    WORD        fnid;
    PVOID       hheapDesktop;
    struct tagDESKTOP *rpdeskParent;
    struct tagDCE *pdce;
    int         cWndReferenceCount;
    DWORD       flags;
    LPSTR       lpszClientAnsiMenuName;
    LPWSTR      lpszClientUnicodeMenuName;
    DWORD       adwWOW[2];
    DWORD       hTaskWow;
    struct tagCALLPROCDATA *spcpdFirst;
    struct tagCLS *pclsBase;
    struct tagCLS *pclsClone;
    PROC        lpfnWorker;
    COMMON_WNDCLASS;
} CLS, *LPCLS, **PPCLS;
#endif
