#pragma pack(push,1)
typedef struct _PARMWP {    /* wp */
    LONG    lParam;     //
    WORD    wParam;     //
    WORD    wMsg;       //
    WORD    hwnd;       //
    WORD    hInst;      // hInstance of window that we are returning to
} PARMWP;


/* EnumPropsProc callback function parameter format
 */
typedef struct _PARMEPP {   /* epp */
    HAND16  hData;
    VPVOID  vpString;
    HWND16  hwnd;
} PARMEPP;


/* EnumWindows/EnumChildWindows/EnumTaskWindows callback function parameter format
 */
typedef struct _PARMEWP {       /* ewp */
    LONG    lParam;             // app-defined data
    HWND16  hwnd;               // 16-bit window handle
} PARMEWP;


/* EnumFonts callback function parameter format
 */
typedef struct _PARMEFP {       /* efp */
    VPVOID  vpData;     // app-defined data
    SHORT   nFontType;      //
    VPVOID  vpTextMetric;   // pointer to TEXTMETRIC16
    VPVOID  vpLogFont;      // pointer to LOGFONT16
} PARMEFP;


/* EnumObj callback function parameter format
 */
typedef struct _PARMEOP {       /* eop */
    VPVOID  vpData;     // app-defined data
    VPVOID  vpLogObject;
} PARMEOP;


/* EnumMetaFile callback function parameter format
 */
typedef struct _PARMEMP {       /* emp */
    VPVOID  vpData;     // app-defined data
    SHORT   nObjects;       // # objects
    VPVOID  vpMetaRecord;   // pointer to METARECORD16
    VPVOID  vpHandleTable;  // pointer to HANDLETABLE16
    HDC16   hdc;        // hdc
} PARMEMP;

/* Hook Callback function parameter format
 */
typedef struct _PARMHKP {       /* hkp */
    VPVOID  lParam;
    SHORT   wParam;
    SHORT   nCode;          // action code
} PARMHKP;

/* Subclass Callback function parameter format
 */
typedef struct _PARMSCP {       /* scp */
    SHORT    iOrdinal;          // oridnal number;
} PARMSCP;

/* LineDDA Callback function parameter format
 */
typedef struct _PARMDDA {   /* dda */
    VPVOID vpData;
    SHORT  y;
    SHORT  x;
} PARMDDA;

/* Graystring callback function parameter format
 */
typedef struct _PARMGST {   /* gst */
    SHORT n;
    DWORD data;
    HDC16 hdc;
} PARMGST;


typedef struct _PARMDIR { /* cdir */
    SHORT wDrive;
    VPSZ  vpDir;  // directory name
} PARMDIR;

typedef struct _PARMSAP { /* sap */
    SHORT  code;    //
    HAND16 hPr;
} PARMSAP;


/* WordBreakProc callback function parameter format
 */
typedef struct _PARMWBP {       /* wbp */
    SHORT   action;
    SHORT   cbEditText;
    SHORT   ichCurrentWord;
    VPVOID  lpszEditText;
} PARMWBP;


/*++

  MultiMedia callback definitions added, and also to _PARM16 - MikeTri

--*/

/* midiInOpen (MidiInFunc) Callback function parameter format
 */

typedef struct _PARMMIF {       /* mif */
    DWORD     dwParam2;
    DWORD     dwParam1;
    DWORD     dwInstance;
    WORD      wMsg;
    HMIDIIN16 hMidiIn;
} PARMMIF;

/* midiOutOpen (MidiOutFunc) Callback function parameter format
 */

typedef struct _PARMMOF {       /* mof */
    DWORD      dwParam2;
    DWORD      dwParam1;
    DWORD      dwInstance;
    WORD       wMsg;
    HMIDIOUT16 hMidiOut;
} PARMMOF;

/* mmioInstallIOProc (IOProc) Callback function parameter format
 */

typedef struct _PARMIOP {      /* iop */
    LONG      lParam2;
    LONG      lParam1;
    WORD      wMsg;
    VPVOID    lpmmioinfo;
} PARMIOP;

/* timeSetEvent (TimeFunc) Callback function parameter format
 */

typedef struct _PARMTIF {       /* tif */
    DWORD     dw2;
    DWORD     dw1;
    DWORD     dwUser;
    WORD      wMsg;
    WORD      wID;
} PARMTIF;

/* waveInOpen (WaveInFunc) Callback function parameter format
 */

typedef struct _PARMWIF {       /* wif */
    DWORD     dwParam2;
    DWORD     dwParam1;
    DWORD     dwInstance;
    WORD      wMsg;
    HWAVEIN16 hWaveIn;
} PARMWIF;

/* waveOutOpen (WaveOutFunc) Callback function parameter format
 */

typedef struct _PARMWOF {       /* wof */
    DWORD      dwParam2;
    DWORD      dwParam1;
    DWORD      dwInstance;
    WORD       wMsg;
    HWAVEOUT16 hWaveOut;
} PARMWOF;

/* WOWCallback16 function parameter format
 */

typedef struct _PARMWCB16 {       /* wcb16 */
    WORD       wArgs[8];
} PARMWCB16;

/* PARM16 is the union of all the callback parameter structures
 */
typedef union _PARM16 {     /* parm16 */
    PARMWP  WndProc;        // for window procs
    PARMEWP EnumWndProc;        // for window enum functions
    PARMEFP EnumFontProc;       // for font enum functions
    PARMEOP EnumObjProc;       // for obj enum functions
    PARMEMP EnumMetaProc;       // for metafile enum functions
    PARMEPP EnumPropsProc;  // for properties
    PARMHKP HookProc;           // for Hooks
    PARMSCP SubClassProc;   // for subclass thunks
    PARMDDA LineDDAProc;    // for LineDDA
    PARMGST GrayStringProc; // for GrayString
    PARMDIR CurDir;
    PARMSAP SetAbortProc;   // for SetAbortProc
    PARMMIF MidiInFunc;         // for midiInOpen functions - MikeTri 27-Mar-1992
    PARMMOF MidiOutFunc;        // for midiOutOpen functions
    PARMIOP IOProc;             // for mmioInstallIOProc functions
    PARMTIF TimeFunc;           // for timeSetEvent functions
    PARMWIF WaveInFunc;         // for waveInOpen functions
    PARMWOF WaveOutFunc;        // for waveOutOpen functions
    PARMWBP WordBreakProc;      // for WordBreakProc
    PARMWCB16 WOWCallback16;    // for WOWCallback16
} PARM16, *PPARM16;
#pragma pack(pop)
typedef struct _DLGDATA {   /* dlgdata */
    VPPROC  vpfnDlgProc;    // 16-bit dialog function
    DWORD   dwUserInitParam;    // user init param, if any
} DLGDATA, *PDLGDATA;


/* Data structure used in thunking LB_GETTEXT special case
 */

typedef struct _THUNKTEXTDWORD  {
    BOOL               fDWORD;     // dword used or text
    DWORD              dwDataItem; // dword
} THUNKTEXTDWORD, *PTHUNKTEXTDWORD;

typedef union _MSGTHUNKBUFFER {
    MSG                msg;
    DRAWITEMSTRUCT     ditem;
    MEASUREITEMSTRUCT  mitem;
    DELETEITEMSTRUCT   delitem;
    COMPAREITEMSTRUCT  cmpitem;
    RECT               rect;
    DLGDATA            dlgdata;
    CREATESTRUCT       cstruct;
    WINDOWPOS          winpos;
    CLIENTCREATESTRUCT clcstruct;
    MDINEXTMENU        mnm;
    MDICREATESTRUCT    mdis;
    DROPSTRUCT         dps;
    POINT              pt[5];                               // WM_GETMINMAXINFO
    UINT               uinteger[2];                         // SBM_GETRANGE
    BYTE               cmdichild[sizeof(CREATESTRUCT) +
                                  sizeof(MDICREATESTRUCT)]; // FinishThunking...
    BYTE               cmdiclient[sizeof(CREATESTRUCT) +
                               sizeof(CLIENTCREATESTRUCT)]; // FinishThunking...
    BYTE               calcsz[sizeof(NCCALCSIZE_PARAMS) +
                                        sizeof(WINDOWPOS)];
    THUNKTEXTDWORD     thkdword;                            // LB_GETTEXT w/no HASSTRINGS
    /* DIVERGENCE(MVDM-HOST-DIV-344): synchronous COPYDATA release keys. */
    struct { DWORD header, data, flags; } copydata;
    /* DIVERGENCE(MVDM-HOST-DIV-347): call progress, no union size change. */
    struct { DWORD flags, native; } text;
    /* DIVERGENCE(MVDM-HOST-DIV-353): original INITIATE node publication. */
    struct { DWORD active, initiator; } ddeinit;
} MSGTHUNKBUFFER, *LPMSGTHUNKBUFFER;

typedef struct _MSGPARAMEX *LPMSGPARAMEX;
typedef BOOL   (FASTCALL *LPFNTHUNKMSG16)(LPMSGPARAMEX lpmpex);
typedef VOID   (FASTCALL *LPFNUNTHUNKMSG16)(LPMSGPARAMEX lpmpex);

typedef struct _MSGPARAMEX {
    PARM16 Parm16;
    HWND hwnd;
    UINT uMsg;
    UINT uParam;
    LONG lParam;
    LONG   lReturn;
    LPFNUNTHUNKMSG16 lpfnUnThunk16;
    PWW            pww;
    INT            iMsgThunkClass;          // thunking aid
    INT            iClass;
    MSGTHUNKBUFFER MsgBuffer[1];
    /* DIVERGENCE(MVDM-HOST-DIV-344): NULL HWND can be a valid thread message. */
    BOOL fThunkSucceeded;
    /* DIV-363: optional owner supplied only by WU32PostMessage. */
    PVOID ddePostConversion;
    BOOL ddePostPacked;
} MSGPARAMEX;
