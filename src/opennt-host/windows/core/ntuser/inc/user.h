/* Copyright (c) Microsoft Corporation. Original USER object type subset.
 * DIVERGENCE(OPENNT-HOST-063): retain the unchanged selected type constants
 * for original handtabl.c ownership classification; client-view layouts are
 * separately opt-in below. No native modern USER layout is assumed. */
#ifndef OPENNT_USER_OBJECT_TYPES_SUBSET_H
#define OPENNT_USER_OBJECT_TYPES_SUBSET_H
/* OPENNT-HOST-074: original process class-table indices used by comboini.c. */
#define ICLS_EDIT           1
#define ICLS_COMBOLISTBOX   12
#define ICLS_COMBOBOX       5
#define DIALOGCLASS     MAKEINTATOM(0x8002)
#define CODEBIT             0x80
/* OPENNT-HOST-074: original client timer IDs and key-state operand.
 * These are control inputs, not native syscall numbers or private pointers. */
#define IDSYS_LBSEARCH      0x0000FFFCL
#define IDSYS_SCROLL        0x0000FFFEL
#define IDSYS_CARET         0x0000FFFFL
#define SYS_ALTERNATE           0x2000
/* OPENNT-HOST-105: original style-bit operands for the owned WND view. */
#define DF3DLOOK                0x0C04
#define DFCONTROL               0x0D04
#define DFNOFAILCREATE          0x0C10
#define MultDiv(x, y, z)        (((INT)(x) * (INT)(y) + (INT)(z) / 2) / (INT)(z))
#define XPixFromXDU(x, cxChar)       MultDiv(x, cxChar, 4)
#define YPixFromYDU(y, cyChar)       MultDiv(y, cyChar, 8)
#define MaskWF(flag)         ((WORD)( (HIBYTE(flag) & 1) ? LOBYTE(flag) << 8 : LOBYTE(flag)))
/* OPENNT-HOST-105: original template-character selection. */
#ifdef UNICODE
#define UTCHAR WCHAR
#define CHARSTOBYTES(cch) ((cch) * sizeof(TCHAR))
#else
#define UTCHAR UCHAR
#define CHARSTOBYTES(cch) (cch)
#endif
/* OPENNT-HOST-090: unchanged message bitmap descriptor. */
typedef struct _WNDMSG {
    UINT maxMsgs;
    BYTE *abMsgs;
} WNDMSG, *PWNDMSG;
/* DIVERGENCE(OPENNT-HOST-072): original EDIT index declarations. */
typedef DWORD  ICH;
typedef ICH *LPICH;
/* OPENNT-HOST-077: original DrawFrame command declarations. */
#define DF_SHIFT0           0x0000
#define DF_SHIFT1           0x0001
#define DF_SHIFT2           0x0002
#define DF_SHIFT3           0x0003
#define DF_PATCOPY          0x0000
#define DF_PATINVERT        0x0004
#define DF_SHIFTMASK (DF_SHIFT0 | DF_SHIFT1 | DF_SHIFT2 | DF_SHIFT3)
#define DF_ROPMASK   (DF_PATCOPY | DF_PATINVERT)
#define DF_HBRMASK   ~(DF_SHIFTMASK | DF_ROPMASK)

#define DF_SCROLLBAR        (COLOR_SCROLLBAR << 3)
#define DF_BACKGROUND       (COLOR_BACKGROUND << 3)
#define DF_ACTIVECAPTION    (COLOR_ACTIVECAPTION << 3)
#define DF_INACTIVECAPTION  (COLOR_INACTIVECAPTION << 3)
#define DF_MENU             (COLOR_MENU << 3)
#define DF_WINDOW           (COLOR_WINDOW << 3)
#define DF_WINDOWFRAME      (COLOR_WINDOWFRAME << 3)
#define DF_MENUTEXT         (COLOR_MENUTEXT << 3)
#define DF_WINDOWTEXT       (COLOR_WINDOWTEXT << 3)
#define DF_CAPTIONTEXT      (COLOR_CAPTIONTEXT << 3)
#define DF_ACTIVEBORDER     (COLOR_ACTIVEBORDER << 3)
#define DF_INACTIVEBORDER   (COLOR_INACTIVEBORDER << 3)
#define DF_APPWORKSPACE     (COLOR_APPWORKSPACE << 3)
#define DF_3DSHADOW         (COLOR_3DSHADOW << 3)
#define DF_3DFACE           (COLOR_3DFACE << 3)
#define DF_GRAY             (COLOR_MAX << 3)
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
/* Original FNID subset for the admitted standard-class client view.
 * Declaration-only extension from this same original user.h owner. */
#define FNID_SCROLLBAR              0x0000029A      // xxxSBWndProc;
/* OPENNT-HOST-090: original control-message bitmap index origin. */
#define FNID_START                  0x0000029A
#define FNID_ICONTITLE              0x0000029B      // xxxTitleWndProc;
#define FNID_MENU                   0x0000029C      // xxxMenuWindowProc;
#define FNID_DESKTOP                0x0000029D      // xxxDesktopWndProc;
#define FNID_BUTTON                 0x0000029F      // No server side proc
#define FNID_COMBOBOX               0x000002A0      // No server side proc
#define FNID_COMBOLISTBOX           0x000002A1      // No server side proc
#define FNID_DIALOG                 0x000002A2      // No server side proc
#define FNID_EDIT                   0x000002A3      // No server side proc
#define FNID_LISTBOX                0x000002A4      // No server side proc
#define FNID_MDICLIENT              0x000002A5      // No server side proc
#define FNID_STATIC                 0x000002A6      // No server side proc
#define FNID_CALLWINDOWPROC         0x000002B2
/* OPENNT-HOST-089: symbolic service selected by original clmsg.c. */
#define FNID_DEFWINDOWPROC          0x0000029E      // xxxDefWindowProc;
#define CPD_ANSI_TO_UNICODE     0x0001
#define CPD_UNICODE_TO_ANSI     0x0002
#define CPD_WND                 0x0020
/* OPENNT-HOST-085: original dialog CallProc conversion scope. */
#define CPD_DIALOG              0x0040
#define FNID_SWITCH                 0x000002B4      // Just used by GetTopMostInserAfter
/* DIVERGENCE(OPENNT-HOST-076): original control lifetime status declarations. */
#define FNID_CLEANEDUP_BIT          0x00004000
#define FNID_DELETED_BIT            0x00008000
#define FNID_STATUS_BITS            (FNID_CLEANEDUP_BIT | FNID_DELETED_BIT)
#define GETFNID(pwnd)       ((pwnd)->fnid & ~FNID_STATUS_BITS)
/* OPENNT-HOST-076: original creation flags used by the EDIT carrier. */
#define Is400Compat(dwExpWinVer)  (LOWORD(dwExpWinVer) >= 0x0400)
#define Is310Compat(dwExpWinVer)  (LOWORD(dwExpWinVer) >= 0x030A)
#define WFWIN31COMPAT           0x0280
/* OPENNT-HOST-084: original dialog state-byte selector. */
#define WFDIALOGWINDOW          0x0201
#define WFALWAYSSENDNCPAINT     0x0301
#define WFANSICREATOR           0x0320
/* OPENNT-HOST-086: original control extra-byte destruction guard. */
#define WFDESTROYED             0x0380
#define WFOLDUI                 0x0408
#define WFWIN40COMPAT           0x0480
#endif

/* DIVERGENCE(OPENNT-HOST-082): original counted-string declarations shared
 * by the admitted dialog call boundary and the original client view. */
#if (defined(WOW_ORIGINAL_LARGE_STRING_LAYOUT) || defined(WOW_ORIGINAL_CLIENT_VIEW_LAYOUT)) && !defined(OPENNT_USER_LARGE_STRING_LAYOUT_H)
#define OPENNT_USER_LARGE_STRING_LAYOUT_H
/* OPENNT-HOST-082: original creation flag, independent of string bAnsi. */
#ifndef WS_EX_ANSICREATOR
#define WS_EX_ANSICREATOR   0x80000000L         // Internal
#endif
typedef struct _LARGE_STRING {
    ULONG Length;
    ULONG MaximumLength : 31;
    ULONG bAnsi : 1;
    PVOID Buffer;
} LARGE_STRING, *PLARGE_STRING;

typedef struct _LARGE_ANSI_STRING {
    ULONG Length;
    ULONG MaximumLength : 31;
    ULONG bAnsi : 1;
    PSTR Buffer;
} LARGE_ANSI_STRING, *PLARGE_ANSI_STRING;

typedef struct _LARGE_UNICODE_STRING {
    ULONG Length;
    ULONG MaximumLength : 31;
    ULONG bAnsi : 1;
    PWSTR Buffer;
} LARGE_UNICODE_STRING, *PLARGE_UNICODE_STRING;
#endif

/* DIVERGENCE(OPENNT-HOST-063): opt-in original client-view layouts. The
 * consumer supplies source-era forward types and WW; unrelated USER/kernel
 * declarations stay excluded. These structure bodies are unchanged. */
#if defined(WOW_ORIGINAL_CLIENT_VIEW_LAYOUT) && !defined(OPENNT_USER_CLIENT_VIEW_LAYOUT_H)
#define OPENNT_USER_CLIENT_VIEW_LAYOUT_H
/* DIVERGENCE(OPENNT-HOST-101): unchanged selectors reached by the
 * recovered EDIT/combo/list bodies. Low-byte private frame state still
 * requires its original producer; these declarations do not synthesize it. */
#define WFMPRESENT              0x0001
#define WFVPRESENT              0x0002
#define WFHPRESENT              0x0004
#define WFCPRESENT              0x0008
#define WFFRAMEON               0x0040
/* OPENNT-HOST-101: remaining original control/dialog style readers. */
#define WEFDLGMODALFRAME        0x0801
#define WEFDRAGOBJECT           0x0802
#define WEFRIGHT                0x0910
#define WEFCONTROLPARENT        0x0A01
#define WFMAXBOX                0x0E01
#define WFMINBOX                0x0E02
#define WFSYSMENU               0x0E08
/* OPENNT-HOST-101: original combo creation consumes these ExStyle selectors;
 * these describe the retained NT4 WND, not a modern private layout. */
#define WEFWINDOWEDGE           0x0901
#define WEFCLIENTEDGE           0x0902
#define WEFEDGEMASK             0x0903
/* OPENNT-HOST-101: original dialog queue operation operand. */
#define QF_DIALOGACTIVE           (UINT)0x04000
#define WFSIZEBOX               0x0E04
#define WFHSCROLL               0x0E10
#define WFVSCROLL               0x0E20
#define WFBORDER                0x0E80
#define WFTILED                 0x0F00
#define WFDISABLED              0x0F08
#define WFICONICPOPUP           0x0FC0
#define CBFDROPDOWN             0x0C02
#define CBFDROPDOWNLIST         0x0C03
#define CBFOWNERDRAWFIXED       0x0C10
#define CBFOWNERDRAWVAR         0x0C20
#define CBFOWNERDRAW            0x0C30
#define CBFAUTOHSCROLL          0x0C40
#define CBFOEMCONVERT           0x0C80
#define CBFSORT                 0x0D01
#define CBFHASSTRINGS           0x0D02
#define CBFNOINTEGRALHEIGHT     0x0D04
#define CBFDISABLENOSCROLL      0x0D08
#define CBFUPPERCASE            0x0D20
#define CBFLOWERCASE            0x0D40
#define EFMULTILINE             0x0C04
#define EFPASSWORD              0x0C20
#define EFREADONLY              0x0D08
#define EFWANTRETURN            0x0D10
#define EFNUMBER                0x0D20
/* OPENNT-HOST-078: original visibility/client-rectangle flags. */
/* OPENNT-HOST-109: unchanged original group-navigation flag encoding. */
#define WFGROUP                 0x0E02
#define WFVISIBLE               0x0F10
#define WFMINIMIZED             0x0F20
#define WFCHILD                 0x0F40
#define WFPOPUP                 0x0F80
#define WFTYPEMASK              0x0FC0
#define TestwndChild(hwnd)   (TestWF(hwnd, WFTYPEMASK) == LOBYTE(WFCHILD))
/* OPENNT-HOST-106: relationship choice only, never native HWND lookup. */
#define GetWindowCreator(pwnd) (TestwndChild(pwnd) ? pwnd->spwndParent : pwnd->spwndOwner)
#define TestwndIPopup(hwnd)  (TestWF(hwnd, WFTYPEMASK) == LOBYTE(WFICONICPOPUP))
#define TestwndTiled(hwnd)   (TestWF(hwnd, WFTYPEMASK) == LOBYTE(WFTILED))
#define TestwndNIPopup(hwnd) (TestWF(hwnd, WFTYPEMASK) == LOBYTE(WFPOPUP))
#define TestwndPopup(hwnd)   (TestwndNIPopup(hwnd) || TestwndIPopup(hwnd))
/* Original-layout consumers only. The cleanup facade has its own finite
 * field view and must not receive these WND/WW byte-offset access macros. */
#define STATEOFFSET (sizeof(WND) - sizeof(WW) + (WND_CNT_WOWDWORDS * sizeof(DWORD)))
#define TestWF(hwnd, flag)   (*(((BYTE *)(hwnd)) + STATEOFFSET + (int)HIBYTE(flag)) & LOBYTE(flag))
#define SetWF(hwnd, flag)    (*(((BYTE *)(hwnd)) + STATEOFFSET + (int)HIBYTE(flag)) |= LOBYTE(flag))
#define ClrWF(hwnd, flag)    (*(((BYTE *)(hwnd)) + STATEOFFSET + (int)HIBYTE(flag)) &= ~LOBYTE(flag))
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

typedef struct _PROCOBJHEAD {
    SHROBJHEAD;
    struct tagPROCESSINFO *ppi;
    DWORD hTaskWow;
} PROCOBJHEAD, *PPROCOBJHEAD;

/* Original MENU/ITEM declarations, selected for the immutable USER16 client
 * view under OPENNT-HOST-063. No menu-server implementation is imported. */
typedef struct tagITEM {
    UINT    fType;          // Item Type  Flags
    UINT    fState;         // Item State Flags
    UINT    wID;
    struct tagMENU *spSubMenu;  /* Handle to a popup */
    HANDLE  hbmpChecked;    /* Bitmap for an on  check */
    HANDLE  hbmpUnchecked;   /* Bitmap for an off check */
#ifdef MEMPHIS_MENUS
    LPWSTR  lpstr;          //item's text
#endif // MEMPHIS_MENUS
    HANDLE  hTypeData;     // if ownerdraw, MAKELONG(width, height)
                            // if bitmap,    hBitmap
                            // if separator, 0L
                            // if string,    LPWSTR
    DWORD   cch;                /* String: WCHAR count */
    DWORD   dwItemData;
    DWORD   xItem;
    DWORD   yItem;
    DWORD   cxItem;

    DWORD   cyItem;
    DWORD   dxTab;
    DWORD   ulX;                /* String: Underline start */
    DWORD   ulWidth;            /* String: underline width */
#ifdef MEMPHIS_MENUS
    HBITMAP hbmp;           // item's bitmap
    int     cxBmp;          // bitmap width
    int     cyBmp;          // bitmap height
#endif // MEMPHIS_MENUS
} ITEM, *PITEM, *LPITEM;

typedef struct tagMENU {
    PROCOBJHEAD     head;
    DWORD           fFlags;         /* Menu Flags */
    int             iItem;          /* Contains the position of the selected
                                       item in the menu. -1 if no selection */
    int             iPopupMenuItem; /* Contains the position of the hierarchical
                                       item menu popup */
    UINT            cAlloced;       // Number of items that can fit in rgItems
    UINT            cItems;         /* Number of items in rgItems */

    DWORD           cxMenu;
    DWORD           cyMenu;
    struct tagWND *spwndNotify;     /* The owner hwnd of this menu */
    PITEM           rgItems;        /* The list of items in this menu */
    DWORD           dwContextHelpId;// Context help Id for the whole menu
#ifdef MEMPHIS_MENU_WATERMARKS
    HBRUSH          hbrBack;        // background brush for menu
#endif // MEMPHIS_MENU_WATERMARKS

} MENU, *PMENU;

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

/* DIVERGENCE(OPENNT-HOST-080): original dialog state declarations only.
 * The caller supplies the original WND layout. This is local client state,
 * never an overlay of modern USER storage or a guest-memory allocation. */
#if defined(WOW_ORIGINAL_DIALOG_LAYOUT) && !defined(OPENNT_USER_DIALOG_LAYOUT_H)
#define OPENNT_USER_DIALOG_LAYOUT_H
typedef struct _DLG {
    WNDPROC lpfnDlg;
    DWORD   flags;          /* Various useful flags -- see definitions below */
    int     cxChar;
    int     cyChar;
    HWND    hwndFocusSave;
    UINT    fEnd      : 1;
    UINT    fDisabled : 1;
    int     result;         /* DialogBox result */
    HANDLE  hData;          /* Global handle for edit ctl storage. */
    HFONT   hUserFont;      /* Handle of the font mentioned by the user in template*/
    HWND    hwndSysModalSave;  /* Previous sysmodal window saved here */
} DLG, *PDLG;

typedef struct _DIALOG {
    WND     wnd;
    long    resultWP;       /* window proc result -- DWL_MSGRESULT (+0) */
    PDLG    pdlg;
    long    unused;         /* DWL_USER (+8) */
    BYTE    reserved[DLGWINDOWEXTRA - sizeof(long) - sizeof(PDLG) - sizeof(long)];
} DIALOG, *PDIALOG;

#define PDLG(pwnd) (((PDIALOG)pwnd)->pdlg)

/*
 * Flags definitions for DLG.flags
 */
#define DLGF_ANSI           0x01    /* lpfnDlg is an ANSI proc */
#endif

/* OPENNT-HOST-086: original CallProc declarations, not a modern USER cast. */
#if defined(WOW_ORIGINAL_DIALOG_GETSET_LAYOUT) && !defined(OPENNT_DIALOG_GETSET_CPD_LAYOUT_H)
#define OPENNT_DIALOG_GETSET_CPD_LAYOUT_H
#define CPDHANDLE_HI            0xFFFF
#define MAKE_CPDHANDLE(h)       (MAKELONG((WORD)h,CPDHANDLE_HI))
#define ISCPDTAG(x)             (HIWORD(x) == CPDHANDLE_HI)
typedef struct _CALLPROCDATA {
    THROBJHEAD      head;
    struct _CALLPROCDATA *pcpdNext;
    DWORD           pfnClientPrevious;
    WORD            wType;
} CALLPROCDATA, *PCALLPROCDATA;
#endif

/* OPENNT-HOST-093: original canonical client-control extent declarations. */
#if defined(WOW_ORIGINAL_CONTROL_EXTENT_LAYOUT) && !defined(OPENNT_CONTROL_EXTENT_BUTTON_LAYOUT_H)
#define OPENNT_CONTROL_EXTENT_BUTTON_LAYOUT_H
typedef struct tagBUTN {
    PWND spwnd;
    UINT buttonState;   // Leave this a word for compatibility with SetWindowWord( 0L )
    HANDLE hFont;
    HANDLE hImage;
} BUTN, *PBUTN;

typedef struct tagBUTNWND {
    WND wnd;
    PBUTN pbutn;
} BUTNWND, *PBUTNWND;
#endif

/* OPENNT-HOST-113: independently selected original bitmap metadata. */
/* OPENNT-HOST-117: original button flags; never modern USER bit offsets. */
/* OPENNT-HOST-119: dialog borrowers need state declarations, not Button binding. */
#if (defined(WOW_ORIGINAL_BUTTON_CLIENT) || defined(WOW_ORIGINAL_BUTTON_STATE)) && !defined(OPENNT_BUTTON_FLAGS_H)
#define OPENNT_BUTTON_FLAGS_H
#define BFTYPEMASK              0x0C0F
/* OPENNT-HOST-117: original user.h tab-stop state used by radio buttons. */
#define WFTABSTOP               0x0E01
#define BFRIGHTBUTTON           0x0C20
#define BFICON                  0x0C40
#define BFBITMAP                0x0C80
#define BFIMAGEMASK             0x0CC0
#define BFLEFT                  0x0D01
#define BFRIGHT                 0x0D02
#define BFCENTER                0x0D03
#define BFHORZMASK              0x0D03
#define BFTOP                   0x0D04
#define BFBOTTOM                0x0D08
#define BFVCENTER               0x0D0C
#define BFVERTMASK              0x0D0C
#define BFALIGNMASK             0x0D0F
#define BFPUSHLIKE              0x0D10
#define BFMULTILINE             0x0D20
#define BFNOTIFY                0x0D40
#define BFFLAT                  0x0D80
#define DOBI_CHECK      1
#define DOBI_DOWN       2
#define DOBI_CHECKDOWN  3
#endif

#if defined(WOW_ORIGINAL_BUTTON_RESOURCES) && !defined(OPENNT_BUTTON_RESOURCE_LAYOUT_H)
#define OPENNT_BUTTON_RESOURCE_LAYOUT_H
/* OPENNT-HOST-113: original resource metadata, not a modern USER overlay. */
typedef struct tagOEMBITMAPINFO
{
    int x;
    int y;
    int cx;
    int cy;
} OEMBITMAPINFO, *POEMBITMAPINFO;
#define OBI_RADIOMASK       52
#define OBI_CHECK           53
#define OBI_CHECK_C         54
#define OBI_CHECK_D         55
#define OBI_CHECK_CD        56
#define OBI_CHECK_CDI       57
#define OBI_RADIO           58
#define OBI_RADIO_C         59
#define OBI_RADIO_D         60
#define OBI_RADIO_CD        61
#define OBI_RADIO_CDI       62
#define OBI_3STATE          63
#define OBI_3STATE_C        64
#define OBI_3STATE_D        65
#define OBI_3STATE_CD       66
#define OBI_3STATE_CDI      67
#define OBI_COUNT           68
#endif

/* OPENNT-HOST-100: original retail handle extraction for retained host WND.
 * Opt-in only after the original HEAD/WND declarations, never the separate
 * DispatchClientMessage call view or a guest-projected pointer. */
#if defined(WOW_ORIGINAL_CLIENT_HANDLES) && !defined(OPENNT_CLIENT_HANDLES_H)
#define OPENNT_CLIENT_HANDLES_H
#define PtoH(p)     ((HANDLE)((p) == NULL ? NULL : ((PHEAD)(p))->h))
#define PtoHq(p)    ((HANDLE)(((PHEAD)p)->h))
#define HW(pwnd)    ((HWND)PtoH(pwnd))
#define HWq(pwnd)   ((HWND)PtoHq(pwnd))
#endif
