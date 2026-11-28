/**************************************************************************\
* Module Name: server.c
*
* Copyright (c) 1985 - 1999, Microsoft Corporation
*
* Server support routines for the CSR stuff. This basically performs the
* startup/initialization for USER.
*
\**************************************************************************/

#define WOW_ORIGINAL_MESSAGE_TABLES
#include "wow_default_message_bindings.h"
#define szUSER32 wow_kernel_szUSER32
#line 12
#pragma hdrstop
/* Original-layout owner: B supplies once-publication and calls this reset
 * only before publication, after an initialization failure. */
SHAREDINFO wow_default_message_state;
VOID WINAPI wow_default_message_reset(VOID)
{
    UINT index;
    if (wow_default_message_state.DefWindowMsgs.abMsgs)
        HeapFree(GetProcessHeap(), 0, wow_default_message_state.DefWindowMsgs.abMsgs);
    if (wow_default_message_state.DefWindowSpecMsgs.abMsgs)
        HeapFree(GetProcessHeap(), 0, wow_default_message_state.DefWindowSpecMsgs.abMsgs);
    for (index = 0; index < ARRAYSIZE(wow_default_message_state.awmControl); ++index)
        if (wow_default_message_state.awmControl[index].abMsgs)
            HeapFree(GetProcessHeap(), 0, wow_default_message_state.awmControl[index].abMsgs);
    ZeroMemory(&wow_default_message_state, sizeof(wow_default_message_state));
}
#line 16
extern WORD gDispatchTableValues;

BOOL gbUserInitialized;

/*
 * Initialization Routines (external).
 */
NTSTATUS     InitQEntryLookaside(VOID);
NTSTATUS     InitSMSLookaside(VOID);

NTSTATUS    InitCreateSharedSection(VOID);
NTSTATUS    InitCreateObjectDirectory(VOID);
BOOL        InitCreateUserSubsystem(VOID);
VOID        InitFunctionTables(VOID);
VOID        InitMessageTables(VOID);
VOID        InitWindowMsgTable(PBYTE*, PUINT, CONST WORD*);

VOID        VerifySyncOnlyMessages(VOID);
BOOL        InitOLEFormats(VOID);
NTSTATUS    Win32UserInitialize(VOID);
#line 49

/*
 * Constants pertaining to the user-initialization.
 */
#define USRINIT_SHAREDSECT_SIZE   32
#define USRINIT_ATOMBUCKET_SIZE   37

#define USRINIT_WINDOWSECT_SIZE  512
#define USRINIT_NOIOSECT_SIZE    128

#define USRINIT_SHAREDSECT_BUFF_SIZE     640
#define USRINIT_SHAREDSECT_READ_SIZE     (USRINIT_SHAREDSECT_BUFF_SIZE-33)


/***************************************************************************\
* Globals stored in the INIT section. These should only be accessed at
* load time!
\***************************************************************************/
#ifdef ALLOC_DATA_PRAGMA
#error Unsupported original build branch
#line 69
#endif
#line 70

CONST WCHAR szCHECKPOINT_PROP_NAME[]  = L"SysCP";
CONST WCHAR szDDETRACK_PROP_NAME[]    = L"SysDT";
CONST WCHAR szQOS_PROP_NAME[]         = L"SysQOS";
CONST WCHAR szDDEIMP_PROP_NAME[]      = L"SysDDEI";
CONST WCHAR szWNDOBJ_PROP_NAME[]      = L"SysWNDO";
CONST WCHAR szIMELEVEL_PROP_NAME[]    = L"SysIMEL";
CONST WCHAR szLAYER_PROP_NAME[]       = L"SysLayer";
CONST WCHAR szUSER32[]                = L"USER32";
CONST WCHAR szCONTEXTHELPIDPROP[]     = L"SysCH";
CONST WCHAR szICONSM_PROP_NAME[]      = L"SysICS";
CONST WCHAR szICON_PROP_NAME[]        = ICON_PROP_NAME;
CONST WCHAR szSHELLHOOK[]             = L"SHELLHOOK";
CONST WCHAR szACTIVATESHELLWINDOW[]   = L"ACTIVATESHELLWINDOW";
CONST WCHAR szOTHERWINDOWCREATED[]    = L"OTHERWINDOWCREATED";
CONST WCHAR szOTHERWINDOWDESTROYED[]  = L"OTHERWINDOWDESTROYED";
CONST WCHAR szOLEMAINTHREADWNDCLASS[] = L"OleMainThreadWndClass";
CONST WCHAR szFLASHWSTATE[]           = L"FlashWState";

#ifdef HUNGAPP_GHOSTING
CONST WCHAR szGHOST[]                 = L"Ghost";
#endif // HUNGAPP_GHOSTING

CONST WCHAR szSHADOW[]                = L"SysShadow";

/***************************************************************************\
* Message Tables
*
*   DefDlgProc
*   MenuWndProc
*   ScrollBarWndProc
*   StaticWndProc
*   ButtonWndProc
*   ListboxWndProc
*   ComboWndProc
*   EditWndProc
*   DefWindowMsgs
*   DefWindowSpecMsgs
*
* These are used in InitMessageTables() to initialize gSharedInfo.awmControl[]
* using the INITMSGTABLE() macro.
*
* 25-Aug-1995 ChrisWil  Created comment block.
\***************************************************************************/

CONST WORD gawDefDlgProc[] = {
    WM_COMPAREITEM,
    WM_VKEYTOITEM,
    WM_CHARTOITEM,
    WM_INITDIALOG,
    WM_QUERYDRAGICON,
    WM_CTLCOLOR,
    WM_CTLCOLORMSGBOX,
    WM_CTLCOLOREDIT,
    WM_CTLCOLORLISTBOX,
    WM_CTLCOLORBTN,
    WM_CTLCOLORDLG,
    WM_CTLCOLORSCROLLBAR,
    WM_CTLCOLORSTATIC,
    WM_ERASEBKGND,
    WM_SHOWWINDOW,
    WM_SYSCOMMAND,
    WM_SYSKEYDOWN,
    WM_ACTIVATE,
    WM_SETFOCUS,
    WM_CLOSE,
    WM_NCDESTROY,
    WM_FINALDESTROY,
    DM_REPOSITION,
    DM_SETDEFID,
    DM_GETDEFID,
    WM_NEXTDLGCTL,
    WM_ENTERMENULOOP,
    WM_LBUTTONDOWN,
    WM_NCLBUTTONDOWN,
    WM_GETFONT,
    WM_NOTIFYFORMAT,
    WM_INPUTLANGCHANGEREQUEST,
    0
};

CONST WORD gawMenuWndProc[] = {
    WM_NCCREATE,
    WM_FINALDESTROY,
    WM_PAINT,
    WM_NCCALCSIZE,
    WM_CHAR,
    WM_SYSCHAR,
    WM_KEYDOWN,
    WM_SYSKEYDOWN,
    WM_TIMER,
    MN_SETHMENU,
    MN_SIZEWINDOW,
    MN_OPENHIERARCHY,
    MN_CLOSEHIERARCHY,
    MN_SELECTITEM,
    MN_SELECTFIRSTVALIDITEM,
    MN_CANCELMENUS,
    MN_FINDMENUWINDOWFROMPOINT,
    MN_SHOWPOPUPWINDOW,
    MN_BUTTONDOWN,
    MN_MOUSEMOVE,
    MN_BUTTONUP,
    MN_SETTIMERTOOPENHIERARCHY,
    WM_ACTIVATE,
    MN_GETHMENU,
    MN_DBLCLK,
    MN_ACTIVATEPOPUP,
    MN_ENDMENU,
    MN_DODRAGDROP,
    WM_ACTIVATEAPP,
    WM_MOUSELEAVE,
    WM_SIZE,
    WM_MOVE,
    WM_NCHITTEST,
    WM_NCPAINT,
    WM_PRINT,
    WM_PRINTCLIENT,
    WM_ERASEBKGND,
    WM_WINDOWPOSCHANGING,
    WM_WINDOWPOSCHANGED,
    0
};

CONST WORD gawDesktopWndProc[] = {
    WM_PAINT,
    WM_ERASEBKGND,
    0
};

CONST WORD gawScrollBarWndProc[] = {
    WM_CREATE,
    WM_SETFOCUS,
    WM_KILLFOCUS,
    WM_ERASEBKGND,
    WM_PAINT,
    WM_LBUTTONDBLCLK,
    WM_LBUTTONDOWN,
    WM_KEYUP,
    WM_KEYDOWN,
    WM_ENABLE,
    SBM_ENABLE_ARROWS,
    SBM_SETPOS,
    SBM_SETRANGEREDRAW,
    SBM_SETRANGE,
    SBM_SETSCROLLINFO,
    SBM_GETSCROLLINFO,
    SBM_GETSCROLLBARINFO,
    WM_PRINTCLIENT,
    WM_MOUSEMOVE,
    WM_MOUSELEAVE,
    0
};

CONST WORD gawStaticWndProc[] = {
    STM_GETICON,
    STM_GETIMAGE,
    STM_SETICON,
    STM_SETIMAGE,
    WM_ERASEBKGND,
    WM_PAINT,
    WM_PRINTCLIENT,
    WM_CREATE,
    WM_DESTROY,
    WM_NCCREATE,
    WM_NCDESTROY,
    WM_FINALDESTROY,
    WM_NCHITTEST,
    WM_LBUTTONDOWN,
    WM_NCLBUTTONDOWN,
    WM_LBUTTONDBLCLK,
    WM_NCLBUTTONDBLCLK,
    WM_SETTEXT,
    WM_ENABLE,
    WM_GETDLGCODE,
    WM_SETFONT,
    WM_GETFONT,
    WM_GETTEXT,
    WM_TIMER,
    WM_INPUTLANGCHANGEREQUEST,
    WM_UPDATEUISTATE,
    0
};

CONST WORD gawButtonWndProc[] = {
    WM_NCHITTEST,
    WM_ERASEBKGND,
    WM_PRINTCLIENT,
    WM_PAINT,
    WM_SETFOCUS,
    WM_GETDLGCODE,
    WM_CAPTURECHANGED,
    WM_KILLFOCUS,
    WM_LBUTTONDBLCLK,
    WM_LBUTTONUP,
    WM_MOUSEMOVE,
    WM_LBUTTONDOWN,
    WM_CHAR,
    BM_CLICK,
    WM_KEYDOWN,
    WM_KEYUP,
    WM_SYSKEYUP,
    BM_GETSTATE,
    BM_SETSTATE,
    BM_GETCHECK,
    BM_SETCHECK,
    BM_SETSTYLE,
    WM_SETTEXT,
    WM_ENABLE,
    WM_SETFONT,
    WM_GETFONT,
    BM_GETIMAGE,
    BM_SETIMAGE,
    WM_NCDESTROY,
    WM_FINALDESTROY,
    WM_NCCREATE,
    WM_INPUTLANGCHANGEREQUEST,
    WM_UPDATEUISTATE,
    0
};

CONST WORD gawListboxWndProc[] = {
    LB_GETTOPINDEX,
    LB_SETTOPINDEX,
    WM_SIZE,
    WM_ERASEBKGND,
    LB_RESETCONTENT,
    WM_TIMER,
    WM_MOUSEMOVE,
    WM_MBUTTONDOWN,
    WM_LBUTTONDOWN,
    WM_LBUTTONUP,
    WM_LBUTTONDBLCLK,
    WM_CAPTURECHANGED,
    LBCB_STARTTRACK,
    LBCB_ENDTRACK,
    WM_PRINTCLIENT,
    WM_PAINT,
    WM_NCDESTROY,
    WM_FINALDESTROY,
    WM_SETFOCUS,
    WM_KILLFOCUS,
    WM_VSCROLL,
    WM_HSCROLL,
    WM_GETDLGCODE,
    WM_CREATE,
    WM_SETREDRAW,
    WM_ENABLE,
    WM_SETFONT,
    WM_GETFONT,
    WM_DRAGSELECT,
    WM_DRAGLOOP,
    WM_DRAGMOVE,
    WM_DROPFILES,
    WM_QUERYDROPOBJECT,
    WM_DROPOBJECT,
    LB_GETITEMRECT,
    LB_GETITEMDATA,
    LB_SETITEMDATA,
    LB_ADDSTRINGUPPER,
    LB_ADDSTRINGLOWER,
    LB_ADDSTRING,
    LB_INSERTSTRINGUPPER,
    LB_INSERTSTRINGLOWER,
    LB_INSERTSTRING,
    LB_INITSTORAGE,
    LB_DELETESTRING,
    LB_DIR,
    LB_ADDFILE,
    LB_SETSEL,
    LB_SETCURSEL,
    LB_GETSEL,
    LB_GETCURSEL,
    LB_SELITEMRANGE,
    LB_SELITEMRANGEEX,
    LB_GETTEXTLEN,
    LB_GETTEXT,
    LB_GETCOUNT,
    LB_SETCOUNT,
    LB_SELECTSTRING,
    LB_FINDSTRING,
    LB_GETLOCALE,
    LB_SETLOCALE,
    WM_KEYDOWN,
    WM_CHAR,
    LB_GETSELITEMS,
    LB_GETSELCOUNT,
    LB_SETTABSTOPS,
    LB_GETHORIZONTALEXTENT,
    LB_SETHORIZONTALEXTENT,
    LB_SETCOLUMNWIDTH,
    LB_SETANCHORINDEX,
    LB_GETANCHORINDEX,
    LB_SETCARETINDEX,
    LB_GETCARETINDEX,
    LB_SETITEMHEIGHT,
    LB_GETITEMHEIGHT,
    LB_FINDSTRINGEXACT,
    LB_ITEMFROMPOINT,
    LB_SETLOCALE,
    LB_GETLOCALE,
    LBCB_CARETON,
    LBCB_CARETOFF,
    LB_GETLISTBOXINFO,
    WM_NCCREATE,
    WM_WINDOWPOSCHANGED,
    WM_MOUSEWHEEL,
    WM_STYLECHANGED,
    WM_STYLECHANGING,
    0
};

CONST WORD gawComboWndProc[] = {
    CBEC_KILLCOMBOFOCUS,
    WM_COMMAND,
    WM_CTLCOLORMSGBOX,
    WM_CTLCOLOREDIT,
    WM_CTLCOLORLISTBOX,
    WM_CTLCOLORBTN,
    WM_CTLCOLORDLG,
    WM_CTLCOLORSCROLLBAR,
    WM_CTLCOLORSTATIC,
    WM_CTLCOLOR,
    WM_GETTEXT,
    WM_GETTEXTLENGTH,
    WM_CLEAR,
    WM_CUT,
    WM_PASTE,
    WM_COPY,
    WM_SETTEXT,
    WM_CREATE,
    WM_ERASEBKGND,
    WM_GETFONT,
    WM_PRINT,
    WM_PRINTCLIENT,
    WM_PAINT,
    WM_GETDLGCODE,
    WM_SETFONT,
    WM_SYSKEYDOWN,
    WM_KEYDOWN,
    WM_CHAR,
    WM_LBUTTONDBLCLK,
    WM_LBUTTONDOWN,
    WM_CAPTURECHANGED,
    WM_LBUTTONUP,
    WM_MOUSEMOVE,
    WM_NCDESTROY,
    WM_FINALDESTROY,
    WM_SETFOCUS,
    WM_KILLFOCUS,
    WM_SETREDRAW,
    WM_ENABLE,
    WM_SIZE,
    CB_GETDROPPEDSTATE,
    CB_GETDROPPEDCONTROLRECT,
    CB_SETDROPPEDWIDTH,
    CB_GETDROPPEDWIDTH,
    CB_DIR,
    CB_SETEXTENDEDUI,
    CB_GETEXTENDEDUI,
    CB_GETEDITSEL,
    CB_LIMITTEXT,
    CB_SETEDITSEL,
    CB_ADDSTRING,
    CB_DELETESTRING,
    CB_INITSTORAGE,
    CB_SETTOPINDEX,
    CB_GETTOPINDEX,
    CB_GETCOUNT,
    CB_GETCURSEL,
    CB_GETLBTEXT,
    CB_GETLBTEXTLEN,
    CB_INSERTSTRING,
    CB_RESETCONTENT,
    CB_GETHORIZONTALEXTENT,
    CB_SETHORIZONTALEXTENT,
    CB_FINDSTRING,
    CB_FINDSTRINGEXACT,
    CB_SELECTSTRING,
    CB_SETCURSEL,
    CB_GETITEMDATA,
    CB_SETITEMDATA,
    CB_SETITEMHEIGHT,
    CB_GETITEMHEIGHT,
    CB_SHOWDROPDOWN,
    CB_SETLOCALE,
    CB_GETLOCALE,
    CB_GETCOMBOBOXINFO,
    WM_MEASUREITEM,
    WM_DELETEITEM,
    WM_DRAWITEM,
    WM_COMPAREITEM,
    WM_NCCREATE,
    WM_HELP,
    WM_MOUSEWHEEL,
    WM_MOUSELEAVE,
    WM_STYLECHANGED,
    WM_STYLECHANGING,
    WM_UPDATEUISTATE,
    0
};

CONST WORD gawEditWndProc[] = {
    EM_CANUNDO,
    EM_CHARFROMPOS,
    EM_EMPTYUNDOBUFFER,
    EM_FMTLINES,
    EM_GETFIRSTVISIBLELINE,
    EM_GETFIRSTVISIBLELINE,
    EM_GETHANDLE,
    EM_GETLIMITTEXT,
    EM_GETLINE,
    EM_GETLINECOUNT,
    EM_GETMARGINS,
    EM_GETMODIFY,
    EM_GETPASSWORDCHAR,
    EM_GETRECT,
    EM_GETSEL,
    EM_GETWORDBREAKPROC,
    EM_SETIMESTATUS,
    EM_GETIMESTATUS,
    EM_LINEFROMCHAR,
    EM_LINEINDEX,
    EM_LINELENGTH,
    EM_LINESCROLL,
    EM_POSFROMCHAR,
    EM_REPLACESEL,
    EM_SCROLL,
    EM_SCROLLCARET,
    EM_SETHANDLE,
    EM_SETLIMITTEXT,
    EM_SETMARGINS,
    EM_SETMODIFY,
    EM_SETPASSWORDCHAR,
    EM_SETREADONLY,
    EM_SETRECT,
    EM_SETRECTNP,
    EM_SETSEL,
    EM_SETTABSTOPS,
    EM_SETWORDBREAKPROC,
    EM_UNDO,
    WM_CAPTURECHANGED,
    WM_CHAR,
    WM_CLEAR,
    WM_CONTEXTMENU,
    WM_COPY,
    WM_CREATE,
    WM_CUT,
    WM_ENABLE,
    WM_ERASEBKGND,
    WM_GETDLGCODE,
    WM_GETFONT,
    WM_GETTEXT,
    WM_GETTEXTLENGTH,
    WM_HSCROLL,
    WM_IME_STARTCOMPOSITION,
    WM_IME_ENDCOMPOSITION,
    WM_IME_COMPOSITION,
    WM_IME_SETCONTEXT,
    WM_IME_NOTIFY,
    WM_IME_COMPOSITIONFULL,
    WM_IME_SELECT,
    WM_IME_CHAR,
    WM_IME_REQUEST,
    WM_INPUTLANGCHANGE,
    WM_KEYUP,
    WM_KEYDOWN,
    WM_KILLFOCUS,
    WM_MBUTTONDOWN,
    WM_LBUTTONDBLCLK,
    WM_LBUTTONDOWN,
    WM_LBUTTONUP,
    WM_MOUSEMOVE,
    WM_NCCREATE,
    WM_NCDESTROY,
    WM_RBUTTONDOWN,
    WM_RBUTTONUP,
    WM_FINALDESTROY,
#if 0
#error Unsupported original build branch
#line 550
#endif
#line 551
    WM_PAINT,
    WM_PASTE,
    WM_PRINTCLIENT,
    WM_SETFOCUS,
    WM_SETFONT,
    WM_SETREDRAW,
    WM_SETTEXT,
    WM_SIZE,
    WM_STYLECHANGED,
    WM_STYLECHANGING,
    WM_SYSCHAR,
    WM_SYSKEYDOWN,
    WM_SYSTIMER,
    WM_UNDO,
    WM_VSCROLL,
    WM_MOUSEWHEEL,
    0
};

CONST WORD gawImeWndProc[] = {
    WM_ERASEBKGND,
    WM_PAINT,
    WM_DESTROY,
    WM_NCDESTROY,
    WM_FINALDESTROY,
    WM_CREATE,
    WM_IME_SYSTEM,
    WM_IME_SELECT,
    WM_IME_CONTROL,
    WM_IME_SETCONTEXT,
    WM_IME_NOTIFY,
    WM_IME_COMPOSITION,
    WM_IME_STARTCOMPOSITION,
    WM_IME_ENDCOMPOSITION,
    WM_IME_REQUEST,
    WM_COPYDATA,
    0
};

/*
 * This array is for all the messages that need to be passed straight
 * across to the server for handling.
 */
CONST WORD gawDefWindowMsgs[] = {
    WM_GETHOTKEY,
    WM_SETHOTKEY,
    WM_SETREDRAW,
    WM_SETTEXT,
    WM_PAINT,
    WM_CLOSE,
    WM_ERASEBKGND,
    WM_CANCELMODE,
    WM_SETCURSOR,
    WM_PAINTICON,
    WM_ICONERASEBKGND,
    WM_DRAWITEM,
    WM_KEYF1,
    WM_ISACTIVEICON,
    WM_NCCREATE,
    WM_SETICON,
    WM_NCCALCSIZE,
    WM_NCPAINT,
    WM_NCACTIVATE,
    WM_NCMOUSEMOVE,
    WM_NCRBUTTONUP,
    WM_NCRBUTTONDOWN,
#ifndef LAME_BUTTON
    WM_NCLBUTTONDOWN,
#endif // !LAME_BUTTON
    WM_NCLBUTTONUP,
    WM_NCLBUTTONDBLCLK,
    WM_KEYUP,
    WM_SYSKEYUP,
#ifndef LAME_BUTTON
    WM_SYSCHAR,
    WM_SYSCOMMAND,
#endif // !LAME_BUTTON
    WM_QUERYDROPOBJECT,
    WM_CLIENTSHUTDOWN,
    WM_SYNCPAINT,
    WM_PRINT,
    WM_GETICON,
    WM_CONTEXTMENU,
    WM_SYSMENU,
    WM_INPUTLANGCHANGEREQUEST,
    WM_INPUTLANGCHANGE,
    WM_UPDATEUISTATE,
    WM_NCUAHDRAWCAPTION,
    WM_NCUAHDRAWFRAME,
    WM_UAHINIT,
    0
};

/*
 * This array is for all messages that can be handled with some special
 * code by the client. DefWindowProcWorker returns 0 for all messages
 * that aren't in this array or the one above.
 */
CONST WORD gawDefWindowSpecMsgs[] = {
    WM_ACTIVATE,
    WM_GETTEXT,
    WM_GETTEXTLENGTH,
    WM_RBUTTONUP,
    WM_QUERYENDSESSION,
    WM_QUERYOPEN,
    WM_SHOWWINDOW,
    WM_MOUSEACTIVATE,
    WM_HELP,
    WM_VKEYTOITEM,
    WM_CHARTOITEM,
    WM_KEYDOWN,
#ifdef LAME_BUTTON
#error Unsupported original build branch
#line 665
#endif // LAME_BUTTON
#line 666
    WM_SYSKEYDOWN,
    WM_UNICHAR,
    WM_DROPOBJECT,
    WM_WINDOWPOSCHANGING,
    WM_WINDOWPOSCHANGED,
    WM_KLUDGEMINRECT,
    WM_CTLCOLOR,
    WM_CTLCOLORMSGBOX,
    WM_CTLCOLOREDIT,
    WM_CTLCOLORLISTBOX,
    WM_CTLCOLORBTN,
    WM_CTLCOLORDLG,
    WM_CTLCOLORSCROLLBAR,
    WM_NCHITTEST,
#ifdef LAME_BUTTON
#error Unsupported original build branch
#line 682
#endif // LAME_BUTTON
#line 683
    WM_NCXBUTTONUP,
    WM_CTLCOLORSTATIC,
    WM_NOTIFYFORMAT,
    WM_DEVICECHANGE,
    WM_POWERBROADCAST,
    WM_MOUSEWHEEL,
    WM_XBUTTONUP,
    WM_IME_KEYDOWN,
    WM_IME_KEYUP,
    WM_IME_CHAR,
    WM_IME_COMPOSITION,
    WM_IME_STARTCOMPOSITION,
    WM_IME_ENDCOMPOSITION,
    WM_IME_COMPOSITIONFULL,
    WM_IME_SETCONTEXT,
    WM_IME_CONTROL,
    WM_IME_NOTIFY,
    WM_IME_SELECT,
    WM_IME_SYSTEM,
    WM_LPKDRAWSWITCHWND,
    WM_QUERYDRAGICON,
    WM_CHANGEUISTATE,
    WM_QUERYUISTATE,
    WM_APPCOMMAND,
    0
};

static CONST LPCWSTR lpszOLEFormats[] = {
    L"ObjectLink",
    L"OwnerLink",
    L"Native",
    L"Binary",
    L"FileName",
    L"FileNameW",
    L"NetworkName",
    L"DataObject",
    L"Embedded Object",
    L"Embed Source",
    L"Custom Link Source",
    L"Link Source",
    L"Object Descriptor",
    L"Link Source Descriptor",
    L"OleDraw",
    L"PBrush",
    L"MSDraw",
    L"Ole Private Data",
    L"Screen Picture",
    L"OleClipboardPersistOnFlush",
    L"MoreOlePrivateData"
};

static CONST LPCWSTR lpszControls[] = {
    L"Button",
    L"Edit",
    L"Static",
    L"ListBox",
    L"ScrollBar",
    L"ComboBox",
    L"MDIClient",
    L"ComboLBox",
    L"DDEMLEvent",
    L"DDEMLMom",
    L"DMGClass",
    L"DDEMLAnsiClient",
    L"DDEMLUnicodeClient",
    L"DDEMLAnsiServer",
    L"DDEMLUnicodeServer",
    L"IME",
};


#ifdef ALLOC_DATA_PRAGMA
#error Unsupported original build branch
#line 756
#endif
#line 757

/***************************************************************************\
* DispatchServerMessage
*
*
* 19-Aug-1992 MikeKe    Created
\***************************************************************************/

#define WRAPPFN(pfn, type)                                   \
LRESULT xxxWrap ## pfn(                                      \
    PWND  pwnd,                                              \
    UINT  message,                                           \
    WPARAM wParam,                                           \
    LPARAM lParam,                                           \
    ULONG_PTR xParam)                                         \
{                                                            \
    DBG_UNREFERENCED_PARAMETER(xParam);                      \
                                                             \
    return xxx ## pfn((type)pwnd, message, wParam, lParam);  \
}


#line 1060
VOID InitWindowMsgTable(
    PBYTE      *ppbyte,
    PUINT      pmax,
    CONST WORD *pw)
{
    UINT i;
    WORD msg;
    UINT cbTable;

    *pmax = 0;
    for (i = 0; (msg = pw[i]) != 0; i++) {
        if (msg > *pmax)
            *pmax = msg;
    }

    cbTable = *pmax / 8 + 1;
    *ppbyte = SharedAlloc(cbTable);
    if (*ppbyte == NULL) {
        return;
    }

    for (i = 0; (msg = pw[i]) != 0; i++)
        (*ppbyte)[msg / 8] |= (BYTE)(1 << (msg & 7));
}

#line 1175
VOID InitMessageTables(VOID)
{
    TRACE_INIT(("UserInit: Initialize Message Tables\n"));

#define INITMSGTABLE(member, procname)                \
    InitWindowMsgTable(&(gSharedInfo.member.abMsgs),  \
                       &(gSharedInfo.member.maxMsgs), \
                       gaw ## procname);

    INITMSGTABLE(DefWindowMsgs, DefWindowMsgs);
    INITMSGTABLE(DefWindowSpecMsgs, DefWindowSpecMsgs);

    INITMSGTABLE(awmControl[FNID_DIALOG       - FNID_START], DefDlgProc);
    INITMSGTABLE(awmControl[FNID_SCROLLBAR    - FNID_START], ScrollBarWndProc);
    INITMSGTABLE(awmControl[FNID_MENU         - FNID_START], MenuWndProc);
    INITMSGTABLE(awmControl[FNID_DESKTOP      - FNID_START], DesktopWndProc);
    INITMSGTABLE(awmControl[FNID_STATIC       - FNID_START], StaticWndProc);
    INITMSGTABLE(awmControl[FNID_BUTTON       - FNID_START], ButtonWndProc);
    INITMSGTABLE(awmControl[FNID_LISTBOX      - FNID_START], ListboxWndProc);
    INITMSGTABLE(awmControl[FNID_COMBOBOX     - FNID_START], ComboWndProc);
    INITMSGTABLE(awmControl[FNID_COMBOLISTBOX - FNID_START], ListboxWndProc);
    INITMSGTABLE(awmControl[FNID_EDIT         - FNID_START], EditWndProc);
    INITMSGTABLE(awmControl[FNID_IME          - FNID_START], ImeWndProc);
}
