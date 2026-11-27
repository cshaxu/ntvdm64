/**************************************************************************\
* Module Name: server.c
*
* Server support routines for the CSR stuff.  This basically performs the
* startup/initialization for USER.
*
* Copyright (c) Microsoft Corp.  1990-1996 All Rights Reserved
*
\**************************************************************************/

#define WOW_ORIGINAL_MESSAGE_TABLES
#line 11
#include "wow_default_message_bindings.h"
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
#line 13

/*
 * Initialization Routines (external).
 */
NTSTATUS     InitQEntryLookaside(VOID);
NTSTATUS     InitKeyStateLookaside(VOID);
NTSTATUS     InitSMSLookaside(VOID);
#line 22
FARPROC gpfnDebugAttachRoutine;
#line 26
/*
 * Constants pertaining to the user-initialization.
 */

#define USRINIT_SHAREDSECT_SIZE   32
#define USRINIT_ATOMBUCKET_SIZE   37
#define USRINIT_WINDOWSECT_SIZE  512
#define USRINIT_NOIOSECT_SIZE    128

#define USRINIT_SHAREDSECT_BUFF_SIZE     640
#define USRINIT_SHAREDSECT_READ_SIZE     (USRINIT_SHAREDSECT_BUFF_SIZE-33)
#line 39
LPCWSTR  szBUTTONCLASS         = TEXT("Button");
LPCWSTR  szSTATICCLASS         = TEXT("Static");
LPCWSTR  szLISTBOXCLASS        = TEXT("ListBox");
LPCWSTR  szSCROLLBARCLASS      = TEXT("ScrollBar");
LPCWSTR  szCOMBOLISTBOXCLASS   = TEXT("ComboLBox");
LPCWSTR  szCOMBOBOXCLASS       = TEXT("ComboBox");
LPCWSTR  szMDICLIENTCLASS      = TEXT("MDIClient");
LPCWSTR  szDDEMLEVENTCLASS     = TEXT("DDEMLEvent");
LPCWSTR  szDDEMLMOTHERCLASS    = TEXT("DDEMLMom");
LPCWSTR  szDDEML16BITCLASS     = TEXT("DMGClass");
LPCWSTR  szDDEMLCLIENTCLASSW   = L"DDEMLUnicodeClient";
LPCWSTR  szDDEMLSERVERCLASSW   = L"DDEMLUnicodeServer";
LPCWSTR  szDDEMLCLIENTCLASSA   = L"DDEMLAnsiClient";
LPCWSTR  szDDEMLSERVERCLASSA   = L"DDEMLAnsiServer";
#ifdef FE_IME
#error
#endif

#line 57
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
    0
};

CONST WORD gawMenuWndProc[] = {
    WM_CREATE,
    WM_FINALDESTROY,
    WM_PAINT,
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
    WM_PRINTCLIENT,
    0
};

CONST WORD gawStaticWndProc[] = {
    STM_GETICON,
    STM_GETIMAGE,
    STM_SETICON,
    STM_SETIMAGE,
    WM_ERASEBKGND,
    WM_PAINT,
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
#line 223
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
    WM_NCCREATE,
    WM_WINDOWPOSCHANGED,
    WM_MOUSEWHEEL,
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
    WM_MEASUREITEM,
    WM_DELETEITEM,
    WM_DRAWITEM,
    WM_COMPAREITEM,
    WM_NCCREATE,
    WM_HELP,
    WM_MOUSEWHEEL,
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
    WM_INPUTLANGCHANGE,
    WM_KEYDOWN,
    WM_KILLFOCUS,
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
#error
#endif
#line 464
    WM_PAINT,
    WM_PASTE,
    WM_PRINTCLIENT,
    WM_SETFOCUS,
    WM_SETFONT,
    WM_SETREDRAW,
    WM_SETTEXT,
    WM_SIZE,
    WM_SYSCHAR,
    WM_SYSKEYDOWN,
    WM_SYSTIMER,
    WM_UNDO,
    WM_VSCROLL,
    WM_MOUSEWHEEL,
    0
};

#ifdef FE_IME
#error
#line 498
#endif
#line 499

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
    WM_QUERYDRAGICON,
    WM_NCCREATE,
    WM_SETICON,
    WM_NCCALCSIZE,
    WM_NCPAINT,
    WM_NCACTIVATE,
    WM_NCMOUSEMOVE,
    WM_NCRBUTTONDOWN,
    WM_NCLBUTTONDOWN,
    WM_NCLBUTTONUP,
    WM_NCLBUTTONDBLCLK,
    WM_KEYUP,
    WM_SYSKEYUP,
    WM_SYSCHAR,
    WM_SYSCOMMAND,
    WM_QUERYDROPOBJECT,
    WM_CLIENTSHUTDOWN,
    WM_SYNCPAINT,
    WM_PRINT,
    WM_GETICON,
    WM_QUERYDRAGICON,
    WM_CONTEXTMENU,
    WM_SYSMENU,
    WM_INPUTLANGCHANGEREQUEST,
    WM_INPUTLANGCHANGE,
    0
};

/*
 * This array is for all messages that can be handled with some special
 * code by the client.  DefWindowProcWorker returns 0 for all messages
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
    WM_SYSKEYDOWN,
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
    WM_CTLCOLORSTATIC,
    WM_NOTIFYFORMAT,
    WM_DEVICECHANGE,
    WM_POWERBROADCAST,
    WM_MOUSEWHEEL,
#ifdef FE_IME
#error
#line 596
#endif
#line 597
    0
};

/***************************************************************************\
* DispatchServerMessage
*
*
* 19-Aug-1992 MikeKe    Created
\***************************************************************************/

#line 607
#define WRAPPFN(pfn, type)                                   \
LONG xxxWrap ## pfn(                                         \
    PWND  pwnd,                                              \
    UINT  message,                                           \
    DWORD wParam,                                            \
    LONG  lParam,                                            \
    DWORD xParam)                                            \
{                                                            \
    return xxx ## pfn((type)pwnd, message, wParam, lParam);  \
}

#line 833
#ifdef FE_IME // amsgsSyncOnly[]
#error
#line 839
#endif
#line 850
#ifdef DEBUG
#error
#line 877
#endif // DEBUG
#line 879

/***************************************************************************\
* InitWindowMsgTables
*
* This function generates a bit-array lookup table from a list of messages.
* The lookup table is used to determine whether the message needs to be
* passed over to the server for handling or whether it can be handled
* directly on the client.
*
* LATER: Some memory (a couple hundred bytes per process) could be saved
*        by putting this in the shared read-only heap.
*
*
* 27-Mar-1992 DarrinM   Created.
* 06-Dec-1993 MikeKe    Added support for all of our window procs.
\***************************************************************************/

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
    RtlZeroMemory(*ppbyte, cbTable);

    for (i = 0; (msg = pw[i]) != 0; i++)
        (*ppbyte)[msg / 8] |= (BYTE)(1 << (msg & 7));
}

#line 934
#ifdef DEBUG
#error
#line 939
#endif
#line 990
#ifdef FE_IME
#error
#else
#line 994
#endif
#line 1002
#ifdef DEBUG
#error
#line 1019
#endif
#line 1022

/***************************************************************************\
* InitMessageTables
*
* Initialize the message tables.
*
*
* 25-Aug-1995 ChrisWil      Created.
\***************************************************************************/

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
#ifdef FE_IME
#error
#endif
#line 1057
}
#line 1150

/***************************************************************************\
* InitGlobalRIPFlags (debug only)
*
* This initializes the global RIP flags from the registry.
*
*
* 25-Aug-1995 ChrisWil      Created.
\***************************************************************************/

VOID InitGlobalRIPFlags(VOID)
{

#ifdef DEBUG
#error
#line 1209
#endif

}

#line 1257
#ifndef PAGE_SIZE
#define PAGE_SIZE 0x1000
#endif
#line 1260

#define ROUND_UP_TO_PAGES(SIZE) \
        (((ULONG)(SIZE) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))
#line 1703
#ifdef ANDREVA_DBG
#error
#endif
#line 1822
#ifdef DEBUG
#error
#endif
#line 1861
#ifdef FE_IME
#error
#endif
