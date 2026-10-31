/* Copyright (c) 1985-91, Microsoft Corporation.
 * Original windows/core/ntuser/client/usercli.h declaration subset.
 * DIVERGENCE(OPENNT-HOST-072/074): retain original EDIT state, validation,
 * initialization macros and lookaside declarations;
 * private precompiled environment and unrelated USER declarations excluded.
 * The consuming facade supplies the original WND/ICH and Windows ABI types.
 */
#ifndef OPENNT_USERCLI_LOOKASIDE_SUBSET_H
#define OPENNT_USERCLI_LOOKASIDE_SUBSET_H

/* OPENNT-HOST-090: original default-window bitmap predicate. */
#define FDEFWINDOWMSG(msg, procname) \
    ((msg <= (gSharedInfo.procname.maxMsgs)) && \
            ((gSharedInfo.procname.abMsgs)[msg / 8] & (1 << (msg & 7))))
#define FWINDOWMSG(msg, fnid) \
    ((msg <= (gSharedInfo.awmControl[fnid - FNID_START].maxMsgs)) && \
            ((gSharedInfo.awmControl[fnid - FNID_START].abMsgs)[msg / 8] & (1 << (msg & 7))))

/* OPENNT-HOST-087: original ID for the retained EDIT menu resource. */
#define ID_EC_PROPERTY_MENU      1

/* OPENNT-HOST-074: unchanged client-side pointer macros, not server locks.
 * Opt in only for original client bodies whose caller retains the objects. */
#ifdef WOW_ORIGINAL_EDIT_CLIENT
#undef ThreadLock
#undef ThreadLockAlways
#undef ThreadLockWithPti
#undef ThreadLockAlwaysWithPti
#undef ThreadUnlock
#undef Lock
#undef Unlock
#define CheckLock(pobj)
#define ThreadLock(pobj, ptl) DBG_UNREFERENCED_LOCAL_VARIABLE(*ptl)
#define ThreadLockAlways(pobj, ptl) DBG_UNREFERENCED_LOCAL_VARIABLE(*ptl)
#define ThreadLockWithPti(pti, pobj, ptl) DBG_UNREFERENCED_LOCAL_VARIABLE(*ptl)
#define ThreadLockAlwaysWithPti(pti, pobj, ptl) DBG_UNREFERENCED_LOCAL_VARIABLE(*ptl)
#define ThreadUnlock(ptl) (ptl)
#define Lock(ppobj, pobj) (*ppobj = pobj)
#define Unlock(ppobj) (*ppobj = NULL)

#ifndef USEREXTS
typedef struct _TL {
    int iBogus;
} TL;
#endif
#endif

/* DIVERGENCE(OPENNT-HOST-075): original native heap and retail global
 * memory macros. These are distinct from globals.h's guest-capable LOCAL*. */
#define UserLocalAlloc(uFlag,uBytes) HeapAlloc(pUserHeap, uFlag, (uBytes))
#define UserLocalReAlloc(p, uBytes, uFlags) HeapReAlloc(pUserHeap, uFlags, (LPSTR)(p), (uBytes))
#define UserLocalFree(p)    HeapFree(pUserHeap, 0, (LPSTR)(p))
#define UserLocalSize(p)    HeapSize(pUserHeap, 0, (LPSTR)(p))
#define UserLocalLock(p)    (LPSTR)(p)
#define UserLocalUnlock(p)
#define UserLocalFlags(p)   0
#define UserLocalHandle(p)  (HLOCAL)(p)

#define USERGLOBALLOCK(h, p)   p = GlobalLock((HANDLE)(h))
#define USERGLOBALUNLOCK(h)    GlobalUnlock((HANDLE)(h))
#define UserGlobalFree(h)      GlobalFree((HANDLE)(h))
#define UserGlobalAlloc(flags, size)        GlobalAlloc(flags, size)
#define UserGlobalReAlloc(pmem, cnt, flags) GlobalReAlloc(pmem,cnt,flags)
#define UserGlobalSize(pmem)                GlobalSize(pmem)

/* OPENNT-HOST-075: original module-specific callback, not task version. */
#define GETEXPWINVER(hModule)                        ((*(pfnGetExpWinVer))((hModule)?(hModule):GetModuleHandle(NULL)))

/* DIVERGENCE(OPENNT-HOST-075): retain the distinct original current-task
 * query; GetClientInfo binds to the existing task initialization owner. */
#define GetAppVer(pti) GetClientInfo()->dwExpWinVer

/*
 * Lookaside definitions
 */
typedef struct _LOOKASIDE {
    PVOID LookasideBase;
    PVOID LookasideBounds;
    ZONE_HEADER LookasideZone;
    DWORD EntrySize;
#if DBG
    ULONG AllocHiWater;
    ULONG AllocCalls;
    ULONG AllocSlowCalls;
    ULONG DelCalls;
    ULONG DelSlowCalls;
#endif // DBG
} LOOKASIDE, *PLOOKASIDE;

NTSTATUS InitLookaside(PLOOKASIDE pla, DWORD cbEntry, DWORD cEntries);
PVOID AllocLookasideEntry(PLOOKASIDE pla);
void FreeLookasideEntry(PLOOKASIDE pla, PVOID pEntry);
#endif

#ifndef WOW_EDIT_LOOKASIDE_DECLARATIONS_ONLY
#ifndef OPENNT_USERCLI_EDIT_STATE_SUBSET_H
#define OPENNT_USERCLI_EDIT_STATE_SUBSET_H

/*
 * Window Proc Window Validation macro. This macro assumes
 * that pwnd and hwnd are existing variables pointing to the window.
 * Checking the BUTTON is for Mavis Beacon.
 */

#define VALIDATECLASSANDSIZE(pwnd, inFNID)                                      \
    switch ((pwnd)->fnid) {                                                     \
    case inFNID:                                                                \
        break;                                                                  \
                                                                                \
    case 0:                                                                     \
        if ((pwnd->cbwndExtra + sizeof(WND)) < (DWORD)(CBFNID(inFNID))) {       \
            RIPMSG3(RIP_ERROR,                                                    \
                   "(%lX %lX) needs at least (%ld) window words for this proc", \
                    pwnd, pwnd->cbwndExtra,                                     \
                    (DWORD)(CBFNID(inFNID)) - sizeof(WND));                     \
            return 0;                                                           \
        }                                                                       \
                                                                                \
        if (inFNID == FNID_BUTTON && *((PUINT)(pwnd + 1))) {                    \
                                                                                \
            RIPMSG3(RIP_WARNING, "Window (%lX) fnid = %lX overrides "             \
                "the extra pointer with %lX\n",                                 \
                pwnd, inFNID, *((PUINT)(pwnd + 1)));                            \
                                                                                \
            NtUserSetWindowLong(hwnd, 0, 0, FALSE);                             \
        }                                                                       \
                                                                                \
        NtUserSetWindowFNID(hwnd, inFNID);                                      \
        break;                                                                  \
                                                                                \
    case (inFNID | FNID_CLEANEDUP_BIT):                                         \
    case (inFNID | FNID_DELETED_BIT):                                           \
    case (inFNID | FNID_STATUS_BITS):                                           \
        return 0;                                                               \
                                                                                \
    default:                                                                    \
        RIPMSG3(RIP_WARNING,                                                      \
              "Window (%lX) not of correct class; fnid = %lX not %lX",          \
              (pwnd), (DWORD)((pwnd)->fnid), (DWORD)(inFNID));                  \
        return 0;                                                               \
    }

/*
 * This macro initializes the lookaside entry for a control.  It assumes
 * that pwnd and hwnd are existing variables pointing to the control's
 * windows and that fInit exists as a BOOL initialization flag.
 */
#define INITCONTROLLOOKASIDE(plaType, type, pwnditem, count)                \
    if (!*((PUINT)(pwnd + 1))) {                                            \
        P ## type pType;                                                    \
        if (fInit) {                                                        \
            if (!NT_SUCCESS(InitLookaside(plaType, sizeof(type), count))) { \
                NtUserSetWindowFNID(hwnd, FNID_CLEANEDUP_BIT);              \
                NtUserDestroyWindow(hwnd);                                  \
                return FALSE;                                               \
            }                                                               \
            fInit = FALSE;                                                  \
        }                                                                   \
        if ((pType = (P ## type)AllocLookasideEntry(plaType))) {            \
            NtUserSetWindowLong(hwnd, 0, (LONG)pType, FALSE);               \
            Lock(&(pType->pwnditem), pwnd);                                 \
        } else {                                                            \
            NtUserSetWindowFNID(hwnd, FNID_CLEANEDUP_BIT);                  \
            NtUserDestroyWindow(hwnd);                                      \
            return FALSE;                                                   \
        }                                                                   \
    }

typedef LRESULT (cdecl *EDITCHARSETPROC)(struct tagED *, UINT, ...);

/* DIVERGENCE(OPENNT-HOST-074): original registration extent, distinct from
 * the four-byte PED field in EDITWND. Retain the original constant. */
/* Window extra bytes */
#define CBEDITEXTRA 6

/* OPENNT-HOST-074: original EDIT allocation, limit and key-modifier
 * declarations from usercli.h; values are consumed by unchanged bodies. */
#define CCHALLOCEXTRA   0x20
#define MAXPIXELWIDTH   30000
#define MAXCLIPENDPOS   32764
#define MAXLINELENGTH   1024
#define MAXTEXT         30000
#define NONEDOWN   0 /* Neither shift nor control down */
#define CTRLDOWN   1 /* Control key only down */
#define SHFTDOWN   2 /* Shift key only down */
#define SHCTDOWN   3 /* Shift and control keys down = CTRLDOWN + SHFTDOWN */
#define NOMODIFY   4 /* Neither shift nor control down */
#define UNDO_NONE   0  /* We can't undo the last operation. */
#define UNDO_INSERT 1  /* We can undo the user's insertion of characters */
#define UNDO_DELETE 2  /* We can undo the user's deletion of characters */

/* OPENNT-HOST-075: original native/WOW word-break dispatch. */
#define CALLWORDBREAKPROC(proc, pText, iStart, cch, iAction)                \
    (((DWORD)(proc) & WNDPROC_WOW) ?                                        \
        (* pfnWowEditNextWord)(pText, iStart, cch, iAction, (DWORD)proc) :  \
        (* proc)(pText, iStart, cch, iAction))

typedef struct tagUNDO {
    UINT    undoType;          /* Current type of undo we support */
    PBYTE   hDeletedText;      /* Pointer to text which has been deleted (for
                                  undo) -- note, the memory is allocated as fixed
                                */
    ICH     ichDeleted;        /* Starting index from which text was deleted */
    ICH     cchDeleted;        /* Count of deleted characters in buffer */
    ICH     ichInsStart;       /* Starting index from which text was
                                  inserted */
    ICH     ichInsEnd;         /* Ending index of inserted text */
} UNDO, *PUNDO;

#define Pundo(ped)             ((PUNDO)&(ped)->undoType)

/*
 * Length of the buffer for ASCII character width caching: for characters
 * 0x00 to 0xff (field charWidthBuffer in PED structure below).
 */
#define CHAR_WIDTH_BUFFER_LENGTH 256

typedef struct tagED {
    HANDLE  hText;             /* Block of text we are editing */
    ICH     cchAlloc;          /* Number of chars we have allocated for hText
                                */
    ICH     cchTextMax;        /* Max number bytes allowed in edit control
                                */
    ICH     cch;               /* Current number of bytes of actual text
                                */
    ICH     cLines;            /* Number of lines of text */

    ICH     ichMinSel;         /* Selection extent.  MinSel is first selected
                                  char */
    ICH     ichMaxSel;         /* MaxSel is first unselected character */
    ICH     ichCaret;          /* Caret location. Caret is on left side of
                                  char */
    ICH     iCaretLine;        /* The line the caret is on. So that if word
                                * wrapping, we can tell if the caret is at end
                                * of a line of at beginning of next line...
                                */
    ICH     ichScreenStart;    /* Index of left most character displayed on
                                * screen for sl ec and index of top most line
                                * for multiline edit controls
                                */
    ICH     ichLinesOnScreen;  /* Number of lines we can display on screen */
    UINT    xOffset;           /* x (horizontal) scroll position in pixels
                                * (for multiline text horizontal scroll bar)
                                */
    UINT    charPasswordChar;  /* If non null, display this character instead
                                * of the real text. So that we can implement
                                * hidden text fields.
                                */
    int     cPasswordCharWidth;/* Width of password char */

    HWND    hwnd;              /* Window for this edit control */
    PWND    pwnd;              /* Pointer to window */
    RECT    rcFmt;             /* Client rectangle */
    HWND    hwndParent;        /* Parent of this edit control window */

                               /* These vars allow us to automatically scroll
                                * when the user holds the mouse at the bottom
                                * of the multiline edit control window.
                                */
    POINT   ptPrevMouse;       /* Previous point for the mouse for system
                                * timer.
                                */
    UINT    prevKeys;          /* Previous key state for the mouse */


    UINT     fSingle       : 1; /* Single line edit control? (or multiline) */
    UINT     fNoRedraw     : 1; /* Redraw in response to a change? */
    UINT     fMouseDown    : 1; /* Is mouse button down? when moving mouse */
    UINT     fFocus        : 1; /* Does ec have the focus ? */
    UINT     fDirty        : 1; /* Modify flag for the edit control */
    UINT     fDisabled     : 1; /* Window disabled? */
    UINT     fNonPropFont  : 1; /* Fixed width font? */
    UINT     fBorder       : 1; /* Draw a border? */
    UINT     fAutoVScroll  : 1; /* Automatically scroll vertically */
    UINT     fAutoHScroll  : 1; /* Automatically scroll horizontally */
    UINT     fNoHideSel    : 1; /* Hide sel when we lose focus? */
    UINT     fKanji        : 1;
    UINT     fFmtLines     : 1; /* For multiline only. Do we insert CR CR LF at
                                * word wrap breaks?
                                */
    UINT     fWrap         : 1; /* Do int  wrapping? */
    UINT     fCalcLines    : 1; /* Recalc ped->chLines array? (recalc line
                                * breaks?)
                                */
    UINT     fEatNextChar  : 1; /* Hack for ALT-NUMPAD stuff with combo boxes.
                                * If numlock is up, we want to eat the next
                                * character generated by the keyboard driver
                                * if user enter num pad ascii value...
                                */
    UINT     fStripCRCRLF  : 1; /* CRCRLFs have been added to text. Strip them
                                * before doing any internal edit control
                                * stuff
                                */
    UINT     fInDialogBox  : 1; /* True if the ml edit control is in a dialog
                                * box and we have to specially treat TABS and
                                * ENTER
                                */
    UINT     fReadOnly     : 1; /* Is this a read only edit control? Only
                                * allow scrolling, selecting and copying.
                                */
    UINT     fCaretHidden  : 1; /* This indicates whether the caret is
                                * currently hidden because the width or height
                                * of the edit control is too small to show it.
                                */
    UINT     fTrueType     : 1; /* Is the current font TrueType? */
    UINT     fAnsi         : 1; /* is the edit control Ansi or unicode */
    UINT     fWin31Compat  : 1; /* TRUE if created by Windows 3.1 app */
    UINT     f40Compat     : 1; /* TRUE if created by Windows 4.0 app */
    UINT     fFlatBorder   : 1; /* Do we have to draw this baby ourself? */
    UINT     fSawRButtonDown : 1;
    UINT     fInitialized  : 1; /* If any more bits are needed, then   */

    UINT     fUnused5      : 1; /* remove from 5 DOWNWARD, leaving the */
    UINT     fUnused4      : 1; /* low flags (ie fUnused1) for LPK use */
    UINT     fUnused3      : 1;
    UINT     fUnused2      : 1;
    UINT     fUnused1      : 1;

    WORD    cbChar;            /* count of bytes in the char size (1 or 2 if unicode) */
    LPICH   chLines;           /* index of the start of each line */

    UINT    format;            /* Left, center, or right justify multiline
                                * text.
                                */
    EDITWORDBREAKPROCA lpfnNextWord;  /* use CALLWORDBREAKPROC macro to call */

                               /* Next word function */
    int     maxPixelWidth;     /* WASICH Width (in pixels) of longest line */

    UNDO;                      /* Undo buffer */

    HANDLE  hFont;             /* Handle to the font for this edit control.
                                  Null if system font.
                                */
    int     aveCharWidth;      /* Ave width of a character in the hFont */
    int     lineHeight;        /* Height of a line in the hFont */
    int     charOverhang;      /* Overhang associated with the hFont */
    int     cxSysCharWidth;    /* System font ave width */
    int     cySysCharHeight;   /* System font height */
    HWND    listboxHwnd;       /* ListBox hwnd. Non null if we are a combo
                                  box */
    LPINT   pTabStops;         /* Points to an array of tab stops; First
                                * element contains the number of elements in
                                * the array
                                */
    LPINT   charWidthBuffer;

    BYTE    charSet;           /* Character set for currently selected font
                                * needed for all versions
                                */
    HKL     hkl;               /* HKL (kbd layout and Locale) */

    UINT    wMaxNegA;          /* The biggest negative A width, */
    UINT    wMaxNegAcharPos;   /* and how many characters it can span accross */
    UINT    wMaxNegC;          /* The biggest negative C width, */
    UINT    wMaxNegCcharPos;   /* and how many characters it can span accross */
    UINT    wLeftMargin;       /* Left margin width in pixels. */
    UINT    wRightMargin;      /* Right margin width in pixels. */

    ICH     ichStartMinSel;
    ICH     ichStartMaxSel;

    EDITCHARSETPROC lpfnCharset; /* if non-NULL, points to lpk function */
    DWORD   dwForeign;           /* A dword for lpk's to use privately
                                  * but leave associcated with the ped
                                  */

    HANDLE  hInstance;         /* for WOW */
    UCHAR   seed;              /* used to encode and decode password text */
    BOOL    fEncoded;          /* is the text currently encoded */
    int     iLockLevel;        /* number of times the text has been locked */
} ED, *PED, **PPED;

typedef struct tagEDITWND {
    WND wnd;
    PED ped;
} EDITWND, *PEDITWND;

/* DIVERGENCE(OPENNT-HOST-075): original EDIT cross-unit declarations. */
/*
 * The following structure is used to store a selection block; In Multiline
 * edit controls, "StPos" and "EndPos" fields contain the Starting and Ending
 * lines of the block. In Single line edit controls, "StPos" and "EndPos"
 * contain the Starting and Ending character positions of the block;
 */
typedef struct tagBLOCK {
    ICH StPos;
    ICH EndPos;
}  BLOCK, *LPBLOCK;

/*  The following structure is used to store complete information about a
 *  a strip of text.
 */
typedef  struct {
    LPSTR   lpString;
    ICH     ichString;
    ICH     nCount;
    int     XStartPos;
}  STRIPINFO;
typedef  STRIPINFO FAR *LPSTRIPINFO;

PSTR ECLock(PED ped);
/* OPENNT-HOST-088: original client state-operation declarations. */
void SetWindowState(PWND pwnd, UINT flags);
void ClearWindowState(PWND pwnd, UINT flags);
void ECUnlock(PED ped);
BOOL ECNcCreate(PED, PWND, LPCREATESTRUCT);
void ECInvalidateClient(PED ped, BOOL fErase);
BOOL ECCreate(PWND, PED, LONG);
void ECWord(PED, ICH, BOOL, ICH*, ICH*);
ICH  ECFindTab(LPSTR, ICH);
void ECNcDestroyHandler(PWND, PED);
BOOL ECSetText(PED, LPSTR);
void ECSetPasswordChar(PED, UINT);
ICH  ECCchInWidth(PED, HDC, LPSTR, ICH, int, BOOL);
void ECEmptyUndo(PUNDO);
void ECSaveUndo(PUNDO pundoFrom, PUNDO pundoTo, BOOL fClear);
BOOL ECInsertText(PED, LPSTR, ICH);
ICH  ECDeleteText(PED);
void ECResetTextInfo(PED ped);
void ECNotifyParent(PED, int);
void ECSetEditClip(PED, HDC, BOOL);
HDC  ECGetEditDC(PED, BOOL);
void ECReleaseEditDC(PED, HDC, BOOL);
ICH  ECGetText(PED, ICH, LPSTR, BOOL);
void ECSetFont(PED, HFONT, BOOL);
void ECSetMargin(PED, UINT, long, BOOL);
ICH  ECCopy(PED);
BOOL ECCalcChangeSelection(PED, ICH, ICH, LPBLOCK, LPBLOCK);
void ECFindXORblks(LPBLOCK, LPBLOCK, LPBLOCK, LPBLOCK);
BOOL ECIsCharNumeric(PED ped, DWORD keyPress);

// ECTabTheTextOut draw codes
#define ECT_CALC        0
#define ECT_NORMAL      1
#define ECT_SELECTED    2

UINT ECTabTheTextOut(HDC, int, int, int, int,
                     LPSTR, int, ICH, PED, int, BOOL, LPSTRIPINFO);
HBRUSH ECGetControlBrush(PED, HDC, LONG);
HBRUSH ECGetBrush(PED ped, HDC hdc);
int  ECGetCaretWidth(BOOL);
int  ECGetModKeys(int);
void ECSize( PED, LPRECT, BOOL);

ICH  MLInsertText(PED, LPSTR, ICH, BOOL);
ICH  MLDeleteText(PED);
BOOL MLEnsureCaretVisible(PED);
void MLDrawText(PED, HDC, ICH, ICH, BOOL);
void MLDrawLine(PED, HDC, int, ICH, int, BOOL);
void MLPaintABlock(PED, HDC, int, int);
int  GetBlkEndLine(int, int, BOOL FAR *, int, int);
void MLBuildchLines(PED, ICH, int, BOOL, PLONG, PLONG);
void MLShiftchLines(PED, ICH, int);
BOOL MLInsertchLine(PED, ICH, ICH, BOOL);
void MLSetCaretPosition(PED,HDC);
void MLIchToXYPos(PED, HDC, ICH, BOOL, LPPOINT);
int  MLIchToLine(PED, ICH);
void MLRepaintChangedSelection(PED, HDC, ICH, ICH);
void MLMouseMotion(PED, UINT, UINT, LPPOINT);
ICH  MLLine(PED, ICH);
void MLStripCrCrLf(PED);
int  MLCalcXOffset(PED, HDC, int);
BOOL MLUndo(PED);
LONG MLEditWndProc(HWND, PED, UINT, DWORD, LONG);
void MLChar(PED, DWORD, int);
void MLKeyDown(PED, UINT, int);
ICH  MLPasteText(PED);
void MLSetSelection(PED, BOOL, ICH, ICH);
LONG MLCreate(HWND, PED, LPCREATESTRUCT);
BOOL MLInsertCrCrLf(PED);
void MLSetHandle(PED, HANDLE);
LONG MLGetLine(PED, ICH, ICH, LPSTR);
ICH  MLLineIndex(PED, ICH);
void MLSize(PED, BOOL);
void MLChangeSelection(PED, HDC, ICH, ICH);
void MLSetRectHandler(PED, LPRECT);
BOOL MLExpandTabs(PED);
BOOL MLSetTabStops(PED, int, LPINT);
LONG MLScroll(PED, BOOL, int, int, BOOL);
int  MLThumbPosFromPed(PED, BOOL);
void MLUpdateiCaretLine(PED ped);
ICH  MLLineLength(PED, ICH);

void SLReplaceSel(PED, LPSTR);
BOOL SLUndo(PED);
void SLSetCaretPosition(PED, HDC);
int  SLIchToLeftXPos(PED, HDC, ICH);
void SLChangeSelection(PED, HDC, ICH, ICH);
void SLDrawText(PED, HDC, ICH);
void SLDrawLine(PED, HDC, int, int, ICH, int, BOOL);
int  SLGetBlkEnd(PED, ICH, ICH, BOOL FAR *);
BOOL SLScrollText(PED, HDC);
void SLSetSelection(PED,ICH, ICH);
ICH  SLInsertText(PED, LPSTR, ICH);
ICH  SLPasteText(PED);
void SLChar(PED, DWORD);
void SLKeyDown(PED, DWORD, int);
ICH  SLMouseToIch(PED, HDC, LPPOINT);
void SLMouseMotion(PED, UINT, UINT, LPPOINT);
LONG SLCreate(HWND, PED, LPCREATESTRUCT);
void SLPaint(PED, HDC);
void SLSetFocus(PED);
void SLKillFocus(PED, HWND);
LONG SLEditWndProc(HWND, PED, UINT, DWORD, LONG);
LONG EditWndProc(PWND, UINT, DWORD, LONG);

#define GetAppVer(pti) GetClientInfo()->dwExpWinVer

#define ISDELIMETERA(ch) ((ch == ' ') || (ch == '\t'))
#define ISDELIMETERW(ch) ((ch == L' ') || (ch == L'\t'))

#define AWCOMPARECHAR(ped,pbyte,awchar) (ped->fAnsi ? (*(PUCHAR)(pbyte) == (UCHAR)(awchar)) : (*(LPWSTR)(pbyte) == (WCHAR)(awchar)))

#endif
#endif /* WOW_EDIT_LOOKASIDE_DECLARATIONS_ONLY */

/* DIVERGENCE(OPENNT-HOST-095): unchanged original ComboBox state and its
 * private constants, selected only for the recovered collaboration group.
 * These declarations never describe a native USER allocation. */
#if defined(WOW_ORIGINAL_COMBO_STATE_LAYOUT) && !defined(OPENNT_COMBO_STATE_LAYOUT_H)
#define OPENNT_COMBO_STATE_LAYOUT_H
/*
 * ID numbers (hMenu) for the child controls in the combo box
 */
#define CBLISTBOXID 1000
#define CBEDITID    1001
#define CBBUTTONID  1002

/*
 * For CBOX.c. BoxType field, we define the following combo box styles. These
 * numbers are the same as the CBS_ style codes as defined in windows.h.
 */
#define SDROPPABLE      2
#define SEDITABLE       1

#define SSIMPLE         SEDITABLE
#define SDROPDOWNLIST   SDROPPABLE
#define SDROPDOWN       (SDROPPABLE | SEDITABLE)


/*
 * CBOX.OwnerDraw & LBIV.OwnerDraw types
 */
#define OWNERDRAWFIXED 1
#define OWNERDRAWVAR   2

#define UPPERCASE   1
#define LOWERCASE   2

#define CaretCreate(plb)    ((plb)->fCaret = TRUE)

/*
 * Special styles for static controls, edit controls & listboxes so that we
 * can do combo box specific stuff in their wnd procs.
 */
#define LBS_COMBOBOX    0x8000L

typedef struct tagCBox {
    struct tagWND *spwnd;      /* Window for the combo box */
    struct tagWND *spwndParent;/* Parent of the combo box */
    RECT    editrc;            /* Rectangle for the edit control/static text
                                  area */
    RECT    buttonrc;          /* Rectangle where the dropdown button is */

    int     cxCombo;            // Width of sunken area
    int     cyCombo;            // Height of sunken area
    int     cxDrop;             // 0x24 Width of dropdown
    int     cyDrop;             // Height of dropdown or shebang if simple

    struct tagWND *spwndEdit;  /* Edit control window handle */
    struct tagWND *spwndList;  /* List box control window handle */

    UINT    CBoxStyle:2;         /* Combo box style */
    UINT    fFocus:1;          /* Combo box has focus? */
    UINT    fNoRedraw:1;       /* Stop drawing? */
    UINT    fMouseDown:1;      /* Was the popdown button just clicked and
                                   mouse still down? */
    UINT    fButtonPressed:1; /* Is the dropdown button in an inverted state?
                                */
    UINT    fLBoxVisible:1;    /* Is list box visible? (dropped down?) */
    UINT    OwnerDraw:2;       /* Owner draw combo box if nonzero. value
                                * specifies either fixed or varheight
                                */
    UINT    fKeyboardSelInListBox:1; /* Is the user keyboarding through the
                                      * listbox. So that we don't hide the
                                      * listbox on selchanges caused by the
                                      * user keyboard through it but we do
                                      * hide it if the mouse causes the
                                      * selchange.
                                      */
    UINT    fExtendedUI:1;     /* Are we doing TandyT's UI changes on this
                                * combo box?
                                */
    UINT    fCase:2;

    UINT    f3DCombo:1;         // 3D or flat border?
    UINT    fNoEdit:1;         /* True if editing is not allowed in the edit
                                * window.
                                */
    HANDLE  hFont;             /* Font for the combo box */
    LONG    styleSave;         /* Temp to save the style bits when creating
                                * window.  Needed because we strip off some
                                * bits and pass them on to the listbox or
                                * edit box.
                                */
} CBOX, *PCBOX;

#endif

/* OPENNT-HOST-086: exact listbox declarations required to preserve the
 * original _SetWindowLong initialized-style guard. Not native USER memory. */
#if defined(WOW_ORIGINAL_DIALOG_GETSET_LAYOUT) && !defined(OPENNT_DIALOG_GETSET_LB_LAYOUT_H)
#define OPENNT_DIALOG_GETSET_LB_LAYOUT_H
/* OPENNT-HOST-086: unchanged original list storage/search declarations,
 * also needed when compiling the full list client, not just its setter. */
#define CITEMSALLOC     32
#define EQ        0
#define PREFIX    1
#define LT        2
#define GT        3
#define XCOORD(l)   ((int)LOWORD(l))
#define YCOORD(l)   ((int)HIWORD(l))
#define mod(a,b)    (a - a/b*b)
#define         SINGLESEL       0
#define         MULTIPLESEL     1
#define         EXTENDEDSEL     2
#define LBI_ADD     0x0004
#define LBUP_RELEASECAPTURE 0x0001
#define LBUP_RESETSELECTION 0x0002
#define LBUP_NOTIFY         0x0004
#define LBUP_SUCCESS        0x0008
#define LBUP_SELCHANGE      0x0010
#ifndef OPENNT_COMBO_STATE_LAYOUT_H
typedef struct tagCBox *PCBOX;
#endif
typedef struct _SCROLLPOS {
    INT cItems;
    UINT iPage;
    INT iPos;
    UINT fMask;
    INT iReturn;
} SCROLLPOS, *PSCROLLPOS;

typedef struct tagLBIV {
    PWND    spwndParent;    /* parent window */
    PWND    spwnd;          /* lbox ctl window */
    INT     iTop;           /* index of top item displayed          */
    INT     iSel;           /* index of current item selected       */
    INT     iSelBase;       /* base sel for multiple selections     */
    INT     cItemFullMax;   /* cnt of Fully Visible items. Always contains
                               result of CItemInWindow(plb, FALSE) for fixed
                               height listboxes. Contains 1 for var height
                               listboxes. */
    INT     cMac;           /* cnt of items in listbox              */
    INT     cMax;           /* cnt of total # items allocated for rgpch.
                               Not all are necessarly in use    */
    PBYTE   rgpch;          /* pointer to array of string offsets    */
    LPWSTR  hStrings;       /* string storage handle                */
    INT     cchStrings;     /* Size in bytes of hStrings            */
    INT     ichAlloc;       /* Pointer to end of hStrings (end of last valid
                               string) */
    INT     cxChar;         /* Width of a character                 */
    INT     cyChar;         /* height of line                       */
    INT     cxColumn;       /* width of a column in multicolumn listboxes */
    INT     itemsPerColumn; /* for multicolumn listboxes */
    INT     numberOfColumns; /* for multicolumn listboxes */
    POINT   ptPrev;         /* coord of last tracked mouse pt. used for auto
                               scrolling the listbox during timer's */

    UINT    OwnerDraw:2;      /* Owner draw styles. Non-zero if ownerdraw. */
    UINT     fRedraw:1;      /* if TRUE then do repaints             */
    UINT     fDeferUpdate:1; /* */
    UINT    wMultiple:2;      /* SINGLESEL allows a single item to be selected.
                             * MULTIPLESEL allows simple toggle multi-selection
                             * EXTENDEDSEL allows extended multi selection;
                             */

    UINT     fSort:1;        /* if TRUE the sort list                */
    UINT     fNotify:1;      /* if TRUE then Notify parent           */
    UINT     fMouseDown:1;   /* if TRUE then process mouse moves/mouseup */
    UINT     fCaptured:1;    // if TRUE then process mouse messages
    UINT     fCaret:1;       /* flashing caret allowed               */
    UINT     fDoubleClick:1; /* mouse down in double click           */
    UINT     fCaretOn:1;     /* if TRUE then caret is on             */
    UINT     fAddSelMode:1;  /* if TRUE, then it is in ADD selection mode */
    UINT     fHasStrings:1;  /* True if the listbox has a string associated
                             * with each item else it has an app suppled LONG
                             * value and is ownerdraw
                             */
    UINT     fHasData:1;    /* if FALSE, then lb doesn't keep any line data
                             * beyond selection state, but instead calls back
                             * to the client for each line's definition.
                             * Forces OwnerDraw==OWNERDRAWFIXED, !fSort,
                             * and !fHasStrings.
                             */
    UINT     fNewItemState:1; /* select/deselect mode? for multiselection lb
                              */
    UINT     fUseTabStops:1; /* True if the non-ownerdraw listbox should handle
                             * tabstops
                             */
    UINT     fMultiColumn:1; /* True if this is a multicolumn listbox */
    UINT     fNoIntegralHeight:1; /* True if we don't want to size the listbox
                                  * an integral lineheight
                                  */
    UINT     fWantKeyboardInput:1; /* True if we should pass on WM_KEY & CHAR
                                   * so that the app can go to special items
                                   * with them.
                                   */
    UINT     fDisableNoScroll:1;   /* True if the listbox should
                                    * automatically Enable/disable
                                    * it's scroll bars. If false, the scroll
                                    * bars will be hidden/Shown automatically
                                    * if they are present.
                                    */
    UINT    fHorzBar:1; // TRUE if WS_HSCROLL specified at create time

    UINT    fVertBar:1; // TRUE if WS_VSCROLL specified at create time
    UINT    fFromInsert:1;  // TRUE if client drawing should be deferred during delete/insert ops
    UINT    fNoSel:1;

    UINT    fHorzInitialized : 1;   // Horz scroll cache initialized
    UINT    fVertInitialized : 1;   // Vert scroll cache initialized

    UINT    fSized : 1;             // Listbox was resized.
    UINT    fIgnoreSizeMsg : 1;     // If TRUE, ignore WM_SIZE message

    UINT    fInitialized : 1;

    INT     iLastSelection; /* Used for cancelable selection. Last selection
                             * in listbox for combo box support
                             */
    INT     iMouseDown;     /* For multiselection mouse click & drag extended
                             * selection. It is the ANCHOR point for range
                             * selections
                             */
    INT     iLastMouseMove; /* selection of listbox items */
    /*
     * IanJa/Win32: Tab positions remain int for 32-bit API ??
     */
    LPINT   iTabPixelPositions; /* List of positions for tabs */
    HANDLE  hFont;          /* User settable font for listboxes */
    int     xOrigin;        /* For horizontal scrolling. The current x origin */
    int     maxWidth;       /* Maximum width of listbox in pixels for
                               horizontal scrolling purposes */
    PCBOX   pcbox;          /* Combo box pointer */
    HDC     hdc;            /* hdc currently in use */
    DWORD   dwLocaleId;     /* Locale used for sorting strings in list box */
    int     iTypeSearch;
    LPWSTR  pszTypeSearch;
    SCROLLPOS HPos;
    SCROLLPOS VPos;
} LBIV, *PLBIV;

typedef struct tagLBItem {
    LONG offsz;
    DWORD itemData;
} LBItem, *lpLBItem;

typedef struct tagLBODItem {
    DWORD itemData;
} LBODItem, *lpLBODItem;

typedef struct tagLBWND {
    WND wnd;
    PLBIV pLBIV;
} LBWND, *PLBWND;
#endif


/* OPENNT-HOST-099: unchanged combo/list collaboration declarations.
 * Caller supplies original CBOX/LBIV layouts and the native public types. */
#if defined(WOW_ORIGINAL_CONTROL_PROTOTYPES) && !defined(OPENNT_CONTROL_PROTOTYPES_H)
#define OPENNT_CONTROL_PROTOTYPES_H
/* OPENNT-HOST-105: unchanged collaboration and shared RTL declarations. */
#define IsComboVisible(pcbox) (!pcbox->fNoRedraw && IsVisible(pcbox->spwnd))
#define IsLBoxVisible(plb)  (plb->fRedraw && IsVisible(plb->spwnd))
#define SAMEWOWHANDLE(h1, h2)  ((BOOL)!(((UINT)(h1) ^ (UINT)(h2)) & 0xffff))
LONG ComboBoxWndProcWorker(PWND pwnd, UINT msg, WPARAM wParam,
        LPARAM lParam, DWORD fAnsi);
LONG EditWndProcWorker(PWND pwnd, UINT msg, WPARAM wParam,
        LPARAM lParam, DWORD fAnsi);
BOOL _GetClientRect(PWND pwnd, LPRECT prc);
BOOL _IsChild(PWND pwndParent, PWND pwnd);
BOOL _AdjustWindowRectEx(LPRECT lprc, LONG style, BOOL fMenu, DWORD dwExStyle);
BOOL _ClientToScreen(PWND pwnd, PPOINT ppt);
BOOL _ScreenToClient(PWND pwnd, PPOINT ppt);
BOOL _IsWindowVisible(PWND pwnd);
BOOL IsVisible(PWND pwnd);
HWND InternalCreateDialog(HANDLE hmod, LPDLGTEMPLATE lpDlgTemplate, DWORD cb,
        HWND hwndOwner, DLGPROC pfnWndProc, LONG dwInitParam, UINT fFlags);
PWND _FindDlgItem(PWND pwndParent, DWORD id);
PWND _GetDlgItem(PWND, int);
VOID xxxRemoveDefaultButton(PWND pwndDlg, PWND pwndStart);
VOID xxxCheckDefPushButton(PWND pwndDlg, HWND hwndOldFocus, HWND hwndNewFocus);
VOID DlgSetFocus(HWND hwnd);
BOOL ValidateDialogPwnd(PWND pwnd);
PWND GetTopLevelWindow(PWND pwnd);
/* OPENNT-HOST-106: original Tab-navigation collaborator declarations. */
PWND _NextControl(PWND pwndDlg, PWND pwnd, UINT uFlags);
PWND _PrevControl(PWND pwndDlg, PWND pwnd, UINT uFlags);
PWND _GetChildControl(PWND pwndDlg, PWND pwndLevel);
PWND _GetNextDlgTabItem(PWND pwndDlg, PWND pwnd, BOOL fPrev);
/* OPENNT-HOST-109: original dialog group-navigation collaborator. */
PWND _GetNextDlgGroupItem(PWND pwndDlg, PWND pwnd, BOOL fPrev);
BOOL _IsDescendant(PWND pwndParent, PWND pwndChild);
BOOL FChildVisible(HWND hwnd);
DWORD FindCharPosition(LPWSTR lpString, WCHAR ch);
UINT TextCopy(PLARGE_UNICODE_STRING pstr, LPWSTR lpstr, UINT size);
DWORD wcsncpycch(LPWSTR pwsDest, LPCWSTR pwszSrc, DWORD cch);
VOID RtlInitLargeUnicodeString(PLARGE_UNICODE_STRING plstr, LPCWSTR psz,
        UINT cchLimit);
/* OPENNT-HOST-105: original resource alignment and dialog helper ABI. */
#ifndef NextWordBoundary
#define NextWordBoundary(p)     ((PBYTE)(p) + ((DWORD)(p) & 1))
#endif
#define NextDWordBoundary(p)    ((PBYTE)(p) + ((DWORD)(-(LONG)(p)) & 3))
void RepositionRect(LPRECT lprc, DWORD dwStyle, DWORD dwExStyle);
#define CCHFILEMAX      MAX_PATH
#define MSGFLAG_SPECIAL_THUNK       0x10000000
extern WCHAR awchSlashStar[];
extern CHAR achSlashStar[];
extern WCHAR szOneChar[];
extern WCHAR szSLASHSTARDOTSTAR[];
/*
 * Parameter for xxxAlterHilite()
 */
#define HILITEONLY      0x0001
#define SELONLY         0x0002
#define HILITEANDSEL    (HILITEONLY + SELONLY)

#define HILITE     1

// LATER IanJa: these vary by country!  For US they are VK_OEM_2 VK_OEM_5.
//       Change lboxctl2.c MapVirtualKey to character - and fix the spelling?
#define VERKEY_SLASH     0xBF   /* Vertual key for '/' character */
#define VERKEY_BACKSLASH 0xDC   /* Vertual key for '\' character */

/*
 * Procedures for combo boxes.
 */
LONG  xxxCBCommandHandler(PCBOX, DWORD, HWND);
long  xxxCBMessageItemHandler(PCBOX, UINT, LPVOID);
int   xxxCBDir(PCBOX, UINT, LPWSTR);
VOID  xxxCBPaint(PCBOX, HDC);
VOID  xxxCBCompleteEditWindow(PCBOX pcbox);
BOOL  xxxCBHideListBoxWindow(PCBOX pcbox, BOOL fNotifyParent, BOOL fSelEndOK);
VOID  xxxCBShowListBoxWindow(PCBOX pcbox, BOOL fTrack);
void xxxCBPosition(PCBOX pcbox);

/*
 * combo.h
 */

/* Initialization code */
long  CBNcCreateHandler(PCBOX, PWND, LPCREATESTRUCT);
long  xxxCBCreateHandler(PCBOX, PWND, LPCREATESTRUCT);
void xxxCBCalcControlRects(PCBOX pcbox, LPRECT lprcList);

/* Destruction code */
VOID  xxxCBNcDestroyHandler(PWND, PCBOX);

/* Generic often used routines */
VOID  xxxCBNotifyParent(PCBOX, SHORT);
VOID  xxxCBUpdateListBoxWindow(PCBOX, BOOL);


/* Helpers' */
VOID  xxxCBInternalUpdateEditWindow(PCBOX, HDC);
VOID  xxxCBGetFocusHelper(PCBOX);
VOID  xxxCBKillFocusHelper(PCBOX);
VOID  xxxCBInvertStaticWindow(PCBOX,BOOL,HDC);
VOID  xxxCBSetFontHandler(PCBOX, HANDLE, BOOL);
VOID  xxxCBSizeHandler(PCBOX);
LONG  xxxCBSetEditItemHeight(PCBOX pcbox, int editHeight);


/*
 * String
 */

INT xxxFindString(PLBIV, LPWSTR, INT, INT, BOOL);

VOID  InitHStrings(PLBIV);

int   xxxLBInsertItem(PLBIV, LPWSTR, int, UINT);

/*
 * Selection
 */
BOOL  ISelFromPt(PLBIV, POINT, LPDWORD);
BOOL  IsSelected(PLBIV, INT, UINT);
VOID LBSetCItemFullMax(PLBIV plb);

VOID  xxxLBSelRange(PLBIV, INT, INT, BOOL);

INT xxxLBSetCurSel(PLBIV, INT);

INT LBoxGetSelItems(PLBIV, BOOL, INT, LPINT);

LONG  xxxLBSetSel(PLBIV, BOOL, INT);

VOID  xxxSetISelBase(PLBIV, INT);

VOID  SetSelected(PLBIV, INT, BOOL, UINT);


/*
 * Caret
 */
void xxxLBSetCaret(PLBIV plb, BOOL fSetCaret);
VOID  xxxCaretDestroy(PLBIV);

/*
 * LBox
 */
LONG  xxxLBCreate(PLBIV, PWND, LPCREATESTRUCT);
VOID  xxxDestroyLBox(PLBIV, PWND);
VOID  xxxLBoxDeleteItem(PLBIV, INT);

VOID  xxxLBoxDoDeleteItems(PLBIV);
VOID  xxxLBoxDrawItem(PLBIV, INT, UINT, UINT, LPRECT);


/*
 * Scroll
 */
INT   LBCalcVarITopScrollAmt(PLBIV, INT, INT);

VOID  xxxLBoxCtlHScroll(PLBIV, INT, INT);

VOID  xxxLBoxCtlHScrollMultiColumn(PLBIV, INT, INT);

VOID  xxxLBoxCtlScroll(PLBIV, INT, INT);

VOID  xxxLBShowHideScrollBars(PLBIV);

/*
 * LBoxCtl
 */
INT xxxLBoxCtlDelete(PLBIV, INT);

VOID  xxxLBoxCtlCharInput(PLBIV, UINT, BOOL);
VOID  xxxLBoxCtlKeyInput(PLBIV, UINT, UINT);
VOID  xxxLBPaint(PLBIV, HDC, LPRECT);

BOOL xxxLBInvalidateRect(PLBIV plb, LPRECT lprc, BOOL fErase);
/*
 * Miscellaneous
 */
VOID  xxxAlterHilite(PLBIV, INT, INT, BOOL, INT, BOOL);

INT CItemInWindow(PLBIV, BOOL);

VOID  xxxCheckRedraw(PLBIV, BOOL, INT);

LPWSTR GetLpszItem(PLBIV, INT);

VOID  xxxInsureVisible(PLBIV, INT, BOOL);

VOID  xxxInvertLBItem(PLBIV, INT, BOOL);

VOID  xxxLBBlockHilite(PLBIV, INT, BOOL);

int   LBGetSetItemHeightHandler(PLBIV plb, UINT message, int item, UINT height);
VOID  LBDropObjectHandler(PLBIV, PDROPSTRUCT);
LONG  LBGetItemData(PLBIV, INT);

INT LBGetText(PLBIV, BOOL, BOOL, INT, LPWSTR);

VOID  xxxLBSetFont(PLBIV, HANDLE, BOOL);
int LBSetItemData(PLBIV, INT, LONG);

BOOL  LBSetTabStops(PLBIV, INT, LPINT);

VOID  xxxLBSize(PLBIV, INT, INT);
INT LastFullVisible(PLBIV);

INT xxxLbDir(PLBIV, UINT, LPWSTR);

INT xxxLbInsertFile(PLBIV, LPWSTR);

VOID  xxxNewITop(PLBIV, INT);

VOID  xxxNotifyOwner(PLBIV, INT);

VOID  xxxResetWorld(PLBIV, INT, INT, BOOL);

VOID  xxxTrackMouse(PLBIV, UINT, POINT);
BOOL  xxxDlgDirListHelper(PWND, LPWSTR, LPBYTE, int, int, UINT, BOOL);
BOOL  xxxDlgDirSelectHelper(PWND pwndDlg, LPWSTR pFileName, int cbFileName,
        PWND pwndListBox);
BOOL xxxLBResetContent(PLBIV plb);
VOID xxxLBSetRedraw(PLBIV plb, BOOL fRedraw);
int xxxSetLBScrollParms(PLBIV plb, int nCtl);
void xxxLBButtonUp(PLBIV plb, UINT uFlags);

/*
 * Variable Height OwnerDraw Support Routines
 */
INT CItemInWindowVarOwnerDraw(PLBIV, BOOL);

INT LBPage(PLBIV, INT, BOOL);


/*
 * Multicolumn listbox
 */
VOID  LBCalcItemRowsAndColumns(PLBIV);

/*
 * Both multicol and var height
 */
BOOL  LBGetItemRect(PLBIV, INT, LPRECT);

VOID  LBSetVariableHeightItemHeight(PLBIV, INT, INT);

INT   LBGetVariableHeightItemHeight(PLBIV, INT);

/*
 * No-data (lazy evaluation) listbox
 */
INT  xxxLBSetCount(PLBIV, INT);

UINT LBCalcAllocNeeded(PLBIV, INT);
#endif

/* OPENNT-HOST-093: original canonical client-control extent declarations. */
#if defined(WOW_ORIGINAL_CONTROL_EXTENT_LAYOUT) && !defined(OPENNT_CONTROL_EXTENT_CLIENT_LAYOUT_H)
#define OPENNT_CONTROL_EXTENT_CLIENT_LAYOUT_H
typedef struct tagCOMBOWND {
    WND wnd;
    PCBOX pcbox;
} COMBOWND, *PCOMBOWND;

typedef struct tagSTAT {
    PWND spwnd;
    union {
        HANDLE hFont;
        BOOL   fDeleteIt;
    };
    HANDLE hImage;
    UINT cicur;
    UINT iicur;
} STAT, *PSTAT;

typedef struct tagSTATWND {
    WND wnd;
    PSTAT pstat;
} STATWND, *PSTATWND;

typedef struct tagMDI {
    UINT    cKids;
    HWND    hwndMaxedChild;
    HWND    hwndActiveChild;
    HMENU   hmenuWindow;
    UINT    idFirstChild;
    UINT    wScroll;
    LPWSTR  pTitle;
    UINT    iChildTileLevel;
} MDI, *PMDI;

typedef struct tagMDIWND {
    WND     wnd;
    UINT    dwReserved;         // quattro pro 1.0 stores stuff here!!
    PMDI    pmdi;
} MDIWND, *PMDIWND;
#endif

/* OPENNT-HOST-117: original Button private state declarations. */
/* OPENNT-HOST-119: state-only use by the original dialog collaborators. */
#if (defined(WOW_ORIGINAL_BUTTON_CLIENT) || defined(WOW_ORIGINAL_BUTTON_STATE)) && !defined(OPENNT_BUTTON_CLIENT_STATE_H)
#define OPENNT_BUTTON_CLIENT_STATE_H
#define BUTTONSTATE(pbutn)   (pbutn->buttonState)
#define BST_CHECKMASK       0x0003
#define BST_INCLICK         0x0010
#define BST_CAPTURED        0x0020
#define BST_MOUSE           0x0040
#define BST_DONTCLICK       0x0080
#define BST_INBMCLICK       0x0100
#define PBF_PUSHABLE        0x0001
#define PBF_DEFAULT         0x0002
#define DBT_TEXT            0x0001
#define DBT_FOCUS           0x0002
#endif
